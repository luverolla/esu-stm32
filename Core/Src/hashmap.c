#include "hashmap.h"

#include <stdlib.h>
#include <string.h>

map_t* map_create(unsigned int max_size) {
    map_t* hashmap = malloc(sizeof(map_t));
    hashmap->size = 0;
    hashmap->max_size = max_size;
    hashmap->table = calloc(max_size, sizeof(map_pair_t));
    return hashmap;
}

int map_hash(map_t* self, char *key) {
    unsigned int hashval = 0;
    for (int i = 0; i < strlen(key); i++) {
        hashval = key[i] + (hashval << 5) - hashval;
    }
    return hashval % self->max_size;
}

void map_insert(map_t* self, char* key, void* value) {
	if (self->size == self->max_size)
		return;

    int index = map_hash(self, key);
    map_pair_t* entry = malloc(sizeof(map_pair_t));
    entry->key = key;
    entry->value = value;
    while (self->table[index] != NULL)
        index = (index + 1) % self->max_size;
    self->size += 1;
    self->table[index] = entry;
}

void* map_get(map_t* self, char* key) {
    int index = map_hash(self, key);
    while (self->table[index] != NULL && strcmp(self->table[index]->key, key) != 0) {
        index = (index + 1) % self->max_size;
    }
    if (self->table[index] == NULL) {
        return NULL;
    } else {
        return self->table[index]->value;
    }
}

void map_delete(map_t* self, char* key) {
    int index = map_hash(self, key);
    while (self->table[index] != NULL && strcmp(self->table[index]->key, key) != 0) {
        index = (index + 1) % self->max_size;
    }
    if (self->table[index] != NULL) {
        free(self->table[index]->key);
        free(self->table[index]->value);
        free(self->table[index]);
        self->table[index] = NULL;
        self->size -= 1;
    }
}

void map_destroy(map_t* self) {
	for (int i = 0; i < self->max_size; i++)
		if (self->table[i] != NULL)
			free(self->table[i]);

	free(self->table);
	free(self);
}
