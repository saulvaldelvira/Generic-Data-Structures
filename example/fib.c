/**
 * Prints the fibonacci sequence, using
 * a Vector to cache the values.
 *
 * $ gcc fib.c ../src/Vector.c ../src/util/compare.c -o fib
 * $ ./fib 12
 *   144
 * $ ./fib 3 8
 *   2
 *   3
 *   5
 *   8
 *   13
 */
#include "../src/Vector.h"
#include <stdio.h>
#include <stdlib.h> // atoi

Vector *fib_cache;

double fib(size_t n){
	double f;
	if (vector_size(fib_cache) > n){
		vector_at(fib_cache, n, &f);
	}else{
		for (size_t i = vector_size(fib_cache); i <= n; i++){
			double tmp;
			vector_at(fib_cache, i - 1, &tmp);
			f = tmp;
			vector_at(fib_cache, i - 2, &tmp);
			f += tmp;
			vector_append(fib_cache, &f);
		}
	}
	return f;
}

int main(int argc, char *argv[]){
	fib_cache = vector_init(sizeof(double), compare_double) ;
	vector_append(fib_cache, &(double){0.0});
	vector_append(fib_cache, &(double){1.0});

	int start = 0;
	int end   = 10;
	if (argc > 1){
		start = atoi(argv[1]);
		if (argc > 2)
			end = atoi(argv[2]);
		else
			end = start;
	}

	for (int i = start; i <= end; i++){
		double f = fib(i);
		printf("%.0f\n", f);
	}

	vector_free(fib_cache);
	return 0;
}
