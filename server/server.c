//***********************************************************************
// server.c
// CSCE 3530 Fall 2017 UNT
// C program: Server allows connection from client & receives url input
// 	      from client. Server gets ip address of URL & connects
//            to webserver. Server then sends get request & receives
//	      webpage & stores it in buffer before sending it to the 
// 	      client.  
// by Miguel Melendez 10/04/17
//**********************************************************************



#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h> 

#define h_addr h_addr_list[0]

int main(int argc, char *argv[])
{
	char recvline[200];
    	int sockfd, newsockfd, port_num, cli_size;
   	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// websever declarations
	char* web_ip;
	int websocket, m;

	struct sockaddr_in web_servaddr;
	char buffer[40000];
	
	struct hostent *he;
	char request[] = "GET /1.1\r\n";

	if(argc < 2)
	{
		printf("ERROR: Usage format: ./a.out <port_number>\n");
		return 0;
	}

    	/* AF_INET - IPv4 IP , Type of socket, protocol*/
   	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	if(sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}
	
	port_num = atoi(argv[1]);
   	bzero(&serv_addr, sizeof(serv_addr)); 
   	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    	serv_addr.sin_port = htons(port_num);
 
    	/* Binds the above details to the socket */
	if(bind(sockfd,  (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR binding");
		exit(1);
	}

	/* Start listening to incoming connections */
	listen(sockfd, 10);
	cli_size = sizeof(cli_addr);

    	while(1)
    	{
      		/* Accepts an incoming connection */
	  	newsockfd = accept(sockfd,  (struct sockaddr*)&cli_addr, &cli_size);
		if(newsockfd < 0)
		{
			perror("ERROR on accepting");
			exit(1);
		}
		
		/* Reads incoming message from client */
		bzero(recvline, sizeof(recvline));	
		n = read(newsockfd, recvline, sizeof(recvline));
		if(n < 0)
		{
			perror("ERROR reading");
			exit(1);	
		}
		
		printf("%s\n", recvline);		
		
		/* Gets host IP Address */
		he = gethostbyname(recvline);
		if(he == NULL)
		{
			perror("ERROR gethostbyname");
			exit(1);
		}

		web_ip = (inet_ntoa(*(struct in_addr*)(he->h_addr_list[0])));	// get ip address 
		printf("we_ip: %s\n", web_ip);
		printf("ip: %s\n", inet_ntoa(*(struct in_addr*)(he->h_addr_list[0])));	
			

		printf("Name: %s\n", he->h_name);

	
		/************ create Web socket *************************/
		websocket = socket(AF_INET, SOCK_STREAM, 0);
		if(websocket < 0)
		{
			perror("ERROR opening websocket");
			exit(1);
		}
		
		bzero(&web_servaddr, sizeof(web_servaddr));
		web_servaddr.sin_family=AF_INET;
		web_servaddr.sin_port=htons(80);

		// covert IPv4 and IPv6 addresses from text to binary form  
		inet_pton(AF_INET, web_ip, &(web_servaddr.sin_addr));
		
		// connect to webserver
		if(connect(websocket, (struct web_sockaddr *)&web_servaddr, sizeof(web_servaddr)) < 0)
		{
			perror("ERROR connecting");
			exit(1);
		}
		
		// send request to webserver
		m = write(websocket, request, strlen(request));
		if(m < 0)
		{
			perror("ERROR writing to sever");
			exit(1);
		}				
			
	
		bzero(buffer, 40000); // clear buffer
		
		// reading from webserver
		while(read(websocket, buffer, 39999) != 0)
		{
			// send buffer to client
			m = write(newsockfd, buffer, strlen(buffer));
			if(m < 0)
			{
				perror("ERROR writing to client");
				exit(1);
			}
			bzero(buffer, 40000); // clear buffer
		}

		close(websocket);

		/************* end WEB socket *********************/
				

      		close (newsockfd); //close the connection
   	 }
}
