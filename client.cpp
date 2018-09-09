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

//UDP server on skel, port nr. 31313

void ProcessPacket(unsigned char*, int);
void PrintData (unsigned char* , int);
int setSocket (int);
unsigned short csum(unsigned short*,int);
void error(const char*);

//Global variables
//For Debuging
int total=0, icmp=0, igmp=0, tcp=0, udp=0, others=0, i, j; //For counting packages

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

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

		


        //Listi af þeim portum sem verða skoðuð
	std::vector<int> ports{
		502,
1089,
1090,
1091,
1541,
1883,
2222,
3480,
4000,
4592,
5050,
5052,
5065,
5450,
10307,
10311,
10364,
10407,
10409,
10412,
10414,
10428,
10431,
10447,
10449,
12316,
12645,
12647,
13722,
11001,
12135,
13724,
13782,
14592,
18000,
19999,
20000,
34962,
34963,
34964,
34980,
38589,
38593,
38000,
38011,
38014,
38200,
38210,
38301,
38400,
38600,
38700,
38971,
39129,
39278,
44818,
45678,
47808,
50001,
50018,
50020,
50025,
50110,
55000,
55003,
55550,
56001,
62900,
62911,
62924,
62930,
62938,
62956,
62963,
62981,
62985,
62992,
63012,
63027,
63041,
63075,
63079,
63082,
63088,
63094,
65443
	};
	
	
	int protocol = 300;
	
	while(protocol < 0 || protocol > 3){
		std::cout << "What kind of scan do you want to do?" << std::endl;
		std::cout << "1. Syn-Ack, 2. TCP," << std::endl;
		std::cout << "Write a number: ";
		std::cin >> protocol;
		std::cout << protocol << std::endl;
		if(protocol == 1 || protocol == 2){ //Make it a range if we add more scans
			break;	
		}
		std::cout << "Please choose one of the available scans" << std::endl;
		sleep(1);
	}
	if(protocol == 1){
		std::cout << "You chose Syn-Ack \nit will start shortly" << std::endl;
		sleep(3);
	}else if(protocol == 2){
		std::cout << "You chose TCP \nit will start shortly" << std::endl;
		sleep(3);
	}

	
	for(std::vector<int>::iterator port = ports.begin(); port != ports.end(); ++port){
		//Wait for 0.5+random time
		//Setja inn srand() fyrir rauverulega random tölu, en ekki rand með seed 1
		srand(time(0));	      
		double x = ((rand() % 500) + 500);
		std::cout << "Sleeping for " << x << " sec" << std::endl;
		usleep(x); //TODO:LAGA ÞETTA!
		

		//Set the socket according the the protocol
		sockfd = setSocket(protocol);
		if (sockfd < 0){
			error("ERROR opening socket");
		}
		
		if(protocol == 1){//SYN-ACK
			//Headerkóði taken from https://www.binarytides.com/raw-sockets-c-code-linux/
			//Reaceivefrom og sendto taken from https://www.binarytides.com/packet-sniffer-code-c-linux
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
			iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data);
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
			tcph->seq = 0;
			tcph->ack_seq = 0;
			tcph->doff = 5;  //tcp header size
			tcph->fin=0;
			tcph->syn=1;
			tcph->rst=0;
			tcph->psh=0;
			tcph->ack=1;
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
			
			//Send a package with the ip header + tcp header and data to the ip address and port
			//With syn flag	
			if(sendto(sockfd,datagram,iph->tot_len,0,(struct sockaddr *) &sin, sizeof(sin))<0){
				error("sendto failed");
			}
			else{
				std::cout << "Packet send. Length: " << iph->tot_len << " to ip: " << ipAddress << " port:" << ntohs(tcph->dest) << std::endl;
			}

			//TEKIÐ AF SNIFFER C LINUX BINARYTIDES
			//Wait for a pakage with the Awk flag
			while(1){
				saddr_size = sizeof(saddr);
				//std::cout << "Before" << std::endl;	 //For debug
				data_size = recvfrom(sockfd,receivedData,65536,0, &saddr, (socklen_t*)&saddr_size);
				//std::cout << "After size: " << data_size << " saddr: " << saddr.sa_data << ", sa_family: " << saddr.sa_family << ", saddr_size: " << saddr_size << std::endl; //For debug		
				if(data_size<0){
					error("receive failed");
					break;
				}
				else if(data_size >0){

					for(int ii = 0; ii < data_size+200; ii++){
						std::cout << "receivedData: " << receivedData[ii] << std::endl;
					}
					std::cout << "Package received!" << std::endl;
					//std::cout << "Processing packet from port no: " << port << std::endl; 
					ProcessPacket(receivedData, data_size);
				}
				break;
			}
		}
		else if( protocol == 2){//TCP
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
			memcpy(	(char *)&serv_addr.sin_addr.s_addr,
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
				std::cout << "Connection to Address: " << ipAddress << " on Port: " << *port << " is open" << std::endl;
			}
			
		}else{//PROTOCOL NOT FOUND
			std::cout << "Something went wrong contact your administrator for further help";
		}			
	}
	close(sockfd);
	return 0;
}

//Taken from https://www.binarytides.com/packet-sniffer-code-c-linux
void ProcessPacket(unsigned char* buffer, int size){
	std::cout << "Processing!" << std::endl;
	unsigned short iphdrlen;	
	struct iphdr *iph = (struct iphdr *) buffer;
	iphdrlen = iph->ihl*4;
	struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen);

	std::cout << "tcph->seq: " << tcph->seq << std::endl;
	std::cout << "tcph->ack_seq: " << tcph->ack_seq << std::endl;
	std::cout << "tcph->doff: " << tcph->doff << std::endl;
	std::cout << "tcph->fin: " << tcph->fin << std::endl;
	std::cout << "tcph->syn: " << tcph->syn << std::endl;
	std::cout << "tcph->rst: " << tcph->rst << std::endl;
	std::cout << "tcph->psh: " << tcph->psh << std::endl;
	std::cout << "tcph->ack: " << tcph->ack << std::endl;
	std::cout << "tcph->urg: " << tcph->urg << std::endl;

	std::cout << "Ack flag: " << tcph->ack << std::endl;
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
			//PrintData(buffer, size);
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

int setSocket(int protocol){

	//Til að fá socket
	//TCP SOCK_STREAM sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//UDP SOCK_DGRAM = sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//Raw access: SOCK_RAW = sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(protocol == 1){ //SYN-ACK
		return socket(AF_INET, SOCK_RAW, IPPROTO_TCP); // Open Socket
	}
	else if(protocol == 2){ //TCP
		return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Open Socket
	}
	return -1;
}


void PrintData (unsigned char* data , int Size)
{
     
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            std::cout << "         " << std::endl;
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128){
                    std::cout << (unsigned char)data[j]; //if its a number or alphabet
		}
		else{
			std::cout << "."; //otherwise print a dot
		}
            }
            std::cout << "\n";
        } 
         
        if(i%16==0){
		std::cout << "   ";
	}
	std::cout << (unsigned int)data[i];
                 
        if( i==Size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++) std::cout << "   "; //extra spaces
             
            std::cout << "         ";
             
            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) std::cout << (unsigned char)data[j];
                else std::cout << ".";
            }
            std::cout << "\n";
        }
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
 
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
 


