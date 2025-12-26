#ifndef VEC_H

#define VEC_H

#include <stddef.h>

struct vec {
  void *buf;
  size_t capacity;
  size_t len;
  size_t element_size;
};

// Initialize new vector with default capacity
void vec_new(struct vec *vec, size_t size_of_elements);
// Initialize new vector with specified capacity
void vec_with_capacity(struct vec *vec, size_t size_of_elements,
                       size_t new_capacity);
// Initialize new vector from array
void vec_from_array(struct vec *vec, size_t size_of_elements, void *array,
                    size_t array_len);

// Sets the internal capacity of `vec` to `new_capacity`
void vec_set_capacity(struct vec *vec, size_t new_capacity);

// Pushes `element` onto the end of `vec` via `memcpy`
void vec_push(struct vec *vec, void *element);

// Sets `index` of `vec` to `element`
void vec_set(struct vec *vec, size_t index, void *element);

// Gets `index` of `vec`
void *vec_get(struct vec *vec, size_t index);

// Removes `index` of `vec`
void vec_remove(struct vec *vec, size_t index);

void vec_free(struct vec *vec);

int vec_eq(struct vec *restrict vec1, struct vec *restrict vec2);

#endif