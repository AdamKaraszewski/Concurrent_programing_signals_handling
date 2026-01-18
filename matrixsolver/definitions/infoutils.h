#include "matrix.h"
#include "processespool.h"
#include <stdio.h>

#ifndef INFOUTILS_H
#define INFOUTILS_H

void print_pool_info(processespool_t *pool);

void print_dispositor_received_ready_signal(int worker_process_pid);
void print_dispositor_received_result_signal(int worker_process_pid, int value);

void print_worker_compute_element(int worker_process_pid, int element_index, int computed_element, int row, int col, FILE *file);
void print_worker_end_work(int worker_process_pid, int element_index, FILE *file,  matrix_t *f, matrix_t *s, matrix_t *r, matrix_t *c, processespool_t *p);

void print_worker_received_start_signal(int worker_process_pool_id, FILE *file);
void print_worker_received_row_signal(unsigned int value, FILE *file);
void print_worker_received_col_signal(unsigned int value, FILE *file);

#endif
