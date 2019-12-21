#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

const int BYTES = 1000000; 

int main() {
    int msqid;
    char pathname[]="task3_queue.c"; 
    key_t key;
    int i, len, maxlen;
    struct mymsgbuf {//for passing one char
 	   long mtype;
       	   struct info {
           char x;
     	   } a;
    } mybuf;

    int result = fork();

    if (result < 0) //fork failed
    	   exit(-1);
    if (result) {//parent
      	   key = ftok(pathname, 0);

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) { //shared memory
           printf("getting msqid failed\n");
           exit(-1);
    }
    mybuf.mtype = 1; //type of the information sent
    mybuf.a.x = 'a'; //the information
    len = sizeof(struct info);
    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0){ 
           printf("sending information failed\n");
           msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
           exit(-1);
    }
        for (i = 1; i <= BYTES; i++){ //sending the information the right amount of times
            mybuf.mtype = 1;
            len = sizeof(struct info);
            if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
                printf("sending information failed\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                exit(-1);
            }
        }
    }
    if(!result) {//child
        key = ftok(pathname, 0);
        if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) { 
            printf("getting msqid failed\n");
            exit(-1);
        }
        len = sizeof(struct info);
        if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, len, 0, 0)) < 0) { 
            printf("receiving information failed\n");
            exit(-1);
        }
        clock_t start = clock();
        for (i = 1; i <= BYTES; i++) { //recieving the information
        if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, len, 0, 0)) < 0) {
                printf("receiving information failed\n");
                exit(-1);
            }
        }
        printf("Time: %.3f ms\n", 1000*((double)clock()-start)/((double)CLOCKS_PER_SEC)); 
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL); //free the memory
    }
}
