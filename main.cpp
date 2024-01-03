#include <stdlib.h>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cuda_runtime.h>
#include "parallelsudoku.cuh"
#include "test.cpp"
#include "CycleTimer.h"
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
    double time = CycleTimer::currentSeconds();
    int count_one_markup;
    while (!done){
        // 0. initial markup
        count_one_markup = 0;
        vector<int> markup[81]; 
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                for (int k = 1; k <= boardSize; k++){
                    if (noConflictsCPU(board, i, j, k) && board[i*boardSize+j] == 0){
                        markup[i*boardSize + j].push_back(k);
                    }
                }
                count_one_markup += (markup[i*boardSize + j].size() == 1);
            }
        }
        cout << "\n" << count_one_markup << endl;
        if(count_one_markup == 0){
#ifndef NDEBUG
        for(int i=0;i<81;i++)
            cout<<"number of markup in row number "<<i+1<<" is "<<markup[i].size()<<endl;  
#endif
            break;
        }
        // 1. elimination
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (markup[i*boardSize + j].size() == 1) {
                    board[i*boardSize + j] = markup[i*boardSize + j][0];
                    markup[i*boardSize + j].clear();
                }
            }
        }
        done = checkDone(board);
    }
    cout << "Elimination takes time: " << CycleTimer::currentSeconds() - time << endl;
    cout << "After elimination : \n";
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++)
            cout << board[i*boardSize+j] << " ";
            cout << endl;
    }
    // 2. backtracking
    if (!done){
        int *d_new_boards;
        int *d_old_boards;
        int *d_solution;
        int *d_board_num;

        int host_solution[boardSize * boardSize];
        int host_board_num = 1;

        int DEPTH = 5;
        int memSize = 81 * pow(9, DEPTH);
        ofstream outputFile;

        
        cout << "MemSize : " << memSize << endl;
        cout << "DEPTH : " << DEPTH << endl;

        outputFile.open("outputTime.csv");
        
        cudaMalloc(&d_new_boards, memSize * sizeof(int));
        cudaMalloc(&d_old_boards, memSize * sizeof(int));
        cudaMalloc(&d_solution, boardSize * boardSize * sizeof(int));
        cudaMalloc(&d_board_num, sizeof(int));

        //check allocation memory
        cudaError_t cudaStatus;
        cudaStatus = cudaMalloc(&d_new_boards, memSize * sizeof(int));
        if (cudaStatus != cudaSuccess) {
            cout << "cudaMalloc failed for d_new_boards" << endl;
            // Handle the memory allocation failure appropriately
            // e.g., clean up any previously allocated memory, return an error code, etc.
        }
        
        cudaStatus = cudaMalloc(&d_old_boards, memSize * sizeof(int));
        if (cudaStatus != cudaSuccess) {
            cout << "cudaMalloc failed for d_old_boards" << endl;
            // Handle the memory allocation failure appropriately
            // e.g., clean up any previously allocated memory, return an error code, etc.
        }
        
        cudaStatus = cudaMalloc(&d_solution, boardSize * boardSize * sizeof(int));
        if (cudaStatus != cudaSuccess) {
            cout << "cudaMalloc failed for d_solution" << endl;
            // Handle the memory allocation failure appropriately
            // e.g., clean up any previously allocated memory, return an error code, etc.
        }
        
        cudaStatus = cudaMalloc(&d_board_num, sizeof(int));
        if (cudaStatus != cudaSuccess) {
            cout << "cudaMalloc failed for d_board_num" << endl;
            // Handle the memory allocation failure appropriately
            // e.g., clean up any previously allocated memory, return an error code, etc.
        }
        cudaMemset(d_new_boards, 0, memSize * sizeof(int));
        cudaMemset(d_old_boards, 0, memSize * sizeof(int));
        cudaMemset(d_solution, 0, boardSize * boardSize * sizeof(int));
        cudaMemset(d_board_num, 0, sizeof(int));

        cudaMemcpy(d_old_boards, board, boardSize * boardSize * sizeof(int), cudaMemcpyHostToDevice);

        BoardGenerator(d_old_boards, d_board_num, d_new_boards, DEPTH);
        
        cudaMemcpy(&host_board_num, d_board_num, sizeof(int), cudaMemcpyDeviceToHost);
        cudaSudokuSolver(d_new_boards, host_board_num, d_solution);

        outputFile << DEPTH << "," << (CycleTimer::currentSeconds() - time) << "\n";
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
    cout << "cudaSudokuSolver takes time: " << CycleTimer::currentSeconds() - time << endl;
    return 0;
}
