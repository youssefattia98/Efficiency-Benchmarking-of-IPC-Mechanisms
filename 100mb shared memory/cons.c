/*
=============================================
gcc prod.c -o prod -lrt -pthread
gcc cons.c -o cons -lrt -pthread

- open two whell windows: run in one prod and run in the second cons, in that order
- running cons first generates a crash (try it).
*/

/*
1 shm_open() opening/creation of a shared memory segment referenced by a name a special file will appear in the file-system under “/dev/shm/” with the provided name The special file represents a POSIX object and it is created for persistence
2 ftruncate(…) resizes memory region to the correct size
3 mmap() mapping of the memory segment referenced by the file descriptor returned by shm_open()
4 munmap() unmapping of the memory segment
5 shm_unlink()
*/

// sem2 cons - run AFTER prod
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"
#define SEM_PATH_3 "/sem_AOS_3"
FILE *reciveddatafd;
void recievddata(unsigned char* data){
  reciveddatafd = fopen("./revieddata","a");
  if (reciveddatafd){
    //printf("log file successfully opened\n");
  }
  else{
    printf("Failed to open the file\n");
  }

  fprintf(reciveddatafd, "%s",data); 
  fclose(reciveddatafd);
}

int size=104857600; //Aand B
int buff=10485760; //CD
int main(int argc, char *argv[]) {
  
  unsigned char *CD= (unsigned char*) malloc(buff*sizeof(unsigned char));
  unsigned char *rec= (unsigned char*) malloc(size*sizeof(unsigned char));
  int shared_seg_size = 10485760;
  char *ptr;
  int j;
  int shmfd = shm_open(SHMOBJ_PATH, O_RDONLY, 0666); //open the shared memory as read only
  //dont need to resize it beacuse the writer did
  ptr = mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);  //mapping of the memory segment referenced by the file descriptor returned by shm_open()
  //copening the semphore that the producer created
  sem_t * sem_id1 = sem_open(SEM_PATH_1, 0);
  sem_t * sem_id2 = sem_open(SEM_PATH_2, 0);
  sem_t* mutexCircBuffer = sem_open(SEM_PATH_3, O_CREAT | O_RDWR, 0666,1);


  CD=rec;
  // repeat 3 times
  while(strlen(rec)<size){
    // wait prod
    sem_wait(sem_id2); //wait for the signal form the other
    sem_wait(mutexCircBuffer);

    memcpy (CD, ptr, shared_seg_size);
    CD= CD+shared_seg_size;

    puts(ptr);
    recievddata(ptr);
    printf("the string size: %ld\n",strlen(ptr));

    sem_post(mutexCircBuffer);
    sem_post(sem_id1); //send signal after reading
  }
  printf("i have colled: %ld bytes\n",strlen(rec));

  shm_unlink(SHMOBJ_PATH);
  sem_close(sem_id1);
  sem_close(sem_id2);
  sem_unlink(SEM_PATH_1);
  sem_unlink(SEM_PATH_2);

  return 0;
}
