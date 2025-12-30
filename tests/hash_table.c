#include "../include/hash_table.h"

int main() {
  struct HashTable table;
  hash_table_new(&table, sizeof(char[32]), sizeof(int));

  for (size_t i = 0; i < table.capacity; i++) {
    assert(table._buffer[i] == NULL);
  }

  char test_key[32] = "test-key-1";
  int test_element = 8;
  assert(hash_table_set(&table, test_key, &test_element) == 0);
  assert(hash_table_set(&table, test_key, &test_element) == -1);
  assert(*(int *)hash_table_get(&table, test_key) == 8);
  assert(hash_table_remove(&table, test_key) == 0);
  assert(hash_table_remove(&table, test_key) == -1);
  assert(hash_table_get(&table, test_key) == NULL);
  hash_table_free(&table);
}