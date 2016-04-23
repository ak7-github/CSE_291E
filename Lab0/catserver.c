/* Code for Catserver: It takes the following arguments:
	File to be read on receving "LINE" from client 
	Port Number 
*/

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

/* Upper-case conversion routine*/

char* upper_case(char str[])
{
	int i = 0;
	char *str_cap=NULL;
	while(str[i]){
		str[i] = toupper(str[i]);
		i++;
	}
	return str;
}	  	

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     FILE *fp;
     char line[256];
     char *line_cap;
     bool flag_eof = false;

     printf("Inside Server Code\n");

     if (argc < 3) {
         fprintf(stderr, "Usage: %s <filename> <port no>\n", argv[0]);
         exit(1);
     }

     /* Socket to Send Capitalized Words to Client*/
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     memset((char *) &serv_addr, 0, sizeof(serv_addr));

     /* Populating Server Information */
     portno = atoi(argv[2]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     /*Server Binds and Listens */
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);

     /* Accepting Connections*/
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     
     /*Open the File for reading*/

     fp = fopen(argv[1], "r");
     if (fp == NULL)
	exit(EXIT_FAILURE);
     
     while(1){

	bzero(buffer,256);
	n = read(newsockfd,buffer,256);

	if (n < 0) 
		error("ERROR reading from socket");

	printf("Here is the message: %s",buffer);


     	if(!strcmp(buffer, "LINE\n")){
	
		if(fgets (line, 256, fp) == NULL){
			printf("eof");
			flag_eof = true;
			fseek(fp, 0L, SEEK_SET);
		} 

        if (flag_eof == true){
                fgets(line, 256, fp);
                flag_eof=false;
        }


 	printf("Server Read = %s", line);
	line_cap = upper_case(line);
	printf("Server Processed = %s", line_cap);
	

	if (feof(fp))
		fseek(fp, 0L, SEEK_SET);

	/* Send that Capitalized Line to the Client */
	n = write(newsockfd, line, sizeof(line));
     
	if (n < 0) 
		error("ERROR writing to socket");
     	}

	else
		exit(1);

     } /*Close after while loops is over i.e., after the client has sent 10 things */
     
     fclose(fp);
     return 0; 
}
