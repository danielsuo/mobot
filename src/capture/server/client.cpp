// http://www.linuxhowtos.org/C_C++/socket.htm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>

struct timeval tp1;
struct timeval tp2;
struct timeval tp3;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    unsigned long remote_timestamp;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    while(1) {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        gettimeofday(&tp1, NULL);
        unsigned long ms1 = tp1.tv_sec * 1000 + tp1.tv_usec / 1000;

        n = write(sockfd, &ms1, sizeof(unsigned long));
        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
        n = read(sockfd, &remote_timestamp,255);
        if (n < 0) 
             error("ERROR reading from socket");
        gettimeofday(&tp2, NULL);
        printf("%ld\n", remote_timestamp);

        unsigned long ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000;
        unsigned long ms3 = tp3.tv_sec * 1000 + tp3.tv_usec / 1000;
        unsigned long ms = ms2 - ms1;

        printf("%ld time diff (device - server)\n", remote_timestamp - (ms1 + ms / 2));
        printf("%ld ms elapsed\n", ms);
        printf("ms1: %ld\nms2: %ld\nms3: %ld\n", ms1, ms2, ms3);
    }
    close(sockfd);
    return 0;
}