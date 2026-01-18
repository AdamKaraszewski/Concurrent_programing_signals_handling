#include "../definitions/getcurrenttime.h"

#include <sys/time.h>
#include <time.h>

long current_milliseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
