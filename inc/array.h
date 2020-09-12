#ifndef _AMO_ARRAY_H
#define _AMO_ARRAY_H

#include <stdint.h>

#define AMO_ARRAY_MIN 50
#define AMO_SCALE_FACTOR 1.5

struct amo_array {
	int   ele_num;
	int   ele_size;
	int   alloc_num;
	void  *buf;
};


/*
 * Create a new dynamic array and allocate the necessary memory with the default
 * number of element-slots.
 *
 * @size: The size of each element in the array
 *
 * Returns: Either a pointer to the created array-struct or NULL if an error
 *          occurred
 */
extern struct amo_array *amo_arr_create(int size);

/*
 * Destroy a dynamic array and free the allocated memory.
 *
 * @arr: Pointer to the array to destroy
 */
extern void amo_arr_free(struct amo_array *arr);


/*
 * Reset the element number to the base value. This function will not reallocate
 * memory but just reset the counter values.
 *
 * @arr: Pointer to the array to reset
 */
extern void amo_arr_reset(struct amo_array *arr);


/*
 * Push a new element to the end of an array and if necessary reallocate the
 * necessary memory.
 *
 * @arr: The array to push the element to
 * @in: A pointer to the element
 *
 * Returns: 0 on success or -1 if an error occurred
 */
extern int amo_arr_push(struct amo_array *arr, void *in);


/*
 * Get an element from the array and copy it's content to the given pointer.
 *
 * @arr: Pointer to the array
 * @idx: The index of the element to get
 * @out: A pointer to the copy the content of the element to
 *
 * Returns: 0 on success or -1 if an error occurred
 */
extern int amo_arr_get(struct amo_array *arr, int idx, void *out);


/*
 * Export the whole array, allocate a new buffer and attach it to the given
 * pointer. Note that to free the array, you still haver to call amo_arr_free().
 * Also after using, you're required to free the returned buffer.
 *
 * @arr: Pointer to the array to free
 * @out: the pointer to attach the exported buffer to
 *
 * Returns: The number of bytes written or -1 if an error occurred
 */
extern int amo_arr_export(struct amo_array *arr, void **out);


/*
 * Resize the array to fit it's content.
 *
 * @arr: The array to resize
 *
 * Returns: 0 on success or -1 if an error occurred
 */
extern int amo_arr_fit(struct amo_array *arr);
 

/*
 * Get the size of the array-buffer in bytes.
 *
 * @arr: The array to get the size of
 *
 * Returns: Either the size of the array-buffer in bytes or -1 if an error
 *          occurred
 */
extern int amo_arr_size(struct amo_array *arr);


/*
 * Get the current number of element in the buffer.
 *
 * @arr: Pointer to the array
 *
 * Returns: Either the number of elements in the array or -1 if an error
 *          occurred
 */
extern int amo_arr_number(struct amo_array *arr);


/*
 * Modify the element-size of the array.
 *
 * @arr: Pointer to the array
 * @size: The new element-size of the array
 *
 * Returns: 0 on success or -1 if an error occurred
 */
extern int amo_arr_modsize(struct amo_array *arr, int size);

/*
 * Display the contents of the array in the terminal.
 *
 * @arr: Pointer to the array to display
 */
extern void amo_arr_print(struct amo_array *arr);

#endif
