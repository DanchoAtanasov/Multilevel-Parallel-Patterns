// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
// 
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition, Prentice 
// Hall, John C. Hull,

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "../../src/library.h"
#include "mpi.h"

// Multi-threaded pthreads header
//#include <pthread.h>
//#include <time.h>

//#define MAX_THREADS 128

//pthread_t threadsTable[MAX_THREADS];
//int threadsTableAllocated[MAX_THREADS];
//pthread_mutexattr_t normalMutexAttr;
//int numThreads = MAX_THREADS;

//Precision to use for calculations
#define fptype float

#define NUM_RUNS 100

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

OptionData* data;
OptionData* filedata;
fptype* prices;
fptype* final_prices;
int numOptions;

int* otype;
fptype* sptprice;
fptype* strike;
fptype* rate;
fptype* volatility;
fptype* otime;
int numError = 0;
int nThreads;

// Cumulative Normal Distribution Function
// See Hull, Section 11.8, P.243-244
#define inv_sqrt_2xPI 0.39894228040143270286

fptype CNDF(fptype InputX)
{
    int sign;

    fptype OutputX;
    fptype xInput;
    fptype xNPrimeofX;
    fptype expValues;
    fptype xK2;
    fptype xK2_2, xK2_3;
    fptype xK2_4, xK2_5;
    fptype xLocal, xLocal_1;
    fptype xLocal_2, xLocal_3;

    // Check for negative value of InputX
    if (InputX < 0.0) {
        InputX = -InputX;
        sign = 1;
    }
    else
        sign = 0;

    xInput = InputX;

    // Compute NPrimeX term common to both four & six decimal accuracy calcs
    expValues = exp(-0.5f * InputX * InputX);
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    xK2 = 0.2316419 * xInput;
    xK2 = 1.0 + xK2;
    xK2 = 1.0 / xK2;
    xK2_2 = xK2 * xK2;
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * 0.319381530;
    xLocal_2 = xK2_2 * (-0.356563782);
    xLocal_3 = xK2_3 * 1.781477937;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_4 * (-1.821255978);
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_5 * 1.330274429;
    xLocal_2 = xLocal_2 + xLocal_3;

    xLocal_1 = xLocal_2 + xLocal_1;
    xLocal = xLocal_1 * xNPrimeofX;
    xLocal = 1.0 - xLocal;

    OutputX = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    return OutputX;
}

fptype BlkSchlsEqEuroNoDiv(fptype sptprice,
    fptype strike, fptype rate, fptype volatility,
    fptype time, int otype, float timet)
{
    fptype OptionPrice;

    // local private working variables for the calculation
    fptype xStockPrice;
    fptype xStrikePrice;
    fptype xRiskFreeRate;
    fptype xVolatility;
    fptype xTime;
    fptype xSqrtTime;

    fptype logValues;
    fptype xLogTerm;
    fptype xD1;
    fptype xD2;
    fptype xPowerTerm;
    fptype xDen;
    fptype d1;
    fptype d2;
    fptype FutureValueX;
    fptype NofXd1;
    fptype NofXd2;
    fptype NegNofXd1;
    fptype NegNofXd2;

    xStockPrice = sptprice;
    xStrikePrice = strike;
    xRiskFreeRate = rate;
    xVolatility = volatility;

    xTime = time;
    xSqrtTime = sqrt(xTime);

    logValues = log(sptprice / strike);

    xLogTerm = logValues;


    xPowerTerm = xVolatility * xVolatility;
    xPowerTerm = xPowerTerm * 0.5;

    xD1 = xRiskFreeRate + xPowerTerm;
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    xD1 = xD1 / xDen;
    xD2 = xD1 - xDen;

    d1 = xD1;
    d2 = xD2;

    NofXd1 = CNDF(d1);
    NofXd2 = CNDF(d2);

    FutureValueX = strike * (exp(-(rate) * (time)));
    if (otype == 0) {
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    }
    else {
        NegNofXd1 = (1.0 - NofXd1);
        NegNofXd2 = (1.0 - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    return OptionPrice;
}

int bs_thread(int numOptions) {
    //printf("In bs_thread with %d %d\n", start, end);
    int i, j;
    fptype price;
    fptype priceDelta;
    //int tid = *(int *)tid_ptr;
    //int start = tid * (numOptions / nThreads);
    //int end = start + (numOptions / nThreads);

    for (j = 0; j < NUM_RUNS; j++) {
        for (i = 0; i < numOptions; i++) { 
            /* Calling main function to calculate option value based on
             * Black & Scholes's equation.
             */
            price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i],
                rate[i], volatility[i], otime[i],
                otype[i], 0);
            prices[i] = price;

        }
    }

    return 0;
}

int main(int argc, char** argv)
{

    // MPI code
    int numtasks, rank, sendcount, recvcount, source;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    MPI_Request reqs[3];
    MPI_Status stats[3];

    MPI_Datatype MPI_OptionData;
    MPI_Datatype type[9] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT,
        MPI_CHAR,  MPI_FLOAT,  MPI_FLOAT };
    int blocklen[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    MPI_Aint disp[9] = { offsetof(OptionData, s), offsetof(OptionData, strike), offsetof(OptionData, r),
        offsetof(OptionData, divq),  offsetof(OptionData, v),  offsetof(OptionData, t),
        offsetof(OptionData, OptionType),  offsetof(OptionData, divs),  offsetof(OptionData, DGrefval), };
    MPI_Type_create_struct(9, blocklen, disp, type, &MPI_OptionData);
    MPI_Type_commit(&MPI_OptionData);
    

    FILE* file;
    fptype* buffer;
    int* buffer2;
    int i;
    int rv;
    char* outputFile = argv[3];
    
    if(rank == 0){
        int loopnum;

        if (argc != 4)
        {
            printf("Usage:\n\t%s <nthreads> <inputFile> <outputFile>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }
        nThreads = atoi(argv[1]);
        char* inputFile = argv[2];

        //Read input data from file
        file = fopen(inputFile, "r");
        if (file == NULL) {
            printf("ERROR: Unable to open file %s.\n", inputFile);
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }
        rv = fscanf(file, "%i", &numOptions);
        if (rv != 1) {
            printf("ERROR: Unable to read from file %s.\n", inputFile);
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }
        if (nThreads > numOptions) {
            printf("WARNING: Not enough work, reducing number of threads to match number of options.\n");
            nThreads = numOptions;
        }
        
        printf("Num of Options: %d\n", numOptions);
        printf("Num of Runs: %d\n", NUM_RUNS);

    //}

    // alloc spaces for the option data
        filedata = (OptionData*)malloc(numOptions * sizeof(OptionData));
        final_prices = (fptype*)malloc(numOptions * sizeof(fptype));

    //if(rank == 0){
        for (loopnum = 0; loopnum < numOptions; ++loopnum)
        {
            rv = fscanf(file, "%f %f %f %f %f %f %c %f %f", &filedata[loopnum].s, &filedata[loopnum].strike, &filedata[loopnum].r, &filedata[loopnum].divq, &filedata[loopnum].v, &filedata[loopnum].t, &filedata[loopnum].OptionType, &filedata[loopnum].divs, &filedata[loopnum].DGrefval);
            if (rv != 9) {
                printf("ERROR: Unable to read from file %s.\n", inputFile);
                fclose(file);
                exit(1);
            }
        }
        rv = fclose(file);
        if (rv != 0) {
            printf("ERROR: Unable to close file %s.\n", inputFile);
            exit(1);
        }
    }

    
    // Broadcasting numOptions to all nodes
    MPI_Ibcast(&numOptions, 1, MPI_INT, 0, MPI_COMM_WORLD, &reqs[2]);
    MPI_Wait(&reqs[2], &stats[2]);

    const int SPLIT = numOptions / numtasks;
    prices = (fptype*)malloc(SPLIT * sizeof(fptype));
    data = (OptionData*)malloc(SPLIT * sizeof(OptionData));

    source = 0;
    sendcount = SPLIT;
    recvcount = SPLIT;

    // Scattering input data to all nodes
    MPI_Iscatter(filedata, sendcount, MPI_OptionData, data, recvcount,
        MPI_OptionData, source, MPI_COMM_WORLD, &reqs[0]);

    MPI_Waitall(1, reqs, stats);

#define PAD 256
#define LINESIZE 64

    buffer = (fptype*)malloc(5 * SPLIT * sizeof(fptype) + PAD);
    sptprice = (fptype*)(((unsigned long long)buffer + PAD) & ~(LINESIZE - 1));
    strike = sptprice + SPLIT;
    rate = strike + SPLIT;
    volatility = rate + SPLIT;
    otime = volatility + SPLIT;

    buffer2 = (int*)malloc(SPLIT * sizeof(fptype) + PAD);
    otype = (int*)(((unsigned long long)buffer2 + PAD) & ~(LINESIZE - 1));

    for (i = 0; i < SPLIT; i++) {
        otype[i] = (data[i].OptionType == 'P') ? 1 : 0;
        sptprice[i] = data[i].s;
        strike[i] = data[i].strike;
        rate[i] = data[i].r;
        volatility[i] = data[i].v;
        otime[i] = data[i].t;
    }

    printf("Size of data: %ld\n", SPLIT * (sizeof(OptionData) + sizeof(int)));

    int from = rank * SPLIT;
    int to = from + SPLIT;
    int res = bs_thread(SPLIT);

    MPI_Igather(prices, sendcount, MPI_FLOAT, final_prices, recvcount,
        MPI_FLOAT, source, MPI_COMM_WORLD, &reqs[1]);

    if (rank == 0) {
        printf("rank 0 after wait\n");
        MPI_Wait(&reqs[1], &stats[1]);
        printf("numOptions:%d\n", numOptions); 
	    for (int i = 0; i < numOptions; i++) {
                printf("%f\n", final_prices[i]);
            }

        //Write prices to output file
        file = fopen(outputFile, "w");
        if (file == NULL) {
            printf("ERROR: Unable to open file %s.\n", outputFile);
            exit(1);
        }
        rv = fprintf(file, "%i\n", numOptions);
        if (rv < 0) {
            printf("ERROR: Unable to write to file %s.\n", outputFile);
            fclose(file);
            exit(1);
        }
        for (i = 0; i < numOptions; i++) {
            rv = fprintf(file, "%.18f\n", final_prices[i]);
            if (rv < 0) {
                printf("ERROR: Unable to write to file %s.\n", outputFile);
                fclose(file);
                exit(1);
            }
        }
        rv = fclose(file);
        if (rv != 0) {
            printf("ERROR: Unable to close file %s.\n", outputFile);
            exit(1);
        }
        free(filedata);
        free(final_prices);
    }

    free(data);
    free(prices);

    printf("rank:%d finished.\n", rank);

    return 0;
}

