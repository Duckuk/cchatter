#include "../include/hash_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define expect_termination(fn)                                                 \
  {                                                                            \
    size_t pid = fork();                                                       \
    if (pid == -1) {                                                           \
      return 1;                                                                \
    } else if (pid > 0) {                                                      \
      int status = 0;                                                          \
      waitpid(pid, &status, 0);                                                \
      assert(status != 0);                                                     \
    } else {                                                                   \
      fn;                                                                      \
      _exit(0);                                                                \
    }                                                                          \
  }

int main() {
  struct HashTable table;
  hash_table_new(&table, sizeof(char[32]), sizeof(int));

  for (size_t i = 0; i < table.capacity; i++) {
    assert(table._buffer[i] == NULL);
  }

  char test_key[32] = "test-key-1";
  int test_element = 8;

  // Test that a non-existing element returns NULL
  assert(hash_table_get(&table, test_key) == NULL);

  // Test that we can't set the same element twice
  hash_table_set(&table, test_key, &test_element);
  expect_termination(hash_table_set(&table, test_key, &test_element));
  assert(table.len == 1);

  // Test that we can retrieve the element
  assert(*(int *)hash_table_get(&table, test_key) == test_element);

  // Test that the move succeeds
  char test_key_new[32] = "test-key-2";
  assert(hash_table_move(&table, test_key, test_key_new) == 0);
  assert(hash_table_move(&table, test_key, test_key_new) == -1);
  assert(hash_table_get(&table, test_key) == NULL);
  assert(*(int *)hash_table_get(&table, test_key_new) == test_element);
  assert(table.len == 1);

  // Test that we can't remove the same element twice
  assert(hash_table_remove(&table, test_key_new) == 0);
  assert(hash_table_remove(&table, test_key_new) == -1);
  assert(table.len == 0);

  // Test that the remove worked
  assert(hash_table_get(&table, test_key_new) == NULL);

  hash_table_free(&table);
}