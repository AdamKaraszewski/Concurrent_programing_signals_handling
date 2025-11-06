# Concurrent programing - signals handling
Program for multiplying matrices using concurrent programming (signal handling and multiple processes).
## Matrix file and matrix generaation
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
