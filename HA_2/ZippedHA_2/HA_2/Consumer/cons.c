/*
compile with: gcc cons.c -o cons -lrt -pthread
run with: ./cons 104857600 102400 3 4 5 
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
#include <sys/types.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <ctype.h>
#include <sys/time.h> 

#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"

int Bsize, CDsize, choice =0;

void writeinlog(char *str){
  /*
  Function to write in log file, how to use it:
  writeinlog("write ur comment here");
  or
  writeinlog("%s\n", stingtobewritten);
  */
  FILE *writeinlog;
  writeinlog = fopen(".././txtfiles/logfile","a");
  fprintf(writeinlog, "%s",str);//write to file
  fclose(writeinlog);//close file
}

double timediff(){

  FILE *readtimefd;
  readtimefd= fopen(".././txtfiles/t0","r+");//open and clear file
  char senstr1[50];
  while(fscanf(readtimefd, "%s", senstr1)!=EOF);
  double t0 = atof(senstr1);

  fclose(readtimefd);

  FILE *readtimefd2;
  readtimefd2= fopen(".././txtfiles/t1","r+");//open and clear file
  char senstr2[50];
  while(fscanf(readtimefd2, "%s", senstr2)!=EOF);
  double t1 = atof(senstr2);
  fclose(readtimefd2);

  return t1-t0;
}

void writetime(){

  FILE *wrtietimefd;
  wrtietimefd = fopen(".././txtfiles/t1","w");//open and clear file

  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  double t1 = ((double)(1000000*(current_time.tv_sec)))+current_time.tv_usec;
  

  fprintf(wrtietimefd, "%f",t1);//write to file
  fclose(wrtietimefd);//close file
}

void error(char *msg, int fd){
    perror(msg);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    exit(0);
}



int main(int argc, char * argv[]){
  Bsize = atoi(argv[1]);
  CDsize = atoi(argv[2]);
  choice = atoi(argv[3]);
  unsigned char *B= (unsigned char*) malloc(Bsize*sizeof(unsigned char)); //array of size 0 to 100 mb according to user input
  unsigned char *CD= (unsigned char*) malloc(CDsize*sizeof(unsigned char)); //array of size 0 to 10kb according to user input

  if(choice == 1){
    //unnamed pipes choice
    writeinlog("consumer : The user chose Unnamed pipes choice\n");
    int pfd[2];
    pfd[0] = atoi(argv[4]);
    pfd[1] =atoi(argv[5]);
    close(pfd[1]); //close the write end of pipe
    /*Read*/
    for (int i = 0; i < Bsize; i=i+65536){
      int byt = read(pfd[0], &B[i], 65536);
    }
    writetime();
    printf("I recived %ld bytes.\n",strlen(B));
    writeinlog("consumer : recived in the unnamed pipe\n");

  }


  else if (choice == 2){
    //named pipes
    writeinlog("Consumer : The user chose named pipes choice\n");
    char * namedpipe = "/tmp/namedpipe";
    mkfifo(namedpipe, 0666); //create namepipe
    writeinlog("Consumer : named pipes is created\n ");
    int fd = open(namedpipe,O_RDONLY); //open fifo as read only
    /*Read*/
    writeinlog("Consumer : fifo is opened as read only\n");
     for (int i = 0; i < Bsize; i=i+65536){
      int byt = read(fd, &B[i], 65536);
     }
     writetime();
     printf("I recived %ld bytes.\n",strlen(B));
     writeinlog("Consumer : recived from the fifo\n");
     close(fd); //close fifo
     unlink("/tmp/namedpipe"); //delete pipe
     writeinlog("Consumer : The named pipe is closed and deleted \n");


  }


  else if (choice == 3){
    //sockets choice
    writeinlog("Consumer : The user chose Sockets\n");
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    portno = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
      error("ERROR opening socket",sockfd);
      writeinlog("Consumer : error opening sockets\n");
    }
    writeinlog("Consumer : opening sockets successfully\n");
    char ipadd[]={'1','2','7','.','0','.','0','.','1','\0'};
    server = gethostbyname(ipadd);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
        writeinlog("Consumer : No such host is found\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    writeinlog("Consumer : Buffer values in socket are set to zero\n");
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
      error("ERROR connecting",sockfd);
      writeinlog("Consumer : error in connecting \n");
    }
    writeinlog("Consumer : connected succuessfully \n");
    char buffer[]={'H','E','L','L','O','\0'};
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0){
      error("ERROR writing to socket",sockfd);
      writeinlog("Consumer : error in writing to socket \n"); 
    }
    writeinlog("Consumer : written to the socket successfully\n "); 
    for (int i = 0; i < Bsize; i=i+65536){
      int byt = read(sockfd, &B[i], 65536);
     }
    writetime();
    printf("I recived %ld bytes.\n",strlen(B));
    if (n < 0){
      error("ERROR reading from socket",sockfd);
      writeinlog("Consumer : Error in reading from the socket \n"); 
    }
    writeinlog("Consumer : Read from the socket successfully \n"); 
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    writeinlog("Consumer : socket closed\n");
  }


  else if(choice == 4){
    //shared memory choice
    writeinlog("Consumer : The user chose shared memory\n");
    int shared_seg_size = CDsize;
    char *ptr;
    int shmfd = shm_open(SHMOBJ_PATH, O_RDONLY, 0666); //open the shared memory as read only
    //dont need to resize it beacuse the writer did
    ptr = mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);  //mapping of the memory segment referenced by the file descriptor returned by shm_open()
    //opening the semphore that the producer created
    sem_t * sem_id1 = sem_open(SEM_PATH_1, 0);
    sem_t * sem_id2 = sem_open(SEM_PATH_2, 0);
    sem_t* mutexCircBuffer = sem_open(SEM_PATH_3, O_CREAT | O_RDWR, 0666,1);
    writeinlog("Consumer : Semaphores in shared memory are opened\n");



    CD=B;
    while(strlen(B)<Bsize){
      // wait prod
      writeinlog("Consumer : waiting for the signal from producer\n");
      sem_wait(sem_id2); //wait for the signal form the other
      sem_wait(mutexCircBuffer);
      

      memcpy (CD, ptr, shared_seg_size);
      CD= CD+shared_seg_size;
      printf("I am reading...\n");
      writeinlog("Consumer : reading from producer in process\n");


      sem_post(mutexCircBuffer);
      sem_post(sem_id1); //send signal after reading
    }
    writetime();
    printf("I recived %ld bytes.\n",strlen(B));
    writeinlog("Consumer : Recived from producer\n");

    shm_unlink(SHMOBJ_PATH);
    sem_close(sem_id1);
    sem_close(sem_id2);
    sem_close(mutexCircBuffer);
    sem_unlink(SEM_PATH_1);
    sem_unlink(SEM_PATH_2);
    sem_unlink(SEM_PATH_3);
  }

  else{
    printf("Fatal error in choice\n");
    exit(1);
    writeinlog("Consumer : error in chosing method\n");
  }
  double speeds = (Bsize/(timediff()/1000000)/1048576);
  printf("The speed is: %0.2f Mb/sec\n",speeds);
  writeinlog("Consumer : The speed is meausred\n");

  return(0);
}