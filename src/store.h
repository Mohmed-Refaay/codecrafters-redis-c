#ifndef STORE_H
#define STORE_H

typedef struct store_item {
    char *value;
    long expiry_time;
} store_item;

void store_init(void);

store_item *get_string_value(const char *key);
/*
    1 = success
    0 = fail
 */
int set_string_value(const char *key, const char *value, const long expiry_time);

#endif
