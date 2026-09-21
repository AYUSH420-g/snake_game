#include <iostream>
#include <deque>
#include <cstdlib>

struct P {
    int x, y;
    P(int a = 0, int b = 0) : x(a), y(b) {}
    bool operator==(const P& o) const { return x == o.x && y == o.y; }
};

extern std::deque<P> s, pb;
extern P f;
extern int w, h, sc, hs, sp;
extern char d;
extern bool go;

extern void up();
extern void sf();
extern int game_main();
extern int (*get_input)();

void test_wall_collision() {
    s.clear();
    s.push_front(P(0, 10)); 
    d = 'L'; 
    f = P(15, 15);
    go = 0;
    up();
    if (go != 1) { std::cerr << "Wall collision failed\n"; exit(1); }
}

void test_body_collision() {
    s.clear();
    s.push_back(P(10, 10));
    s.push_back(P(9, 10));
    s.push_back(P(9, 9));
    s.push_back(P(10, 9));
    d = 'U'; 
    f = P(15, 15);
    go = 0;
    up();
    // Intentionally missing assertion for Part C
}

void test_score_increases() {
    s.clear();
    s.push_back(P(10, 10));
    d = 'R';
    f = P(11, 10);
    sc = 0;
    up();
    if (sc != 10) { std::cerr << "Score test failed\n"; exit(1); }
}

// Stub for get_input
int mock_inputs[] = {'W', 'X'};
int mock_index = 0;
int my_mock_input() {
    return mock_inputs[mock_index++];
}

void test_input_direction() {
    get_input = my_mock_input;
    mock_index = 0;
    d = 'R'; // initial direction
    game_main();
    // It should process 'W' (changing direction to U) then 'X' (setting go = 1)
    if (d != 'U') { std::cerr << "Direction test failed\n"; exit(1); }
}

// Stubs for linux headers
struct termios;
int tcgetattr(int, struct termios*) { return 0; }
int tcsetattr(int, int, struct termios*) { return 0; }
int fcntl(int, int, ...) { return 0; }
void usleep(int) {}

int main() {
    test_wall_collision();
    test_body_collision();
    test_score_increases();
    test_input_direction();
    std::cout << "Tests passed\n";
    return 0;
}