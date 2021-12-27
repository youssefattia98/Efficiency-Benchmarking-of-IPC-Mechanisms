/*
compile with: gcc Master.c -o master -lm
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <signal.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <ctype.h>
#include <math.h>
int main(){
  char ABsizestr[5];
  char CBsizestr[5];
  char choicestr[5];
  char senstr1[50];
  char senstr2[50];
  char senstr3[50];
  char senstr4[50];
  char senstr5[50];
  int pfd[2];
  printf("Hello this is the master process of a game comparison between speed of data trasmission.\n");
  printf("Fist of all choose the amout of data to trasfer in MB.\n You can choose between 1 and 100 MB of data. Input your choice:");
  scanf("%s",ABsizestr);
  printf("Secondly choose the size of buffer in KB.\n You can choose between 1 and 10 KB of data. Input your choice:");
  scanf("%s",CBsizestr);
  printf("Interesting! Now the type of IPC you want to test:\n press 1: Unnamed Pipes \n press 2: Named Pipes \n press 3: Sockets \n press 4: Shared Memory\n Any other input will not be accepted. Input now:");
  scanf("%s",choicestr);
  int ABsizeMB = atoi(ABsizestr);
  int CBsizeKB = atoi(CBsizestr);
  int choice = atoi(choicestr);
  int ABsizebytes = 1048576 *ABsizeMB;
  long int CBsizebytes = pow((double)2,(double)CBsizeKB);
  printf("%d\n",ABsizebytes);
  printf("%ld\n",CBsizebytes);
  printf("%d\n",choice);

  pipe(pfd);
  int pid =fork();
  if(pid==0){
    //first child to run the prdoucer
    sprintf(senstr1, "%d", ABsizebytes);//from int to sting
    sprintf(senstr2, "%ld", CBsizebytes);//from int to sting
    sprintf(senstr3, "%d", choice);//from int to sting
    sprintf(senstr4, "%d", pfd[0]);
    sprintf(senstr5, "%d", pfd[1]);
    //with konsole
    //char * arg_list_1[] = { "/usr/bin/konsole","-e", "./prod",(char*)senstr1,(char*)senstr2,(char*)senstr3,(char*)senstr4,(char*)senstr5,(char*)NULL };
    //execvp ("/usr/bin/konsole", arg_list_1);
    //without consle
    char * arg_list_1[] = {"./prod",(char*)senstr1,(char*)senstr2,(char*)senstr3,(char*)senstr4,(char*)senstr5,(char*)NULL };
    execvp(arg_list_1[0],arg_list_1);

  }


  else{
    int pid2=fork();
    if(pid2==0){
      //the son will be the master
      printf("hello from the master\n");
    }
    else{
      //the faster should run the consumer
      sprintf(senstr1, "%d", ABsizebytes);//from int to sting
      sprintf(senstr2, "%ld", CBsizebytes);//from int to sting
      sprintf(senstr3, "%d", choice);//from int to sting
      sprintf(senstr4, "%d", pfd[0]);
      sprintf(senstr5, "%d", pfd[1]);
      //with konsole
      //char * arg_list_2[] = { "/usr/bin/konsole","-e", "./prod",(char*)senstr1,(char*)senstr2,(char*)senstr3,(char*)senstr4,(char*)senstr5,(char*)NULL };
      //execvp ("/usr/bin/konsole", arg_list_2);
      //without consle
      char * arg_list_2[] = {"./cons",(char*)senstr1,(char*)senstr2,(char*)senstr3,(char*)senstr4,(char*)senstr5,(char*)NULL };
      sleep(2);
      execvp(arg_list_2[0],arg_list_2);
    }
  }
  return(0);
}