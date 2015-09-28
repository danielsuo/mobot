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

// Manipulate file descriptors
#include <fcntl.h>

using namespace std;

// Server buffer size to read each round
#define BUFFER_SIZE 4096

// Timeout period in seconds before server closes client connection
#define CONNECTION_TIMEOUT 5

// Timeout in number of empty reads (iOS keeps spamming \0)
#define EMPTY_READ_TIMEOUT 1000

// End of transmission string
#define TRANSMISSION_EOF = "edu.princeton.vision.capture.tcpWriter.EOF"

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
void mkdirp(const char *dir, mode_t mode) {

    // Find the string length of the directory path
    int len = 0;
    while (*(dir + len) != 0) len++;

    // Create a temporary string to hold the current directory
    char *tmp = (char *)malloc(sizeof(char) * ++len);
    char *p = NULL;

    // Copy the directory path to tmp
    snprintf(tmp, len, "%s", dir);

    // Remove any trailing /
    if(tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    struct stat st = {0};
            
    // Loop through each character in the directory path
    for(p = tmp + 1; *p; p++) {

        // If the character is /, temporarily replace with \0 to terminate
        // string and create directory at the parent path
        if(*p == '/') {

            *p = 0;

            if (stat(tmp, &st) == -1) {
                mkdir(tmp, mode);
            }

            // Change \0 back to /
            *p = '/';
        }
    }
            
    // Create the last directory in the hierarchy
    if (stat(tmp, &st) == -1) {
        mkdir(tmp, mode);
    }

    free(tmp);
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

void *handler_client_data(void *device_pointer)
{
    Device *device = (Device *)device_pointer;

    // Make our accept calls non-blocking
    fcntl(device->dat_fd, F_SETFL, O_NONBLOCK);

    // File type: directory (0), file (1)
    char file_type = 0;

    // Byte index in the current file
    uint32_t file_index = 0;

    // Byte syze of the current file
    uint32_t file_length = 0;

    // Current file path
    char *file_path;

    // Keep track of where we are in the buffer
    uint32_t data_index = 0;

    // Get current file we're writing to
    FILE *outfile = NULL;

    // Count number of reads with only \0
    int num_empty_reads = 0;

    // The server reads characters from the socket connection into this buffer.
    // This code initializes the buffer using the bzero() function, and then
    // reads from the socket. Note that the read call uses the new file
    // descriptor, the one returned by accept(), not the original file
    // descriptor returned by socket(). Note also that the read() will block
    // until there is something for it to read in the socket, i.e. after the
    // client has executed a write().
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    while (1) {
        // buffer_length is the return value for the read() and write() calls;
        // i.e. it contains the number of characters read or written
        uint32_t buffer_length;

        // Create file descriptor set so we can check which descriptors have
        // reads available
        fd_set readfds;

        // Create a struct to hold the connection timeout length
        struct timeval tv;

        // Give connection timeout in seconds and microseconds
        tv.tv_sec = CONNECTION_TIMEOUT;
        tv.tv_usec = 0;

        // Set reads available to false for all file descriptors except the
        // current socket. We only want to track our socket
        FD_ZERO(&readfds);
        FD_SET(device->dat_fd, &readfds);

        // don't care about writefds and exceptfds:
        select(device->dat_fd+1, &readfds, NULL, NULL, &tv);

        // If we haven't had a read within our timeout, return from this
        // function and close the connection
        if (!FD_ISSET(device->dat_fd, &readfds)) {
            printf("Timed out from no data.\n");
            break;
        }

        // It will read either the total number of characters in the socket or
        // 255, whichever is less, and return the number of characters read.
        buffer_length = data_index + read(device->dat_fd, buffer + data_index, BUFFER_SIZE - data_index - 1);

        // If we've read 0 bytes more than EMPTY_READ_TIMEOUT times, close the
        // connection. Otherwise, continue. If we've read a positive number of
        // bytes, reset the number of empty reads we've seen.
        if (buffer_length == 0) {
            num_empty_reads++;
            if (num_empty_reads > EMPTY_READ_TIMEOUT) {
                printf("Timed out from empty reads.\n");
                break;
            } else {
                continue;
            }
        } else {
            num_empty_reads = 0;
        }

        // printf("\n\nNUMBER OF BYTES READ: %d\n-----------------------------------------\n", buffer_length);
        // printf("Data read: %s\n", buffer);
        // printf("Begin File index: %d, file length, %d\n", file_index, file_length);

        data_index = 0;

        // TODO: handle case where buffer is not long enough to even hold
        // metadata
        if (file_index == file_length) {
            // Get file type
            file_type = buffer[data_index];
            data_index += sizeof(char);
            // printf("File type: %d\n", file_type);

            // Get timestamp
            double *timestamp_ptr = subarray(double, buffer, data_index, 1);
            double timestamp = *timestamp_ptr;
            data_index += sizeof(double);
            // printf("Timestamp: %llu\n", timestamp);

            // Get path length
            char path_length = buffer[data_index];
            data_index += sizeof(char);
            // printf("Path length: %d\n", path_length);

            // Get file path
            file_path = substr(buffer, data_index, path_length);
            // printf("Creating %s %s\n", file_type ? "file" : "directory", file_path);

            // If we're writing a directory, mkdir
            if (file_type == 0) {
                mkdirp(file_path, S_IRWXU);
            } else if (file_type == 1) {
                // Open file for appending bytes
                if (outfile != NULL) {
                    fclose(outfile);
                }
                outfile = fopen(file_path, "ab");                
            }

            free(file_path);

            data_index += path_length;
            // printf("Path: %s\n", file_path);

            // Get file length
            uint32_t *file_length_ptr = subarray(uint32_t, buffer, data_index, 1);
            file_length = *file_length_ptr;
            data_index += sizeof(uint32_t);
            // printf("File length: %u\n", file_length);
        }

        // If we're writing a file, append to file
        if (file_type == 1) {

            // If we're writing a new file, file_index is 0, so we write the
            // lesser of the file's length and the amount of data left in the
            // buffer. If we're continuing a file, data_index is 0, so we write
            // the lesser of the remaining file length or the amount of data in
            // the buffer. int data_length = min(file_length - file_index,
            // BUFFER_SIZE - data_index);
            int data_length = min(file_length - file_index, buffer_length - data_index);

            fwrite(buffer + data_index, sizeof(char), data_length, outfile);

            data_index += data_length;
            file_index += data_length;
        }

        // printf("End File index: %d, file length, %d\n", file_index, file_length);

        // We've finished a file and must save off any remaining data
        if (file_index == file_length) {
            // Clean up
            file_index = 0;
            file_length = 0;
            file_type = 0;
            if (outfile != NULL) {
                fclose(outfile);
                outfile = NULL;
            }

            // If we have data left, keep it
            if (data_index < buffer_length) {

                // Shuffle remaining bytes forward
                for (int i = data_index; i < buffer_length; i++) {
                    buffer[i - data_index] = buffer[i];
                }
            }

            // Update data_index after the shuffle. If no shuffle, nothing
            // happens (assumes data_index is always smaller than buffer_length)
            data_index = buffer_length - data_index;

            // Zero out the rest of the buffer
            zeros(buffer + data_index, buffer_length - data_index);

            // print data left
            // printf("Data left: %s\n", buffer);
        }

        // We are still in the middle of a file
        else {
            // Reset buffer
            data_index = 0;
            zeros(buffer, BUFFER_SIZE);
        }
    }

    close(device->dat_fd);
    pthread_exit(NULL);
    return 0;
}