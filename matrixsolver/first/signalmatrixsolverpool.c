#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <sys/time.h>

#include "../definitions/matrix.h"
#include "../definitions/processespool.h"
#include "../definitions/infoutils.h"
#include "../definitions/signalvaluetransform.h"
#include "../definitions/getcurrenttime.h"

matrix_t *FIRST_MATRIX;
matrix_t *SECOND_MATRIX;
matrix_t *RESULT_MATRIX;

int DISPOSITOR_PID;
int WORKER_POOL_ID;

processespool_t *WORKERS_POOL;

// FILE *log;
int ELEMENT_TO_BE_COMPUTED;

int NEXT_ELEMENT = 0;
int TOTAL_ELEMENTS;
int ELEMENTS_COMPUTED = 0;

int WORKERS_NUMBER = 5;
int WORKERS_READY = 0;

void compute_element_value() {

    int result = 0;
    unsigned int row = ELEMENT_TO_BE_COMPUTED / RESULT_MATRIX->columns;
    unsigned int col = ELEMENT_TO_BE_COMPUTED % RESULT_MATRIX->columns;

    for (unsigned int i = 0; i < FIRST_MATRIX->columns; i++) {
        result += FIRST_MATRIX->content[row][i] * SECOND_MATRIX->content[i][col];
    }
    // print_worker_compute_element(worker_pool_id, element_to_be_computed, result, row, col, log);
    int message_content = WORKER_POOL_ID << 28;
    message_content = message_content | result; 
    union sigval message;
    message.sival_int = message_content;
    sigqueue(DISPOSITOR_PID, SIGRTMIN + 1, message);
}

//signals handlers - workers
void handle_start_work_signal(int sig /* SIGALRM 14*/) {
    //print_worker_received_start_signal(worker_pool_id, log); //write logs
    compute_element_value();
}

// signals handlers - dispositor
// real time signals may be queued
void handle_ready_signal(int sig, siginfo_t *info, void *ucontext) {

    WORKERS_READY++;

    if (WORKERS_READY == WORKERS_NUMBER) {
        kill(0, 14); //doesnt affect dispositor
    }
}

void handle_calculate_element(int sig, siginfo_t *info, void *ucontext) {

    ELEMENT_TO_BE_COMPUTED = info->si_value.sival_int;

    if (ELEMENT_TO_BE_COMPUTED != INT_MAX)  {
        compute_element_value();
    } else {
        //end worker process
        //print_worker_end_work(worker_pool_id, element_to_be_computed, log, first_matrix, second_matrix, result_matrix, control_matrix, workers_pool);
        delete_matrix(FIRST_MATRIX);
        delete_matrix(SECOND_MATRIX);
        delete_matrix(RESULT_MATRIX);
        delete_pool(WORKERS_POOL);
        exit(0);
    }
}

// result signal value structure:
// 4 first bits -> process id in pool (not pid) - determines 8 workers 
// 28 bits -> [-268435456; 268435455]

void handle_result_signal(int sig, siginfo_t *info, void *ucontext) {
    
    if (NEXT_ELEMENT == TOTAL_ELEMENTS - 1) {
        NEXT_ELEMENT = INT_MAX;
    }
    if (NEXT_ELEMENT < TOTAL_ELEMENTS) {
        NEXT_ELEMENT++;
    }
    
    unsigned int row = WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->element / RESULT_MATRIX->columns;
    unsigned int col = WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->element % RESULT_MATRIX->columns;
    
    RESULT_MATRIX->content[row][col] = get_value(info->si_value.sival_int);

    WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->element = NEXT_ELEMENT;
    WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->count += 1;

    ELEMENTS_COMPUTED++;

    union sigval request;
    request.sival_int = NEXT_ELEMENT;
    sigqueue(WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->process_id, SIGRTMIN + 2, request);
}

// void printMatrix(matrix_t *matrix) {
//     for (int i = 0; i < matrix->rows; i++) {
//         for (int j = 0; j < matrix->columns; j++) {
//             printf("%d ", matrix->content[i][j]);
//         }
//         printf("\n");
//     }
// }

int main(int argc, char *argv[]) {

    long i_time_start;
    long i_time_stop;
    long c_time_start;
    long c_time_stop;
    
    FIRST_MATRIX = create_matrix_from_file(argv[1]);
    SECOND_MATRIX = create_matrix_from_file(argv[2]);

    RESULT_MATRIX = create_empty_matrix(FIRST_MATRIX->rows, SECOND_MATRIX->columns);
    
    TOTAL_ELEMENTS = RESULT_MATRIX->rows * RESULT_MATRIX->columns;

    DISPOSITOR_PID = getpid();

    struct sigaction sig_ready_act = { 0 };
    sig_ready_act.sa_flags = SA_SIGINFO;
    sig_ready_act.sa_sigaction = &handle_ready_signal;
    sigaction(SIGRTMIN, &sig_ready_act, NULL);
    
    struct sigaction sig_result_act = { 0 };
    sig_result_act.sa_flags = SA_SIGINFO;
    sig_result_act.sa_sigaction = &handle_result_signal;
    sigaction(SIGRTMIN + 1, &sig_result_act, NULL);

    signal(14, SIG_IGN); //ignore start signal - it should be only handled by workers

    WORKERS_POOL = create_pool(WORKERS_NUMBER);

    // // +-----+-------+----------------+
    // // | idx |  pid  | element index  |  <--- Simple workers pool with 4 workers
    // // +-----+-------+----------------+
    // // |  0  |  100  |       0        |              
    // // +-----+-------+----------------+
    // // |  1  |  231  |       1        |
    // // +-----+-------+----------------+
    // // |  2  |  681  |       2        |
    // // +-----+-------+----------------+
    // // |  3  |  43   |       3        |
    // // +-----+-------+----------------+

    c_time_start = current_milliseconds();
    
    for (int i = 0; i < WORKERS_POOL->pool_size; i++) {

        int pid = fork(); //fork returns child pid
        
        if(pid > 0) {
            WORKERS_POOL->processes[i]->process_id = pid; //store process id
            WORKERS_POOL->processes[i]->element = NEXT_ELEMENT;
            WORKERS_POOL->processes[i]->count = 0;
        } else {

            WORKER_POOL_ID = i;

            //adding start work (SIGALRM) handling
            signal(14, handle_start_work_signal);


            ELEMENT_TO_BE_COMPUTED = NEXT_ELEMENT;

            //send signal to inform parent that process is ready
            union sigval ready_worker;
            ready_worker.sival_int = getpid();
            sigqueue(DISPOSITOR_PID, SIGRTMIN, ready_worker);

            struct sigaction sig_calculate_act = { 0 };
            sig_calculate_act.sa_flags = SA_SIGINFO;
            sig_calculate_act.sa_sigaction = &handle_calculate_element;
            sigaction(SIGRTMIN + 2, &sig_calculate_act, NULL);

            for (;;) {
                pause(); // <- wait for signal that represents start 
            }
        }

        if (i < WORKERS_POOL->pool_size - 1) {
                NEXT_ELEMENT++;
            }
    }

    int finished = 0; 
    int status;
    while (finished < WORKERS_NUMBER) { 
        pid_t pid = waitpid(-1, &status, 0); 
        if (pid > 0) { 
            finished++; 
        } 
    }
    c_time_stop = current_milliseconds();

    i_time_start = current_milliseconds();
    matrix_t *control_matrix = multiply_matrices(FIRST_MATRIX, SECOND_MATRIX);
    i_time_stop = current_milliseconds();

    if (compare_matrices(control_matrix, RESULT_MATRIX)) {
        printf("%ld,%ld", i_time_stop - i_time_start, c_time_stop - c_time_start);
        
        for (int i = 0; i < WORKERS_NUMBER; i++) {
        printf(",%d", WORKERS_POOL->processes[i]->count); 
        }
    } else {
        printf("error");
    }
    printf("\n");
    fflush(stdout);

    delete_matrix(FIRST_MATRIX);
    delete_matrix(SECOND_MATRIX);
    delete_matrix(RESULT_MATRIX);
    delete_matrix(control_matrix);
    delete_pool(WORKERS_POOL);

    return 0;
}