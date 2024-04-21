#include "../src/Vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct Person {
        int age;
        char *name;
};

void destroy_person(void *p){
        struct Person person = * (struct Person*) p;
        free(person.name);
}

int main(void){
        Vector *v = vector_init(sizeof(char*), compare_equal);
        // The destructor will treat every element in the
        // vector as a pointer that must be freed
        vector_set_destructor(v, destroy_ptr);

        // Allocate 100 strings, and add them to the vector
        for (int i = 0; i < 100; i++){
                char *ptr = malloc(sizeof(char[4]));
                sprintf(ptr, "%d", i);
                vector_append(v, &ptr);
        }
        // When the vector is freed, it takes care of cleaning it's content
        vector_free(v);

        /**
         * Another example. This time is a struct, and only one of it's
         * members must be freed. We define a custom function "destroy_person"
        */
        v = vector_init(sizeof(struct Person), compare_equal);
        vector_set_destructor(v, destroy_person);
        for (int i = 0; i < 100; i++){
                struct Person p = {
                        .age = i,
                        .name = malloc(sizeof(char[15]))
                };
                sprintf(p.name, "Person %d", i);
                vector_append(v, &p);
        }
        vector_free(v);


        /**
         * Remove function example
        */
        v = vector_init(sizeof(char*), compare_equal);
        vector_set_destructor(v, destroy_ptr);

        for (int i = 0; i < 100; i++){
                char *ptr = malloc(sizeof(char[4]));
                sprintf(ptr, "%d", i);
                vector_append(v, &ptr);
        }

        for (int i = 0; i < 100; i++)
                vector_remove_back(v);

        /** The above is the same as:
        for (int i = 0; i < 100; i++) {
                void *ptr;
                vector_pop_back(v, &ptr);
                free(ptr);
        }
        */

        // The vector is empty, but no memory is leaked, since
        // the previous remove calls freed the pointers
        size_t size = vector_size(v);
        assert(size == 0);

        vector_free(v);
}

/**
 * $ gcc destructors.c ../src/Vector.c ../src/util/compare.c -o destructors
 * $ valgrind destructors
 * Output:
 *      HEAP SUMMARY:
 *      ==11375==     in use at exit: 0 bytes in 0 blocks
 *      ==11375==   total heap usage: 318 allocs, 318 frees, 14,348 bytes allocated
 *      ==11375==
 *      ==11375== All heap blocks were freed -- no leaks are possible
*/
