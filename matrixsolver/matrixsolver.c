#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

matrix_t *multiplyMatrices(matrix_t *a, matrix_t *b) {
    matrix_t *result = malloc(sizeof(matrix_t));
    result->rows = a->rows;
    result->columns = b->columns;
    result->content = malloc(result->rows * sizeof(int *));
    for (int i = 0; i < result->rows; i++) {
        result->content[i] = calloc(result->columns, sizeof(int));
    }

    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->columns; j++) {
            for (int k = 0; k < a->rows; k++) {
                result->content[i][j] += a->content[i][k] * b->content[k][j];
            } 
        }
    }
    return result;
}

void printMatrix(matrix_t *matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->columns; j++) {
            printf("%d ", matrix->content[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    matrix_t *firstMatrix = createMatrixFromFile(argv[1]);
    matrix_t *secondMatrix = createMatrixFromFile(argv[2]);
    matrix_t *result = multiplyMatrices(firstMatrix, secondMatrix);
    printMatrix(result);
    deleteMatrix(firstMatrix);
    deleteMatrix(secondMatrix);
    deleteMatrix(result);
}