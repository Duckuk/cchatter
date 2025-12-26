#include "vec.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define DEFAULT_CAPACITY 32

void vec_new(struct vec *vec, size_t size_of_elements) {
  vec_with_capacity(vec, size_of_elements, DEFAULT_CAPACITY);
}

void vec_with_capacity(struct vec *vec, size_t size_of_elements,
                       size_t capacity) {
  vec->buf = malloc(size_of_elements * capacity);
  vec->capacity = capacity;
  vec->len = 0;
  vec->element_size = size_of_elements;
}

void vec_from_array(struct vec *vec, size_t size_of_elements, void *array,
                    size_t array_len) {
  size_t capacity = MAX(array_len, DEFAULT_CAPACITY);
  vec->buf = malloc(size_of_elements * capacity);
  memcpy(vec->buf, array, size_of_elements * array_len);
  vec->capacity = capacity;
  vec->len = array_len;
  vec->element_size = size_of_elements;
}

void vec_set_capacity(struct vec *vec, size_t new_capacity) {
  if (new_capacity < vec->len) {
    fprintf(stderr, "Error: cannot reduce vec capacity below length!");
    exit(1);
  }

  void *new_buf = realloc(vec->buf, vec->element_size * new_capacity);
  if (new_buf == NULL) {
    fprintf(stderr, "Error: failed to set capacity of vec");
    exit(1);
  }
  vec->buf = new_buf;
  vec->capacity = new_capacity;
}

void vec_push(struct vec *vec, void *element) {
  if (vec->len + 1 > vec->capacity) {
    vec_set_capacity(vec, MAX(vec->capacity * 2, 1));
  }

  size_t offset = vec->element_size * vec->len;
  memcpy(vec->buf + offset, element, vec->element_size);
  vec->len += 1;
}

void vec_set(struct vec *vec, size_t index, void *element) {
  if (index >= vec->len) {
    fprintf(stderr, "Error: out of bounds vec_set");
    exit(1);
  }

  size_t offset = vec->element_size * index;
  memcpy(vec->buf + offset, element, vec->element_size);
}

void *vec_get(struct vec *vec, size_t index) {
  if (index >= vec->len) {
    fprintf(stderr, "Error: out of bounds vec_get");
    exit(1);
  }

  size_t offset = vec->element_size * index;
  return vec->buf + offset;
}

void vec_remove(struct vec *vec, size_t index) {
  if (index >= vec->len) {
    fprintf(stderr, "Error: out of bounds vec_remove");
    exit(1);
  }

  if (index == vec->len - 1) {
    vec->len -= 1;
    return;
  }

  size_t offset = vec->element_size * index;
  memmove(vec->buf + offset, vec->buf + offset + vec->element_size,
          vec->element_size * (vec->len - index));
  vec->len -= 1;
}

void vec_free(struct vec *vec) { free(vec->buf); }

int vec_eq(struct vec *restrict vec1, struct vec *restrict vec2) {
  if (vec1->capacity != vec2->capacity) {
    return 0;
  }

  if (vec1->element_size != vec2->element_size) {
    return 0;
  }

  if (vec1->len != vec2->len) {
    return 0;
  }

  if (memcmp(vec1->buf, vec2->buf, vec1->element_size * vec1->len) != 0) {
    return 0;
  }

  return 1;
}