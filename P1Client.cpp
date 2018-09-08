#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <netinet.h>
#include <netdb.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>


// ATH  *** Client kóði *** gott að byrja með það

/*from file
struct sockaddr {
    sa_familt sa_family;
    char      sa_data[14];
};*/

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}



int main(int argc, char *argv[])
{
    vector<int> myports; //= {80,21,17,1,3,120,22};
    for(unsigned int i = 1; i < 1025; i++)
    {
        myports.push_back(i);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;           // Socket address structure
    struct hostent *server;


    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    for(portno = 1; portno < 1024; portno++)
    {

    //portno = 80;
    //portno = atoi(argv[2]);     // Read Port No from command line

    //sockfd = socket(AF_INET, SOCK_STREAM, 0); // Open Socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // for TCP
    //sock_fd = socket(AF_INET, SOCK_DGRAM, IPPOROTO_UDP) // for UDP
    //sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)    // for IP

    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);        // Get host from IP

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

    /*memcpy((char *)&serv_addr.sin_addr.s_addr,
         (char *)&server->h_addr ,
         server->h_length);*/


    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
        // close Port
    }
    else
    {
        cout << "Open Port" << endl;
    }

    random_shuffle(myports.begin(), myports.end());

    }


    // Read and write to socket
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));

    if (n < 0)
         error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}

/*
| Port  | Description                           |
+-------+---------------------------------------+
| 502   | Modbus                                |
| 1089  | Foundation Fieldbus HSE               |
| 1090  | Foundation Fieldbus HSE               |
| 1091  | Foundation Fieldbus HSE               |
| 1541  | Foxboro/Invensys Foxboro DCS Informix |
| 1883  | MQ Telemetry Transport                |
| 2222  | EtherNet/IP                           |
| 3480  | OPC UA Discovery Server               |
| 4000  | Emerson/Fisher ROC Plus               |
| 4592  | Project/SCADA Node Primary Port       |
| 5050  | Telvent OASyS DNA                     |
| 5052  | Telvent OASyS DNA                     |
| 5065  | Telvent OASyS DNA                     |
| 5450  | OSIsoft PI Server                     |
| 10307 | ABB Ranger 2003                       |
| 10311 | ABB Ranger 2003                       |
| 10364 | ABB Ranger 2003                       |
| 10407 | ABB Ranger 2003                       |
| 10409 | ABB Ranger 2003                       |
| 10412 | ABB Ranger 2003                       |
| 10414 | ABB Ranger 2003                       |
| 10428 | ABB Ranger 2003                       |
| 10431 | ABB Ranger 2003                       |
| 10447 | ABB Ranger 2003                       |
| 10449 | ABB Ranger 2003                       |
| 12316 | ABB Ranger 2003                       |
| 12645 | ABB Ranger 2003                       |
| 12647 | ABB Ranger 2003                       |
| 13722 | ABB Ranger 2003                       |
| 11001 | Johnson Controls Metasys N1           |
| 12135 | Telvent OASyS DNA                     |
| 13724 | ABB Ranger 2003                       |
| 13782 | ABB Ranger 2003                       |
| 14592 | SCADA Node Secondary Port             |
| 18000 | Iconic Genesis32 GenBroker (TCP)      |
| 19999 | DNP                                   |
| 20000 | DNP3                                  |
| 34962 | PROFINET                              |
| 34963 | PROFINET                              |
| 34964 | PROFINET                              |
| 34980 | EtherCAT                              |
| 38589 | ABB Ranger 2003                       |
| 38593 | ABB Ranger 2003                       |
| 38000 | SNC GENe                              |
| 38011 | SNC GENe                              |
| 38014 | SNC GENe                              |
| 38200 | SNC GENe                              |
| 38210 | SNC GENe                              |
| 38301 | SNC GENe                              |
| 38400 | SNC GENe                              |
| 38600 | ABB Ranger 2003                       |
| 38700 | SNC GENe                              |
| 38971 | ABB Ranger 2003                       |
| 39129 | ABB Ranger 2003                       |
| 39278 | ABB Ranger 2003                       |
| 44818 | EtherNet/IP                           |
| 45678 | Foxboro/Invensys Foxboro DCS AIMAPI   |
| 47808 | BACnet/IP                             |
| 50001 | Siemens Spectrum Power TG             |
| 50018 | Siemens Spectrum Power TG             |
| 50020 | Siemens Spectrum Power TG             |
| 50025 | Siemens Spectrum Power TG             |
| 50110 | Siemens Spectrum Power TG             |
| 55000 | FL-net Reception                      |
| 55003 | FL-net Transmission                   |
| 55550 | Foxboor/Invensys Foxboro DCS FoxAPI   |
| 56001 | Telvent OASyS DNA                     |
| 62900 | SNC GENe                              |
| 62911 | SNC GENe                              |
| 62924 | SNC GENe                              |
| 62930 | SNC GENe                              |
| 62938 | SNC GENe                              |
| 62956 | SNC GENe                              |
| 62963 | SNC GENe                              |
| 62981 | SNC GENe                              |
| 62985 | SNC GENe                              |
| 62992 | SNC GENe                              |
| 63012 | SNC GENe                              |
| 63027 | SNC GENe                              |
| 63041 | SNC GENe                              |
| 63075 | SNC GENe                              |
| 63079 | SNC GENe                              |
| 63082 | SNC GENe                              |
| 63088 | SNC GENe                              |
| 63094 | SNC GENe                              |
| 65443 | SNC GENe
*/
