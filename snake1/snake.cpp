#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <ncurses.h>

using namespace std;

#define MAX_LENGTH 1000

// Directions
const char DIR_UP = 'U';
const char DIR_DOWN = 'D';
const char DIR_LEFT = 'L';
const char DIR_RIGHT = 'R';

int consoleWidth, consoleHeight;

void initScreen()
{
    initscr(); // Initialize ncurses mode
    noecho(); // Don't echo input characters
    curs_set(0); // Hide the cursor
    keypad(stdscr, TRUE); // Enable arrow keys
    timeout(100); // Non-blocking getch with 100ms timeout
    getmaxyx(stdscr, consoleHeight, consoleWidth); // Get screen size
}

void drawWalls() {
    for (int x = 0; x < consoleWidth; ++x) {
        mvprintw(0, x, "*");
        mvprintw(consoleHeight - 1, x, "*");
    }
    for (int y = 0; y < consoleHeight; ++y) {
        mvprintw(y, 0, "*");
        mvprintw(y, consoleWidth - 1, "*");
    }
}

struct Point {
    int xCoord;
    int yCoord;
    Point() {}
    Point(int x, int y) : xCoord(x), yCoord(y) {}
};

class Snake {
    int length;
    vector<Point> body;
    char direction;
public:
    Snake() : length(1), direction(DIR_RIGHT) {
        body.push_back(Point(consoleWidth / 2, consoleHeight / 2));
    }

    void move() {
        Point head = body.front();
        Point newHead = head;

        switch (direction) {
            case DIR_UP: newHead.yCoord--; break;
            case DIR_DOWN: newHead.yCoord++; break;
            case DIR_LEFT: newHead.xCoord--; break;
            case DIR_RIGHT: newHead.xCoord++; break;
        }

        body.insert(body.begin(), newHead);
        if (body.size() > length) {
            body.pop_back();
        }
    }

    void changeDirection(char newDirection) {
        // Prevent the snake from reversing
        if ((direction == DIR_UP && newDirection == DIR_DOWN) ||
            (direction == DIR_DOWN && newDirection == DIR_UP) ||
            (direction == DIR_LEFT && newDirection == DIR_RIGHT) ||
            (direction == DIR_RIGHT && newDirection == DIR_LEFT)) {
            return;
        }
        direction = newDirection;
    }

    void grow() {
        length++;
    }

    void draw() {
        for (const auto & point : body) {
            mvprintw(point.yCoord, point.xCoord, "O");
        }
    }

    bool checkCollision() {
        Point head = body.front();
        // Check for wall collisions
        if (head.xCoord <= 0 || head.xCoord >= consoleWidth - 1 || head.yCoord <= 0 || head.yCoord >= consoleHeight - 1) {
            return true;
        }
        // Check for self collisions
        for (size_t i = 1; i < body.size(); ++i) {
            if (body[i].xCoord == head.xCoord && body[i].yCoord == head.yCoord) {
                return true;
            }
        }
        return false;
    }

    Point getHead() {
        return body.front();
    }
};

class Food {
    Point position;
public:
    Food() {
        generate();
    }

    void generate() {
        position.xCoord = rand() % (consoleWidth - 2) + 1;
        position.yCoord = rand() % (consoleHeight - 2) + 1;
    }

    void draw() {
        mvprintw(position.yCoord, position.xCoord, "X");
    }

    Point getPosition() {
        return position;
    }
};

int main() {
    srand(time(0));
    initScreen();
    Snake snake;
    Food food;
    bool gameOver = false;

    while (!gameOver) {
        clear();
        drawWalls();
        snake.draw();
        food.draw();
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP: snake.changeDirection(DIR_UP); break;
            case KEY_DOWN: snake.changeDirection(DIR_DOWN); break;
            case KEY_LEFT: snake.changeDirection(DIR_LEFT); break;
            case KEY_RIGHT: snake.changeDirection(DIR_RIGHT); break;
        }

        snake.move();
        if (snake.checkCollision()) {
            gameOver = true;
        }

        if (snake.getHead().xCoord == food.getPosition().xCoord && snake.getHead().yCoord == food.getPosition().yCoord) {
            snake.grow();
            food.generate();
        }

        this_thread::sleep_for(chrono::milliseconds(100)); // Move the snake every 100ms
    }

    endwin(); // End ncurses mode
    cout << "Game Over!" << endl;
    return 0;
}