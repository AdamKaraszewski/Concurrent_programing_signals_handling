#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int rows;
    int columns;
    int **content;
} matrix_t;

matrix_t *createMatrixFromFile(char *fileName);
void deleteMatrix(matrix_t *matrix);

#endif
