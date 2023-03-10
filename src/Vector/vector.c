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

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Util/error.h"
#include "../Util/definitions.h"
#include <string.h>

#include "vector.h"
#define VECTOR_DEFAULT_SIZE 12

struct _Vector {
	size_t n_elements;
	size_t max_elements;
	size_t data_size;
	// Comparator function for 2 elements
	comparator_function_t compare;
	void *elements;
};

/// INITIALIZE ////////////////////////////////////////////////////////////////

Vector* vector_empty(size_t data_size, comparator_function_t cmp){
	if (data_size <= 0){
		printerr_data_size(vector_empty);
		return NULL;
	}
	return vector_init(data_size, VECTOR_DEFAULT_SIZE, cmp);
}

Vector* vector_init(size_t data_size, size_t max_elements, comparator_function_t cmp){
	if (data_size <= 0){
		printerr_data_size(vector_init);
		return NULL;
	}
	if (!cmp){
		printerr_null_param(vector_init);
		return NULL;
	}

	Vector *list = malloc(sizeof(*list));
	void *elements = malloc(max_elements * data_size);

	if (!list || !elements){
		printerr_allocation(vector_init);
		return NULL;
	}

	list->elements = elements;
	list->data_size = data_size;
	list->n_elements = 0;
	list->max_elements = max_elements;
	list->compare = cmp;
	return list;
}

void vector_configure(Vector *list, comparator_function_t cmp){
	if (!list || !cmp){
		printerr_null_param(vector_configure);
		return;
	}
	list->compare = cmp;
}

size_t vector_get_data_size(Vector *list){
	if (!list){
		printerr_null_param(vector_data_size);
		return 0;
	}
	return list->data_size;
}

comparator_function_t vector_get_comparator(Vector *list){
	if (!list){
		printerr_null_param(vector_get_comparator);
		return NULL;
	}
	return list->compare;
}

///////////////////////////////////////////////////////////////////////////////

/// ADD-SET ///////////////////////////////////////////////////////////////////////

int vector_append(Vector *list, void *element){
	if (!list || !element){
		printerr_null_param(vector_append);
		return NULL_PARAMETER_ERROR;
	}
	if(list->n_elements >= list->max_elements){ // If the list is empty, double the array size
		list->max_elements *= 2;
		list->elements = realloc(list->elements, list->max_elements * list->data_size);
		if(!list->elements){
			printerr_allocation(vector_append);
			return ALLOCATION_ERROR;
		}
	}

	void *tmp = void_offset(list->elements, list->n_elements * list->data_size);
	tmp = memmove(tmp , element, list->data_size);
	if (!tmp){
		printerr_memory_op(vector_append);
		return MEMORY_OP_ERROR;
	}

	list->n_elements++;
	return SUCCESS;
}

int vector_push_front(Vector *list, void *element){
	if (!list || !element){
		printerr_null_param(vector_push_front);
		return NULL_PARAMETER_ERROR;
	}
	if(list->n_elements >= list->max_elements){ // If the list is empty, double the array size
		list->max_elements *= 2;
		list->elements = realloc(list->elements, list->max_elements * list->data_size);
		if(!list->elements){
			printerr_allocation(vector_push_front);
			return ALLOCATION_ERROR;
		}
	}

	void *tmp = void_offset(list->elements, list->data_size);
	tmp = memmove(tmp, list->elements, list->n_elements * list->data_size);
	void *dst = memcpy(list->elements, element, list->data_size);
	if (!tmp || !dst){
		printerr_memory_op(vector_push_front);
		return MEMORY_OP_ERROR;
	}
	list->n_elements++;
	return SUCCESS;
}

int vector_append_array(Vector *list, void *array, size_t array_length){
	if (!list || !array){
		printerr_null_param(vector_append_array);
		return NULL_PARAMETER_ERROR;
	}

	if (list->n_elements + array_length > list->max_elements){
		list->max_elements = (list->max_elements + array_length) * 2;
		list->elements = realloc(list->elements, list->max_elements * list->data_size);
		if(!list->elements){
			printerr_allocation(vector_append_array);
			return ALLOCATION_ERROR;
		}
	}
	void *tmp = void_offset(list->elements, list->n_elements * list->data_size);
	tmp = memcpy(tmp, array, array_length * list->data_size);
	if (!tmp){
		printerr_memory_op(vector_append_array);
		return MEMORY_OP_ERROR;
	}
	list->n_elements += array_length;

	return SUCCESS;
}

int vector_push_front_array(Vector *list, void *array, size_t array_length){
	if (!list || !array){
		printerr_null_param(vector_push_front_array);
		return NULL_PARAMETER_ERROR;
	}
	if (list->n_elements + array_length > list->max_elements){
		list->max_elements = (list->max_elements + array_length) * 2;
		list->elements = realloc(list->elements, list->max_elements * list->data_size);
		if(!list->elements){
			printerr_allocation(vector_push_front_array);
			return ALLOCATION_ERROR;
		}
	}
	void *tmp = void_offset(list->elements, array_length * list->data_size);
	tmp = memmove(tmp, list->elements, list->n_elements * list->data_size);
	if (!tmp){
		printerr_memory_op(vector_push_front_array);
		return MEMORY_OP_ERROR;
	}
	tmp = memmove(list->elements, array, array_length * list->data_size);
	if (!tmp){
		printerr_memory_op(vector_push_front_array);
		return MEMORY_OP_ERROR;
	}
	list->n_elements += array_length;
	return SUCCESS;
}

int vector_set_at(Vector *list, size_t index, void *element){
	if (!list || !element){
		printerr_null_param(vector_set_at);
		return NULL_PARAMETER_ERROR;
	}
	if (index >= list->n_elements){
		printerr_out_of_bounds(index, vector_set_at);
		return INDEX_BOUNDS_ERROR;
	}

	void *tmp = void_offset(list->elements, index * list->data_size);
	tmp = memmove(tmp, element , list->data_size);
	if(!tmp){
		printerr_memory_op(vector_set_at);
		return MEMORY_OP_ERROR;
	}
	return SUCCESS;
}

int vector_set(Vector *list, void *element, void *replacement){
	if (!list || !element || !replacement){
		printerr_null_param(vector_set);
		return NULL_PARAMETER_ERROR;
	}
	void *ptr;
	for (size_t i=0; i < list->n_elements; i++){
		ptr = void_offset(list->elements, i * list->data_size);
		if ((*list->compare) (ptr, element) == 0){
			ptr = memmove(ptr, replacement, list->data_size);
			if(ptr){
				return SUCCESS;
			}else{
				printerr_memory_op(vector_set);
				return MEMORY_OP_ERROR;
			}
		}
	}
	return ELEMENT_NOT_FOUND_ERROR;
}

int vector_insert(Vector *list, void *element, void *insert){
	if (!list || !element || !insert){
		printerr_null_param(vector_insert_at);
		return NULL_PARAMETER_ERROR;
	}
	index_t index = vector_indexof(list, element);
	if (index.status != SUCCESS){
		return index.status;
	}
	return vector_insert_at(list, index.value, insert);
}

int vector_insert_at(Vector *list, size_t index, void *element){
	if (!list || !element){
		printerr_null_param(vector_insert_at);
		return NULL_PARAMETER_ERROR;
	}
	if (list->n_elements == list->max_elements){
		list->max_elements = list->max_elements * 2;
		list->elements = realloc(list->elements, list->max_elements * list->data_size);
		if(!list->elements){
			printerr_allocation(vector_insert_at);
			return ALLOCATION_ERROR;
		}
	}
	void *src = void_offset(list->elements, index * list->data_size);
	void *dst = void_offset(list->elements, (index + 1) * list->data_size);
	int n = list->n_elements - index; // number of elements to shift
	dst = memmove(dst, src, n * list->data_size); // Shift elements to the right
	src = memcpy(src, element, list->data_size); // Insert the element
	if (!dst || !src){
		printerr_memory_op(vector_insert_at);
		return MEMORY_OP_ERROR;
	}
	list->n_elements++;
	return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

/// REMOVE ////////////////////////////////////////////////////////////////////

int vector_remove_at(Vector *list, size_t index){
	if (!list){
		printerr_null_param(vector_remove_at);
		return NULL_PARAMETER_ERROR;
	}
	if (index >= list->n_elements){
		printerr_out_of_bounds(index, vector_remove_at);
		return INDEX_BOUNDS_ERROR;
	}

	if (index < list->n_elements - 1){
		size_t leftover = (list->n_elements - index - 1) * list->data_size;
		void *dst = void_offset(list->elements, index * list->data_size);
		void *src = void_offset(list->elements, (index+1) * list->data_size);
		if(!memmove(dst, src, leftover)){
			printerr_memory_op(vector_remove_at);
			return MEMORY_OP_ERROR;
		}
	}
	list->n_elements--;
	return SUCCESS;
}

int vector_remove(Vector *list, void *element){
	if (!list || !element){
		printerr_null_param(vector_remove);
		return NULL_PARAMETER_ERROR;
	}
	index_t i = vector_indexof(list, element);
	if (!i.status){
		return i.status;
	}
	return vector_remove_at(list, i.value);
}

void* vector_pop_front(Vector *list, void *dest){
	if (!list || !dest){
		printerr_null_param(vector_pop_front);
		return NULL;
	}
	if (list->n_elements == 0){
		return NULL;
	}
	dest = memcpy(dest, list->elements, list->data_size);
	if (!dest){
		printerr_memory_op(vector_pop_front);
		return NULL;
	}
	vector_remove_at(list, 0);
	return dest;
}

void* vector_pop_back(Vector *list, void *dest){
	if (!list || !dest){
		printerr_null_param(vector_pop_back);
		return NULL;
	}
	if (list->n_elements == 0){
		return NULL;
	}
	void *src = void_offset(list->elements, (list->n_elements - 1) * list->data_size);
	dest = memcpy(dest, src, list->data_size);
	if (!dest){
		printerr_memory_op(vector_pop_front);
		return NULL;
	}
	vector_remove_at(list, list->n_elements - 1);
	return dest;
}

int vector_remove_array(Vector *list, void *array, size_t array_length){
	if (!list || !array){
		printerr_null_param(vector_remove_array);
		return NULL_PARAMETER_ERROR;
	}
	void *tmp;
	int status;
	for (size_t i = 0; i < array_length; i++){
		tmp = void_offset(array, i * list->data_size);
		status = vector_remove(list, tmp);
		if (status != SUCCESS){
			return status;
		}
	}
	return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

/// SEARCH-GET ////////////////////////////////////////////////////////////////

index_t vector_indexof(Vector *list, void *element){
	if (!list || !element){
		printerr_null_param(vector_indexof);
		return index_t(0,NULL_PARAMETER_ERROR);
	}

	void *ptr; // Current element in the iteration
	for (size_t i=0; i < list->n_elements; i++){
		ptr = void_offset(list->elements, i * list->data_size);
		if ((*list->compare) (ptr, element) == 0){
			return index_t(i,SUCCESS);
		}
	}
	return index_t(0,ELEMENT_NOT_FOUND_ERROR);
}

bool vector_exists(Vector *list, void *element){
	if (!list){
		printerr_null_param(vector_exists);
		return false;
	}
	return vector_indexof(list, element).status == SUCCESS;
}

bool vector_isempty(Vector *list){
	if (!list){
		printerr_null_param(vector_isempty);
		return false;
	}
	return list->n_elements == 0;
}

void* vector_get_at(Vector *list, size_t index, void *dest){
	if (!list || !dest){
		printerr_null_param(vector_get_at);
		return NULL;
	}
	if (index >= list->n_elements){
		printerr_out_of_bounds(index, vector_get_at);
		return NULL;
	}
	void *tmp = void_offset(list->elements, index * list->data_size);
	tmp = memcpy(dest, tmp, list->data_size);
	return tmp;
}

void* vector_get(Vector *list, void *element, void *dest){
	if (!list || !element || !dest){
		printerr_null_param(vector_get);
		return NULL;
	}
	index_t index = vector_indexof(list, element);
	if (!index.status){
		return NULL;
	}
	return vector_get_at(list, index.value, dest);
}

void* vector_get_front(Vector *list, void *dest){
	if (!list || !dest){
		printerr_null_param(vector_get_front);
		return NULL;
	}
	return vector_get_at(list, 0, dest);
}

void* vector_get_back(Vector *list, void *dest){
	if (!list || !dest){
		printerr_null_param(vector_get_back);
		return NULL;
	}
	return vector_get_at(list, list->n_elements - 1, dest);
}

void* vector_get_into_array(Vector *list, void *array, size_t array_length){
	if (!list || !array){
		printerr_null_param(vector_get_into_array);
		return NULL;
	}
	if (array_length > list->n_elements){
		array_length = list->n_elements;
	}
	if (!memcpy(array, list->elements, array_length * list->data_size)){
		printerr_memory_op(vector_get_into_array);
		return NULL;
	}

	return array;
}

void* vector_get_array(Vector *list, size_t array_length){
	if (!list){
		printerr_null_param(vector_get_array);
		return NULL;
	}
	if (array_length == GET_ALL_ELEMENTS || array_length > list->n_elements){
		array_length = list->n_elements;
	}
	void *array = malloc(list->data_size * array_length);
	if (!array){
		return NULL;
	}
	if (!vector_get_into_array(list, array, array_length)){
		free(array);
		return NULL;
	}
	return array;
}

///////////////////////////////////////////////////////////////////////////////

/// OTHER /////////////////////////////////////////////////////////////////////

static void* get_position(Vector *list, size_t index){
	return void_offset(list->elements, index * list->data_size);
}

int vector_swap(Vector *list, size_t index_1, size_t index_2){
	if (!list){
		printerr_null_param(vector_swap);
		return NULL_PARAMETER_ERROR;
	}
	if (index_1 >= list->n_elements){
		printerr_out_of_bounds(index_1, vector_swap);
		return INDEX_BOUNDS_ERROR;
	}
	if (index_2 >= list->n_elements){
		printerr_out_of_bounds(index_2, vector_swap);
		return INDEX_BOUNDS_ERROR;
	}

	void *tmp = malloc(list->data_size);
	if (!tmp){
		printerr_allocation(vector_swap);
		return ALLOCATION_ERROR;
	}

	if (!vector_get_at(list, index_1, tmp)){
		return ERROR;
	}

	void *e1 = get_position(list, index_1);
	void *e2 = get_position(list, index_2);
	if(!memcpy(e1, e2, list->data_size)){
		printerr_memory_op(vector_swap);
		return MEMORY_OP_ERROR;
	}

	if(!memcpy(e2, tmp, list->data_size)){
		printerr_memory_op(vector_swap);
		return MEMORY_OP_ERROR;
	}

	free(tmp);

	return SUCCESS;

}

int vector_compare(Vector *list, size_t index_1, size_t index_2){
	if (!list){
		printerr_null_param(vector_swap);
		return NULL_PARAMETER_ERROR;
	}
	void *e1 = get_position(list, index_1);
	void *e2 = get_position(list, index_2);
	return (*list->compare) (e1, e2);
}

size_t vector_size(Vector *list){
	if (!list){
		printerr_null_param(vector_size);
		return false;
	}
	return list->n_elements;
}

Vector* vector_join(Vector *list_1, Vector *list_2){
	if (!list_1 || !list_2){
		printerr_null_param(vector_join);
		return NULL;
	}
	if (list_1->data_size != list_2->data_size){
		fprintf(stderr, "ERROR: the lists have different data sizes. In function vector_join\n");
		return NULL;
	}

	size_t n_elements = list_1->n_elements + list_2->n_elements;
	if (n_elements < VECTOR_DEFAULT_SIZE){
		n_elements = VECTOR_DEFAULT_SIZE;
	}
	Vector *list_joint = vector_init(list_1->data_size, n_elements, list_1->compare);
	if (!list_joint){
		return NULL;
	}

	int status;

	// Get the elements of the first list
	void *tmp = vector_get_array(list_1, list_1->n_elements);
	if (tmp != NULL){
		// Add the elements of the first list
		status = vector_append_array(list_joint, tmp, list_1->n_elements);
		free(tmp);
		if (status != SUCCESS){
			goto exit_err;
		}
	}

	// Get the elements of the second list
	tmp = vector_get_array(list_2, list_2->n_elements);
	if (tmp != NULL){
		// Add the elements of the second list
		status = vector_append_array(list_joint, tmp, list_2->n_elements);
		free(tmp);
		if (status != SUCCESS){
			exit_err:
			free(list_joint);
			return NULL;
		}
	}

	return list_joint;
}

///////////////////////////////////////////////////////////////////////////////

/// FREE //////////////////////////////////////////////////////////////////////

int vector_free(Vector *list){
	if (!list){
		printerr_null_param(vector_free);
		return NULL_PARAMETER_ERROR;
	}
	free(list->elements);
	free(list);
	return SUCCESS;
}

Vector* vector_reset(Vector *list){
	if (!list){
		printerr_null_param(vector_reset);
		return NULL;
	}
	free(list->elements);
	list->elements = NULL;
	list->elements = malloc(VECTOR_DEFAULT_SIZE * list->data_size);

	if (!list->elements){
		printerr_allocation(vector_reset);
		return NULL;
	}

	list->n_elements = 0;
	list->max_elements = VECTOR_DEFAULT_SIZE;
	return list;
}
