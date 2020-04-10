#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

int result(int a, int index){
	a=index;
	a = sqrt((double)a);
	a = (a%150);
	int b = (index%15);
	a += b;
	int exam = 5;
	for(int p=0; p<100000; ++p){
		if (p != a) { exam += exam; }
	}
	a += exam;
	return a;
}

int main(int argc, char* argv[])
{
	 int *resdata;
	 int *optresdata;
	 unsigned long length = 100000L;
	 unsigned long memsize = (4*length)/1024/1024;
	 struct timeval start;
	 struct timeval stop;
	 unsigned long elapsed;

	resdata = malloc(length*sizeof(int));
	optresdata = nanos6_dmalloc(length*sizeof(int), nanos6_equpart_distribution, 0, NULL);

	//print memory layout
	printf("Number of integers: %lu \n Size in memory: %lu MB \n", length, memsize);
	
    #pragma oss task inout(optresdata[0;length]) label(simple)
		optresdata[0] = 1;
	#pragma oss taskwait

	// //going to result func
	gettimeofday(&start, NULL);
        for(int i=0; i<length; ++i){
                resdata[i] = result(resdata[i], i);
        }
        gettimeofday(&stop, NULL);
        elapsed =1000000*(stop.tv_sec - start.tv_sec);
        elapsed += stop.tv_usec - start.tv_usec;
        printf("Elapsed for intermediate result loop: %lu \n", elapsed);


	//going to optimized result func
        gettimeofday(&start, NULL);
        for(int i=0; i<length; i+=length/8){
		    #pragma oss task inout(optresdata[i;length/8]) label(forloop)
		        for(int j=i; j<i+length/8; ++j){
                        	optresdata[j] = result(optresdata[j], j);
		        }
        }
	    #pragma oss taskwait
        gettimeofday(&stop, NULL);
        elapsed =1000000*(stop.tv_sec - start.tv_sec);
        elapsed += stop.tv_usec - start.tv_usec;
        printf("Elapsed for optimized intermediate result loop: %lu \n", elapsed);

	 nanos6_dfree(optresdata, length*sizeof(int));
	 free(resdata);

	return 0;
}
