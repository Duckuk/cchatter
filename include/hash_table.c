#include "hash_table.h"
#include "packet.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t hash(void *restrict buf, size_t buf_size) {
  size_t padding =
      (sizeof(size_t) - (buf_size % sizeof(size_t))) % sizeof(size_t);
  size_t size = buf_size + padding;
  assert(size % sizeof(size_t) == 0);

  size_t *buffer = malloc(size);
  memset(buffer, 0, size);
  memcpy(buffer, buf, buf_size);

  size_t hash_value = 0;
  for (size_t i = 0; i < size / sizeof(size_t); i++) {
    hash_value ^= buffer[i];
  }

  free(buffer);

  return hash_value;
}

#define TEST_SIZE (INT32_MAX / 5)
double calculate_collision_rate() {
  ConnectionID buf;
  memset(buf, 0, sizeof buf);

  srandom(1234567890);

  size_t *test = calloc(TEST_SIZE, sizeof(size_t));
  for (size_t x = 0; x < TEST_SIZE; x++) {
    for (size_t y = 0; y < sizeof buf; y++) {
      buf[y] = 32 + (char)(random() % 94);
    }

    test[x] = hash(buf, sizeof buf) % 1024;
  }

  double collisions = 0;
  for (size_t i = 0; i < TEST_SIZE; i++) {
    for (size_t j = 0; i < TEST_SIZE; i++) {
      if (test[i] == test[j]) {
        collisions++;
      }
    }
  }

  free(test);

  return collisions / (double)TEST_SIZE;
}

int hash_table_new(struct HashTable *restrict table, size_t key_size,
                   size_t element_size) {
  memset(table, 0, sizeof *table);

  table->capacity = 1024;
  table->len = 0;
  table->key_size = key_size;
  table->element_size = element_size;
  table->_hash_func = hash;

  table->_buffer = (void **)calloc(sizeof(void *), table->capacity);
  if (table->_buffer == NULL) {
    free((void *)table->_buffer);
    return -1;
  }

  return 0;
}

int hash_table_set(struct HashTable *restrict table, void *restrict key,
                   void *restrict element) {
  size_t hash = table->_hash_func(key, table->key_size) % table->capacity;
  if (table->_buffer[hash] != NULL) {
    fprintf(stderr, "hash table collisions not yet handled\n");
    return -1;
  }

  table->_buffer[hash] = malloc(table->element_size);
  if (table->_buffer[hash] == NULL) {
    fprintf(stderr, "hash table allocation failed\n");
    return -1;
  }
  memcpy(table->_buffer[hash], element, table->element_size);
  table->len += 1;

  return 0;
}

int hash_table_set_ptr(struct HashTable *restrict table, void *restrict key,
                       void *restrict ptr) {
  assert(ptr != NULL);

  size_t hash = table->_hash_func(key, table->key_size) % table->capacity;
  if (table->_buffer[hash] != NULL) {
    fprintf(stderr, "hash table collisions not yet handled\n");
    return -1;
  }

  table->_buffer[hash] = ptr;
  table->len += 1;

  return 0;
}

int hash_table_remove(struct HashTable *restrict table, void *restrict key) {
  size_t hash = table->_hash_func(key, table->key_size) % table->capacity;
  if (table->_buffer[hash] == NULL) {
    fprintf(stderr, "invalid hash table key\n");
    return -1;
  }

  free(table->_buffer[hash]);
  table->_buffer[hash] = NULL;
  table->len -= 1;

  return 0;
}

void *hash_table_get(struct HashTable *restrict table, void *restrict key) {
  size_t hash = table->_hash_func(key, table->key_size) % table->capacity;
  if (table->_buffer[hash] == NULL) {
    fprintf(stderr, "invalid hash table key\n");
    return NULL;
  }

  return table->_buffer[hash];
}

void hash_table_free(struct HashTable *restrict table) {
  for (size_t i = 0; i < table->capacity; i++) {
    free(table->_buffer[i]);
  }
  free((void *)table->_buffer);
}