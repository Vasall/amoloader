#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern struct amo_array *amo_arr_create(int size)
{
	struct amo_array *arr;

	if(!(arr = malloc(sizeof(struct amo_array))))
		return NULL;

	arr->ele_num = 0;
	arr->ele_size = size;
	arr->alloc_num = AMO_ARRAY_MIN;

	if(!(arr->buf = malloc(arr->alloc_num * arr->ele_size)))
		goto err_free_arr;

	return arr;

err_free_arr:
	free(arr);
	return NULL;
}


extern void amo_arr_free(struct amo_array *arr)
{
	if(!arr)
		return;

	/* Free the element-buffer */
	free(arr->buf);

	/* Free the array-struct */
	free(arr);
}


extern void amo_arr_reset(struct amo_array *arr)
{
	if(!arr)
		return;

	arr->ele_num = AMO_ARRAY_MIN;
}


extern int amo_arr_push(struct amo_array *arr, void *in)
{
	void *ptr = NULL;

	if(!arr || !in)
		return -1;

	/* Resize the buffer if necessary */
	if(arr->ele_num + 1 > arr->alloc_num) {
		arr->alloc_num *= AMO_SCALE_FACTOR;

		/* Reallocate memory */
		if(!(ptr = realloc(arr->buf, arr->alloc_num * arr->ele_size)))
			return -1;

		arr->buf = ptr;
	}

	/* Copy element into buffer */
	ptr = (uint8_t *)arr->buf + arr->ele_num * arr->ele_size;
	memcpy(ptr, in, arr->ele_size);

	/* Increment element-number */
	arr->ele_num += 1;

	/* Return the number of elements */
	return arr->ele_num;
}


extern int amo_arr_get(struct amo_array *arr, int idx, void *out)
{
	if(!arr)
		return -1;

	if(idx >= arr->ele_num || idx < 0)
		return -1;

	memcpy(out, (char *)arr->buf + (idx * arr->ele_size), arr->ele_size);
	return 0;
}


extern int amo_arr_export(struct amo_array *arr, void **out)
{
	void *ptr;
	int size;

	if(!arr)
		return -1;

	size = arr->ele_num * arr->ele_size;

	/* Allocate memory for output-buffer */
	if(!(ptr = malloc(size)))
		return -1;

	/* Copy content into buffer */
	memcpy(ptr, arr->buf, size);

	*out = ptr;
	return size;
}


extern int amo_arr_fit(struct amo_array *arr)
{
	void *ptr;
	int size;

	if(!arr)
		return -1;

	if(arr->ele_num != arr->alloc_num) {
		size = arr->ele_num * arr->ele_size;
		if(!(ptr = realloc(arr->buf, size)))
			return -1;
		arr->buf = ptr;

		arr->alloc_num = arr->ele_num;
	}

	return 0;
}


extern int amo_arr_size(struct amo_array *arr)
{
	if(!arr)
		return -1;

	return arr->ele_num * arr->ele_size;
}


extern int amo_arr_number(struct amo_array *arr)
{
	if(!arr)
		return -1;

	return arr->ele_num;
}


extern int amo_arr_modsize(struct amo_array *arr, int size)
{
	int tmp;
	void *ptr;
		
	if(!arr)
		return -1;

	tmp = (arr->ele_size * arr->alloc_num) / size;

	if(!(ptr = realloc(arr->buf, tmp * size)))
		return -1;

	arr->buf = ptr;
	arr->ele_num = 0;
	arr->ele_size = size;
	arr->alloc_num = tmp;
	return 0;
}


extern void amo_arr_print(struct amo_array *arr)
{
	int i;
	uint8_t val;

	printf("Number: %d | Allocated: %d\n", arr->ele_num, arr->alloc_num);

	for(i = 0; i < arr->ele_num; i++) {
		printf("%d: ", i);
		val = *((uint8_t *)arr->buf + (i * arr->ele_size));
		printf("%x ", val);	
		printf("\n");
	}
}
