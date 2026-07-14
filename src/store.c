#include "store.h"
#include "hash_table.h"
#include <stdlib.h>
#include <string.h>

ht *store_table;


void store_init() {
    store_table = create_ht(1024);
}

store_item *get_string_value(const char *key) {
    ht_item *item = get_item(store_table, key);

    if(item == NULL) return NULL;


    return item->value;
}


int set_string_value(const char *key, const char *value, const long expiry_time) {
    char *new_value = strdup(value);
    if (new_value == NULL) return 0;

    store_item store_value = {new_value, expiry_time};

    ht_item *item = set_item(store_table, key, &store_value, sizeof(store_item));
    if(item == NULL) return 0;

    return 1;
}











