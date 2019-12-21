#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

const int INFO_SZ = 10; //size of memory we can pass

struct dpipe_t {//struct for pipe descriptors
    int txd[2]; //child receives from here
    int rxd[2]; //parent receives from here
};

int main() {
    struct dpipe_t dp; 
    size_t sz;
    int i;
    char  str, const_string;

    if((pipe(dp.txd) == -1) || (pipe(dp.rxd) == -1)) {
        printf("pipe failed\n");
        exit(-1);
    }

    int child_pid = fork();

    if(child_pid == -1) {
        printf("fork failed\n");
        exit(-1);
    }
    
    else {
        if(child_pid) { //parent
            const_string = 'p';//parent will send this string to child

            if(close(dp.txd[0]) == -1) {
                printf("closing reading error in parent\n"); //parent cant close the reading end of his pipe
                exit(-1);
            }
            
            if(close(dp.rxd[1]) == -1) {
                printf("closing writing error in parent\n"); //parent cant close the writing end of his pipe
                exit(-1);
            }

            for(i = 0; i < INFO_SZ; i++) { //cycle for communication 

                sz = write(dp.txd[1], &const_string, 1);
                if(sz == -1) {
                    printf("writing error in parent\n"); 
                    exit(-1);
                }

                sz = read(dp.rxd[0], &str, 1);          
                if(sz == -1) {
                    printf("reading error in parent\n");
                    exit(-1);
                }

                printf("PARENT RECEIVED: %c\n", str);
            }

            if((close(dp.rxd[0]) == -1) || (close(dp.txd[1]) == -1)) {
                printf("closing pipe error in parent\n");
                exit(-1);
            }
        }
        
        else {   //child_pid = 0, child
        
            const_string = 'c';//child will send this string to parent

            if(close(dp.txd[1]) == -1) {
                printf("closing writing error in child\n");
                exit(-1);
            }
            
            if(close(dp.rxd[0]) == -1) {
                printf("closing reading error in child\n");
                exit(-1);
            }

            for(i = 0; i < INFO_SZ; i ++) { //cycle for communication
            	
                sz = read(dp.txd[0], &str, 1);
                if(sz == -1) {
                    printf("redaing error in child\n");
                    exit(-1);
                }
                
                printf("CHILD RECEIVED: %c\n", str);  
                
                sz = write(dp.rxd[1], &const_string, 1);
                if(sz == -1) {
                    printf("writing error in child\n");
                    exit(-1);
                }                         
            }

            if((close(dp.txd[0]) == -1) || (close(dp.rxd[1]) == -1)) {
                printf("closing pipe error in child\n");
                exit(-1);
            }
        }
	}
   return 0;
}
