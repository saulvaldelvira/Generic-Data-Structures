/**
 *  Copyright (C) 2023 - Saúl Valdelvira
 *
 *  This program is free software. You can modify
 *  and/or redistribute it under the terms of the GNU
 *  General Public License version 3, or any later version.
 *  See <https://www.gnu.org/licenses/>.
 *
 *  Email: saulvaldelvira@gmail.com
 *  Version: 17-01-2023
 */
#pragma once
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "../Util/comparator.h"

/**
 * @brief Linked List structure.
 */
typedef struct _LinkedList LinkedList;

/**
 * Initializes the linked list
 * @param data_size size of the data stored.
 * @param cmp pointer to a function that receives two void pointers and returns 1 if the first one is greater than the
 *              second, 0 if they are equal and -1 if the first one is smaller than the second one
*/
LinkedList* lnkd_list_init(size_t data_size, comparator_function_t cmp);

/**
 * Changes the comparator function of the list
 * @param cmp the new comparator function
*/
void lnkd_list_configure(LinkedList *list, comparator_function_t cmp);

/**
 * Adds the given element to the end of the list
 * @return 1 if the operation is successful
*/
int lnkd_list_append(LinkedList *list, void *element);

/**
 * Pushes back in the list the first [array_length] elements in the array
*/
int lnkd_list_append_array(LinkedList *list, void *array, size_t array_length);

/**
 * Adds the given element to the start of the list
 * @return 1 if the operation is successful
*/
int lnkd_list_push_front(LinkedList *list, void *element);

/**
 * Pushes front in the list the first [array_length] elements in the array
*/
int lnkd_list_push_front_array(LinkedList *list, void *array, size_t array_length);

/**
 * Changes the given element in the list with the replacement, if it exists.
 *
 * @return 1 if the operation is successful
*/
int lnkd_list_set(LinkedList *list, void *element, void *replacement);

/**
 * @param dest adress to store the reuslt in. MUST BE INITIALIZED.
 * @return a pointer to the first element in the list that is equal to the parameter element.
*/
void* lnkd_list_get(LinkedList *list, void *element, void *dest);

/**
 * @return The first element in the list, or NULL if it does not exists
*/
void* lnkd_list_get_front(LinkedList *list, void *dest);

/**
 * @return The last element in the list, or NULL if it does not exists
*/
void* lnkd_list_get_back(LinkedList *list, void *dest);

/**
 * Copies into the array the first [array_length] elements of the list
*/
void* lnkd_list_get_into_array(LinkedList *list, void *array, size_t array_length);

/**
 * Creates an array with the first [array_length] elements of the list.
 * @param array_length the number of elements to get. Pass GET_ALL_ELEMENTS to get all the elements in the list
 * @note Remember to free the array.
*/
void* lnkd_list_get_array(LinkedList *list, size_t array_length);

/**
* @return true if the element exists in the list
*/
bool lnkd_list_exists(LinkedList *list, void *element);

/**
 * @return the number of elements in the list
*/
size_t lnkd_list_size(LinkedList *list);

/**
* @return true if the list is empty
*/
bool lnkd_list_isempty(LinkedList *list);

/**
 * Removes the element from the list
 * @return 1 if the operation is successful
*/
int lnkd_list_remove(LinkedList *list, void *element);

/**
 * Removes the first element in the list and copies it into dest
 * @return the pointer dest, or NULL if error
*/
void* lnkd_list_pop_front(LinkedList *list, void *dest);

/**
 * Removes the last element in the list and copies it into dest
 * @return the pointer dest, or NULL if error
*/
void* lnkd_list_pop_back(LinkedList *list, void *dest);

/**
 * Removes from the list the first [array_length] elements of the array
*/
int lnkd_list_remove_array(LinkedList *list, void *array, size_t array_length);

/**
 * @return a new LinkedList with the elements of the two given lists.
 * @note No particular order of elements is guaranteed.
 * @note ATTENTION: The lists must store the same data. At least, they must have
 * the same data size. Also, the comparator function of list_1 will be taken,
 * because it is assumed that both list have the same comparator functions.
*/
LinkedList* lnkd_list_join(LinkedList *list_1, LinkedList *list_2);

/**
 * Frees the memory allocated on the list.
*/
int lnkd_list_free(LinkedList *list);

/**
 * Frees the memory allocated on the list, and resets it to it's initial state
 * @return the same pointer if sucess, NULL if the operation fails
*/
LinkedList* lnkd_list_reset(LinkedList *list);

#ifdef __cplusplus
}
#endif

#endif
