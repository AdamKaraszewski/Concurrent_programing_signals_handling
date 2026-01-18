#include <stdio.h>
#include <stdlib.h>
#include "../definitions/matrix.h"

matrix_t *create_matrix_from_file(char *file_name) {
    matrix_t *matrix = malloc(sizeof(matrix_t));
    FILE *matrix_file = fopen(file_name, "r");
    
    fscanf(matrix_file, "%d %d", &matrix->rows, &matrix->columns);
    matrix->content = malloc(matrix->rows * sizeof(int *));
    for (unsigned int i = 0; i < matrix->rows; i++) {
        matrix->content[i] = malloc(matrix->columns * sizeof(int));
    }

    for (unsigned int i = 0; i < matrix->rows; i++) {
        for (unsigned int j = 0; j < matrix->columns; j++) {
            fscanf(matrix_file, "%d", &matrix->content[i][j]);
        }
    }
    fclose(matrix_file);
    return matrix;
}

void delete_matrix(matrix_t *matrix) {
    for (unsigned int i = 0; i < matrix->rows; i++) {
        free(matrix->content[i]);
    }
    free(matrix->content);
    free(matrix);
}

matrix_t *create_empty_matrix(unsigned int rows, unsigned int cols) {
    matrix_t *matrix = malloc(sizeof(matrix_t));
    matrix->rows = rows;
    matrix->columns = cols;
    matrix->content = malloc(matrix->rows * sizeof(int *));

    for (unsigned int i = 0; i < matrix->rows; i++) {
        matrix->content[i] = calloc(matrix->columns, sizeof(int));
    }
    return matrix;
}

matrix_t *multiply_matrices(matrix_t *a_matrix, matrix_t *b) {
    matrix_t *result = malloc(sizeof(matrix_t));
    result->rows = a_matrix->rows;
    result->columns = b->columns;
    result->content = malloc(result->rows * sizeof(int *));
    for (unsigned int i = 0; i < result->rows; i++) {
        result->content[i] = calloc(result->columns, sizeof(int));
    }

    for (unsigned int i = 0; i < result->rows; i++) {
        for (unsigned int j = 0; j < result->columns; j++) {
            for (int k = 0; k < a_matrix->columns; k++) {
                result->content[i][j] += a_matrix->content[i][k] * b->content[k][j];
            } 
        }
    }
    return result;
}

bool compare_matrices(matrix_t *a_matrix, matrix_t *b_matrix) {
    for (unsigned int i = 0; i < a_matrix->rows; i++) {
        for (unsigned int j = 0; j < b_matrix->columns; j++) {
            if (a_matrix->content[i][j] != b_matrix->content[i][j]) {
                return false;
            }
        }
    }
    return true;
};
