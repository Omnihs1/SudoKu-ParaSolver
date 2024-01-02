#include <iostream>
using namespace std;

void convertTo2DArray(int flatArray[81], int twoDArray[9][9]) {
    int k = 0;  // Biến k để duyệt qua từng phần tử của mảng 1 chiều

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            twoDArray[i][j] = flatArray[k++];
        }
    }
}

int main() {
    // Mảng 1 chiều 81 phần tử
    int flatArray[81] = {
        5, 3, 0, 0, 7, 0, 0, 0, 0,
        6, 0, 0, 1, 9, 5, 0, 0, 0,
        0, 9, 8, 0, 0, 0, 0, 6, 0,
        8, 0, 0, 0, 6, 0, 0, 0, 3,
        4, 0, 0, 8, 0, 3, 0, 0, 1,
        7, 0, 0, 0, 2, 0, 0, 0, 6,
        0, 6, 0, 0, 0, 0, 2, 8, 0,
        0, 0, 0, 4, 1, 9, 0, 0, 5,
        0, 0, 0, 0, 8, 0, 0, 7, 9
    };

    // Mảng 2 chiều 9x9
    int twoDArray[9][9];

    // Chuyển từ mảng 1 chiều sang mảng 2 chiều
    convertTo2DArray(flatArray, twoDArray);

    // In mảng 2 chiều ra terminal để kiểm tra
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            cout << twoDArray[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}