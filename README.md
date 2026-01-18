# Concurrent programing - signals handling
Program for multiplying matrices using concurrent programming (signal handling and multiple processes).
## Matrix file and matrix generation
Program retrieves matrices files that follow pattern:
```
MATRIX_ROWS_NUMBER MATRIX_COLUMNS_NUMBER
M[0][0] M[0][1] ... M[0][MATRIX_COLUMNS_NUMBER - 1]
M[1][0] M[1][1] ... M[1][MATRIX_COLUMNS_NUMBER - 1]
...
M[MATRIX_ROWS_NUMBER -1][0] M[MATRIX_ROWS_NUMBER - 1][1] ... M[MATRIX_ROWS_NUMBER - 1][MATRIX_COLUMNS_NUMBER - 1]
```
Example (Simple matix file)
```
3 3
1 2 3
4 5 6
7 8 9
```
## Program architecture
**Dispositor (parent process/main process)** - reads two matrices from .mtx files and creates workers that camputes specified field values (result matrix). Dispositor is responsible for distributing result matrix fields indexes that need to be computed, beetwen workers.

**Worker (child process)** - computes specified field value (result matrix).

## How does the program work?
Dispositor process uses two methods to compute result matrix and stores their time results:
- iteration method (i_result_time = i_time_stop - i_time_start) 
- concurrent method (c_result_time = c_time_stop - c_time_start)

Dispositor reads matrices from .mtx files. Matrix is stored as matix_t struct.

```C
typedef struct {
    unsigned int rows;
    unsigned int columns;
    int **content;
} matrix_t;

matrix_t *create_matrix_from_file(char *file_name);
matrix_t *create_empty_matrix(unsigned int rows, unsigned int cols);
```
Dispositor creates empty result matrices: i_result_matrix (control matrix) and c_result_matrix.

Dispositor stores the field index of result matrix that need to be computed by first available worker (first WORKERS_NUMBER indexes are distributed during workers initialization).

```C
int NEXT_ELEMENT = 0;
TOTAL_ELEMENTS = RESULT_MATRIX->rows * RESULT_MATRIX->columns;
```

Dispositor creates processpool_t struct and creates WORKERS_NUMBER workers. Each worker has: unique worker id [0 - WORKERS_NUMBER - 1], unique pid, unique element (result matrix field), count (computed elements number = 0).

```C
WORKERS_POOL = create_pool(WORKERS_NUMBER);
for (int i = 0; i < WORKERS_POOL->pool_size; i++) {
    if(pid > 0) {
        WORKERS_POOL->processes[i]->process_id = pid; 
        WORKERS_POOL->processes[i]->element = NEXT_ELEMENT;
        WORKERS_POOL->processes[i]->count = 0;
    } else {
        WORKER_POOL_ID = i;
        ELEMENT_TO_BE_COMPUTED = NEXT_ELEMENT;
        (...)
    }
    if (i < WORKERS_POOL->pool_size - 1) {
        NEXT_ELEMENT++;
    }
}
```


| worker_id | process_id | element | count |
| --------- | ---------- | ------- | ----- |
| 0 | 100 | 0 | 0 |
| 1 | 200 | 1 | 0 |
| 2 | 102 | 2 | 0 |
| 3 | 400 | 3 | 0 |
| 4 | 900 | 4 | 0 |


When worker is ready it sends READY SIGNAL (REAL TIME SIGNAL - SIGRTMIN, real time signals are queued - can't be lost) to dispositor. When dispositor receives ready signal it increments WORKERS_READY variable and checks if WORKERS_READY equals WORKERS_NUMBER. If WORKERS_READY is equal WORKERS_NUMBER dispositor sends not real time signal (14) to all workers. When workers receive this signal they start computing specified values. 

As soon as worker finishes its computation it sends RESULT SIGNAL (SIGRTMIN + 1) with int value (32 bits):

| 4 bits | 28 bits |
| ------ | ------- |
| worker_id (4 bits allow to change WORKER_NUMBER to 15) | computed value [-268435456; 268435455] |

When dispositor receives RESULT SIGAL (SIGRTMIN + 1) it performs one of these actions:
- if no RESULT SIGNAL is processed, the received RESULT SIGNAL will be processed,
- if other RESULT SIGNAL is processed, the received RESULT SIGNAL will be queued.

#### RESULT SIGNAL processing example

let WORKERS_NUMBER = 3

let TOTAL_ELEMENTS = 4 [0, 1, 2, 3] <- simple matrix

| worker_id | process_id | element | count |
| --------- | ---------- | ------- | ----- |
| 0 | 100 | 0 | 0 |
| 1 | 200 | 1 | 0 |
| 2 | 102 | 2 | 0 |

NEXT_ELEMENT = 2 

Three workers send RESULT SIGNAL at the same time:
- worker 0 [pid: a], sends signal with int value: [worker id: 0, computed_field_value: v1],
- worker 1 [pid: b], sends signal with int value: [worker id: 1, computed_field_value: v2],
- worker 2 [pid: c], sends signal with int
value: [worker id: 2, computed_field_value: v3].

Real time signal from worker 1 is processed and signals from worker 0 and worker 2 are queued.

Dispositor reads worker_id value from int that is part of the RESULT SIGNAL - [worker_id: 1]

Dispositor loads element value from processpool_t struct: [worker_id: 1, element: 0]

Dispositor fill specified result matrix field: 

```C
unsigned int row = WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->element / RESULT_MATRIX->columns;
unsigned int col = WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->element % RESULT_MATRIX->columns;
    
RESULT_MATRIX->content[row][col] = get_value(info->si_value.sival_int);
```

Dispositor check if there is any result_matrix field index left (current NEXT_FIELD = 2, so there is one field left). Dispositor increments NEXT_FIELD (2 -> 3) and sends the CALCULATE ELEMENT SIGNAL (SIGRTMIN + 2) with int value which is equal NEXT_FIELD = 3 to worker 1. Dispositor updates processespool_t entry.

```C
if (NEXT_ELEMENT < TOTAL_ELEMENTS) {
    NEXT_ELEMENT++;
}

WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->element = NEXT_ELEMENT;
WORKERS_POOL->processes[get_process_pool_id(info->si_value.sival_int)]->count += 1;

ELEMENTS_COMPUTED++;
```

| worker_id | process_id | element | count |
| --------- | ---------- | ------- | ----- |
| 0 | 100 | 0 | 0 |
| **1** | **200** | **3** | **1** |
| 2 | 102 | 2 | 0 |


Dispositor starts processing real time signal from worker 0. Dispositor check if there is any result_matrix field left (current NEXT_FIELD = 3, so there is no field left). There are no fields left - dispositor changes NEXT_FIELD to MAX_INT and send the CALCULATE ELEMENT SIGNAL with int value which is equal NEXT_FIELD = MAX_INT to worker 0. When worker recives MAX_INT value it call exit(1). 

```C
if (NEXT_ELEMENT == TOTAL_ELEMENTS - 1) {
    NEXT_ELEMENT = INT_MAX;
}
```

| worker_id | process_id | element | count |
| --------- | ---------- | ------- | ----- |
| **0** | **100** | **MAX_INT** | **1** |
| 1 | 200 | 3 | 1 |
| 2 | 102 | 2 | 0 |

Dispositor starts processing real time signal from worker 2. Dispositor check if there is any result_matrix field (current NEXT_FIELD = MAX_INT, so there is no field left). Dispositor sends CALCULATE ELEMENT SIGNAL with int value which is equal NEXT_FIELD = MAX_INT to worker 2.

| worker_id | process_id | element | count |
| --------- | ---------- | ------- | ----- |
| 0 | 100 | MAX_INT | 1 |
| 1 | 200 | 3 | 1 |
| **2** | **102** | **MAX_INT** | **1** |

Program returns i_time (iteration method result time) and c_time (concurrent method time), w0, w1, w2, w3, w4 count.

## Run program


check current path
```
$ pwd 
/home/user/Concurrent_programing_signals_handling
$ ls -l
total 16
-rw-rw-r-- 1 user user 1530 Jan 18 22:40 matrixgen.c
drwxrwxr-x 5 user user 4096 Jan 18 22:40 matrixsolver
-rw-rw-r-- 1 user user 7163 Jan 18 22:40 README.md
```

gcc matrixgen.c
```
$ gcc matrixgen.c
$ ls -l
total 36
-rwxrwxr-x 1 user user 16488 Jan 19 00:12 a.out
-rw-rw-r-- 1 user user  1530 Jan 18 22:40 matrixgen.c
drwxrwxr-x 5 user user  4096 Jan 18 22:40 matrixsolver
-rw-rw-r-- 1 user user  7163 Jan 18 22:40 README.md
``` 
generate matrices ./a.out [first_matrix_rows] [first_matix_cols_and_second_matrix_rows] [second_matrix_cols]
```
./a.out 5 3 5
```

check generated matrices 
```
$ ls -l
total 44
-rw-rw-r-- 1 user user    34 Jan 19 00:19 A.mtx
-rwxrwxr-x 1 user user 16488 Jan 19 00:12 a.out
-rw-rw-r-- 1 user user    34 Jan 19 00:19 B.mtx
-rw-rw-r-- 1 user user  1530 Jan 18 22:40 matrixgen.c
drwxrwxr-x 5 user user  4096 Jan 18 22:40 matrixsolver
-rw-rw-r-- 1 user user  7163 Jan 18 22:40 README.md
$ cat A.mtx 
5 3
2 4 9
3 9 8
5 7 9
6 4 0
3 9 9
$ cat B.mtx
3 5
0 8 2 1 7
6 0 2 6 6
7 8 8 0 7
```
compile matrix solver (remember to grand +x to build.sh) 
```
$ cd matrixsolver/first/
$ ls -l
total 12
-rwxrwxrwx 1 user user  254 Jan 18 22:40 build.sh
-rw-rw-r-- 1 user user 7393 Jan 18 22:40 signalmatrixsolverpool.c
```
```
./build.sh
```

run program 
```
./solver ../../A.mtx ../../B.mtx 
```

## Results 
Program was run 10 times for result matrix with:
- 10 fields,
- 100 fields,
- 1000 fields, 
- 10000 fields,
- 100000 fields

Table and diagrams shows avarage result (program was run 10 times) for each matrix:

| i_time (miliseconds) | c_time (miliseconds) | matrix field |
| ------ | ------ | ------------ |
| 0 | 2,7 | 10 |
| 0 | 4,1 | 100 |
| 0,2 | 31,3 | 1000 |
| 0,5 | 138,3 | 10000 |
| 5,1 | 1454,7 | 100000 |
