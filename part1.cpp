#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

// Set terminal input to non-blocking / no echo
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
    // ANSI escape: move cursor to row = y+1, col = x+1
    cout << "\033[" << (y+1) << ";" << (x+1) << "H";
}

void clearScreen() {
    // Clear screen and move cursor home
    cout << "\033[2J\033[H";
}

// Point class to represent coordinates
class Point {
public:
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// Food class
class Food {
private:
    Point position;
    char symbol;
public:
    Food() : symbol('*') {}
    
    void spawn(int gridWidth, int gridHeight, const deque<Point>& snakeBody) {
        bool validPosition = false;
        while (!validPosition) {
            position.x = rand() % (gridWidth - 2) + 1;
            position.y = rand() % (gridHeight - 2) + 1;
            validPosition = true;
            for (const auto& segment : snakeBody) {
                if (segment == position) {
                    validPosition = false;
                    break;
                }
            }
        }
    }
    
    Point getPosition() const { return position; }
    char getSymbol() const { return symbol; }
};

// Snake class
class Snake {
private:
    deque<Point> body;
    char direction;
    char headSymbol;
    char bodySymbol;
    bool growing;
    
public:
    Snake(int startX, int startY)
        : direction('R'), headSymbol('O'), bodySymbol('o'), growing(false) {
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX - 1, startY));
        body.push_back(Point(startX - 2, startY));
    }
    
    void setDirection(char newDir) {
        if ((direction == 'U' && newDir == 'D') ||
            (direction == 'D' && newDir == 'U') ||
            (direction == 'L' && newDir == 'R') ||
            (direction == 'R' && newDir == 'L')) {
            return;
        }
        direction = newDir;
    }
    
    char getDirection() const { return direction; }
    
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
        if (!growing) {
            body.pop_back();
        } else {
            growing = false;
        }
    }
    
    void grow() {
        growing = true;
    }
    
    Point getHead() const {
        return body.front();
    }
    
    const deque<Point>& getBody() const {
        return body;
    }
    
    bool checkSelfCollision() const {
        Point head = body.front();
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i] == head) return true;
        }
        return false;
    }
    
    char getHeadSymbol() const { return headSymbol; }
    char getBodySymbol() const { return bodySymbol; }
};

// GameBoard class with optimized rendering
class GameBoard {
private:
    int width, height;
    deque<Point> previousBody;
    
public:
    GameBoard(int w, int h) : width(w), height(h) {}
    
    void drawBorder() {
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
    
    void display(const Snake& snake, const Food& food, int score) {
        // Clear previous snake position
        for (const auto& seg : previousBody) {
            setCursorPosition(seg.x, seg.y);
            cout << ' ';
        }
        
        // Draw food
        Point foodPos = food.getPosition();
        setCursorPosition(foodPos.x, foodPos.y);
        cout << food.getSymbol();
        
        // Draw snake
        const auto& body = snake.getBody();
        for (size_t i = 0; i < body.size(); i++) {
            setCursorPosition(body[i].x, body[i].y);
            if (i == 0) {
                cout << snake.getHeadSymbol();
            } else {
                cout << snake.getBodySymbol();
            }
        }
        
        // Update score and controls
        setCursorPosition(0, height);
        cout << "Score: " << score << "   ";
        setCursorPosition(0, height + 1);
        cout << "Controls: W(Up) A(Left) S(Down) D(Right) | X(Exit)";
        
        previousBody = body;
        cout.flush();
    }
    
    bool checkBoundaryCollision(const Point& head) const {
        return head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1;
    }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void clear() {
        clearScreen();
    }
};

// Game class - Main game controller
class Game {
private:
    GameBoard* board;
    Snake* snake;
    Food* food;
    int score;
    bool gameOver;
    int speed;  // milliseconds between moves
    
public:
    Game(int boardWidth = 30, int boardHeight = 20)
        : score(0), gameOver(false), speed(150) {
        board = new GameBoard(boardWidth, boardHeight);
        snake = new Snake(boardWidth / 2, boardHeight / 2);
        food = new Food();
        food->spawn(boardWidth, boardHeight, snake->getBody());
    }
    
    ~Game() {
        delete board;
        delete snake;
        delete food;
    }
    
    void handleInput() {
        if (kbhit()) {
            char key = toupper(getch_nonblocking());
            switch (key) {
                case 'W': snake->setDirection('U'); break;
                case 'S': snake->setDirection('D'); break;
                case 'A': snake->setDirection('L'); break;
                case 'D': snake->setDirection('R'); break;
                case 'X': gameOver = true; break;
            }
        }
    }
    
    void update() {
        snake->move();
        
        if (board->checkBoundaryCollision(snake->getHead())) {
            gameOver = true;
            return;
        }
        
        if (snake->checkSelfCollision()) {
            gameOver = true;
            return;
        }
        
        if (snake->getHead() == food->getPosition()) {
            snake->grow();
            score += 10;
            food->spawn(board->getWidth(), board->getHeight(), snake->getBody());
            if (speed > 50) speed -= 3;
        }
    }
    
    void render() {
        board->display(*snake, *food, score);
    }
    
    void run() {
        board->clear();
        board->drawBorder();
        
        setNonBlocking(true);
        while (!gameOver) {
            handleInput();
            update();
            render();
            usleep(speed * 1000);
        }
        setNonBlocking(false);
        
        showGameOver();
    }
    
    void showGameOver() {
        board->clear();
        cout << "\n\n";
        cout << "  |------------------------------------|\n";
        cout << "  |          GAME OVER!                |\n";
        cout << "  |------------------------------------|\n";
        cout << "  |                                    |\n";
        cout << "  |       Final Score: " << score;
        cout << "               |\n";
        cout << "  |                                    |\n";
        cout << "  |------------------------------------|\n\n";
        
        cout << "  Play again? (Y/N): ";
        char choice;
        cin >> choice;
        
        if (toupper(choice) == 'Y') {
            delete board;
            delete snake;
            delete food;
            
            board = new GameBoard(30, 20);
            snake = new Snake(15, 10);
            food = new Food();
            food->spawn(30, 20, snake->getBody());
            score = 0;
            gameOver = false;
            speed = 150;
            
            run();
        }
    }
    
    bool isGameOver() const { return gameOver; }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    cout << "\n\n";
    cout << "  |------------------------------------|\n";
    cout << "  |            SNAKE GAME              |\n";
    cout << "  |------------------------------------|\n\n";
    cout << "  Instructions:\n";
    cout << "  - Use W/A/S/D\n";
    cout << "  - Eat food (*) to grow and score\n";
    cout << "  - Don't hit walls or yourself!\n";
    cout << "  - Press X to exit\n\n";
    cout << "  Press any key to start...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    Game game(30, 20);
    game.run();
    
    cout << "\n\n  Thanks for playing!\n\n";
    return 0;
}
