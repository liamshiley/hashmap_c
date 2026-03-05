#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry Entry;
typedef struct HashTable HashTable;
// Hashing algorithim
uint64_t hash_fnv1a(const char *key);
// Creates a hashtable
HashTable *hashtable_create(void);
// Destroys hashtable
void hashtable_destroy(HashTable *table);
// Returns the amount of entries in a hashtbable
size_t hashtable_length(HashTable *table);
// Either sets or creates a new entry depending on if an entry with that key
// exsists
bool hashtable_set(HashTable *table, const char *key, uint64_t value);
// Will return a value with the associated key in the hashtable if it exsists.
uint64_t hashtable_get(HashTable *table, const char *key);
// Deletes hashtable entry in hashtable
bool hashtable_delete(HashTable *table, const char *key);
// Expands the hashtable if you try an add an entry when count / capacity >= .75
static bool hashtable_expand(HashTable *table);

typedef struct Entry {
  char *key;
  uint64_t value;
} Entry;

typedef struct HashTable {
  Entry *entries;
  size_t capacity;
  size_t count;
} HashTable;

#define FNV_OFFSET 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3
#define TOMBSTONE ((char *)0x1)

uint64_t hash_fnv1a(const char *key) {
  uint64_t hash = FNV_OFFSET;
  while (*key) {
    hash ^= (uint8_t)*key++;
    hash *= FNV_PRIME;
  }
  return hash;
}

#define INITIAL_SIZE 16

HashTable *hashtable_create(void) {
  HashTable *HT = malloc(sizeof(HashTable));
  if (HT == NULL) {
    return NULL;
  }
  HT->capacity = INITIAL_SIZE;
  HT->count = 0;
  HT->entries = calloc(HT->capacity, sizeof(Entry));
  if (HT->entries == NULL) {
    free(HT);
    return NULL;
  }
  return HT;
}

void hashtable_destroy(HashTable *table) {
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->entries[i].key != NULL && table->entries[i].key != TOMBSTONE) {
      free(table->entries[i].key);
    }
  }
  free(table->entries);
  free(table);
}

size_t hashtable_length(HashTable *table) { return table->count; }

bool hashtable_set(HashTable *table, const char *key, uint64_t value) {
  // We need to check load factor
  if (table->count * 10 / table->capacity >= 7) {
    if (!hashtable_expand(table)) {

      return false;
    }
  }
  uint64_t hash = hash_fnv1a(key);
  size_t index = hash % table->capacity;
  size_t start = index;
  size_t tombstoneIndex = 0;
  bool tombStoneFound = false;
  // Open addresing, lineaer probe
  while (table->entries[index].key != NULL) {
    if (table->entries[index].key == TOMBSTONE) {
      if (tombStoneFound == false) {
        tombStoneFound = true;
        tombstoneIndex = index;
      }
    } else if (strcmp(key, table->entries[index].key) == 0) {
      table->entries[index].value = value;
      return true;
    }
    if (index + 1 == table->capacity) {
      index = 0;
    } else {
      index++;
    }
    if (index == start)
      return false;
  }
  // We need to see if we found a tombstone before null
  if (tombStoneFound == true) {
    index = tombstoneIndex;
  }
  // Add key to heap, we need a copy, what if key user passes through is freed
  table->entries[index].key = strdup(key);
  if (table->entries[index].key == NULL) {
    return false;
  }
  table->entries[index].value = value;
  table->count++;
  return true;
}

static bool hashtable_expand(HashTable *table) {
  HashTable *newHashTable = hashtable_create();
  if (newHashTable == NULL) {
    return false;
  }
  free(newHashTable->entries);
  newHashTable->capacity = table->capacity * 2;
  newHashTable->entries = calloc(newHashTable->capacity, sizeof(Entry));
  if (newHashTable->entries == NULL) {
    free(newHashTable);
    return false;
  }
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->entries[i].key == NULL || table->entries[i].key == TOMBSTONE) {
      continue;
    }
    hashtable_set(newHashTable, table->entries[i].key, table->entries[i].value);
  }
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->entries[i].key != NULL && table->entries[i].key != TOMBSTONE) {
      free(table->entries[i].key);
    }
  }
  free(table->entries);
  table->entries = newHashTable->entries;
  table->capacity = newHashTable->capacity;
  table->count = newHashTable->count;
  free(newHashTable);
  return true;
}

bool hashtable_delete(HashTable *table, const char *key) {
  uint64_t hash = hash_fnv1a(key);
  size_t index = hash % table->capacity;
  size_t start = index;

  while (table->entries[index].key != NULL) {
    if (table->entries[index].key != TOMBSTONE &&
        strcmp(table->entries[index].key, key) == 0) {
      free(table->entries[index].key);
      table->entries[index].key = TOMBSTONE;
      table->entries[index].value = 0;
      table->count--;
      return true;
    }
    if (index + 1 == table->capacity) {
      index = 0;
    } else {
      index++;
    }
    if (index == start) {
      return false;
    }
  }
  return false;
}

uint64_t hashtable_get(HashTable *table, const char *key) {
  uint64_t hash = hash_fnv1a(key);
  size_t index = hash % table->capacity;
  size_t start = index;

  while (table->entries[index].key != NULL) {
    if (table->entries[index].key != TOMBSTONE &&
        strcmp(table->entries[index].key, key) == 0) {
      return table->entries[index].value;
    }
    if (index + 1 == table->capacity) {
      index = 0;
    } else {
      index++;
    }
    if (index == start) {
      // Return false
      return UINT64_MAX;
    }
  }
  // return false
  return UINT64_MAX;
}

int main() {}