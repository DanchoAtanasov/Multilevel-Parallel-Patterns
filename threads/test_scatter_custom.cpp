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
    MPI_Datatype MPI_OptionData;
    MPI_Datatype type[9] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT,
        MPI_CHAR,  MPI_FLOAT,  MPI_FLOAT };
    int blocklen[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    //OptionData arr[4];
    OptionData * arr = (OptionData*)malloc(4 * sizeof(OptionData));
    arr[0].s = 0.5f; arr[0].strike = 0.2f;
    arr[1].s = 1.5f; arr[1].strike = 1.2f;
    arr[2].s = 2.5f; arr[2].strike = 2.2f;
    arr[3].s = 3.5f; arr[3].strike = 3.2f;
    MPI_Aint disp[9] = { offsetof(OptionData, s), offsetof(OptionData, strike), offsetof(OptionData, r),
        offsetof(OptionData, divq),  offsetof(OptionData, v),  offsetof(OptionData, t),
        offsetof(OptionData, OptionType),  offsetof(OptionData, divs),  offsetof(OptionData, DGrefval), };

    source = 0;
    sendcount = 2;
    recvcount = 2;

    //bla arr[2];
    //arr[0].x = 5;    
    //arr[1].x = 9;    

    MPI_Type_create_struct(9, blocklen, disp, type, &MPI_OptionData);
    MPI_Type_commit(&MPI_OptionData);

    //OptionData dua[1];
    OptionData * dua = (OptionData*)malloc(recvcount * sizeof(OptionData));
    // Scattering matrix1 to all nodes in chunks
    MPI_Iscatter(arr,sendcount, MPI_OptionData,dua,recvcount,
        MPI_OptionData,source,MPI_COMM_WORLD, &reqs[0]);

    // Wait for messages to be received
    MPI_Waitall(1, reqs, stats);

    printf("rank:%d, received:%f, %f\n", rank, dua[0].s, dua[0].strike);
    printf("rank:%d, received:%f, %f\n", rank, dua[1].s, dua[1].strike);

    MPI_Finalize();
}

