#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>

int file, wait_sym = 0, wait = 0, i = 0, stopper = 1, child_pid = 0, parent_pid = 0;
uint8_t mnoj = 1, data = 0;

void one(int a) {
    data += mnoj;
    i ++;
    mnoj *= 2;
    kill(child_pid, SIGUSR1);
}

void zero(int a) {
    i ++;
    mnoj *= 2;
    kill(child_pid, SIGUSR1);
}

void stop(int a) {
    stopper = 0;
}

void wake_up(int a) {
    wait = 0;
}

void get_symbol(int a) {
    wait_sym = 0;
}

int main() {
	child_pid = fork();
	if(child_pid == -1) {
		printf("fork failed\n");
		exit(-1);
	} else if(child_pid) {//parent - receiver
	signal(SIGUSR1, one); 
    	signal(SIGUSR2, zero);
    	signal(SIGINT, stop);
    	int file = open("new", O_CREAT|O_WRONLY, 0666);
    	while(stopper) {
        	while(i < 8)
            	if(stopper == 0)
                	break;
		if(stopper == 0)
                	break;
        	i = 0;
        	mnoj = 1;
        	write(file, &data, 1);
        	data = 0;
		kill(child_pid, SIGUSR2);



    	}
	} else {//child - sender
	sleep(2);
    	uint8_t current_bit;
    	int s;
	parent_pid = getppid();
        	
	signal(SIGUSR1, wake_up);
	signal(SIGUSR2, get_symbol);
	int file = open("old", O_RDONLY); //open file 'old' only for reading. we'll be sending this file to the child

    	while(1) {
        	s = read(file, &data, 1);//read 'file' and put it in 'data'
	        if(s == 0) {//we reached the end of the file
	            kill(parent_pid, SIGINT); //SIGINT is used for terminating a process
    	        break;
        	}
  
        	for(int i = 0; i < 8; i++) {
           		current_bit = data % 2;
            		if(current_bit == 1)
                		kill(parent_pid, SIGUSR1);//if the read bit is 1, send signal 1
            		else
                		kill(parent_pid, SIGUSR2);//if the read bit is 0, send signsl 2
            		data = data / 2;
            		wait = 1;
            		while(wait);
        	}
		wait_sym = 1;
        	while(wait_sym);
    	}
	}
	close(file);
	return 0;
}
