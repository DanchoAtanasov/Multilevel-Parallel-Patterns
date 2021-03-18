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
#include "../../src/mlpplib.h"


//Precision to use for calculations
#define fptype float

#define NUM_RUNS 100

const int NODES = 2;

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
FILE* file;
int rv;

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

int bs_thread(int start, int end) {
    printf("In bs_thread with %d %d\n", start, end);
    int i, j;
    fptype price;
    fptype priceDelta;

    for (j = 0; j < NUM_RUNS; j++) {
        for (i = start; i < end; i++) {
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

void readFile(char* inputFile) {
    printf("Beginning of readFile\n");

    //Read input data from file
    file = fopen(inputFile, "r");
    if (file == NULL) {
        printf("ERROR: Unable to open file %s.\n", inputFile);
        Abort();
        exit(1);
    }
    rv = fscanf(file, "%i", &numOptions);
    if (rv != 1) {
        printf("ERROR: Unable to read from file %s.\n", inputFile);
        fclose(file);
        Abort();
        exit(1);
    }
    if (nThreads > numOptions) {
        printf("WARNING: Not enough work, reducing number of threads to match number of options.\n");
        nThreads = numOptions;
    }

    printf("Num of Options: %d\n", numOptions);
    printf("Num of Runs: %d\n", NUM_RUNS);

    // alloc spaces for the option data
    filedata = (OptionData*)malloc(numOptions * sizeof(OptionData));
    final_prices = (fptype*)malloc(numOptions * sizeof(fptype));

    for (int loopnum = 0; loopnum < numOptions; ++loopnum)
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
    printf("Load done\n");
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        printf("Usage:\n\t%s <nthreads> <inputFile> <outputFile>\n", argv[0]);
        Abort();
        exit(1);
    }

    fptype* buffer;
    int* buffer2;
    int i;

    nThreads = atoi(argv[1]);
    char* inputFile = argv[2];
    char* outputFile = argv[3];
    
    Init();
    Load(readFile, inputFile);

    // Broadcasting numOptions to all nodes
    Broadcast(&numOptions);

    const int SPLIT = numOptions / NODES;
    prices = (fptype*)malloc(SPLIT * sizeof(fptype));
    data = (OptionData*)malloc(SPLIT * sizeof(OptionData));

    // Make new MPI_Datatype
    doSomething(&data[0], &OptionData::s, &OptionData::strike, &OptionData::r,
        &OptionData::divq, &OptionData::v, &OptionData::t, &OptionData::OptionType,
        &OptionData::divs, &OptionData::DGrefval);

    // Scattering input data to all nodes
    Scatter(filedata, SPLIT, data);

    printf("Scatter done, SPLIT:%d, numOptions:%d\n", SPLIT, numOptions);

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
        
	    printf("%d, %.2f, %.2f, %.2f, %.2f, %.2f\n",
	        otype[i],sptprice[i],strike[i],rate[i], volatility[i],otime[i]);
    }

    printf("Size of data: %ld\n", SPLIT * (sizeof(OptionData) + sizeof(int)));

    int res = Farm(2, SPLIT, bs_thread);

    Gather(prices, SPLIT, final_prices);

    free(data);
    free(prices);

    Finish();

    printf("rank 0 after wait\n");
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

    return 0;
}

