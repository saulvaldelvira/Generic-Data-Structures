#pragma once
#ifndef COMPARATOR_H
#define COMPARATOR_H
    #include <stdbool.h>
    #include <math.h>

    #define COMPARATOR_FUNCTION(name) int (*name) (void*,void*)

    // COMMON COMPARATOR FUNCTIONS
    int compare_int(void *e_1, void *e_2);
    int compare_char(void *e_1, void *e_2);
    int compare_float(void *e_1, void *e_2);
    int compare_double(void *e_1, void *e_2);

    #define DEFAULT_FLOAT_PRECISION 100000000
    #define DEFAULT_DOUBLE_PRECISION 100000000000000000

#ifndef DISABLE_COMPARATOR
    static struct {
        int (*integer)             (void*, void*);
        int (*character)           (void*, void*);
        int (*floating)            (void*, void*);
        int (*double_floating)     (void*, void*);
    } Comparator= { 
                    .integer = *compare_int,
                    .character = *compare_char,
                    .floating = *compare_float,
                    .double_floating = *compare_double,
                };
#endif

    static struct {
        long long float_precision;
        long long double_precision;
    } Precision = {
        .float_precision = DEFAULT_FLOAT_PRECISION,
        .double_precision = DEFAULT_DOUBLE_PRECISION
    };

    static inline void set_float_precision(int num_decimals){
        Precision.float_precision = pow(10, num_decimals);
    }
    
    static inline void set_double_precision(int num_decimals){
        Precision.double_precision = pow(10, num_decimals);
    }
#endif