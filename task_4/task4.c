#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>

#define POINTS 10000000 //total amount of points in the integral

double TOTAL_AREA = 0; //variable for the total area i.e. the integral

double function(double x) {//function to integrate
    return x; 
}

struct segment {
    double a; //left end
    double b; //right end
    long int points; //points on the segment
};

void* area(void* r) {
    struct segment* seg = (struct segment*)r; 
    double length = (double)(seg -> b - seg -> a)/seg -> points; //we divide the segment to 'points' small segments with the length 'length'
    double max = 0;
    for(int i = 0; i <= seg -> points; i++) 
        if(function(seg->a + length * i) > max)
            max = function(seg -> a + length*i);//finding the maximum of the function
    long int entry = 0; //variable for the amount of random points that are below the graph
    srand(4);
    for(int i = 0; i < seg -> points; i++) {
        double x = seg -> a + (double)(seg -> b - seg -> a) * rand()/RAND_MAX; //a random x from the segment
        double y = (double)max * rand()/RAND_MAX; //a random y from 0 to max
        if(y < function(x))
            entry ++; //if (x,y) is below the graph, augment the 'entry' variable
    }
    TOTAL_AREA += (seg -> b - seg -> a) * max * ((double)entry/seg -> points); 
    return NULL;
}

int main() {
    int  threads;
    double left, right;

    printf("enter the amount of threads:\n"); 
    scanf("%d", &threads);
    printf("enter the left end of the segment:\npls enter a positive value!!\n");
    scanf("%lf", &left); 
    printf("enter the right end of the segment:\npls make sure it is greater than the left one!!\n");
    scanf("%lf", &right);
    
    if((left < 0) || (right < left)) {
    	printf("you entered inappropriate segment ends values. next time pls be more careful.\n");
    	return -1;
	}

    pthread_t* thread_ids = (pthread_t*)malloc(threads * sizeof(pthread_t)); //allocate memory for thread ids
    struct segment* segs = (struct segment*)malloc(threads * sizeof(struct segment)); //allocate memory for segment ids

    double length = (double)(right - left)/threads; 

    for(int i = 0; i < threads; i ++) {
        segs[i].a = left + length * i;
        segs[i].b = segs[i].a + length;
        segs[i].points = POINTS/threads;//we have 'threads' segments with this a, b, points values
    }

    clock_t start = clock();//start timer

    for(int i = 0; i < threads; i ++) {
        int create = pthread_create(thread_ids + i, (pthread_attr_t*)NULL, area, segs + i); //creating 'threads' threads and executing the area function in each of them
        if(create) {//if success, create = 0
            printf ("error when creating threads, return value: %d\n", create);
            exit(-1);
        }
    }

    for(int i = 0; i < threads; i ++) 
        pthread_join(thread_ids[i], (void**)NULL);//waiting for all the threads to terminate
	
    printf("time: %.3f ms\n", 1000*((double)clock()-start)/((double)CLOCKS_PER_SEC));
    printf("the integral is equal to %lf\n", TOTAL_AREA);
    free(thread_ids); 
    free(segs);
    return 0;
}
