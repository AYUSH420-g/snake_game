// snake_part2.cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

void clearScreen() {
    cout << "\033[2J\033[H";
}

void drawBorder(int width, int height) {
    clearScreen();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                cout << '#';
            } else {
                cout << ' ';
            }
        }
        cout << endl;
    }
}

int main() {
    int boardWidth = 30;
    int boardHeight = 20;
    drawBorder(boardWidth, boardHeight);
    
    // Display score and controls
    cout << "\033[" << (boardHeight + 1) << ";0H"; // move cursor
    cout << "Score: 0   ";
    cout << "\n";
    cout << "Controls: W(Up) A(Left) S(Down) D(Right) | X(Exit)";
    
    cout << "\n\nPress Enter to exit…";
    cin.get();
    return 0;
}
