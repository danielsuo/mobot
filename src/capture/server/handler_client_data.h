/*******************************************************************************
 * server.cpp: C++ implementation of PVCapture TCP server
 *
 * Serial packet format
 * --------------------------------------------------------------
 * Description                     |    Variable | Length (bytes)
 * --------------------------------------------------------------
 * 1. File type (0: dir, 1: file)  |         N/A |              1
 * 2. Timestamp                    |         N/A |              8
 * 3. String length of file path   | PATH_LENGTH |              1
 * 4. File path                    |        N/A  |    PATH_LENGTH
 * 5. Byte length of file          | FILE_LENGTH |              4
 * 6. File contents                |        N/A  |    FILE_LENGTH
 *
 ******************************************************************************/

// Check if directory exists
#include <sys/stat.h>

// Check system limits
#include <sys/syslimits.h>

// Check errors
#include <errno.h>

// Manipulate file descriptors
#include <fcntl.h>

using namespace std;

// Server buffer size to read each round
#define BUFFER_SIZE 4096

// Timeout period in seconds before server closes client connection
#define CONNECTION_TIMEOUT 5

// Max metadata size in bytes (approximate; this is a fudge)
#define MAX_METADATA_SIZE 200

// Get subarray
#define subarray(type, arr, off, len) (type (&)[len])(*(arr + off));

// Get character substrings
char *substr(char *arr, int begin, int len)
{
    char *res = (char *)malloc(sizeof(char) * len);
    for (int i = 0; i < len; i++)
        res[i] = *(arr + begin + i);
    res[len] = 0;

    return res;
}

// Create file directories recursively
void mkdirp(char *dir, mode_t mode, bool is_dir) {

    // Find the string length of the directory path
    int len = 0;
    while (*(dir + len) != 0) len++; len++;

    char *p = NULL;

    // Remove any trailing /
    if (dir[len - 1] == '/') {
        dir[len - 1] = 0;
    }

    struct stat st = {0};

    // Loop through each character in the directory path
    for (p = dir + 1; *p; p++) {

        // If the character is /, temporarily replace with \0 to terminate
        // string and create directory at the parent path
        if (*p == '/') {

            *p = 0;

            if (stat(dir, &st) == -1) {
                mkdir(dir, mode);
            }

            // Change \0 back to /
            *p = '/';
        }
    }

    // Create the last directory in the hierarchy
    if (stat(dir, &st) == -1 && is_dir) {
        mkdir(dir, mode);
    }

    // free(tmp);
}

// Zero out an array
void zeros(char *array, int len) {
    for (int i = 0; i < len; i++) {
        array[i] = 0;
    }
}

// This function is called when a system call fails. It displays a message about
// the error on stderr and then aborts the program. The perror man page gives
// more information: http://www.linuxhowtos.org/data/6/perror.txt
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void refill_buffer(int fd, char *buffer, uint32_t &buffer_index, int &buffer_length, int data_length) {
    // fprintf(stderr, "Device: %s | Refilling buffer of size %d at index %d\n", device->name, buffer_length, buffer_index);

    // Need to refill buffer
    if (buffer_index + data_length > buffer_length) {

        // Buffer currently has some data
        if (buffer_index > 0) {
            // Shuffle existing buffer data forward
            for (int i = buffer_index; i < buffer_length; i++) {
                buffer[i - buffer_index] = buffer[i];
            }

            // Zero out remaining buffer
            buffer_index = buffer_length - buffer_index;
            zeros(buffer + buffer_index, buffer_length - buffer_index);
        }

        // fprintf(stderr, "Device: %s | Updated buffer index %d\n", device->name, buffer_index);

        buffer_length = buffer_index + read(fd, buffer + buffer_index, BUFFER_SIZE - buffer_index - 1);

        // if (buffer_length < 0) {
        //     // fprintf(stderr, "Device: %s | Failed to refill buffer with error %d\n", device->name, errno);
        // } else {
        //     // Log stuff
        //     FILE *flog = NULL;

        //     flog = fopen("flog", "ab");
        //     fwrite(buffer + buffer_index, sizeof(char), buffer_length - buffer_index, flog);
        //     fclose(flog);
        // }

        buffer_index = 0;
    }
}

void *handler_client_data(void *device_pointer) {
    Device *device = (Device *)device_pointer;

    // File type: directory (0), file (1)
    char file_type = 0;

    // Byte index in the current file
    uint32_t file_index = 0;

    // Byte syze of the current file
    uint32_t file_length = 0;

    // Current file path
    char *file_path;

    // Keep track of where we are in the buffer
    uint32_t buffer_index = 0;

    // Get current file we're writing to
    FILE *outfile = NULL;

    // The server reads characters from the socket connection into this buffer.
    // This code initializes the buffer using the bzero() function, and then
    // reads from the socket. Note that the read call uses the new file
    // descriptor, the one returned by accept(), not the original file
    // descriptor returned by socket(). Note also that the read() will block
    // until there is something for it to read in the socket, i.e. after the
    // client has executed a write().
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    // buffer_length is the return value for the read() and write() calls;
    // i.e. it contains the number of characters read or written
    int buffer_length = 0;

    while (1) {
        // // Create file descriptor set so we can check which descriptors have
        // // reads available
        // fd_set readfds;

        // // Create a struct to hold the connection timeout length
        // struct timeval tv;

        // // Give connection timeout in seconds and microseconds
        // tv.tv_sec = CONNECTION_TIMEOUT;
        // tv.tv_usec = 0;

        // // Set reads available to false for all file descriptors except the
        // // current socket. We only want to track our socket
        // FD_ZERO(&readfds);
        // FD_SET(device->dat_fd, &readfds);

        // // don't care about writefds and exceptfds:
        // select(device->dat_fd + 1, &readfds, NULL, NULL, &tv);

        // // If we haven't had a read within our timeout, return from this
        // // function and close the connection
        // if (!FD_ISSET(device->dat_fd, &readfds)) {
        //     // fprintf(stderr, "Device: %s | Timed out from no data.\n", device->name);
        //     break;
        // }

        refill_buffer(device->dat_fd, buffer, buffer_index, buffer_length, BUFFER_SIZE);
        if (buffer_length <= 0) continue;

        // fprintf(stderr, "Device: %s | Begin file index: %d, file length: %d\n", device->name, file_index, file_length);

        if (file_index == file_length) {
            // fprintf(stderr, "Device: %s | \nBYTES READ: %d\n-----------------------------------------\n", device->name, buffer_length);

            // Get file type
            file_type = buffer[buffer_index];
            buffer_index += sizeof(char);
            fprintf(stderr, "Device: %s | File type: %02x\n", device->name, file_type);

            // Get timestamp
            double *timestamp_ptr = subarray(double, buffer, buffer_index, 1);
            double timestamp = *timestamp_ptr;
            buffer_index += sizeof(double);
            // fprintf(stderr, "Device: %s | Timestamp: %f\n", device->name, timestamp);

            // Get path length
            char path_length = buffer[buffer_index];
            buffer_index += sizeof(char);
            // fprintf(stderr, "Device: %s | Path length: %d\n", device->name, path_length);

            // Get file path
            file_path = substr(buffer, buffer_index, path_length);

            if (timestamp > 0) device->processTimestamp(file_path, timestamp);

            // If we're writing a directory, mkdir
            if (file_type == 0) {
                mkdirp(file_path, S_IRWXU, true);
                fprintf(stderr, "Device: %s | Creating directory %s\n", device->name, file_path);
            } else if (file_type == 1) {
                // Open file for appending bytes
                if (outfile != NULL) {
                    fclose(outfile);
                }
                mkdirp(file_path, S_IRWXU, false);
                outfile = fopen(file_path, "ab");
                if (outfile == NULL) fprintf(stderr, "Device: %s | Creating file %s, error: %d\n", device->name, file_path, errno);
            }

            free(file_path);

            buffer_index += path_length;
            fprintf(stderr, "Device: %s | Path: %s\n", device->name, file_path);

            // Get file length
            uint32_t *file_length_ptr = subarray(uint32_t, buffer, buffer_index, 1);
            file_length = *file_length_ptr;
            buffer_index += sizeof(uint32_t);
            fprintf(stderr, "Device: %s | File length: %u\n", device->name, file_length);
        }

        // If we're writing a file, append to file
        if (file_type == 1) {
            // fprintf(stderr, "Device: %s | Writing to file...");

            // If we're writing a new file, file_index is 0, so we write the
            // lesser of the file's length and the amount of data left in the
            // buffer. If we're continuing a file, buffer_index is 0, so we write
            // the lesser of the remaining file length or the amount of data in
            // the buffer. int data_length = min(file_length - file_index,
            // BUFFER_SIZE - buffer_index);
            int data_length = min(file_length - file_index, buffer_length - buffer_index);
            fprintf(stderr, "Device: %s | Data length: %d; data index: %d; buffer length: %d\n", device->name, data_length, buffer_index, buffer_length);

            // int fno;

            // char filePath[PATH_MAX];
            // if (outfile == NULL) {
            //     fprintf(stderr, "Device: %s | Outfile null...\n", device->name);
            // } else {
            //     fprintf(stderr, "Device: %s | %p", (void *)outfile);
            // }
            // fno = fileno(outfile);
            // fprintf(stderr, "Device: %s | fno: %d\n", device->name, fno);
            // if (fcntl(fno, F_GETPATH, filePath) != -1) {
            //     fprintf(stderr, "Device: %s | Outfile exists at %s\n", device->name, filePath);
            // } else {
            //     fprintf(stderr, "Device: %s | Outfile doesn't exist!\n", device->name);
            // }

            fwrite(buffer + buffer_index, sizeof(char), data_length, outfile);

            fprintf(stderr, "Device: %s | Wrote to file!\n", device->name);

            buffer_index += data_length;
            file_index += data_length;
        }

        // fprintf(stderr, "Device: %s | End file index: %d, file length, %d\n", device->name, file_index, file_length);

        // We've finished a file and must save off any remaining data
        if (file_index == file_length) {
            // fprintf(stderr, "Device: %s | Cleaning up...\n", device->name);
            // Clean up
            file_index = 0;
            file_length = 0;
            file_type = 0;
            if (outfile != NULL) {
                fclose(outfile);
                outfile = NULL;
            }
        }

        // We are still in the middle of a file
        else {
            zeros(buffer, BUFFER_SIZE);
        }
    }

    close(device->dat_fd);
    pthread_exit(NULL);
    return 0;
}