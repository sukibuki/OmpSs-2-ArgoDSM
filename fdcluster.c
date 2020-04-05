#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
//herogas
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

	int *data;
	int *optdata;
	int *resdata;
	int *resdata2;
	int *optresdata;
	int length = 100000;
	int memsize = 4*length/1024/1024;
	struct timeval start;
	struct timeval stop;
	unsigned long elapsed;

	resdata = malloc(length*sizeof(int));
	//resdata2 = malloc(length*sizeof(int));
	data = malloc(length*sizeof(int));
	optdata = nanos6_dmalloc(length*sizeof(int), nanos6_equpart_distribution, 0, NULL);
	optresdata = nanos6_dmalloc(length*sizeof(int), nanos6_equpart_distribution, 0, NULL);
    //optdata = nanos6_dmalloc(length*sizeof(int), nanos6_equpart_distribution, 0, NULL);
	//print memory layout
	printf("Number of integers: %d \n Size in memory: %d MB \n", length, memsize);
	
	//first
	gettimeofday(&start, NULL);
	for(int i=0; i<length; ++i){
		data[i] = i + 1;
	}
	gettimeofday(&stop, NULL);
	elapsed =1000000*(stop.tv_sec - start.tv_sec);
	elapsed += stop.tv_usec - start.tv_usec;
	printf("Elapsed for first: %lu \n", elapsed);  

	// optimized first
        gettimeofday(&start, NULL);
	for(int j=0; j<length; j+=10000){
		#pragma oss task inout(optdata[j;j+10000])
        	for(int i=j; i<j+10000; ++i){
                	optdata[i] = i + 1;
        	}
	}
	#pragma oss taskwait
        gettimeofday(&stop, NULL);
        elapsed =1000000*(stop.tv_sec - start.tv_sec);
        elapsed += stop.tv_usec - start.tv_usec;
        printf("Elapsed for optimized first: %lu \n", elapsed);

	//check validity
	for(int j=0; j<length; ++j){
		if (data[j] != optdata[j]) { printf("ERROR in position: %d \n", j);  break; }
		else if (j == length -1) { printf("No Errors :) \n"); }
	}
	
	//nanos6_dfree(optdata, length*sizeof(int));

	//going to result func
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
        for(int i=0; i<length; i+=5000){
		#pragma oss task inout(optresdata[i;i+5000])
		for(int j=i; j<i+5000; ++j){
                	optresdata[j] = result(optresdata[j], j);
		}
        }
        /*for(int i=0; i<length; i+=1){
            #pragma oss task in(optresdata[i])
            optresdata[i] = result(optresdata[i], i);
        }*/


    /*for(int i=length-1; i>=0; i-=10000){
		#pragma oss task in(optresdata[i-10000;i])
		for(int j=i; j>i-10000; --j){
                	optresdata[j] = result(optresdata[j], j);
		}
        }*/

	#pragma oss taskwait
        gettimeofday(&stop, NULL);
        elapsed =1000000*(stop.tv_sec - start.tv_sec);
        elapsed += stop.tv_usec - start.tv_usec;
        printf("Elapsed for optimized intermediate result loop: %lu \n", elapsed);
	//nanos6_dfree(optdata, length*sizeof(int));



    //check validity
	#pragma oss task in(optresdata[0;length],resdata[0;length])
        for(int j=0; j<length; ++j){
                if (resdata[j] != optresdata[j]) { printf("ERROR(1) in position: %d \n", j);  break; }
                else if (j == length -1) { printf("No Errors in res(1) :) \n"); }
        }
	#pragma oss taskwait


	// //second
	// gettimeofday(&start, NULL);
	// for(int i=0; i<length; ++i){
	// 	data[i] += data[1+i];
	// }
	// gettimeofday(&stop, NULL);
    //     elapsed = 1000000*(stop.tv_sec - start.tv_sec);
	// elapsed += stop.tv_usec - start.tv_usec;
    //     printf("Elapsed for second: %lu \n", elapsed);


    //     // optimized second
    //     gettimeofday(&start, NULL);
    //     for(int i=0; i<length; ++i){
    //             data[i] += data[1+i];
    //     }
    //     gettimeofday(&stop, NULL);
    //     elapsed = 1000000*(stop.tv_sec - start.tv_sec);
    //     elapsed += stop.tv_usec - start.tv_usec;
    //     printf("Elapsed for optimized second: %lu \n", elapsed);
	
	nanos6_dfree(optdata, length*sizeof(int));
	nanos6_dfree(optresdata, length*sizeof(int));
	free(data);
	free(resdata);

	return 0;
}
