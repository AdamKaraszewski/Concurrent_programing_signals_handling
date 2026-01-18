#include "poolentry.h"

#ifndef PROCESSESPOOL_H
#define PROCESSESPOOL_H

typedef struct {
    int pool_size;
    poolentry_t **processes;
} processespool_t;

processespool_t *create_pool(int size);
void delete_pool(processespool_t *pool);

#endif
