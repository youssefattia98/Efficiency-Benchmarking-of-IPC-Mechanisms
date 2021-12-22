/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c -lsocket
*/
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#include <fcntl.h> 
#include <sys/stat.h> 
#include <ctype.h>
#include <time.h>

int size=104857600; //user should input th mbs this is 100 mbs
int main(int argc, char *argv[]){
     
     unsigned char *rec= (unsigned char*) malloc(size*sizeof(unsigned char));
     //nsigned char rec[10000000];
     int fd;
     char * namedpipe = "/tmp/namedpipe";
     mkfifo(namedpipe, 0666); //create namepipe


     fd = open(namedpipe,O_RDONLY); //open fifo as read only
     //reading part
     for (int i = 0; i < size; i=i+65536){
         int byt = read(fd, &rec[i], 65536);
     }

     int bytes = strlen(rec);
     printf("i recived %d\n",bytes);


     close(fd); //close fifo
     unlink("/tmp/namedpipe");
     return 0; /* we never get here */
}

