#include <stdbool.h> 
#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    unsigned int rows;
    unsigned int columns;
    int **content;
} matrix_t;

matrix_t *create_matrix_from_file(char *file_name);

matrix_t *create_empty_matrix(unsigned int rows, unsigned int cols);

void delete_matrix(matrix_t *matrix);

matrix_t *multiply_matrices(matrix_t *a_matrix, matrix_t *b_matrix);

bool compare_matrices(matrix_t *a_matrix, matrix_t *b_matrix);

#endif
