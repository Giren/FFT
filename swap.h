//
//  swap.h
//  hello
//
//  Created by Michael Nienhaus on 24.04.15.
//
//

#ifndef __hello__swap__
#define __hello__swap__

#define BYTE 8

typedef struct komplex {
    float real;
    float imagin;
} kfloat;

void swap(kfloat* A, kfloat* B, int const n);


#endif /* defined(__hello__swap__) */
