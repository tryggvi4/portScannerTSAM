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
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

void ProcessPacket(unsigned char*, int);

//Global variables
//For Debuging
int total=0, icmp=0, igmp=0, tcp=0, udp=0, others=0; //For counting packages

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};
 
/*
    Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}
 


int main(int argc, char *argv[])
{
	//BREYTUR
	//FYRIR recvfrom()
	//Tekið af binarytides.com/packet-sniffer-code-c-linux
	struct sockaddr saddr;
	int saddr_size;
	int data_size;
	unsigned char *receivedData = (unsigned char *)malloc(65536);
	

	//Tekið af annarri binarytides um TCP //TODO:FINNA HANA AFTUR
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;           // Socket address structure
	
    struct hostent *server;
    char ipAddress[30] = "45.33.32.156"; //IP FYRIR scanme.nmap.org
	char sourceIpAddress[30] = "192.168.1.2";
    char buffer[256];
    /*if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }*/


		


    portno = 80; //Set the port number

    //Listi af þeim portum sem verða skoðuð
    std::vector<int> ports{
	80,
      100,
      200,
      300,
      400,
      22
    };

	//Búa til ip og tcp header 



	//Segja kernel að nota custom header (sjá comment í for)
	//send packets á öll port
	//Hafa sniffer socket í sér þræði
	//Keyrir recvfrom í lúppu
	//analizar svörin
		//- Eru rétt flög 
		//- Er retur protocol
	//binarytides.com
	


    /*for(std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it){
      std::cout << ' ' << *it;
    }*/

    for(std::vector<int>::iterator port = ports.begin(); port != ports.end(); ++port){

	std::cout << *port << std::endl;
	//Wait for 0.5+random time
	//Setja inn srand() fyrir rauverulega random tölu, en ekki rand með seed 1
	srand(time(0));	      
	double x = ((rand() % 500) + 500) / 1000.0;
      std::cout << "Sleeping for " << x << " sec" << std::endl;
      sleep(1); //TODO:LAGA ÞETTA!

      //Til að fá socket
      //TCP SOCK_STREAM sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      //UDP SOCK_DGRAM = sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      //UDP server on skel, port nr. 31313
      //Raw access: SOCK_RAW = sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
      sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP); // Open Socket
      if (sockfd < 0){
        error("ERROR opening socket");
      }

	//Tekið af https://www.binarytides.com/raw-sockets-c-code-linux/
	//Buffer
	char datagram[4096], datagramrecv[4096], *data, *pseudogram;

	//Núll stilla
	memset(datagram, 0, 4096);
	//IP header
	struct iphdr *iph = (struct iphdr *) datagram;

	//TCP header
	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
	struct sockaddr_in sin;
	struct pseudo_header psh;

	//Data part
	data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
	strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	
	//some address resolution
	 sin.sin_family = AF_INET;
	 sin.sin_port = htons(*port);
	 sin.sin_addr.s_addr = inet_addr (ipAddress);

	//Fill in the IP header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
   	iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + sockfd;
   	iph->id = htonl (54321);
   	iph->frag_off = 0;
   	iph->ttl = 255;
   	iph->protocol = IPPROTO_TCP;
   	iph->check = 0;
   	iph->saddr = inet_addr(sourceIpAddress);
   	iph->daddr = sin.sin_addr.s_addr;

	//Ip checksum
	iph->check = csum ((unsigned short *) datagram, iph->tot_len);

	//TCP Header
	tcph->source = htons (1234);
	tcph->dest = htons (*port);
	std::cout << "tcph->dest " << ntohs(tcph->dest) << std::endl;
	tcph->seq = 0;
	tcph->ack_seq = 0;
	tcph->doff = 5;  //tcp header size
	tcph->fin=0;
	tcph->syn=1;
	tcph->rst=0;
	tcph->psh=0;
	tcph->ack=0;
	tcph->urg=0;
	tcph->window = htons (5840); /* maximum allowed window size */
	tcph->check = 0; //leave checksum 0 now, filled later by pseudo header
	tcph->urg_ptr = 0;

	//Now the TCP checksum
	psh.source_address = inet_addr( sourceIpAddress );
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data) );

	int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
	pseudogram = (char*)malloc(psize);

	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));

	tcph->check = csum( (unsigned short*) pseudogram , psize);

	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;

	if (setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(0);
	}
	
	//std::cout << "sin.sa_data: " << (struct sockaddr *)sin.sa_data << std::endl;
	if(sendto(sockfd,datagram,iph->tot_len,0,(struct sockaddr *) &sin, sizeof(sin))<0){
		error("sendto failed");
	}
	else{
		printf("Packet send. Length: %d \n" , iph->tot_len);
	}

	//TEKIÐ AF SNIFFER C LINUX BINARYTIDES
	while(1){
		saddr_size = sizeof(saddr);
		std::cout << "Before" << std::endl;	
		data_size = recvfrom(sockfd,receivedData,65536,0, &saddr, (socklen_t*)&saddr_size);
		std::cout << "After size: " << data_size << " saddr: " << saddr.sa_data << ", sa_family: " << saddr.sa_family << ", saddr_size: " << saddr_size << std::endl;	
		if(data_size<0){ 
			error("receive failed");
			break;
		}
		//std::cout << "Processing packet from port no: " << port << std::endl; 
		ProcessPacket(receivedData, data_size);
		break;
	}
	
	

	/*Gamall TCP kóði ekki fyrir RAW
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
      }*/
      close(sockfd);
    }

    return 0;
}


void ProcessPacket(unsigned char* buffer, int size){
	std::cout << "Processing!" << std::endl;	
	struct iphdr *iph = (struct iphdr *) buffer;
	++total;
	//std::cout << iph->protocol << std::endl;
	switch(iph->protocol){
		case 1:
			++icmp;
			break;
		case 2:
			++igmp;
			break;
		case 6:
			++tcp;
			break;
		case 17:
			++udp;
			break;
		default:
			++others;
			break;
	}
	printf("TCP : %d UDP : %d  ICMP : %d  IGMP : %d  Others : %d  Total : %d \n", tcp, udp, icmp, igmp, others, total);
	std::cout << "Processed!" << std::endl;
	
}
