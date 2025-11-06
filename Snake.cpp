#include <iostream>
#include <deque>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

// Point class to represent coordinates
class Point {
public:
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {}
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

// Food class
class Food {
private:
    Point position;
public:
    void spawn(int w, int h, const deque<Point>& snake) {
        bool valid = false;
        while (!valid) {
            position.x = rand() % (w - 2) + 1;
            position.y = rand() % (h - 2) + 1;
            valid = true;
            for (auto &p : snake) {
                if (p == position) {
                    valid = false;
                    break;
                }
            }
        }
    }
    
    Point getPosition() const { return position; }
};

// Snake class
class Snake {
private:
    deque<Point> body;
    deque<Point> prevBody;
    char direction;
public:
    Snake() : direction('R') {
        body.push_back(Point(15, 10));
        body.push_back(Point(14, 10));
        body.push_back(Point(13, 10));
    }
    
    void setDirection(char d) { direction = d; }
    char getDirection() const { return direction; }
    
    Point getHead() const { return body.front(); }
    const deque<Point>& getBody() const { return body; }
    const deque<Point>& getPrevBody() const { return prevBody; }
    
    void move() {
        prevBody = body;
        Point next = body.front();
        switch (direction) {
            case 'U': next.y--; break;
            case 'D': next.y++; break;
            case 'L': next.x--; break;
            case 'R': next.x++; break;
        }
        body.push_front(next);
    }
    
    void grow() {
        // Don't pop back, snake grows
    }
    
    void shrink() {
        body.pop_back();
    }
    
    bool checkSelfCollision() const {
        Point head = body.front();
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i] == head) return true;
        }
        return false;
    }
    
    void reset() {
        body.clear();
        prevBody.clear();
        direction = 'R';
        body.push_back(Point(15, 10));
        body.push_back(Point(14, 10));
        body.push_back(Point(13, 10));
    }
};

// GameBoard class
class GameBoard {
private:
    int width, height;
    int score, highScore;
    int speed;
    bool gameOver;
    Snake snake;
    Food food;
    
    void enableNonBlocking(bool e) {
        static termios oldt, newt;
        if (e) {
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
    
    void moveCursor(int x, int y) {
        cout << "\033[" << y + 1 << ";" << x + 1 << "H";
    }
    
    void clearScreen() {
        cout << "\033[2J\033[H";
    }
    
    void loadHighScore() {
        ifstream i("h.txt");
        if (i.is_open()) {
            i >> highScore;
            i.close();
        } else {
            highScore = 0;
        }
    }
    
    void saveHighScore() {
        if (score > highScore) {
            ofstream o("h.txt");
            if (o.is_open()) {
                o << score;
                o.close();
            }
        }
    }
    
    void drawBorder() {
        clearScreen();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
                    cout << '.';
                else
                    cout << ' ';
            }
            cout << '\n';
        }
    }
    
    void render() {
        for (auto &p : snake.getPrevBody()) {
            moveCursor(p.x, p.y);
            cout << ' ';
        }
        moveCursor(food.getPosition().x, food.getPosition().y);
        cout << 'x';
        const deque<Point>& body = snake.getBody();
        for (size_t i = 0; i < body.size(); i++) {
            moveCursor(body[i].x, body[i].y);
            cout << (i ? 'O' : '0');
        }
        moveCursor(0, height);
        cout << "Score: " << score << "  High: " << highScore << "   ";
        cout.flush();
    }
    
    void handleInput() {
        int c = getchar();
        if (c != EOF) {
            c = toupper(c);
            if (c == 'W' && snake.getDirection() != 'D') snake.setDirection('U');
            else if (c == 'S' && snake.getDirection() != 'U') snake.setDirection('D');
            else if (c == 'A' && snake.getDirection() != 'R') snake.setDirection('L');
            else if (c == 'D' && snake.getDirection() != 'L') snake.setDirection('R');
            else if (c == 'X') gameOver = true;
        }
    }
    
    void update() {
        snake.move();
        Point head = snake.getHead();
        
        if (head == food.getPosition()) {
            score += 10;
            snake.grow();
            food.spawn(width, height, snake.getBody());
            if (speed > 50) speed -= 3;
        } else {
            snake.shrink();
        }
        
        if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
            gameOver = true;
        }
        
        if (snake.checkSelfCollision()) {
            gameOver = true;
        }
    }
    
public:
    GameBoard(int w = 30, int h = 20) : width(w), height(h), score(0), highScore(0), speed(150), gameOver(false) {}
    
    void init() {
        score = 0;
        speed = 150;
        gameOver = false;
        snake.reset();
        loadHighScore();
        food.spawn(width, height, snake.getBody());
        drawBorder();
        enableNonBlocking(true);
    }
    
    void run() {
        while (!gameOver) {
            handleInput();
            update();
            render();
            usleep(speed * 1000);
        }
        enableNonBlocking(false);
        saveHighScore();
        clearScreen();
        cout << "\nGAME OVER!\n";
        cout << "Score: " << score << "  High: " << highScore;
        if (score > highScore) cout << "  NEW RECORD!";
        cout << "\n\n";
    }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    GameBoard game(30, 20);
    char choice;
    do {
        game.init();
        game.run();
        cout << "Press R to Restart, Q to Quit: ";
        cin >> choice;
        choice = toupper(choice);
    } while (choice == 'R');
    cout << "\nThanks for playing!\n\n";
    return 0;
}
