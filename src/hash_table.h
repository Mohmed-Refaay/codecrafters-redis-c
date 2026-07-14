#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>

typedef struct hash_table_item {
    char *key;
    void *value;
    size_t item_size;
} ht_item;

typedef struct hash_table {
    ht_item *items;
    size_t size;
} ht;

ht *create_ht(size_t size);

ht_item *set_item(ht *table, const char *key, const void *value, const size_t item_size);
ht_item *get_item(ht *table, const char *key);

void destroy_ht(ht *table);

#endif
