#define AUTO 1 // bool for automatic code running
#define CYCLES 10000 // Cycles of operation to average compute time
#define VERBOSE 0 // bool for console output
#define SETSIZE 2 //Matrix size
#define SINGLETHREAD 0 //bool for using single thread

#ifndef SINGLETHREAD
#define SINGLETHREAD 0
#endif
#ifndef AUTO
#define AUTO 0
#endif
#ifndef CYCLES
#define CYCLES 1
#endif
#ifndef VERBOSE
#define VERBOSE 1
#endif
#ifndef SETSIZE
#define SETSIZE 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

double totalTime;

struct vectorV {
    int n;
    int i;
    int** first;
    int** second;
    int** out;
};

int defineSize(){
    int n;
    if(SETSIZE){
        n=SETSIZE;
    }
    else if(AUTO){
        n=(rand() % 11);
    }
    else{
        printf("Size of NxN array (max 100):\n\r");
        scanf("%d",&n);
        if((n<=0) || (n>100)){
            printf("Matrix size invalid\n\r");
            exit(1);
        }
    }
    return n;
}

int** defineMatrix(int n)
{
    int* values = calloc(n*n, sizeof(int));
    int** rows = malloc(n*sizeof(int*));
    for (int i=0; i<n; ++i)
    {
        rows[i] = values + i*n;
    }
    return rows;
}

void killMatrix(int** arr)
{
    free(*arr);
    free(arr);
}

void enterValues(int len, int** arr){
    for (int i = 0; i < len; i++)
        for (int ii = 0; ii < len; ii++){
            if(AUTO){
                arr[i][ii]=(rand() % 100);
            }
            else{
                printf("Enter matrix values:\n\r");
                printf("%d:",ii+(i*len));
                scanf("%d", &arr[i][ii]);
            }
        }     
}

void printMatrix(int len, int** arr){
    int max=-2147483647;
    for (int i = 0; i < len; i++)
        for (int ii = 0; ii < len; ii++){
            if(arr[i][ii]>max)
                max=arr[i][ii];
        }
    
    int ret = ceil(log10((double)max+0.001))+1;

    for (int i = 0; i < len; i++){
        printf("| ");
        for (int ii = 0; ii < len; ii++)
            printf("%-*d",ret,arr[i][ii]); 
        printf("|\n\r");
    }
        
}

void *vectorMultiply(void *arguments){
    struct vectorV *args = arguments;


    for (int ii = 0; ii < args->n; ii++) {
        int sum = 0;
        for (int iii = 0; iii < args->n; iii++) {
            sum = sum + args->first[args->i][iii] * args->second[iii][ii];
        }
        args->out[args->i][ii] = sum;
    }

}


void multiplyMatrix(int n, int** first, int** second, int** out){
    pthread_t tid[n];
    struct vectorV values[n];
    for (int i = 0; i < n; i++) {
            if(!SINGLETHREAD){
                values[i].n=n;
                values[i].i=i;
                values[i].first = first;
                values[i].second=second;
                values[i].out=out;
                pthread_create(&tid[i], NULL, vectorMultiply, (void *)&values[i]); 
            }
            else{
                clock_t time; 
                time = clock(); 
                time = clock() - time; 
                totalTime+=time;
            }
            
        
    }
    if(!SINGLETHREAD){
        clock_t time; 
        time = clock(); 
        for(int i=0; i < n; i++)
            pthread_join( tid[i], NULL); 
        time = clock() - time; 
        totalTime+=time;
    }
}

int main(void){
    for(int i=0;i<CYCLES;i++){
    srand(time(NULL));    //Start random seed

    int n;
    n=defineSize();

    int** first = defineMatrix(n);
    enterValues(n,first);

    int** second = defineMatrix(n);
    enterValues(n,second);

    if(VERBOSE){
        printMatrix(n,first);
        printf("X\n\r");
        printMatrix(n,second);
    }

    int** out = defineMatrix(n);

    multiplyMatrix(n,first,second,out);
    
    if(VERBOSE){
        printf("=\n\r");
        printMatrix(n,out);
    }

    killMatrix(first);
    killMatrix(second);
    killMatrix(out);
    }

    double timeTaken = ((double)totalTime)/((long)CLOCKS_PER_SEC);
    printf("Finished in %f seconds on average (%f total) after %d cycles \n\r",timeTaken/CYCLES ,timeTaken, CYCLES);
    
  return 0;
}
