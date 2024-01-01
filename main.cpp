#include <stdlib.h>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
// #include <cuda_runtime.h>
#include "parallelsudoku.cuh"

#define UPDIV(n, d) (((n)+(d)-1) / (d))
using namespace std;

int main(int argc, char* argv[]) {
    // read sudoku board from file
    int board[boardSize * boardSize];
    ifstream myFile ("sudoku.txt");
    for (int i = 0; i < boardSize * boardSize; i++) {
        for (int j = 0; j < boardSize; j++) 
            myFile >> board[i*boardSize + j];
    }
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++)
            cout << board[i*boardSize+j] << " ";
            cout << endl;
    }
}
