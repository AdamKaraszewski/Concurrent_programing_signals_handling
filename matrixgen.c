#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int *generateMatrix(int rowsNumber, int columnNumber) {
    int *ptr = malloc((rowsNumber * columnNumber) * sizeof(int)); //malloc

    for(int i = 0; i < rowsNumber * columnNumber; i++) {
        ptr[i] = rand() % 10;
    }

    return ptr;
}

void saveMatrixFile(char* priority, char *rowsNumber, char *columnsNumber, int *matrix) {
    int rows = atoi(rowsNumber);
    int columns = atoi(columnsNumber);

    int fileNameSize = strlen("_Matrix.txt") + 1;
    char *fileName = malloc(fileNameSize); //malloc
    strcpy(fileName, priority);
    strcat(fileName, "_Matrix.txt");

    FILE *matrixFile = fopen(fileName, "w"); //write mode
    fprintf(matrixFile, "%d %d\n", rows, columns);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            fprintf(matrixFile, "%d", matrix[i * columns + j]);
            if (j < columns - 1) fprintf(matrixFile, " ");
        }
        fprintf(matrixFile, "\n");
    }
    fclose(matrixFile);
    free(fileName);
}

// first matrix n x m
// second matrix m x p
// main retrieves 3 args: n, m, p
int main(int argc, char *argv[]) {
    
    srand(time(NULL));
    
    int *firstMatrix = generateMatrix(atoi(argv[1]), atoi(argv[2])); //malloc
    int *secondMatrix = generateMatrix(atoi(argv[2]), atoi(argv[3]));  //malloc

    saveMatrixFile("A", argv[1], argv[2], firstMatrix);
    saveMatrixFile("B", argv[2], argv[3], secondMatrix);

    free(firstMatrix);
    free(secondMatrix);
}