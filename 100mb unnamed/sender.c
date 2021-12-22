#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <fcntl.h> 
#include <sys/stat.h> 
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
int size=104857600; //user should input th mbs this is 100 mbs


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

int main(int argc, char *argv[]){

    /*
    struct timespec time_1, time_2;
	clock_gettime(CLOCK_REALTIME, &time_1);
    //put lines of code here
    clock_gettime(CLOCK_REALTIME, &time_2);
    float dts = ((float)(1000000000*(time_2.tv_sec - time_1.tv_sec)+(time_2.tv_nsec-time_1.tv_nsec))/1000000000.0);
    printf("the time diffrence is: %fs\n",dts);
    */
   
    unsigned char *data= (unsigned char*) malloc(size*sizeof(unsigned char));
    datafiller(data);
    int fd;
    char * namedpipe = "/tmp/namedpipe"; 
    mkfifo(namedpipe, 0666); //create namepipe
    fd = open(namedpipe, O_WRONLY); //open fifo as write only
    struct timespec time_1;
    clock_gettime(CLOCK_REALTIME, &time_1);
    write(fd, data, strlen(data)+1); //write in fifo
    return 0;
}
