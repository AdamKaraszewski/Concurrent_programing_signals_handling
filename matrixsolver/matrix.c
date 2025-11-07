#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

matrix_t *createMatrixFromFile(char *fileName) {
    matrix_t *matrix = malloc(sizeof(matrix_t));
    FILE *matrixFile = fopen(fileName, "r");
    
    fscanf(matrixFile, "%d %d", &matrix->rows, &matrix->columns);
    matrix->content = malloc(matrix->rows * sizeof(int *));
    for (int i = 0; i < matrix->rows; i++) {
        matrix->content[i] = malloc(matrix->columns * sizeof(int));
    }

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->columns; j++) {
            fscanf(matrixFile, "%d", &matrix->content[i][j]);
        }
    }
    fclose(matrixFile);
    return matrix;
}

void deleteMatrix(matrix_t *matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        free(matrix->content[i]);
    }
    free(matrix->content);
    free(matrix);
}