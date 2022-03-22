/*
compile with: gcc prod.c -o prod -lrt -pthread
run with: ./prod 104857600 102400 3 4 5
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
#include <ctype.h>
#include <sys/time.h> 

// Defining semaphores directory are used in shared memory //
#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"

int Asize, CDsize, choice =0;


// Function to write in logfile in with every action in the code //
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


// Function Caculate the time of each transfering  method
void writetime(){

  FILE *wrtietimefd;
  wrtietimefd = fopen(".././txtfiles/t0","w");//open and clear file

  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  double t0 = ((double)(1000000*(current_time.tv_sec)))+current_time.tv_usec;

  fprintf(wrtietimefd, "%f",t0);//write to file
  fclose(wrtietimefd);//close file
}

void Randomdata(unsigned char* data){
  FILE *Randomdatafd;
  Randomdatafd = fopen(".././txtfiles/array","r");

  if ( Randomdatafd ){
    int counter =0;
    for(data[counter++]=getc(Randomdatafd);data[counter]!=EOF && counter!=Asize;data[counter++]=getc(Randomdatafd));
  }
  else{
    printf("Failed to open the file\n");
  }
  
  fclose(Randomdatafd);
}

void error(char *msg, int fd){
    perror(msg);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    exit(0);
}



int main(int argc, char * argv[]){
  Asize = atoi(argv[1]);
  CDsize = atoi(argv[2]);
  choice = atoi(argv[3]);
  unsigned char *A= (unsigned char*) malloc(Asize*sizeof(unsigned char)); //array of size 0 to 100 mb according to user input
  unsigned char *CD= (unsigned char*) malloc(CDsize*sizeof(unsigned char)); //array of size 0 to 10kb according to user input
  Randomdata(A); //read random data into array A
  printf("I am the producer, will send:%ld bytes.\n",strlen(A));

  if(choice == 1){
    //unnamed pipes choice
    writeinlog("Producer : The user chose Unnamed pipes choice\n");
    int pfd[2];
    pfd[0] = atoi(argv[4]);
    pfd[1] = atoi(argv[5]);
    close(pfd[0]); //close the read side
    writetime();
    int bytes = write(pfd[1],A,strlen(A)); //write to pipe
    writeinlog("Producer : written in the unnamed pipe\n");

  }

  else if (choice == 2){
    //named pipes choice 
    writeinlog("Producer : The user chose named pipes choice\n");
    char * namedpipe = "/tmp/namedpipe"; 
    if(mkfifo(namedpipe, 0666)==-1){//create namepipe
      perror("faild to create FIFO.\n");
      exit(5);
    }
    writeinlog("Producer : named pipes is created \n");
    int fd = open(namedpipe, O_WRONLY); //open fifo as write only
    writeinlog("Producer : fifo is opened as write only\n");
    writetime();
    write(fd, A, strlen(A)); //write in fifo
    writeinlog("Producer : written in the fifo\n");
  }


  else if (choice == 3){
    //sockets choice
    writeinlog("Producer : The user chose Sockets\n");
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
      error("ERROR opening socket",sockfd);
      writeinlog("Producer : error opening sockets\n");
    }
    writeinlog("Producer : opening sockets successfully\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    writeinlog("Producer : Buffer values in socket are set to zero\n");
    portno = 8000;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
      error("ERROR on binding",sockfd);
      writeinlog("Producer : error binding a socket to an address\n");
    }
    writeinlog("Producer : Binding The socket to an address\n");
    listen(sockfd,5);
    writeinlog("Producer : The process is allowed to listen on the socket for connections\n");
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
      error("ERROR on accept",newsockfd);
    }
    writeinlog("Producer : the process is blocked until a client connects to the server\n");
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0){
      error("ERROR reading from socket",newsockfd);
      writeinlog("Producer : ERROR reading from socket\n");
    }
    writeinlog("Producer : reading from socket successfully\n");
    writetime();
    n = write(newsockfd,A,strlen(A));
    if (n < 0){
      error("ERROR writing to socket",newsockfd);
      writeinlog("Producer : ERROR writing from socket\n");
    }
    writeinlog("Producer : writing from socket sucessfully\n");

    shutdown(newsockfd, SHUT_RDWR);
    close(newsockfd); 
    writeinlog("Producer : socket closed\n");

  }


  else if(choice == 4){
    //shared memory choice
    writeinlog("Producer : The user chose shared memory\n");
    char * ptr;
    int shared_seg_size = CDsize; //int having size of the shared memoery 
    int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, 0666); //open and create the shared memory as read and write
    ftruncate(shmfd, shared_seg_size); //sets the size of the memory 
    ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0); //mapping of the memory segment referenced by the file descriptor returned by shm_open()
    //creates other two semaphores
    sem_t * sem_id1= sem_open(SEM_PATH_1, O_CREAT | O_RDWR, 0666, 1); 
    sem_t * sem_id2= sem_open(SEM_PATH_2, O_CREAT | O_RDWR, 0666, 1);
    sem_t* mutexCircBuffer= sem_open(SEM_PATH_3, O_CREAT | O_RDWR, 0666,1);
    //intlaizes these two sempahores
    sem_init(sem_id1, 1, 1); // initialized to 1
    sem_init(sem_id2, 1, 0); // initialized to 0
    writeinlog("Producer : Semaphores in shared memory are created and initialized \n");


    writetime();
    CD=A;
    for(int i=0;i<Asize;i=i+shared_seg_size){
      sem_wait(sem_id1); //wait for a signal from the cons
      sem_wait(mutexCircBuffer);

      memcpy (ptr, CD, shared_seg_size);
      CD= CD+shared_seg_size;
      printf("i am at iteraion: %d\n",(i/shared_seg_size));

      sem_post(mutexCircBuffer);
      sem_post(sem_id2);//send a signal to the consumer
      writeinlog("Producer : The signal sent to the consumer\n");

    }

    shm_unlink(SHMOBJ_PATH);
    sem_close(sem_id1);
    sem_close(sem_id2);
    sem_close(mutexCircBuffer);
    sem_unlink(SEM_PATH_1);
    sem_unlink(SEM_PATH_2);
    sem_unlink(SEM_PATH_3);
    munmap(ptr, shared_seg_size);
  }

  else{
    printf("Fatal error in choice\n");
    exit(1);
    writeinlog("Producer : error in chosing method\n");

  }
  
  return(0);
}