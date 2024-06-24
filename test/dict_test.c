#include "../src/Dictionary.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_simple(void){

        Dictionary *dic = dict_init(sizeof(int), sizeof(char), hash_int);
        int itmp = 1;
        char ctmp = 'A';
        dict_put(dic, &itmp, &ctmp);

        assert(dict_exists(dic, &itmp));
        char c;
        assert(dict_get(dic, &itmp, &c) != NULL);
        assert(c == ctmp);

        assert(dict_remove(dic, &itmp));

        assert(!dict_exists(dic, &itmp));
        assert(dict_get(dic, &itmp, &c) == NULL);
        dict_free(dic);
}

void brute(void){
        test_step("Brute");
        int n = 1000;
        Dictionary *dic = dict_init(sizeof(int), sizeof(int), hash_int);
        assert(dic);
        for (int i = 0; i < n; i++){
                assert(dict_put(dic, &i, &i) == SUCCESS);
        }
        int tmp;
        for (int i = 0; i < n; i++){
                assert(i == * (int*) dict_get(dic, &i, &tmp));
                assert(dict_exists(dic, &i));
                assert(dict_remove(dic, &i));
                assert(!dict_exists(dic, &i));
        }
        dict_free(dic);
        test_ok();
}

void config(void){
        Dictionary *dic = dict_init(sizeof(int), sizeof(int), hash_int);

        assert(dict_configure(dic, DOUBLE_HASHING, 0.05, 0.15, NULL));

        dict_free(dic);

}

void random_test(void){
        test_step("Random");
        int n = 2048;
        srand(time(0));

        // Create 2 arrays for the keys and
        // values, and shuffle them randomly
        int *keys = int_array(0,n);
        int *values = int_array(0,n);
        shuffle_array(keys, n);
        shuffle_array(values, n);

        Dictionary *dic = dict_init(sizeof(int), sizeof(int), hash_int);
        int *exp = malloc(n * sizeof(int));
        int *exp2 = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++){
                int k = keys[i];
                int v = values[i];
                exp[i] = k;
                exp2[i] = v;
                assert(dict_put(dic, &k, &v) == SUCCESS);
        }
        int tmp;
        for (int i = 0; i < n; i++){
                assert(dict_exists(dic, &exp[i]) == SUCCESS);
                assert(dict_get(dic, &exp[i], &tmp) != NULL);
                assert(tmp == exp2[i]);
        }
        free(exp);
        free(exp2);
        free(keys);
        free(values);
        dict_free(dic);
        test_ok();
}

void string_test(void){
        test_step("String");
        Dictionary *dict = dict_init(sizeof(char*), sizeof(int), hash_string);
        dict_set_destructor(dict,destroy_ptr);
        const int n = 100;
        char *strs[n];

        for (int i = 0; i < n; i++) {
                char *str = malloc(1000);
                strs[i] = str;
                sprintf(str, "%d", i);
                dict_put(dict, &str, &i);
        }

        for (int i = 0; i < n; i++) {
                int tmp;
                assert(dict_exists(dict,&strs[i]));
                assert(i == * (int*) dict_get(dict,&strs[i],&tmp));
        }
        dict_free(dict);
        test_ok();
}

// Struct test

struct key{
        int i;
        char c;
};

struct person{
        char name[30];
        int age;
};

int64_t hash_structs(const void *e_1){
        struct key *k = (struct key*) e_1;
        int64_t hash = (int64_t)k->i << sizeof(k->c);
        hash += k->c;
        return hash;
}

void struct_test(void){
        test_step("Struct");
        struct key k = {
                .i = 12,
                .c = 2
        };
        struct person p = {
                .age = 23
        };
        strcpy(p.name, "alejandro");

        Dictionary *d = dict_init(sizeof(struct key), sizeof(struct person), hash_structs);
        assert(dict_put(d, &k, &p) == SUCCESS);
        assert(dict_exists(d, &k));
        struct person per;
        assert(dict_get(d, &k, &per));
        assert(per.age == p.age);
        assert(strcmp(per.name, p.name) == 0);

        dict_free(d);
        test_ok();
}

void destructor_test(void){
	Dictionary *dict = dict_init(sizeof(int), sizeof(char*), hash_int);
	dict_set_destructor(dict, destroy_ptr);

        char *tmp = NULL;
        assert(dict_get(dict, &(int){12}, &tmp) == NULL);

	for (int i = 0; i < 120; i++){
		char *str = malloc(sizeof(char[12]));
		assert(str);
		sprintf(str, "%d", i);
		dict_put(dict, &i, &str);
	}

	// Make sure the pointers are also
	// freed when we overwrite the values
	char *str = malloc(sizeof(char[12]));
	dict_put(dict, &(int){30}, &str);

	dict_free(dict);
}

int main(void){
	test_start("Dictionary.c");


        test_simple();
        brute();
        config();
        random_test();
        string_test();
        struct_test();
	destructor_test();


	test_end("Dictionary.c");
        return 0;
}
