#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
const int size = 9;
const int box_size = 3;
bool noConflicts(int matrix[size * size], int row, int col, int num) {
    if (num > size) return false;
    for (int i = 0; i < size; i++) {
        if (i == row)   continue;
        if (matrix[i * size + col] == num) {
            return false;
        }
    }
    
    for (int j = 0; j < size; j++) {
        if (j == col)   continue;
        if (matrix[row * size + j] == num) {
            return false;
        }
    }
    
    for (int i = 0; i < box_size; i++) {
        for (int j = 0; j < box_size; j++) {
            int mat_row = (row/box_size)*box_size + i;
            int mat_col = (col/box_size)*box_size + j;
            if (mat_row == row && mat_col == col)   continue;
            if (matrix[mat_row * size + mat_col] == num) {
                // cout << "box conflict: i=" << i <<"j="<<j<< endl;
                return false;
            }
        }
    }
    // for (int i = 0; i < box_size; i++) {
    //     for (int j = 0; j < box_size; j++) {
    //         if (i == row && j == col)   continue;
    //         if (matrix[((row/box_size)*box_size + i) * size + (col/box_size)*box_size + j] == num) {
    //             cout << "box conflict: i=" << i <<"j="<<j<< endl;
    //             return false;
    //         }
    //     }
    // }
    return true;
}

int getTotalUnfilledCellsNum(int matrix[size * size]) {
    int cnt = 0;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            cnt += (matrix[i][j] == 0);
    return cnt;
}

bool checkDone(int matrix[size * size]) {
    int emptyCnt;;
    emptyCnt = getTotalUnfilledCellsNum(matrix);
    return (emptyCnt == 0);
}