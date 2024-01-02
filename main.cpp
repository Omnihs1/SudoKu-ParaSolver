#include <stdlib.h>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cuda_runtime.h>
#include "parallelsudoku.cuh"
#include "test.cpp"
#define UPDIV(n, d) (((n)+(d)-1) / (d))
#define NDEBUG 1
using namespace std;

int main(int argc, char* argv[]) {
    // read sudoku board from file
    int board[boardSize * boardSize];
    ifstream myFile ("sudoku.txt");
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) 
            myFile >> board[i*boardSize + j];
    }
    // print board
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++)
            cout << board[i*boardSize+j] << " ";
            cout << endl;
    }
    bool done = false;
    while (!done){
        // 0. initial markup
        vector<int> markup[81]; 
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                for (int k = 1; k <= boardSize; k++){
                    if (noConflictsCPU(board, i, j, k) && board[i*boardSize+j] == 0){
                        markup[i*boardSize + j].push_back(k);
                    }
                }
            }
        }
        // 1. elimination
#ifndef NDEBUG
        for(int i=0;i<81;i++)
            cout<<"number of markup in row number "<<i+1<<" is "<<markup[i].size()<<endl;  
#endif
        
    
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (markup[i*boardSize + j].size() == 1) {
                    board[i*boardSize + j] = markup[i*boardSize + j][0];
                }
            }
        }
        done = checkDone(board);
        break;
        // 2. lone ranger
        // if(!done){

        // }
        // 3. find preemptive set with different sizes
    }
    cout << "After elimination : \n";
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++)
            cout << board[i*boardSize+j] << " ";
            cout << endl;
    }
    // 4. backtracking
    if (!done){
        int *d_new_boards;
        int *d_old_boards;
        int *d_solution;
        int *d_board_num;

        int host_solution[boardSize * boardSize];
        int host_board_num = 1;

        const int memSize = 81 * pow(9, DEPTH);


        cudaMalloc(&d_new_boards, memSize * sizeof(int));
        cudaMalloc(&d_old_boards, memSize * sizeof(int));
        cudaMalloc(&d_solution, boardSize * boardSize * sizeof(int));
        cudaMalloc(&d_board_num, sizeof(int));

        cudaMemset(d_new_boards, 0, memSize * sizeof(int));
        cudaMemset(d_old_boards, 0, memSize * sizeof(int));
        cudaMemset(d_solution, 0, boardSize * boardSize * sizeof(int));
        cudaMemset(d_board_num, 0, sizeof(int));

        cudaMemcpy(d_old_boards, board, boardSize * boardSize * sizeof(int), cudaMemcpyHostToDevice);

        BoardGenerator(d_old_boards, d_board_num, d_new_boards);
        
        cudaMemcpy(&host_board_num, d_board_num, sizeof(int), cudaMemcpyDeviceToHost);
        cudaSudokuSolver(d_new_boards, host_board_num, d_solution);

        memset(host_solution, 0, boardSize * boardSize * sizeof(int));
        cudaMemcpy(host_solution, d_solution, boardSize * boardSize * sizeof(int), cudaMemcpyDeviceToHost);

        // print solution
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++)
                cout << host_solution[i*boardSize+j] << " ";
                cout << endl;
        }

        // free device memory
        cudaFree(&d_new_boards);
        cudaFree(&d_old_boards);
        cudaFree(&d_solution);
        cudaFree(&d_board_num);
    }
    return 0;
}
