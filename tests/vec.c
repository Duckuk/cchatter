#include "../include/vec.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOLINTNEXTLINE
int main() {
  //
  // `vec_eq` test
  //
  printf("Starting vec_eq test...\n");
  {
    const int test_array[4] = {1, 2, 3, 4};

    struct vec vec1;
    struct vec vec2;

    int *buf1 = (int *)malloc(sizeof(test_array));
    memcpy(buf1, test_array, sizeof(test_array));
    vec1.buf = buf1;
    vec1.capacity = 4;
    vec1.len = 4;
    vec1.element_size = sizeof(int);

    int *buf2 = (int *)malloc(sizeof(test_array));
    memcpy(buf2, test_array, sizeof(test_array));
    vec2.buf = buf2;
    vec2.capacity = 4;
    vec2.len = 4;
    vec2.element_size = sizeof(int);

    // Test equality check
    assert(vec_eq(&vec1, &vec2) == 1);

    // Test capacity check
    vec1.capacity += 1;
    assert(vec_eq(&vec1, &vec2) == 0);
    vec1.capacity -= 1;

    // Test element size check
    vec1.element_size += 1;
    assert(vec_eq(&vec1, &vec2) == 0);
    vec1.capacity -= 1;

    // Test len check
    vec1.len += 1;
    assert(vec_eq(&vec1, &vec2) == 0);
    vec1.len -= 1;

    // Test buf check
    ((int *)vec1.buf)[0] += 1;
    assert(vec_eq(&vec1, &vec2) == 0);
    ((int *)vec1.buf)[0] -= 1;

    vec_free(&vec1);
    vec_free(&vec2);
  }
  printf("vec_eq test succeeded!\n");

  //
  // `vec_set_capacity` test
  //
  printf("Starting vec_set_capacity test...\n");
  {
    const int test_array[4] = {1, 2, 3, 4};

    struct vec vector;

    int *buf = (int *)malloc(sizeof(test_array));
    memcpy(buf, test_array, sizeof(test_array));
    vector.buf = buf;
    vector.capacity = 4;
    vector.len = 4;
    vector.element_size = sizeof(int);

    vec_set_capacity(&vector, 8);
    assert(vector.capacity == 8);
    memset(vector.buf, 0, vector.element_size * vector.capacity);

    vec_free(&vector);
  }
  printf("vec_set_capacity test succeeded!\n");

  //
  // `vec_with_capacity` test
  //
  printf("Starting vec_with_capacity test...\n");
  {
    const int test_size = 8;

    struct vec actual_vector;
    vec_with_capacity(&actual_vector, sizeof(int), test_size);
    memset(actual_vector.buf, 0, sizeof(int) * test_size);

    void *buf = malloc(sizeof(int) * test_size);
    memset(buf, 0, sizeof(int) * test_size);
    struct vec ref_vector = {.buf = buf,
                             .capacity = test_size,
                             .element_size = sizeof(int),
                             .len = 0};

    assert(vec_eq(&actual_vector, &ref_vector));

    vec_free(&actual_vector);
    vec_free(&ref_vector);
  }
  printf("vec_with_capacity test succeeded!\n");

  //
  // `vec_from_array` test
  //
  printf("Starting vec_from_array test...\n");
  {
    const int test_size = 8;
    const int test_array[test_size] = {1, 2, 3, 4, 5, 6, 7, 8};

    struct vec actual_vector;
    vec_from_array(&actual_vector, sizeof(int), (void *)test_array, test_size);

    struct vec ref_vector;
    void *buf = malloc(sizeof(test_array));
    memcpy(buf, test_array, sizeof(test_array));
    ref_vector.buf = buf;
    ref_vector.capacity = sizeof(test_array);
    ref_vector.element_size = sizeof(int);
    ref_vector.len = test_size;

    assert(vec_eq(&actual_vector, &ref_vector));

    vec_free(&actual_vector);
    vec_free(&ref_vector);
  }
  printf("vec_from_array test succeeded!\n");

  //
  // `vec_push` test
  //
  printf("Starting vec_push test...\n");
  {
    const int test_size = 8;
    const int test_array[test_size] = {1, 2, 3, 4, 5, 6, 7, 8};

    struct vec actual_vector;
    vec_from_array(&actual_vector, sizeof(int), (void *)test_array, test_size);
    int e = 9;
    vec_push(&actual_vector, &e);

    const int ref_array[test_size + 1] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    struct vec ref_vector;
    vec_from_array(&ref_vector, sizeof(int), (void *)ref_array, test_size + 1);

    assert(vec_eq(&actual_vector, &ref_vector));

    vec_free(&actual_vector);
    vec_free(&ref_vector);
  }
  printf("vec_push test succeeded!\n");

  //
  // `vec_set` test
  //
  printf("Starting vec_set test...\n");
  {
    const int test_size = 8;
    const int test_array[test_size] = {1, 2, 3, 4, 5, 6, 7, 8};

    struct vec actual_vector;
    vec_from_array(&actual_vector, sizeof(int), (void *)test_array, test_size);
    int e = 0;
    vec_set(&actual_vector, 0, &e);
    e = 1;
    vec_set(&actual_vector, 4, &e);
    e = 2;
    vec_set(&actual_vector, 7, &e);

    const int ref_array[test_size] = {0, 2, 3, 4, 1, 6, 7, 2};
    struct vec ref_vector;
    vec_from_array(&ref_vector, sizeof(int), (void *)ref_array, test_size);

    assert(vec_eq(&actual_vector, &ref_vector));

    vec_free(&actual_vector);
    vec_free(&ref_vector);
  }
  printf("vec_set test succeeded!\n");

  //
  // `vec_get` test
  //
  printf("Starting vec_get test...\n");
  {
    const int test_size = 8;
    const int test_array[test_size] = {1, 2, 3, 4, 5, 6, 7, 8};

    struct vec actual_vector;
    vec_from_array(&actual_vector, sizeof(int), (void *)test_array, test_size);
    assert(*(int *)vec_get(&actual_vector, 0) == 1);
    assert(*(int *)vec_get(&actual_vector, 4) == 5);
    assert(*(int *)vec_get(&actual_vector, 7) == 8);

    vec_free(&actual_vector);
  }
  printf("Passed!\n");

  //
  // `vec_remove` test
  //
  printf("Starting vec_remove test...\n");
  {
    const int test_size = 8;
    const int test_array[test_size] = {1, 2, 3, 4, 5, 6, 7, 8};

    struct vec actual_vector;
    vec_from_array(&actual_vector, sizeof(int), (void *)test_array, test_size);
    vec_remove(&actual_vector, 0);

    {
      const int ref_array[test_size - 1] = {2, 3, 4, 5, 6, 7, 8};
      struct vec ref_vector;
      vec_from_array(&ref_vector, sizeof(int), (void *)ref_array,
                     test_size - 1);
      assert(vec_eq(&actual_vector, &ref_vector));
      vec_free(&ref_vector);
    }

    vec_remove(&actual_vector, 6);

    {
      const int ref_array[test_size - 2] = {2, 3, 4, 5, 6, 7};
      struct vec ref_vector;
      vec_from_array(&ref_vector, sizeof(int), (void *)ref_array,
                     test_size - 2);
      assert(vec_eq(&actual_vector, &ref_vector));
      vec_free(&ref_vector);
    }

    vec_free(&actual_vector);
  }
  printf("vec_remove test succeeded!\n");

  return 0;
}