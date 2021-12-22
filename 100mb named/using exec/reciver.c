/* this example shows how unnamed pipes may be used */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
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

int size=104857600; //user should input th mbs this is 100 mbs


int main(int argc, char * argv[]) {
   unsigned char *rec= (unsigned char*) malloc(size*sizeof(unsigned char));
   int pfd[2];
   pfd[0] = atoi(argv[1]);
   pfd[1] =atoi(argv[2]);
   close(pfd[1]); /* close the write end of pipe */
   for (int i = 0; i < size; i=i+65536){
      int byt = read(pfd[0], &rec[i], 65536);
   }
   int bytes = strlen(rec);
   printf("i recived %d\n",bytes);
   //printf("%s",rec);
   return(0);
}