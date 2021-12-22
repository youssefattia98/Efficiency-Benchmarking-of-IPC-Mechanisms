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

int main(int argc, char * argv[]) {
    unsigned char *data= (unsigned char*) malloc(size*sizeof(unsigned char));
    datafiller(data);
    int pfd[2];
    pfd[0] = atoi(argv[1]);
    pfd[1] = atoi(argv[2]);
    /* child program */
    close(pfd[0]); /* close the read end */
    int bytes = write(pfd[1],data,strlen(data)); /*Write to pipe*/
    return(0);
}