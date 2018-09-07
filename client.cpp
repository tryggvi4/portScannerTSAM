#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <errno.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;           // Socket address structure
    struct hostent *server;

    char buffer[256];
    /*if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }*/

    portno = 21;     // Read Port No from command line

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Open Socket
    std::cout << "Sockfd: " << sockfd << std::endl;
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname("45.33.32.156");        // Get host from IP

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; // This is always set to AF_INET

    // Host address is stored in network byte order
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);


    //for(int i = 0; i < 2000){


    //}
    serv_addr.sin_port = htons(portno);



    // Read and write to socket
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    //n = write(sockfd,buffer,strlen(buffer));

    //if (n < 0)
    //     error("ERROR writing to socket");



    //if (send(sockfd,buffer,sizeof(buffer),0) < 0)
    //    error("ERROR connecting");
    std::cout << "Sockfd: " << sockfd << std::endl << "Buffer: " << buffer << std::endl;
    std::cout << "Error:" <<  strerror(errno) << std::endl;
    long j = send(sockfd,buffer,sizeof(buffer),0);
    std::cout << j << "Error:" <<  strerror(errno) << std::endl;



    /*bzero(buffer,256);
    //n = read(sockfd,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");*/
    //printf("%ld", j);
    close(sockfd);
    return 0;
}
