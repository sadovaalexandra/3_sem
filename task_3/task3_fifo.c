#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int BYTES = 10000000;

int main() {
	int i, fd, r;
   	size_t  size;
   	char resstring;
   	char name[] = "aaa.fifo"; 
   	umask(0);

   	if(mknod(name, S_IFIFO | 0666, 0) < 0) { //creating a file
     	printf("file creating failed\n");
     	exit(-1);
   	}
	r = fork();
   	if(r < 0) {
      		printf("fork failed\n");
      		exit(-1);
   	} else if (r) { //parent
      		if((fd = open(name, O_WRONLY)) < 0) { //opening file only for writing
         		printf("O_WRONLY failed\n");
	 		exit(-1);
      		}
      		for(i = 0; i < BYTES; i++) {//sending the information
	  	        char x = 'a';
        		size = write(fd, &x, 1);
		        if(size != 1) { 
            			printf("writing failed\n");
            			exit(-1);
        		}
      		}

      		if(close(fd) < 0) { 
        		printf("closing failed in parent\n"); 
			exit(-1);
      		}
   	} else { //child
      		if((fd = open(name, O_RDONLY)) < 0) {
         	printf("O_RDONLY failed\n");
	 	exit(-1);
      		}
      		clock_t start = clock();//starting timer
      		for(i = 0; i < BYTES; i++) {//receiving the information
      		        size = read(fd, &resstring, 1);
		        if(size < 0) { 
            			printf("reading failed\n");
            			exit(-1);
        		}
      		}		
      		printf("Time: %.3f ms\n", 1000*((double)clock()-start)/((double)CLOCKS_PER_SEC));

      		if(close(fd) < 0) {
        		printf("closing failed in child\n"); 	
			exit(-1);
      		}
      		remove("aaa.fifo"); //deleting the file
   	}
   	return 0;
}
