/* CATCLIENT <hostname> <server port number> <path to file>*/ 
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

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
    int sockfd, portno, n;
    int x = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char line[256];
    char buffer[256];
    FILE *fp;
    char *line_cap;
    bool flag_eof = false;

    printf("Inside Client Code\n");

    if (argc < 3) {
       fprintf(stderr,"Usage: %s <hostname of server> <port number of server>\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    /*sockfd is the socket used by the client to talk to the server*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    /*Setting fields of serv_addr*/
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    memset(buffer, 0, 256);
   
    fp = fopen(argv[3], "r");
    if (fp == NULL)
    	exit(EXIT_FAILURE);
    
    while (x < 10) {
	strcpy(buffer, "LINE\n");
 
	/*Send LINE followed  by newline to server*/
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
		error("ERROR writing to socket");
	printf("Sent %s", buffer);
	
	/*Clear the buffer for receiving bytes*/
	memset(buffer, 0, 256);
  	n = read(sockfd, buffer, 256);
    	if (n < 0) 
		error("ERROR reading from socket");
	printf("Read from Server = %s",buffer);


	/*Read the file strings.txt line by line*/
	if(fgets(line, 256, fp) == NULL) {
		printf("eof");
		flag_eof=true;
		fseek(fp, 0L, SEEK_SET);
	}
	
	if (flag_eof == true){
		fgets(line, 256, fp);
		flag_eof=false;
	}

	printf("Read from File = %s",line);
	upper_case(line);
	printf("After Capitalizing = %s", line);
	if (strcmp(line, buffer) == 0)
		printf("OK\n\n");
	else
		printf("Missing\n\n");

	memset(buffer, 0, 256);

	/*3 second sleep */
	sleep(3);  

	x++;
    }//end of while  
    return 0;
}
