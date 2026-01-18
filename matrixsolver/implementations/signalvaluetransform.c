#include "../definitions/signalvaluetransform.h"

int get_process_pool_id(int message) {
    return (message >> 28);
}

int get_value(int message) {
    int s = message & ~(15 << 28); 
    return s;
}
