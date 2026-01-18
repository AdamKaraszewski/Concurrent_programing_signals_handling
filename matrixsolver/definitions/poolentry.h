#include <stdbool.h>

#ifndef POOLENTRY_H
#define POOLENTRY_H

typedef struct {
    int process_id;
    int element;
    int count;
} poolentry_t;

poolentry_t *create_entry();
void delete_entry(poolentry_t *entry);
#endif
