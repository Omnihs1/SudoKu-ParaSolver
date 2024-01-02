#ifndef CUDA_SUDOKU_CUDA_CUH
#define CUDA_SUDOKU_CUDA_CU
const int boardSize = 9;
const int DEPTH = 5;
void cudaSudokuSolver(int * old_board, int num, int*solution);
void BoardGenerator(int *prev_board, int *pred_board_num, int* new_boards);
#endif
