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
    }
    else{
      cout << "Port"
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
