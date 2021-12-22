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

int size=1048576; //user should input th mbs this is 100 mbs


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

int main() {
   unsigned char *data= (unsigned char*) malloc(size*sizeof(unsigned char));
   unsigned char *rec= (unsigned char*) malloc(size*sizeof(unsigned char));
   datafiller(data);
   int ret_val;
   int pfd[2];
   char buff[32];
   char string1[]="String for pipe I/O";

   ret_val = pipe(pfd);                 /* Create pipe */
   //printf("before forking: pfd[0]=%d\n",pfd[0]);
   //printf("before forking: pfd[1]=%d\n",pfd[1]);
   if (ret_val != 0){             /* Test for success */
      printf("Unable to create a pipe; errno=%d\n",errno);
      exit(1);                         /* Print error message and exit */
   }


   if (fork() == 0) {
      /* child program */
      //printf("AFTER forking,CHILD: pfd[0]=%d\n",pfd[0]);
      //printf("AFTER forking,CHILD: pfd[1]=%d\n",pfd[1]);
      close(pfd[0]); /* close the read end */
      ret_val = write(pfd[1],data,strlen(data)); /*Write to pipe*/
      if (ret_val != strlen(data)){
         printf("Write did not return expected value\n");
         exit(2);                       /* Print error message and exit */
      }
   }



   else {
      /* parent program */
      //printf("AFTER forking,PARENT: pfd[0]=%d\n",pfd[0]);
      //printf("AFTER forking,PARENT: pfd[1]=%d\n",pfd[1]);
      close(pfd[1]); /* close the write end of pipe */
      //ret_val = read(pfd[0],buff,strlen(string1)); /* Read from pipe */
      for (int i = 0; i < size; i=i+65536){
         int byt = read(pfd[0], &rec[i], 65536);
      }
      int bytes = strlen(rec);
      printf("i recived %d\n",bytes);
      //printf("%s",rec);
      //printf("parent read %s from the child program\n",buff);
   }
   
   exit(0);
}