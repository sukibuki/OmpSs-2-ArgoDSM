#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define fptype float


typedef struct OptionData_ {
        fptype s;          // spot price
        fptype strike;     // strike price
        fptype r;          // risk-free interest rate
        fptype divq;       // dividend rate
        fptype v;          // volatility
        fptype t;          // time to maturity or option expiration in years 
                           //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)  
        char OptionType;   // Option type.  "P"=PUT, "C"=CALL
        fptype divs;       // dividend vals (not used in this test)
        fptype DGrefval;   // DerivaGem Reference Value
} OptionData;

OptionData *data;
OptionData *data2;
int numOptions;

int main (int argc, char **argv)
{
    FILE *file;
    int i;
    int loopnum;
    int rv;
    char *inputFile = argv[1];

    //Read input data from file
    file = fopen(inputFile, "r");
    if(file == NULL) {
      printf("ERROR: Unable to open file `%s'.\n", inputFile);
      exit(1);
    }
    rv = fscanf(file, "%i", &numOptions);
    if(rv != 1) {
      printf("ERROR: Unable to read from file `%s'.\n", inputFile);
      fclose(file);
      exit(1);
    }

    data = (OptionData*)malloc(numOptions*sizeof(OptionData));

    for ( loopnum = 0; loopnum < numOptions; ++ loopnum )
    {
        rv = fscanf(file, "%f %f %f %f %f %f %c %f %f", &data[loopnum].s, &data[loopnum].strike, &data[loopnum].r, &data[loopnum].divq, &data[loopnum].v, &data[loopnum].t, &data[loopnum].OptionType, &data[loopnum].divs, &data[loopnum].DGrefval);
        if(rv != 9) {
          printf("ERROR: Unable to read from file `%s'.\n", inputFile);
          fclose(file);
          exit(1);
        }
    }
    rv = fclose(file);
    if(rv != 0) {
      printf("ERROR: Unable to close file `%s'.\n", inputFile);
      exit(1);
    }

    data2 = (OptionData*)nanos6_dmalloc(numOptions*sizeof(OptionData), nanos6_equpart_distribution, 0, NULL);

    for (int p=0; p<numOptions; ++p){
      data2[p] = data[p];
    }

   #pragma oss task inout(data2[0;numOptions]) 
    for(int k=0; k<numOptions; ++k){
      data2[k].r += 10.0;
    }
  #pragma oss taskwait

  for (int p=0; p<10; ++p){
    printf("data[%d].r = %f , while data2[%d].r = %f \n",p,data[p].r,p,data2[p].r);
  }

    nanos6_dfree(data2, numOptions*sizeof(OptionData));
    printf("everything ok \n");

    return 0;
}
