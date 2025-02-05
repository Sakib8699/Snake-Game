#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <stdexcept>

using namespace std;

bool gameOver;
const int width = 40;
const int height = 20;
int x, y, fruitX, fruitY, score, highScore;
int tailX[100], tailY[100];
int nTail;

int obstacles[][2] = {
    {8, 8}, {8, 9}, {8, 6}, {8, 13}, {8, 7}, {8, 12}, {8, 10}, {8, 11},
    {width / 2, 6}, {width / 2, 7}, {width / 2, 8}, {width / 2, 9}, {width / 2, 10},
    {width / 2, 11}, {width / 2, 12}, {width / 2, 13}, {14, height / 3}, {9, height / 3},
    {10, height / 3}, {11, height / 3}, {12, height / 3}, {13, height / 3}, {15, height / 3},
    {16, height / 3}, {17, height / 3}, {18, height / 3}, {19, height / 3}
};

const int numObstacles = sizeof(obstacles) / sizeof(obstacles[0]);

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void LoadHighScore() {
    ifstream file("highscore.data");
    if (file.is_open()) {
        try {
            if (!(file >> highScore)) {
                throw runtime_error("Error reading high score.");
            }
        } catch (const exception& e) {
            cerr << "Exception: " << e.what() << endl;
            highScore = 0;
        }
        file.close();
    } else {
        highScore = 0;
    }
}

void SaveHighScore() {
    if (score > highScore) {
        highScore = score;
        ofstream file("highscore.data");
        if (file.is_open()) {
            try {
                file << highScore;
            } catch (const exception& e) {
                cerr << "Exception: " << e.what() << endl;
            }
            file.close();
        } else {
            cerr << "Error: Unable to save high score." << endl;
        }
    }
}

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 6;
    y = height / 3;

    do {
        fruitX = rand() % width;
        fruitY = rand() % height;
    } while ([&]() {
        for (int i = 0; i < numObstacles; i++)
            if (obstacles[i][0] == fruitX && obstacles[i][1] == fruitY)
                return true;
        return false;
    }());

    score = 0;
    LoadHighScore();
}

void Draw() {
    system("cls");
    for (int i = 0; i < width + 2; i++) cout << "-";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "|";

            if (i == y && j == x) {
                SetColor(10);
                cout << "o";
                SetColor(7);
            } else if (i == fruitY && j == fruitX) {
                SetColor(12);
                cout << "*";
                SetColor(7);
            } else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        SetColor(10);
                        cout << "o";
                        SetColor(7);
                        print = true;
                    }
                }
                for (int k = 0; k < numObstacles; k++) {
                    if (obstacles[k][0] == j && obstacles[k][1] == i) {
                        cout << "X";
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) cout << "|";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "-";
    cout << endl;
    cout << "Score: " << score << "  |  High Score: " << highScore << endl;
}

void Input() {
    if (_kbhit()) {
        int key = getch();
        if (key == 224) {
            key = getch();
            switch (key) {
                case 75: if (dir != RIGHT) dir = LEFT; break;
                case 77: if (dir != LEFT) dir = RIGHT; break;
                case 72: if (dir != DOWN) dir = UP; break;
                case 80: if (dir != UP) dir = DOWN; break;
            }
        } else {
            switch (key) {
                case 'a': if (dir != RIGHT) dir = LEFT; break;
                case 'd': if (dir != LEFT) dir = RIGHT; break;
                case 'w': if (dir != DOWN) dir = UP; break;
                case 's': if (dir != UP) dir = DOWN; break;
                case 'x': gameOver = true; break;
            }
        }
    }
}

void Logic() {
    int prevX = tailX[0], prevY = tailY[0], prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
        case LEFT: x--; break;
        case RIGHT: x++; break;
        case UP: y--; break;
        case DOWN: y++; break;
        default: break;
    }

    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    for (int i = 0; i < numObstacles; i++)
        if (obstacles[i][0] == x && obstacles[i][1] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 10;
        nTail++;
        do {
            fruitX = rand() % width;
            fruitY = rand() % height;
        } while ([&]() {
            for (int i = 0; i < numObstacles; i++)
                if (obstacles[i][0] == fruitX && obstacles[i][1] == fruitY)
                    return true;
            return false;
        }());
    }
}

int main() {
    try {
        Setup();
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            Sleep(100);
        }
        SaveHighScore();
        cout << "Game Over! Final Score: " << score << endl;
        cout << "Press any key to exit..." << endl;
        _getch();
    } catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
    }
    return 0;
}
