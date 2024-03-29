/*
 * Heap.h - Binary Heap definition.
 * Author: Saúl Valdelvira (2023)
 */
#pragma once
#ifndef HEAP_H
#define HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "./util/definitions.h"
#include "./util/compare.h"

typedef struct Heap Heap;

/**
 * @return a new Binary Heap of mins with the given data size and comparator function
*/
Heap* heap_init(size_t data_size, comparator_function_t cmp);

/**
 * Sets the comparator function of the heap
 * @param cmp the new comparator function
*/
void heap_set_comparator(Heap *heap, comparator_function_t cmp);

/**
 * Sets the destructor function of the heap
*/
void heap_set_destructor(Heap *heap, destructor_function_t destructor);

/**
 * Adds the element into de heap
*/
int heap_add(Heap *heap, void *element);

/**
 * Adds[array_length] elements from [array] into de heap
*/
int heap_add_array(Heap *heap, void *array, size_t array_length);

/**
 * Gets the min element in the array.
*/
void* heap_pop_min(Heap *heap, void *dest);

/**
 * Allocs an array with [array_length] elements and fills it with the elements in the heap
 * @param array_length the number of elements to get. Pass 0 to get all the elements in the heap
*/
void* heap_get_array(Heap *heap, size_t array_length);

/**
 * Fills [array] with the first [array_length] elements from the heap.
 * @param array_length the number of elements to get. Pass GET_ALL_ELEMENTS to get all the elements in the heap
*/
void* heap_get_into_array(Heap *heap, void *array, size_t array_length);

/**
 * Copies to dest the min element in the heap, without deleting it
 * @return dest, or NULL if error.
*/
void* heap_peek(Heap *heap, void *dest);

/**
 * Removes an element from the heap.
 * @return 1 if the operation is successful
*/
int heap_remove(Heap *heap, void *element);

/**
 * Removes all the elements from the heap.
*/
void heap_clear(Heap *heap);

/**
 * Changes the priority of element to replacement
*/
int heap_change_priority(Heap *heap, void *element, void *replacement);

/**
 * @return true if the element exists in the heap
*/
bool heap_exists(Heap *heap, void *element);

/**
 * @return the number of elements in the heap
*/
size_t heap_size(Heap *heap);

/**
 * @return true if the heap is empty
*/
bool heap_isempty(Heap *heap);

void heap_free(Heap *h, ...);

/**
 * Frees all the given heaps.
 */
#define heap_free(...) heap_free(__VA_ARGS__, NULL)

#ifdef __cplusplus
}
#endif

#endif // HEAP_H
