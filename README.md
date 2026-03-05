# C Hash Table
A lightweight open-addressing hash table written in C featuring FNV-1a hashing, linear probing, tombstone deletion, and automatic resizing.

## Features
- **FNV-1a Hashing** - Fast, well-distributed 64-bit hash function
- **Linear Probing** - Open addressing with tombstone-based deletion to preserve probe chain integrity
- **Dynamic Resizing** - Automatically expands capacity at 70% load factor
- **Heap-Copied Keys** - Keys are duplicated internally so callers don't need to keep them alive
- **Minimal API** - Six functions covering the full create/read/update/delete lifecycle

## Building

### Requirements
- GCC or compatible C compiler
- C99 or later

### Installation
```
git clone https://github.com/liamshiley/hashtable.git
cd hashtable
cc -std=c99 -Wall -Wextra -o hashtable hashtable.c
```

## Usage
```c
HashTable *ht = hashtable_create();

hashtable_set(ht, "apples", 42);
hashtable_set(ht, "bananas", 7);

uint64_t v = hashtable_get(ht, "apples");  // 42
uint64_t missing = hashtable_get(ht, "grapes");  // UINT64_MAX (not found)

hashtable_delete(ht, "bananas");

printf("count: %zu\n", hashtable_length(ht));  // 1

hashtable_destroy(ht);
```

## API

| Function | Description |
|---|---|
| `hashtable_create()` | Allocates and returns a new hash table |
| `hashtable_set(table, key, value)` | Inserts or updates an entry |
| `hashtable_get(table, key)` | Returns the value for a key, or `UINT64_MAX` if not found |
| `hashtable_delete(table, key)` | Removes an entry by key |
| `hashtable_length(table)` | Returns the number of live entries |
| `hashtable_destroy(table)` | Frees all memory associated with the table |

## Technical Highlights
- **FNV-1a Hashing** - XOR-multiply hash over each byte of the key, producing a well-distributed 64-bit index
- **Tombstone Deletion** - Deleted slots are marked with a sentinel pointer rather than cleared, preserving the probe chain for subsequent lookups
- **Tombstone Reuse** - On insertion, the first tombstone encountered is cached and reused if no matching key is found, keeping the table compact
- **Load Factor Expansion** - Table doubles in capacity when `count / capacity >= 0.7`, rehashing all live entries into the new backing array

## Notes
- `UINT64_MAX` is the sentinel for "not found" — if you need to store `UINT64_MAX` as a legitimate value, change `hashtable_get` to accept an output parameter and return a `bool`
- The table is not thread-safe
- Initial capacity is 16 slots

## License
MIT License - feel free to use this code for learning or your own projects.

## Author
Liam Shiley

Built to practice C programming, hash table internals, and memory management.
