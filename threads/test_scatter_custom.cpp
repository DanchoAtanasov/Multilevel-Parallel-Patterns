#include "mpi.h"
#include <stdio.h>
#include <stddef.h>

typedef struct bla_{
    int x;
} bla;

typedef struct OptionData_ {
    float s;          // spot price
    float strike;     // strike price
    float r;          // risk-free interest rate
    float divq;       // dividend rate
    float v;          // volatility
    float t;          // time to maturity or option expiration in years 
                       //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)  
    char OptionType;   // Option type.  "P"=PUT, "C"=CALL
    float divs;       // dividend vals (not used in this test)
    float DGrefval;   // DerivaGem Reference Value
} OptionData;

int main(int argc, char *argv[])  {
    int numtasks, rank, sendcount, recvcount, source;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Request reqs[1];
    MPI_Status stats[1];
    MPI_Datatype OptionData;
    MPI_Datatype type[1] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT,
        MPI_CHAR,  MPI_FLOAT,  MPI_FLOAT };
    int blocklen[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    OptionData arr[2];
    arr[0].s = 5.5f;
    arr[1].s = 7.2f;
    MPI_Aint disp[9] = { offsetof(OptionData, s), offsetof(OptionData, strike), offsetof(OptionData, r),
        offsetof(OptionData, divq),  offsetof(OptionData, v),  offsetof(OptionData, t),
        offsetof(OptionData, OptionType),  offsetof(OptionData, divs),  offsetof(OptionData, DGrefval), };

    source = 0;
    sendcount = 1;
    recvcount = 1;

    //bla arr[2];
    //arr[0].x = 5;    
    //arr[1].x = 9;    

    MPI_Type_create_struct(1, blocklen, disp, type, &OptionData);
    MPI_Type_commit(&OptionData);

    OptionData dua[1];
    // Scattering matrix1 to all nodes in chunks
    MPI_Iscatter(arr,sendcount, OptionData,dua,recvcount,
        OptionData,source,MPI_COMM_WORLD, &reqs[0]);

    // Wait for messages to be received
    MPI_Waitall(1, reqs, stats);

    printf("rank:%d, received:%d\n", rank, dua[0].x);

    MPI_Finalize();
}

