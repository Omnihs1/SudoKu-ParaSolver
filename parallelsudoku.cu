#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>
#include "board.hpp"
#include "parallelsudoku.cuh"
#include "CycleTimer.h"
#define UPDIV(n, d) (((n)+(d)-1)/(d))
#define NDEBUG 1
const int threadsPerBlock = 512;

// function to examine if there are conflicts or not if cell [row][col] is num
__device__
bool noConflicts(int matrix[boardSize * boardSize], int row, int col, int num) {
    if (num <= 0 || num > boardSize) return false;
    for (int i = 0; i < boardSize; i++) {
        if (i == row)   continue;
        if (matrix[i * boardSize + col] == num) {
            return false;
        }
    }
    
    for (int j = 0; j < boardSize; j++) {
        if (j == col)   continue;
        if (matrix[row * boardSize + j] == num) {
            return false;
        }
    }
    
    for (int i = 0; i < box_size; i++) {
        for (int j = 0; j < box_size; j++) {
            int mat_row = (row/box_size)*box_size + i;
            int mat_col = (col/box_size)*box_size + j;
            if (mat_row == row && mat_col == col)   continue;
            if (matrix[mat_row * boardSize + mat_col] == num) {
                return false;
            }
        }
    }
    return true;
}

// find the next empty cell index 
__device__
int findNextEmptyCellIndex(int matrix[boardSize*boardSize], int start) {
    int i;
    for (i = start; i < boardSize*boardSize; i++) {
        if (matrix[i] == 0) {
            return i;
        }
    }
    return i;
}

// the kernel that solves sudoku problem on each board
// each thread works on a board in the boards array
__global__
void SolvingKernel(int* boards, int boardCnt, int* solution, int numThreads, int *finished) {
    int tidx = blockIdx.x * blockDim.x + threadIdx.x;
    // printf("\n");
    int localBoard[boardSize*boardSize];
    for (int idx = tidx; (idx < boardCnt) && (*finished == 0); idx += numThreads) {
        int emptyCnt = 0;
        int emptyIndex[boardSize*boardSize];
        int start = idx * boardSize * boardSize;
        for (int i = start; i < (idx+1) * boardSize * boardSize; i++) {
            localBoard[i-start] = boards[i];
            if (!localBoard[i-start]) {
                emptyIndex[emptyCnt] = i-start;
                emptyCnt++;
            }
        }
#ifndef NDEBUG
        printf("\n%d, %d\n", idx, emptyCnt);
#endif
        // backtracking algorithm
        int depth = 0;
        while (depth >= 0 && depth < emptyCnt) {
            int next = emptyIndex[depth];
            int row = next / boardSize;
            int col = next % boardSize;
            localBoard[next]++;
            if (noConflicts(localBoard, row, col, localBoard[next])) depth++;
            else if (localBoard[next] >= boardSize) {
                localBoard[next] = 0;
                depth--;
            }
        }
        if (depth == emptyCnt) {
            // solved board found 
            *finished = 1;
            // copy board to output
            // printf("\n%d\n", idx);
            memcpy(solution, localBoard, boardSize*boardSize*sizeof(int));
            break;
        }
    }
}



// kernel that generates new boards from previous ones
// call this kernel multiple times to have 
__global__
void BoardGenerationKernel(int* board_num, int prev_board_num, int* new_boards, int numThreads) {
    int tidx = blockIdx.x * blockDim.x + threadIdx.x;
    
    int* localBoard = (int*) malloc(sizeof(int)*boardSize*boardSize);

    if (!prev_board_num) prev_board_num++;
    for (int idx = tidx; idx < prev_board_num; idx+=numThreads) {
        int start = idx * boardSize * boardSize;
        for (int i = start; i < (idx+1) * boardSize * boardSize; i++) {
            localBoard[i-start] = new_boards[i];
        }
        int emptyIdx = findNextEmptyCellIndex(localBoard, 0);
        if (emptyIdx == boardSize*boardSize)  return;
        for (int k = 1; k <= boardSize; k++) {
            localBoard[emptyIdx] = k;
            if (noConflicts(localBoard, emptyIdx/boardSize, emptyIdx%boardSize, k)) {
                int offset = atomicAdd(board_num, 1);
                for (int ii = 0; ii < boardSize*boardSize; ii++) {
                    __syncthreads();
                    new_boards[boardSize*boardSize*offset+ii] = localBoard[ii];
                    __syncthreads();
                }
            }
            else{
                localBoard[emptyIdx] = 0;
            }
        }
    }
}

void 
BoardGenerator(int* prev_board_num, int* new_boards, int DEPTH) {
    int i;
    int num = 1;
    for (i = 0; i < DEPTH; i++) {
        int block = UPDIV(num, threadsPerBlock);
        cudaMemset(prev_board_num, 0, sizeof(int));
        BoardGenerationKernel<<<block, threadsPerBlock>>>(prev_board_num, num, new_boards, block*threadsPerBlock);
        cudaMemcpy(&num, prev_board_num, sizeof(int), cudaMemcpyDeviceToHost);
        printf("total boards after an iteration %d: %d \n", i + 1, num);
    }
#ifndef NDEBUG
    int memSize = pow(2, 26);
    int *board = (int*)malloc(memSize * sizeof(int));
    cudaMemcpy(board, new_boards, memSize * sizeof(int), cudaMemcpyDeviceToHost);
    for(int i = 0; i < num; i++){
      for(int j = 0; j < 81; j++){
        printf("%5d", board[i*81+j]);
      }
      printf("\n");
    }
#endif
}

void 
cudaSudokuSolver(int* boards, int board_num, int* solution) {
    int block = UPDIV(board_num, threadsPerBlock);
    int *finished;
    cudaMalloc(&finished, sizeof(int));
    cudaMemset(finished, 0, sizeof(int));
    cout << "Block: " << block << ", " << "threadsPerBlock: " << threadsPerBlock << endl;
    SolvingKernel<<<block, threadsPerBlock>>>(boards, board_num, solution, block*threadsPerBlock, finished);
    cudaDeviceSynchronize();
}