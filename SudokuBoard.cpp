#include <iostream>
using namespace std;
#include <algorithm>
#include <vector>

void displayBoard(int a[9][9], vector<int> b)
{
    for (int i = 0; i < 9; i++)
    {
        if (i % 3 == 0 && i != 0)
        {
            cout << "------------------------------\n";
        }
        for (int j = 0; j < 9; j++)
        {

            if (j % 3 == 0 && j != 0)
            {
                cout << "| ";
            }
            int val = i * 9 + j;
            bool exists = false;
            auto it = find(b.begin(), b.end(), val);
            if (it != b.end())
            {
                exists = true;
            }
            if (!exists)
                cout << "\033[32m" << a[i][j] << "\033[0m "
                     << " ";
            else
                cout << "\033[37m" << a[i][j] << "\033[0m "
                     << " ";
        }
        cout << endl;
    }
}

void printSudoku(int board[9][9]) {
    for (int i = 0; i < 9; ++i) {
        if (i % 3 == 0 && i != 0) {
            cout << "---------------------\n";
        }

        for (int j = 0; j < 9; ++j) {
            if (j % 3 == 0 && j != 0) {
                cout << "| ";
            }

            if (board[i][j] == 0) {
                cout << ". ";
            } else {
                cout << board[i][j] << " ";
            }
        }
        cout << "\n";
    }
}

void convertTo2DArray(int flatArray[81], int twoDArray[9][9]) {
    int k = 0;  // Biến k để duyệt qua từng phần tử của mảng 1 chiều

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            twoDArray[i][j] = flatArray[k++];
        }
    }
}