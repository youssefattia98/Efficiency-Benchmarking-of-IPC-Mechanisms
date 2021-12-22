#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int size=104857600; //user should input th mbs this is 1 mbs


void datafiller(unsigned char* data){
    for (int i = 0; i < size; i++){
        srand ( time(NULL) );
        int val = rand()%2;
        if (val ==0){
            data[i]='0';
        }
        else{
            data[i]='1';
        }
    }
    printf("filled the array\n");
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    
    int sockfd, portno, n;
    char ipadd[]={'1','2','7','.','0','.','0','.','1','\0'};
    unsigned char *data= (unsigned char*) malloc(size*sizeof(unsigned char));
    datafiller(data);

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    portno = 6969;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(ipadd);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256); //clears the buffer
    fgets(buffer,255,stdin); //scnas user input to the buffer
    n = write(sockfd,data,strlen(data)); //writes the buffer
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    return 0;
}
