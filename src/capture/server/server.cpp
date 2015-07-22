/*******************************************************************************
 * server.cpp: C++ implementation of PVCapture TCP server
 *
 * Serial packet format
 * --------------------------------------------------------------
 * Description                    |     Variable | Length (bytes)
 * --------------------------------------------------------------
 * 1. File type (0: dir, 1: file)  |        N/A  |              1
 * 2. String length of file path   | PATH_LENGTH |              3
 * 3. File path                    |        N/A  |    PATH_LENGTH
 * 4. Byte length of file          | FILE_LENGTH |             10
 * 5. File contents                |        N/A  |    FILE_LENGTH
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This header file contains definitions of a number of data types used in
// system calls. These types are used in the next two include files.
#include <sys/types.h>

// The header file socket.h includes a number of definitions of structures
// needed for sockets.
#include <sys/socket.h>

// The header file in.h contains constants and structures needed for internet
// domain addresses.
#include <netinet/in.h>

// This function is called when a system call fails. It displays a message about
// the error on stderr and then aborts the program. The perror man page gives
// more information: http://www.linuxhowtos.org/data/6/perror.txt
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void processConnection (int sock)
{
    // n is the return value for the read() and write() calls; i.e. it contains
    // the number of characters read or written
    int n;

    // The server reads characters from the socket connection into this buffer
    char buffer[256];
      
    // This code initializes the buffer using the bzero() function, and then
    // reads from the socket. Note that the read call uses the new file
    // descriptor, the one returned by accept(), not the original file
    // descriptor returned by socket(). Note also that the read() will block
    // until there is something for it to read in the socket, i.e. after the
    // client has executed a write().
    bzero(buffer, 256);

    // It will read either the total number of characters in the socket or 255,
    // whichever is less, and return the number of characters read.
    n = read(sock, buffer, 255);
    if (n < 0) error("ERROR reading from socket");

    // Print message we got
    printf("Here is the message: %s\n", buffer);

    // Confirm we received the message
    n = write(sock,"I got your message", 18);
    if (n < 0) error("ERROR writing to socket");
}

int main(int argc, char *argv[])
{
    // Store value returned by the socket system call
    int sockfd;

    // Store value returned by the accept system call
    int newsockfd;

    // Stores port number where server accepts connections
    int portno;

    // Stores size of the address of the client for the accept system call
    socklen_t clilen;

    // A sockaddr_in is a structure containing an internet address. This
    // structure is defined in netinet/in.h
    // 
    // An in_addr structure, defined in the same header file, contains only one
    // field, a unsigned long called s_addr.
    //
    // struct sockaddr_in
    // {
    //   short   sin_family; /* must be AF_INET */
    //   u_short sin_port;
    //   struct  in_addr sin_addr; 
    //   char    sin_zero[8]; /* Not used, must be zero */
    // };
    //
    // The variable serv_addr will contain the address of the server, and
    // cli_addr will contain the address of the client which connects to the
    // server.
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    // The user needs to pass in the port number on which the server will accept
    // connections as an argument. This code displays an error message if the
    // user fails to do this
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // The socket() system call creates a new socket. There are three arguments:
    // 
    // 1. The address domain of the socket: there are two possible address
    // domains, the unix domain for two processes which share a common file
    // system, and the Internet domain for any two hosts on the Internet. The
    // symbol constant AF_UNIX is used for the former, and AF_INET for the
    // latter (there are actually many other options which can be used here for
    // specialized purposes).
    // 
    // 2. The second argument is the type of socket. Recall that there are two
    // choices here, a stream socket in which characters are read in a
    // continuous stream as if from a file or pipe, and a datagram socket, in
    // which messages are read in chunks. The two symbolic constants are
    // SOCK_STREAM and SOCK_DGRAM.
    //
    // 3. The third argument is the protocol. If this argument is zero (and it
    // always should be except for unusual circumstances), the operating system
    // will choose the most appropriate protocol. It will choose TCP for stream
    // sockets and UDP for datagram sockets.
    //
    // The socket system call returns an entry into the file descriptor table
    // (i.e. a small integer). This value is used for all subsequent references
    // to this socket.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // If the socket call fails, it returns -1
    if (sockfd < 0) {
       error("ERROR opening socket");
    }

    // The function bzero() sets all values in a buffer to zero. It takes two
    // arguments, the first is a pointer to the buffer and the second is the
    // size of the buffer. Thus, this line initializes serv_addr to zeros.
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Read port number as int
    portno = atoi(argv[1]);

    // Address type is network
    serv_addr.sin_family = AF_INET;

    // Get port address in network byte order
    serv_addr.sin_port = htons(portno);

    // The third field of sockaddr_in is a structure of type struct in_addr
    // which contains only a single field unsigned long s_addr. This field
    // contains the IP address of the host. For server code, this will always be
    // the IP address of the machine on which the server is running, and there
    // is a symbolic constant INADDR_ANY which gets this address.
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // The bind() system call binds a socket to an address, in this case the
    // address of the current host and port number on which the server will run.
    // It takes three arguments, the socket file descriptor, the address to
    // which is bound, and the size of the address to which it is bound. The
    // second argument is a pointer to a structure of type sockaddr, but what is
    // passed in is a structure of type sockaddr_in, and so this must be cast to
    // the correct type. This can fail for a number of reasons, the most obvious
    // being that this socket is already in use on this machine.
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // The listen system call allows the process to listen on the socket for
    // connections. The first argument is the socket file descriptor, and the
    // second is the size of the backlog queue, i.e., the number of connections
    // that can be waiting while the process is handling a particular
    // connection. This should be set to 5, the maximum size permitted by most
    // systems.
    listen(sockfd,5);

    // Get length of client address
    clilen = sizeof(cli_addr);

    // Accept new connections and data until we kill the process
    while (1) {
        // The accept() system call causes the process to block until a client
        // connects to the server. Thus, it wakes up the process when a connection
        // from a client has been successfully established. It returns a new file
        // descriptor, and all communication on this connection should be done using
        // the new file descriptor. The second argument is a reference pointer to
        // the address of the client on the other end of the connection, and the
        // third argument is the size of this structure.
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
        }
        
        // After a connection is established, call fork() to create a new
        // process.
        int pid = fork();
        if (pid < 0) {
            error("ERROR on fork");
        }

        // The child process will close sockfd and call processConnection
        // passing the new socket file descriptor as an argument. When the two
        // processes have completed their conversation, as indicated by
        // processConnection returning, this process simply exits.
        if (pid == 0)  {
            close(sockfd);
            processConnection(newsockfd);
            exit(0);
        }

        // Otherwise, close the connection
        else {
            close(newsockfd);
        }

        // Reap the child process when it exits so we don't have zombie
        // processes floating around.
        waitpid(pid, 0, WNOHANG);
    }

    // Close the sockets
    close(sockfd);
    return 0; 
}