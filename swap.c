//
//  swap.c
//
//  Created by Michael Nienhaus on 24.04.15.
//
//

#include "swap.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int getPosition(int pos, int const n){
    int resPos = 0;
    int i;
    for(i=0;i<n;i++){
        resPos = resPos << 1;
        resPos |= pos&1;
        pos = pos >> 1;
    }
    return resPos;
}


void swap (kfloat* toSwap, kfloat* swapped, int const n)
{
    int i;
    
    for(i=0;i<n;i++)
        swapped[i] = toSwap[getPosition(i, log2(n))];
    
//    for(i=0; i<n; i++)
//        printf("Vorher: %d: Re:%f , Im:%f\n",i,toSwap[i].real,toSwap[i].imagin);
//
//    for(i=0; i<n; i++)
//        printf("Nacher: %d: Re:%f , Im:%f\n",i,swapped[i].real,swapped[i].imagin);
}