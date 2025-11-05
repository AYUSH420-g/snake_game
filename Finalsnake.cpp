#include <iostream>
#include <deque>
#include <fstream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
using namespace std;

struct P {
    int x, y;
    P(int a = 0, int b = 0) : x(a), y(b) {}
    bool operator==(const P& o) const { return x == o.x && y == o.y; }
};

deque<P> s, pb;
P f;
int w = 30, h = 20, sc = 0, hs = 0, sp = 150;
char d = 'R';
bool go = 0;

void nb(bool e) {
    static termios o, n;
    if (e) {
        tcgetattr(0, &o);
        n = o;
        n.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &n);
        fcntl(0, F_SETFL, O_NONBLOCK);
    } else {
        tcsetattr(0, TCSANOW, &o);
        fcntl(0, F_SETFL, 0);
    }
}

void cp(int x, int y) {
    cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

void cs() {
    cout << "\033[2J\033[H";
}

void lh() {
    ifstream i("h.txt");
    if (i.is_open()) {
        i >> hs;
        i.close();
    }
}

void sh() {
    if (sc > hs) {
        ofstream o("h.txt");
        o << sc;
        o.close();
    }
}

void sf() {
    bool v = 0;
    while (!v) {
        f.x = rand() % (w - 2) + 1;
        f.y = rand() % (h - 2) + 1;
        v = 1;
        for (auto& p : s)
            if (p == f) v = 0;
    }
}

void dr() {
    for (auto& p : pb) {
        cp(p.x, p.y);
        cout << ' ';
    }
    cp(f.x, f.y);
    cout << '*';
    for (size_t i = 0; i < s.size(); i++) {
        cp(s[i].x, s[i].y);
        cout << (i ? 'o' : 'O');
    }
    cp(0, h);
    cout << "Score: " << sc << "  High: " << hs << "   ";
    pb = s;
    cout.flush();
}

void up() {
    P n = s.front();
    switch (d) {
        case 'U': n.y--; break;
        case 'D': n.y++; break;
        case 'L': n.x--; break;
        case 'R': n.x++; break;
    }
    s.push_front(n);
    if (n == f) {
        sc += 10;
        sf();
        if (sp > 50) sp -= 3;
    } else {
        s.pop_back();
    }
    if (n.x <= 0 || n.x >= w - 1 || n.y <= 0 || n.y >= h - 1)
        go = 1;
    for (size_t i = 1; i < s.size(); i++)
        if (s[i] == n) go = 1;
}

int main() {
    srand(time(0));
    lh();
    
    s.push_back(P(15, 10));
    s.push_back(P(14, 10));
    s.push_back(P(13, 10));
    sf();
    
    cs();
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            cout << (i == 0 || i == h - 1 || j == 0 || j == w - 1 ? '#' : ' ');
        cout << '\n';
    }
    
    nb(1);
    while (!go) {
        int c = getchar();
        if (c != EOF) {
            c = toupper(c);
            if (c == 'W' && d != 'D') d = 'U';
            else if (c == 'S' && d != 'U') d = 'D';
            else if (c == 'A' && d != 'R') d = 'L';
            else if (c == 'D' && d != 'L') d = 'R';
            else if (c == 'X') go = 1;
        }
        up();
        dr();
        usleep(sp * 1000);
    }
    nb(0);
    
    sh();
    cs();
    cout << "\nGAME OVER!\n";
    cout << "Score: " << sc << "  High: " << hs;
    if (sc > hs) cout << "  NEW RECORD!";
    cout << "\n\n";
    
    return 0;
}
