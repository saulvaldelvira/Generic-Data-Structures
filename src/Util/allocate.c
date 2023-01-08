/**
 * Autor:   Saúl Valdelvira Iglesias
 * Email:   saulvaldelvira@gmail.com
 * Version: 08-01-2023
 * Copyright: Saúl Valdelvira Iglesias (2023)
 * Licensed under the GNU GPL V.3. See /LICENSE file for more info
*/
#include "allocate.h"
#include <stdlib.h>

void* alloc_int(int n){
        int * ptr = malloc(sizeof(int));
        *ptr = n;
        return ptr;
}

void* alloc_char(char c){
        char * ptr = malloc(sizeof(char));
        *ptr = c;
        return ptr;
}

void* alloc_float(float f){
        float * ptr = malloc(sizeof(float));
        *ptr = f;
        return ptr;
}

void* alloc_double(double d){
        double * ptr = malloc(sizeof(double));
        *ptr = d;
        return ptr;
}

void* alloc_long(long l){
        long * ptr = malloc(sizeof(long));
        *ptr = l;
        return ptr;
}

void* alloc_long_long(long long ll){
        long long * ptr = malloc(sizeof(long long));
        *ptr = ll;
        return ptr;
}