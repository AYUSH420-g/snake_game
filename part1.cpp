#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

void setNonBlocking(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, 0);
    }
}

bool kbhit() {
    int c = getchar();
    if (c != EOF) {
        ungetc(c, stdin);
        return true;
    }
    return false;
}

char getch_nonblocking() {
    int c = getchar();
    if (c == EOF) return 0;
    return static_cast<char>(c);
}

void setCursorPosition(int x, int y) {
    cout << "\033[" << (y+1) << ";" << (x+1) << "H";
}
void clearScreen() {
    cout << "\033[2J\033[H";
}

class Point {
public:
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
private:
    deque<Point> body;
    char direction;
    char headSymbol;
    char bodySymbol;
public:
    Snake(int startX, int startY) 
      : direction('R'), headSymbol('O'), bodySymbol('o') {
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX - 1, startY));
        body.push_back(Point(startX - 2, startY));
    }
    void setDirection(char newDir) {
        if ((direction == 'U' && newDir == 'D') ||
            (direction == 'D' && newDir == 'U') ||
            (direction == 'L' && newDir == 'R') ||
            (direction == 'R' && newDir == 'L')) return;
        direction = newDir;
    }
    void move() {
        Point head = body.front();
        Point newHead = head;
        switch (direction) {
            case 'U': newHead.y--; break;
            case 'D': newHead.y++; break;
            case 'L': newHead.x--; break;
            case 'R': newHead.x++; break;
        }
        body.push_front(newHead);
        body.pop_back();
    }
    const deque<Point>& getBody() const {
        return body;
    }
    char getHeadSymbol() const { return headSymbol; }
    char getBodySymbol() const { return bodySymbol; }
};

class Food {
private:
    Point position;
    char symbol;
public:
    Food() : symbol('*') {}
    void spawn(int gridWidth, int gridHeight, const deque<Point>& snakeBody) {
        bool valid = false;
        while (!valid) {
            position.x = rand() % (gridWidth - 2) + 1;
            position.y = rand() % (gridHeight - 2) + 1;
            valid = true;
            for (auto& seg : snakeBody) {
                if (seg == position) { valid = false; break; }
            }
        }
    }
    Point getPosition() const { return position; }
    char getSymbol() const { return symbol; }
};

int main() {
    srand((unsigned)time(nullptr));
    int boardWidth = 30, boardHeight = 20;
    Snake snake(boardWidth/2, boardHeight/2);
    Food food;
    food.spawn(boardWidth, boardHeight, snake.getBody());
    int score = 0;
    clearScreen();
    while (true) {
        if (kbhit()) {
            char key = toupper(getch_nonblocking());
            switch(key) {
                case 'W': snake.setDirection('U'); break;
                case 'S': snake.setDirection('D'); break;
                case 'A': snake.setDirection('L'); break;
                case 'D': snake.setDirection('R'); break;
                case 'X': return 0;
            }
        }
        snake.move();
        if (snake.getBody().front() == food.getPosition()) {
            score += 10;
            food.spawn(boardWidth, boardHeight, snake.getBody());
        }
        // draw food
        setCursorPosition(food.getPosition().x, food.getPosition().y);
        cout << food.getSymbol();
        // draw snake
        for (auto& seg : snake.getBody()) {
            setCursorPosition(seg.x, seg.y);
            cout << ( &seg == &snake.getBody().front() ? snake.getHeadSymbol() : snake.getBodySymbol() );
        }
        // draw score
        setCursorPosition(0, boardHeight);
        cout << "Score: " << score << "   ";
        usleep(150 * 1000);
    }
    return 0;
}
