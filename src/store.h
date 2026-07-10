#ifndef STORE_H
#define STORE_H


void store_init(void);

char *get_string_value(const char *key);
/*
    1 = success
    0 = fail
 */
int set_string_value(const char *key, const char *value);

#endif
