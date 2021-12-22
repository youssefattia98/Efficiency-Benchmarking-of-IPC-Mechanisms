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


int main() {
   int pfd[2];
   char senstr1[100];
   char senstr2[100];
   pipe(pfd); // creates a pipe
   int pid= fork();
   if (pid==0){
      //child
      sprintf(senstr1, "%d", pfd[0]);
      sprintf(senstr2, "%d",  pfd[1]);
      char * arg_list_1[] = {"./reciver",(char*) senstr1,(char*) senstr2,(char*)NULL};
      execvp (arg_list_1[0], arg_list_1);
   }
   else{
      //father
      sprintf(senstr1, "%d", pfd[0]);
      sprintf(senstr2, "%d",  pfd[1]);
      char * arg_list_2[] = {"./sender",(char*) senstr1,(char*) senstr2,(char*)NULL};
      execvp (arg_list_2[0], arg_list_2);
   }
   
   exit(0);
}