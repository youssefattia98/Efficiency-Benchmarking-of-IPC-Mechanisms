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

#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"

void error(char *msg, int fd){
    perror(msg);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    exit(0);
}


int Bsize, CDsize, choice =0;
int main(int argc, char * argv[]){
  Bsize = atoi(argv[1]);
  CDsize = atoi(argv[2]);
  choice = atoi(argv[3]);
  unsigned char *B= (unsigned char*) malloc(Bsize*sizeof(unsigned char)); //array of size 0 to 100 mb according to user input
  unsigned char *CD= (unsigned char*) malloc(CDsize*sizeof(unsigned char)); //array of size 0 to 10kb according to user input

  if(choice == 1){
    int pfd[2];
    pfd[0] = atoi(argv[4]);
    pfd[1] =atoi(argv[5]);
    close(pfd[1]); //close the write end of pipe
    /*Read*/
    for (int i = 0; i < Bsize; i=i+65536){
      int byt = read(pfd[0], &B[i], 65536);
    }
    printf("i recived %ld\n",strlen(B));
  }


  else if (choice == 2){
    //named pipes
    char * namedpipe = "/tmp/namedpipe";
    mkfifo(namedpipe, 0666); //create namepipe
    int fd = open(namedpipe,O_RDONLY); //open fifo as read only
    /*Read*/
     for (int i = 0; i < Bsize; i=i+65536){
      int byt = read(fd, &B[i], 65536);
     }
     printf("i recived %ld\n",strlen(B));
     close(fd); //close fifo
     unlink("/tmp/namedpipe"); //delete pipe

  }


  else if (choice == 3){
    //sockets
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    portno = 4000;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
      error("ERROR opening socket",sockfd);
    }
    char ipadd[]={'1','2','7','.','0','.','0','.','1','\0'};
    server = gethostbyname(ipadd);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
      error("ERROR connecting",sockfd);
    }
    char buffer[]={'H','E','L','L','O','\0'};
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0){
      error("ERROR writing to socket",sockfd);
    }
    for (int i = 0; i < Bsize; i=i+65536){
      int byt = read(sockfd, &B[i], 65536);
     }
    printf("i recived %ld\n",strlen(B));
    if (n < 0){
      error("ERROR reading from socket",sockfd);
    }
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
  }


  else if(choice == 4){
    //shared memory
    int shared_seg_size = CDsize;
    char *ptr;
    int j;
    int shmfd = shm_open(SHMOBJ_PATH, O_RDONLY, 0666); //open the shared memory as read only
    //dont need to resize it beacuse the writer did
    ptr = mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);  //mapping of the memory segment referenced by the file descriptor returned by shm_open()
    //copening the semphore that the producer created
    sem_t * sem_id1 = sem_open(SEM_PATH_1, 0);
    sem_t * sem_id2 = sem_open(SEM_PATH_2, 0);
    sem_t* mutexCircBuffer = sem_open(SEM_PATH_3, O_CREAT | O_RDWR, 0666,1);


    CD=B;
    while(strlen(B)<Bsize){
      // wait prod
      sem_wait(sem_id2); //wait for the signal form the other
      sem_wait(mutexCircBuffer);

      memcpy (CD, ptr, shared_seg_size);
      CD= CD+shared_seg_size;
      printf("I am reading...\n");

      sem_post(mutexCircBuffer);
      sem_post(sem_id1); //send signal after reading
    }
    printf("I have colled: %ld bytes\n",strlen(B));

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
  }
  return(0);
}