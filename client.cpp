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
#include <vector>
#include <time.h>

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
    char ipAddress[30] = "45.33.32.156"; //IP FYRIR scanme.nmap.org
    char buffer[256];
    /*if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }*/

    portno = 80; //Set the port number

    //Listi af þeim portum sem verða skoðuð
    std::vector<int> ports{
      100,
      200,
      300,
      400,
      22,
      80
    };



    /*for(std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it){
      std::cout << ' ' << *it;
    }*/

    for(std::vector<int>::iterator port = ports.begin(); port != ports.end(); ++port){
      //Wait for 0.5+random time
      double x = ((double) rand())/ ((double)RAND_MAX) / 2.0 + 0.5;
      //std::cout << "Sleeping for " << x << " sec" << std::endl;
      sleep(x);
      //Til að fá socket
      //TCP SOCK_STREAM sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      //UDP SOCK_DGRAM = sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      //UDP server on skel, port nr. 31313
      //Raw access: SOCK_RAW = sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
      sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Open Socket
      if (sockfd < 0){
        error("ERROR opening socket");
      }

      //Get server
      server = gethostbyname("45.33.32.156");        // Get host from IP
      if (server == NULL) {
          fprintf(stderr,"ERROR, no such host\n");
          exit(0);
      }

      //Empty the server address struct
      bzero((char *) &serv_addr, sizeof(serv_addr));

      serv_addr.sin_family = AF_INET; // This is always set to AF_INET

      // Host address is stored in network byte order
      memcpy((char *)&serv_addr.sin_addr.s_addr,
             (char *)server->h_addr,
            server->h_length);

      //Set the port number
      serv_addr.sin_port = htons(*port); //Port númerið
      // Connect to the ip address and ip and return if connection is a success or not
      if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
        std::cout << strerror(errno) << " when connecting to port no: " << *port << std::endl;
      }
      else{
        std::cout << "else" << std::endl;
        std::cout << "Connection to Address: " << ipAddress << " on Port: " << *port << " is open: Error: " << strerror(errno)<< std::endl;
      }
      close(sockfd);
    }

    return 0;
}
