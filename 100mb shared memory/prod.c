/*
=============================================
gcc prod.c -o prod -lrt -pthread
gcc cons.c -o cons -lrt -pthread

- open two whell windows: run in one prod and run in the second cons, in that order
/*
1 shm_open() opening/creation of a shared memory segment referenced by a name a special file will appear in the file-system under “/dev/shm/” with the provided name The special file represents a POSIX object and it is created for persistence
2 ftruncate(…) resizes memory region to the correct size
3 mmap() mapping of the memory segment referenced by the file descriptor returned by shm_open()
4 munmap() unmapping of the memory segment
5 shm_unlink()
*/
//sem1 prod
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

#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"

char * ptr;
int i, j;
int x=0;

int size=104857600; //user should input th mbs this is 100 mbs
int buff =10485760;
FILE *log_file_poin;
void logfile(unsigned char* data){
  log_file_poin = fopen("./array","r");
  if (log_file_poin){
    //printf("log file successfully opened\n");
  }
  else{
    printf("Failed to open the file\n");
  }

  //fprintf(log_file_poin, "%s",data); 
  if ( log_file_poin )
   {
       while(fscanf(log_file_poin, "%s", data)!=EOF);
   }
   else
    {
         printf("Failed to open the file\n");
    }
  fclose(log_file_poin);
}

void datafiller(unsigned char* data){
  printf("filling...\n");
    for (int i = 0; i < size; i++){
        srand (time(NULL));
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

int main(int argc, char *argv[]) {
  unsigned char *data= (unsigned char*) malloc(size*sizeof(unsigned char));
  //datafiller(data);
  logfile(data);
  unsigned char *CD= (unsigned char*) malloc(buff*sizeof(unsigned char));
  //int shared_seg_size = (sizeof(shared_data)); //int having size of the shared memoery 
  int shared_seg_size = 10485760; //int having size of the shared memoery 
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


  

  CD=data;
  printf("the shared seg size is: %d\n",shared_seg_size);
  // repeat 3 times
  for(int i=0;i<104857600;i=i+shared_seg_size){
    printf("waiting for the consumer to send signal\n");
    sem_wait(sem_id1); //wait for a signal from the cons
    sem_wait(mutexCircBuffer);

    memcpy (ptr, CD, shared_seg_size);
    CD= CD+shared_seg_size;
    puts(ptr);
    printf("i am at ittraion: %d\n",(i/shared_seg_size));

    sem_post(mutexCircBuffer);
    sem_post(sem_id2);//send a signal to the consumer
    printf("i have sent the signal\n");
  }

  shm_unlink(SHMOBJ_PATH);
  sem_close(sem_id1);
  sem_close(sem_id2);
  sem_unlink(SEM_PATH_1);
  sem_unlink(SEM_PATH_2);
  munmap(ptr, shared_seg_size);

  return 0;
}
