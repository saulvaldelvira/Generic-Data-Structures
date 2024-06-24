/*
 * Dictionary.c - Dictionary implementation.
 * Author: Saúl Valdelvira (2023)
 */
#include "Dictionary.h"
#include "./util/error.h"
#include "./Vector.h"
#include "util/compare.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

typedef struct DictionaryNode {
        void *key;
        void *value;
        enum {
                EMPTY, FULL, DELETED
        } state;
} DictionaryNode;

struct Dictionary{
        size_t value_size;      ///< Size (in bytes) of the value data type
        size_t key_size;        ///< Size (in bytes) of the key data type
        Vector *vec_elements;   ///< Vector to store the elements
        size_t n_elements;      ///< Number of elements in the Dictionary
        size_t vec_size;        ///< Capacity of the vector
        size_t prev_vec_size;   ///< Previous capacity of the vector
        float min_lf;           ///< Minimun load factor before shrinking
        float max_lf;           ///< Maximun load factor before redispersing
        enum Redispersion redispersion;         ///< Type of redispersion to apply
        hash_function_t hash;                   ///< Hashing function pointer
        destructor_function_t destructor;       ///< Destructor function pointer
};

/**
 * Calculates the load factor.
 * @param x number of elements
 * @param B capacity of the table
*/
#define LF(x,B) ((x) * 1.0 / (B))
#define DICT_INITIAL_SIZE 11

static inline int64_t abs_i64(int64_t n) { return n < 0 ? -n : n; }

/// PRIME //////////////////////////////////////////////////////////////////////

static bool is_prime(int n){
        for (int i = n-1; i > 1; --i){
                if (n % i == 0)
                        return false;
        }
        return true;
}

static int get_prev_prime(int n){
        do{
                --n;
        }while(!is_prime(n));
        return n;
}

static int get_next_prime(int n){
        do{
                ++n;
        }while(!is_prime(n));
        return n;
}

/// INITIALIZE /////////////////////////////////////////////////////////////////

/**
 * Initializes a node.
 */
static void init_node(void *node, void *args){
        (void) args;
        DictionaryNode *n = (DictionaryNode*) node;
        n->key = NULL;
        n->value = NULL;
        n->state = EMPTY;
}

/**
 * Frees the memory allocated for a node.
 */
static void free_node(void *node, void *args){
        destructor_function_t destructor = * (destructor_function_t*) args;
        DictionaryNode *n = (DictionaryNode*) node;
        if (destructor && n->value)
                destructor(n->value);
        free(n->key);
        free(n->value);
        n->key = NULL;
        n->value = NULL;
}

Dictionary* dict_init(size_t key_size, size_t value_size, hash_function_t hash_func){
        assert(hash_func && key_size > 0 && value_size > 0);
        Dictionary *dict = malloc(sizeof(*dict));
        if (!dict) return NULL;
        dict->value_size = value_size;
        dict->key_size = key_size;
        dict->min_lf = DICT_DEF_MIN_LF;
        dict->max_lf = DICT_DEF_MAX_LF;
        dict->destructor = NULL;
        dict->vec_elements = vector_init(sizeof(DictionaryNode), compare_equal);
        if (!dict->vec_elements){
                free(dict);
                return NULL;
        }
        vector_resize(dict->vec_elements, DICT_INITIAL_SIZE);
        dict->n_elements = 0;
        vector_map(dict->vec_elements, init_node, NULL);
        dict->hash = hash_func;
        dict->redispersion = DICT_DEF_REDISPERSION;
        dict->vec_size = DICT_INITIAL_SIZE;
        dict->prev_vec_size = get_prev_prime(DICT_INITIAL_SIZE);
        return dict;
}

int dict_configure(Dictionary *dict, enum Redispersion redispersion, double min_lf, double max_lf, hash_function_t hash_func){
        assert(dict);
        float min, max;
        if (min_lf > 0.0f || min_lf == DICT_NO_SHRINKING)
                min = min_lf;
        else
                min = dict->min_lf;

        if (max_lf > 0.0f)
                max = max_lf;
        else
                max = dict->max_lf;

        if (min >= max)
                return INVALID_PARAMETER_ERROR;

        dict->min_lf = min;
        dict->max_lf = max;

        if (redispersion >= 0)
                dict->redispersion = redispersion;
        if (hash_func)
                dict->hash = hash_func;

        return SUCCESS;
}

void dict_set_destructor(Dictionary *dict, destructor_function_t value_destructor){
        if (dict)
                dict->destructor = value_destructor;
}
//// REDISPERSE ///////////////////////////////////////////////////////////////

static bool is_full(void *element) {
        DictionaryNode *e = (DictionaryNode*) element;
        return e->state == FULL;
}

static void destroy_non_full(void *element, void *args) {
        DictionaryNode *e = (DictionaryNode*) element;
        destructor_function_t destructor = * (destructor_function_t*) args;
        if (e->state != FULL) {
                if (destructor)
                        destructor(e->value);
                free(e->key);
                free(e->value);
        }
        e->key = NULL;
        e->value = NULL;
}

/**
 * Redisperses the dictionary.
 * Can be used to expand or shrink the table.
 */
static int dict_redisperse(Dictionary *dict, size_t new_size){
        assert(dict->n_elements < new_size);
        /* Store FULL nodes in a temporary Vector and destroy the rest */
        Vector *elements = vector_filter(dict->vec_elements, is_full);
        vector_map(dict->vec_elements, destroy_non_full, &dict->destructor);

        /// Reset the vector
        if (new_size < dict->vec_size)
                vector_clear(dict->vec_elements);
        int status = vector_resize(dict->vec_elements, new_size);
        if (status != SUCCESS)
                return status;

        vector_map(dict->vec_elements, init_node, NULL);

        // Change vec_size and n_elements
        dict->n_elements = 0;
        if (dict->vec_size < new_size)
                dict->prev_vec_size = dict->vec_size;
        else
                dict->prev_vec_size = get_prev_prime(new_size);
        dict->vec_size = new_size;

        // Add the elements to the new dictionary
        status = SUCCESS;
        size_t n_elements = vector_size(elements);
        for (size_t i = 0; i < n_elements; ++i){
                DictionaryNode n;
                vector_at(elements, i, &n);
                if (status == SUCCESS)
                        status = dict_put(dict, n.key, n.value);
                free(n.key);
                free(n.value);
        }
        vector_free(elements);
        return status;
}

//// PUT //////////////////////////////////////////////////////////////////////

/**
 * Retuns an index to store the key into
 * @param n_it number of tries, to handle collisions.
 */
static size_t dict_get_pos(Dictionary *dict, void *key, size_t n_it){
        size_t pos = 0;
        switch (dict->redispersion){
        case LINEAR:
                pos = abs_i64(dict->hash(key)) + n_it;
                break;
        case QUADRATIC:
                pos =  abs_i64(dict->hash(key)) + n_it * n_it;
                break;
        case DOUBLE_HASHING: ;
                int h1 = abs_i64(dict->hash(key));
                int h2 = dict->prev_vec_size - h1 % dict->prev_vec_size;
                pos = h1 + n_it * h2;
                break;
        }
        return pos % dict->vec_size;
}

int dict_put(Dictionary *dict, void *key, void *value){
        assert(dict && key && value);
        size_t pos = 0;
        DictionaryNode node = {0};

        for (size_t i = 0; i < dict->vec_size; ++i){
                pos = dict_get_pos(dict, key, i);
                vector_at(dict->vec_elements, pos, &node);

                if (node.state == FULL){
                        int64_t h1 = dict->hash(key);
                        int64_t h2 = dict->hash(node.key);
                        if (h1 == h2) break;
                }else {
                        break;
                }
        }

        /* if the node is empty, we need to allocate
           memory for the key and value. */
        if (!node.key){
                node.key = malloc(dict->key_size);
                if (!node.key)
                        return ERROR;
        }
        if (!node.value){
                node.value = malloc(dict->value_size);
                if (!node.value){
                        free(node.key);
                        return ERROR;
                }
        }

        if (node.state == FULL || node.state == DELETED){
                if (node.value && dict->destructor)
                        dict->destructor(node.value);
        }

        if (node.state != FULL)
                dict->n_elements++;

        memcpy(node.key, key, dict->key_size);
        memcpy(node.value, value, dict->value_size);

        node.state = FULL;

        /* Very important!. We need to copy back the node into the vector.
           Remember that the get function returns a COPY of the element. */
        vector_set_at(dict->vec_elements, pos, &node);

        // Resize if needed
        if (LF(dict->n_elements, dict->vec_size) >= dict->max_lf){
                size_t new_size = get_next_prime(dict->vec_size * 2);
                int status = dict_redisperse(dict, new_size);
                if (status != SUCCESS)
                        return status;
        }
        return SUCCESS;
}

//// GET_EXISTS ///////////////////////////////////////////////////////////////

void* dict_get(Dictionary *dict, void *key, void *dest){
        assert(dict && key);
        for (size_t i = 0; i < dict->vec_size; i++) {
                size_t pos = dict_get_pos(dict, key, i);
                DictionaryNode node;
                vector_at(dict->vec_elements, pos, &node);
                if (node.state == FULL){
                        int64_t h1 = dict->hash(key);
                        int64_t h2 = dict->hash(node.key);
                        if (h1 == h2)
                                return memcpy(dest, node.value, dict->value_size);
                }
                if (node.state == EMPTY) break;
        }
        return NULL;
}

bool dict_exists(Dictionary *dict, void *key){
        assert(dict && key);
        for (size_t i = 0; i < dict->vec_size; i++) {
                size_t pos = dict_get_pos(dict, key, i);
                DictionaryNode node;
                vector_at(dict->vec_elements, pos, &node);
                if (node.state == FULL){
                        int64_t h1 = dict->hash(key);
                        int64_t h2 = dict->hash(node.key);
                        if (h1 == h2)
                                return true;
                }
                if (node.state == EMPTY) break;
        }
        return false;
}

/// REMOVE ////////////////////////////////////////////////////////////////////

static int dict_delete_node(Dictionary *dict, size_t pos){
        DictionaryNode node;
        vector_at(dict->vec_elements, pos, &node);
        node.state = DELETED;
        vector_set_at(dict->vec_elements, pos, &node);
        dict->n_elements--;
        if (dict->min_lf > 0 && LF(dict->n_elements, dict->vec_size) <= dict->min_lf){
                size_t new_size = get_prev_prime(dict->vec_size / 2);
                int status = dict_redisperse(dict, new_size);
                if (status != SUCCESS)
                        return status;
        }
        return SUCCESS;
}

int dict_remove(Dictionary *dict, void *key){
        assert(dict && key);
        for (size_t i = 0; i < dict->vec_size; i++){
                size_t pos = dict_get_pos(dict, key, i);
                DictionaryNode node;
                vector_at(dict->vec_elements, pos, &node);
                if (node.state == FULL){
                        int64_t h1 = dict->hash(key);
                        int64_t h2 = dict->hash(node.key);
                        if (h1 == h2)
                                return dict_delete_node(dict, pos);
                }
                if (node.state == EMPTY) break;
        }
        return ELEMENT_NOT_FOUND_ERROR;
}

//// FREE //////////////////////////////////////////////////////////////////////

static void _dict_free(Dictionary *dict){
        if (!dict)
                return;
        vector_map(dict->vec_elements, free_node, &dict->destructor);
        vector_free(dict->vec_elements);
        free(dict);
}

void (dict_free)(Dictionary *d, ...){
        if (!d)
                return;
        va_list arg;
        va_start(arg, d);
        do {
                _dict_free(d);
                d = va_arg(arg, Dictionary*);
        } while (d);
        va_end(arg);
}

void dict_clear(Dictionary *dict){
        if (!dict)
                return;
        vector_map(dict->vec_elements, free_node, &dict->destructor);
        vector_reset(dict->vec_elements);
        vector_resize(dict->vec_elements, DICT_INITIAL_SIZE);
        vector_map(dict->vec_elements, init_node, NULL);
        dict->n_elements = 0;
        dict->vec_size = DICT_INITIAL_SIZE;
        dict->prev_vec_size = get_prev_prime(DICT_INITIAL_SIZE);
}
