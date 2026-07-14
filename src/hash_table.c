#include "hash_table.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t fnv1a_64_buf(const char *buf, size_t len) {
    const uint8_t *bp = (const uint8_t *)buf;
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < len; i++) {
        hash ^= bp[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

static size_t get_index_by_key(const char *key, size_t table_size) {
    uint64_t hash_for_key = fnv1a_64_buf(key, strlen(key));

    size_t index = hash_for_key %  table_size;

    return index;
}


static int get_available_index_for_key(ht *table, const char *key) {
    size_t original_index = get_index_by_key(key, table->size);

    if(table->items[original_index].key == NULL 
    || strcmp(table->items[original_index].key, key) == 0)  return original_index;

    size_t i = (original_index + 1) % table->size;
    while(i != original_index) {
        ht_item *curr = &table->items[i];
        if(curr->key == NULL || strcmp(curr->key, key) == 0) {
            return i;
        }
        i++;
        i = i % table->size;
    }

    return -1;
}

ht *create_ht(size_t size) {
    if (size == 0) return NULL;
    ht *table = calloc(1, sizeof(ht));
    if (table == NULL) return NULL;
    table->items = calloc(size, sizeof(ht_item));
    if (table->items == NULL) {
        free(table);
        return NULL;
    }
    table->size = size;
    return table;
}  

ht_item *set_item(ht *table, const char *key, const void *value, const size_t item_size) {
    int item_index = get_available_index_for_key(table, key);
    if (item_index == -1) {
        return NULL;
    }

    char *new_key = strdup(key);
    if(new_key == NULL) {
        return NULL;
    }
    void *new_value = malloc(item_size);
    if(new_value == NULL) {
        free(new_key);
        return NULL;
    }
    memcpy(new_value, value, item_size);


    ht_item *item = &table->items[item_index];

    item->key = new_key;
    item->value = new_value;

    return item;
}

ht_item *get_item(ht *table, const char *key) { 
    int item_index = get_available_index_for_key(table, key);
    if (item_index == -1) {
        return NULL;
    }

    if(table->items[item_index].key == NULL)  return NULL;

    return &table->items[item_index];
}



void destroy_ht(ht *table) {
    if(table == NULL) return;
    for (size_t i = 0; i < table->size; i++) {
        ht_item *item = &table->items[i];
        if (item->key == NULL) continue;
        free(item->key);
        free(item->value);
    }
    free(table->items);
    free(table);
}




