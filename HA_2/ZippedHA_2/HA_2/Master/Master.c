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



float DELAY=10000;
void textSpawn(char* string, float delay){
  char str_length=strlen(string);
  for(int i=0; string[i]!= '\0'; i++){
      printf("%c",string[i]);
      fflush(stdout);
      usleep(delay);
  }
}

void writeinlog(char *str){
  /*
  Function to write in log file.
  */
  FILE *writeinlog;
  writeinlog = fopen(".././txtfiles/logfile","a");
  fprintf(writeinlog, "%s",str);//write to file
  fclose(writeinlog);//close file
}

int main(){
  char senstr1[50];
  char senstr2[50];
  char senstr3[50];
  char senstr4[50];
  char senstr5[50];
  int pfd[2];

  int ABsizeMB = 200;
  int CBsizeKB = 20;
  int choice = 10;
  
  textSpawn("Hello this is the master process of a game comparison between speed of data transmission.\n",DELAY);
  textSpawn("First of all choose the amout of data to transfer from 1 to 100 MB.Input your choice:",DELAY);
  scanf("%d",&ABsizeMB);
  while (!( (1<=ABsizeMB) && (ABsizeMB<=100) ))  {
    textSpawn("Wrong input please enter a number between 0 to 100 MB \n",DELAY);
    scanf("%d",&ABsizeMB);
  }
  writeinlog("User entered the data to be transfered\n  ");

  textSpawn("Secondly choose the size of buffer in KB.\nYou can choose between 1 and 10 KB of data. Input your choice:",DELAY);
  scanf("%d",&CBsizeKB);
  while (!( (1<=CBsizeKB) && (CBsizeKB<=10) ))  {
    textSpawn("Wrong input please enter a number between 0 to 10 KB \n",DELAY);
    scanf("%d",&CBsizeKB);
  }
  writeinlog("User entered the buffer size\n   ");

  textSpawn("Interesting! Now the type of IPC you want to test:\n",DELAY);
  printf("press 1: Unnamed Pipes \n");
  usleep(DELAY*4);
  printf("press 2: Named Pipes \n");
  usleep(DELAY*4);
  printf("press 3: Sockets \n");
  usleep(DELAY*4);
  printf("press 4: Shared Memory\n");
  textSpawn("Any other input will not be accepted. Input now:", DELAY);
  scanf("%d",&choice);
  while (!( (0<choice) && (choice<=4) ))  {
    textSpawn("Wrong input please enter a number between 0 to 4 \n",DELAY);
    scanf("%d",&choice);
  }
    writeinlog("User entered the choice number \n ");

  
  
  int ABsizebytes = 1048576 *ABsizeMB;
  long int CBsizebytes = pow((double)2,(double)CBsizeKB);
  

  if(pipe(pfd)==-1){
    perror("Failed to create pipe.\n");
    exit(1);
  }
  int pid =fork();
  if (pid==-1){
    perror("failed to fork.\n");
    exit(2);
  }
  if(pid==0){
    //first child to run the prdoucer
    writeinlog("producer run\n");
    sprintf(senstr1, "%d", ABsizebytes);//from int to sting
    sprintf(senstr2, "%ld", CBsizebytes);//from int to sting
    sprintf(senstr3, "%d", choice);//from int to sting
    sprintf(senstr4, "%d", pfd[0]);
    sprintf(senstr5, "%d", pfd[1]);
    char * arg_list_1[] = {"../Producer/prod",(char*)senstr1,(char*)senstr2,(char*)senstr3,(char*)senstr4,(char*)senstr5,(char*)NULL };
    if(execvp(arg_list_1[0],arg_list_1)==1){
      perror("faild to exec producer process.\n");
      exit(3);
    }
  }

  else{
      //the father should run the consumer
      writeinlog("Consumer run\n");
      sprintf(senstr1, "%d", ABsizebytes);//from int to sting
      sprintf(senstr2, "%ld", CBsizebytes);//from int to sting
      sprintf(senstr3, "%d", choice);//from int to sting
      sprintf(senstr4, "%d", pfd[0]);
      sprintf(senstr5, "%d", pfd[1]);
      char * arg_list_2[] = {"../Consumer/cons",(char*)senstr1,(char*)senstr2,(char*)senstr3,(char*)senstr4,(char*)senstr5,(char*)NULL };
      sleep(1);
      if(execvp(arg_list_2[0],arg_list_2)==-1){
      perror("faild to exec consumer process.\n");
      exit(4);
      }
  }
  return(0);
}