#include <stdlib.h>
#include "../definitions/poolentry.h"

poolentry_t *create_entry() {
    poolentry_t *entry = malloc(sizeof(poolentry_t));
    return entry;
}

void delete_entry(poolentry_t *entry) {
    free(entry);
}