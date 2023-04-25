#ifndef INC_MAP_H
#define INC_MAP_H

#include <stdlib.h>

typedef struct map_pair {
    char* key;
    void* value;
} map_pair_t;

typedef struct map {
	size_t size;
	size_t max_size;
    map_pair_t** table;
} map_t;

map_t* map_create(size_t max_size);

int map_hash(map_t* self, char* key);

void map_insert(map_t* self, char* key, void* value);

void *map_get(map_t* hashmap, char* key);

void map_delete(map_t* hashmap, char* key);

void map_destroy(map_t* hashmap);

#endif
