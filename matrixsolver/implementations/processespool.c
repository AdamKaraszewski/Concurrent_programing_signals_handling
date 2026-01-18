#include <stdlib.h>
#include "../definitions/processespool.h"

processespool_t *create_pool(int pool_size) {
    
    processespool_t *pool = malloc(sizeof(processespool_t));
    
    pool->pool_size = pool_size;
    pool->processes = malloc(pool_size * sizeof(poolentry_t *));
    
    for (int i = 0; i < pool_size; i++) {
        pool->processes[i] = create_entry();
    }

    return pool;
}

void delete_pool(processespool_t *pool) {

    for (int i = 0; i < pool->pool_size; i++) {
        delete_entry(pool->processes[i]);
    }

    free(pool->processes);
    free(pool);
}