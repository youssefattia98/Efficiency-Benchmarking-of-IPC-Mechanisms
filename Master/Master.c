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



float DELAY=30000;
void textSpawn(char* string, float delay){
  char str_length=strlen(string);
  //printf("you should print %s", string);
  for(int i=0; string[i]!= '\0'; i++){
    //printf("you should print %d", string[i]);
      printf("%c",string[i]);
      fflush(stdout);
      usleep(delay);
  }
}

void writeinlog(char *str){
  /*
  Function to write in log file, how to use it:
  writeinlog("write ur comment here");
  or
  writeinlog("%s\n", stingtobewritten);
  */
  FILE *writeinlog;
  writeinlog = fopen("./logfile","a");
  fprintf(writeinlog, "%s",str);//write to file
  fclose(writeinlog);//close file
}

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
  
  textSpawn("Hello this is the master process of a game comparison between speed of data trasmission.\n",DELAY);
  textSpawn("Fist of all choose the amout of data to trasfer from 0 to 100 MB.Input your choice:",DELAY);
  scanf("%s",ABsizestr);
  textSpawn("Secondly choose the size of buffer in KB.\nYou can choose between 1 and 10 KB of data. Input your choice:",DELAY);
  scanf("%s",CBsizestr);
  textSpawn("Interesting! Now the type of IPC you want to test:\n",DELAY);
  printf("press 1: Unnamed Pipes \n");
  usleep(DELAY*4);
  printf("press 2: Named Pipes \n");
  usleep(DELAY*4);
  printf("press 3: Sockets \n");
  usleep(DELAY*4);
  printf("press 4: Shared Memory\n");
  textSpawn("Any other input will not be accepted. Input now:", DELAY);
  scanf("%s",choicestr);
  
  int ABsizeMB = atoi(ABsizestr);
  int CBsizeKB = atoi(CBsizestr);
  int choice = atoi(choicestr);
  int ABsizebytes = 1048576 *ABsizeMB;
  long int CBsizebytes = pow((double)2,(double)CBsizeKB);
  //printf("%d\n",ABsizebytes);
  //printf("%ld\n",CBsizebytes);
  //printf("%d\n",choice);

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
      sleep(1);
      execvp(arg_list_2[0],arg_list_2);

  }
  return(0);
}