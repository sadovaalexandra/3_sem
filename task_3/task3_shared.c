#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

const int BYTES = 1024*4096;

int main() {
    char* array; //buffer
    int i, shmid;
    char pathname[] = "task3_shared.c"; 
    key_t key;
    char mas[100]; 
    int result = fork();
    if (result == -1)//fork failed
        exit(-1);
    if (result) {//parent
        if((key = ftok(pathname,0)) < 0) { //getting the key
        	printf("getting key failed\n");
        	exit(-1);
        }
        if((shmid = shmget(key, 102*sizeof(char), 0666|IPC_CREAT|IPC_EXCL)) < 0) { //getting shared memory
            if(errno != EEXIST) { //error
                printf("creating shared memory failed\n");
                exit(-1);
            } else {
            	if((shmid = shmget(key, 102*sizeof(char), 0)) < 0) { //allocating memory
                	printf("getting memory failed\n");
                	exit(-1);
            	}
            }
        }
        if((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)) { //joining with shared memory 
            printf("joining shared memory failed\n");
            exit(-1);
        }
        for(i=0; i < 102; i++)
        	array[i] = 0;
        int j = 0, count = 0;
        while(1) {
            while(array[100] == 1) //101 element syncronises work, parent waits if it is equal to 1
            array[j] = 's'; 
            j++;
            count++;
            if((j==100)||(count == BYTES)) {//buffer is full or the information has ended
                array[101] = count % 100; //0 if buffer is full
                if(count == BYTES) 
    			array[101] = -1;//sent all information
                array[100] = 1; //for parent to wait and child to read
                j = 0;
                if(count == BYTES) 
                    break;
            }
        }
        if(shmdt(array) < 0) { //detaching memory
            printf("detaching shared memory failed\n");
            exit(-1);
        }
    }
    if(!result) {//child
        int new_;
        sleep(1); //waiting for parent to create memory
        if((key = ftok(pathname,0)) < 0) {
            printf("getting key failed\n");
            exit(-1);
        }
        if((shmid = shmget(key, 102*sizeof(char), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
            if(errno != EEXIST){
                printf("creating shared memory failed\n");
                exit(-1);
            } else {
            	if((shmid = shmget(key, 102*sizeof(char), 0)) < 0) {
                	printf("finding shared memory failed\n");
                	exit(-1);
            	}
            new_ = 0; 
            }
        }
        if((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)) { 
    		printf("attaching shared memory failed\n");
            	exit(-1);
        }
        int j = 0, count = 0;
        clock_t start = clock();
        while(1) {
            while(array[100] == 0)//waiting
            mas[j] = array[j]; //reading from array
            j++;
            count++;
            if((j == 100)||(count == BYTES)) {
                array[100] = 0;
                j = 0;
                if(count == BYTES) 
                    break;
            }
        }
        printf("Time: %.3f ms\n", 1000*((double)clock()-start)/((double)CLOCKS_PER_SEC)); 
        if(shmdt(array) < 0) {
            printf("detaching shared memory failed\n");
            exit(-1);
        }
        shmctl(shmid, IPC_RMID, (struct shmid_ds *) NULL); //free the memory
    }
    return 0;
}
