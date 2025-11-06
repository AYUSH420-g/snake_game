Snake Game (C++ Terminal Version)

📘 Project Overview
This project is a classic Snake Game implemented in C++, designed to run directly in the terminal. The player controls a snake that moves around the screen, eating food (x) to grow longer. The game ends when the snake collides with the wall or with itself.
It demonstrates object-oriented programming (OOP) concepts, file handling, non-blocking keyboard input, and real-time screen updates — all using pure C++ and standard libraries.

🧩 Features
•	Real-time gameplay (no waiting for Enter key)
•	Snake grows when eating food
•	Score increases dynamically
•	High score saved in a text file (h.txt)
•	Adjustable game speed
•	Replay (Restart or Quit) options
•	Works in Linux / Mac terminal.

🏗️ Concepts Used
•	Classes & Objects: Used for Snake, Food, and GameBoard to organize logic
•	File Handling: Stores and loads the high score using ifstream and ofstream
•	Non-blocking Input: Uses <termios.h> and <fcntl.h> to capture key presses instantly
•	Dynamic Data Structure: std::deque used to efficiently add/remove snake body segments
•	Randomization: Food spawns at random locations using rand()
•	Control Flow: Continuous game loop with real-time updates and collision detection

⚙️ How to Run the Game
On Linux / Mac Terminal:
g++ snake.cpp -o snake
./snakepp -o snake.exe

🎮 Controls
•	W - Move Up
•	A - Move Left
•	S - Move Down
•	D - Move Right
•	X - Exit Game
•	R - Restart (after Game Over)
•	Q - Quit Game


🧰 Libraries Used
•	<iostream> - Input/Output
•	<deque> - Snake body storage
•	<fstream> - File handling for high score
•	<cstdlib> - Random number generation
•	<ctime> - Random seed
•	<unistd.h> - Sleep control
•	<termios.h>, <fcntl.h> - Non-blocking input control

📈 Future Enhancements
•	Add color to snake using UI/UX.


🏁 Conclusion
This project helped us understand object-oriented programming, real-time user input, and terminal-based UI programming in C++. It was a fun and educational experience recreating one of the most iconic games of all time!

📄 Submitted by:
Team ( Byte and bite brigades)
College Name: [DAIICT]
Session: [!st year / Semester-1]
