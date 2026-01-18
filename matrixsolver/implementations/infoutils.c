#include "../definitions/infoutils.h"
#include <unistd.h>

void print_pool_info(processespool_t *pool) {
    printf("Pool size %d: %d\n", pool->pool_size, getpid());
    for (int i = 0; i < pool->pool_size; i++) {
        printf("%d. worker id: %d, element: %d\n", i, pool->processes[i]->process_id, pool->processes[i]->element);
    }
}

void print_dispositor_received_ready_signal(int worker_process_pid) {
    printf("worker with pid: %d send READY\n", worker_process_pid);
    fflush(stdout);
}

void print_dispositor_received_result_signal(int worker_process_pid, int value) {
    printf("dispositor from(%d) received RESULT: %d\n", worker_process_pid, value);
    fflush(stdout);
}

void print_worker_received_start_signal(int worker_process_pool_id, FILE* file) {
    fprintf(file, "worker (%d) with pid: %d received START signal\n", worker_process_pool_id, getpid());
    fflush(file);
}

void print_worker_compute_element(int worker_process_pid, int element_index, int computed_element, int row, int col, FILE* file) {
    fprintf(file, "worker (%d) with pid: %d computed element (%d): row(%d) col(%d) value (%d)\n", worker_process_pid, getpid(), element_index, row, col, computed_element);
    fflush(file);
}

void print_worker_end_work(int worker_process_pid, int element_index, FILE *file, matrix_t *f, matrix_t *s, matrix_t *r, matrix_t *c, processespool_t *p) {
    fprintf(file, "worker (%d) with pid: %d was requestd with EMPTY VALUE(%d)\ndelete first matrix: (%p), delete second matrix: (%p), delete result matrix: (%p), delete control matrix(%p), delete pool(%p)", worker_process_pid, getpid(), element_index, (void*)&f, (void*)&s, (void*)&r, (void*)&c, (void*)&p);
    fflush(file);
}

