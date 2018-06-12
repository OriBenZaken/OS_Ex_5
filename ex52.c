//
// Created by ori on 6/11/18.
//

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

#define BOARD_SIZE 20
#define HORIZONTAL 0
#define VERTICAL 1

typedef struct GameBoard {
    char board[BOARD_SIZE][BOARD_SIZE];
    int x_pos;
    int y_pos;
    int shapeMode;
} GameBoard;

GameBoard gameBoard;

void clearBoard(GameBoard* gameBoard) {
    int i, j;
    for (i = 0; i < BOARD_SIZE - 1; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (j == 0 || j == BOARD_SIZE - 1) {
                gameBoard->board[i][j] = '*';
                continue;
            }
            gameBoard->board[i][j] = ' ';
        }
    }

    for (j = 0; j < BOARD_SIZE; j++) {
        gameBoard->board[BOARD_SIZE - 1][j] = '*';
    }
}

void updateShapePosition(GameBoard* gameBoard, int x, int y, int flipped);

void initializeGameBoard(GameBoard* gameBoard) {
    gameBoard->shapeMode = HORIZONTAL;
    gameBoard->x_pos = 0;
    gameBoard->y_pos = BOARD_SIZE / 2 - 1;
    clearBoard(gameBoard);
    updateShapePosition(gameBoard, gameBoard->x_pos, gameBoard->y_pos, 0);

}

void clearShape(GameBoard* gameBoard) {
    if (gameBoard->shapeMode == HORIZONTAL) {
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos] = ' ';
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos + 1] = ' ';
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos + 2] = ' ';
    } else {
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos] = ' ';
        gameBoard->board[gameBoard->x_pos + 1][gameBoard->y_pos] = ' ';
        gameBoard->board[gameBoard->x_pos + 2][gameBoard->y_pos] = ' ';
    }
}

void updateShapePosition(GameBoard* gameBoard, int x, int y, int flipped) {


    if (gameBoard->shapeMode == HORIZONTAL) {
        if (y <= 0)
            return;
        if (y + 2 >= BOARD_SIZE -1 )
            return;
    }
    if (gameBoard->shapeMode == VERTICAL ) {
        if (y <= 0)
            return;
        if (y >= BOARD_SIZE - 1)
            return;
    }

    // erase previous shape
    if (!flipped)
        clearShape(gameBoard);

    gameBoard->x_pos = x;
    gameBoard->y_pos = y;
    // update current shape
    if (gameBoard->shapeMode == HORIZONTAL) {
        gameBoard->board[x][y] = '-';
        gameBoard->board[x][y + 1] = '-';
        gameBoard->board[x][y + 2] = '-';

        if (x >= BOARD_SIZE - 1) {
            initializeGameBoard(gameBoard);
        }

    } else {
        gameBoard->board[x][y] = '-';
        gameBoard->board[x + 1][y] = '-';
        gameBoard->board[x + 2][y] = '-';

        if (x + 2 >= BOARD_SIZE - 1) {
            initializeGameBoard(gameBoard);
        }
    }
}

void printBoard(GameBoard* gameBoard) {
    system("clear");
    int i, j;
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            printf("%c", gameBoard->board[i][j]);
        }
        printf("\n");
    }
}

void changeShapeMode(GameBoard* gameBoard) {
    clearShape(gameBoard);
    int prev_x_pos = gameBoard->x_pos;
    int prev_y_pos = gameBoard->y_pos;
    if (gameBoard->shapeMode == HORIZONTAL) {
        gameBoard->shapeMode = VERTICAL;
        updateShapePosition(gameBoard , prev_x_pos - 1, prev_y_pos + 1, 1);
    } else  {
        int x = prev_x_pos + 1;
        int y = prev_y_pos - 1;

        if (y + 2 >= BOARD_SIZE - 1) {
            y = BOARD_SIZE - 4;
        }
        gameBoard->shapeMode = HORIZONTAL;
        updateShapePosition(gameBoard, x, y, 1);
    }
}


void getGameKey(int sig) {
    signal(SIGUSR2, getGameKey);

    char ch;
    ch = (char)getchar();
    switch (ch) {
        case 's':
            updateShapePosition(&gameBoard, gameBoard.x_pos + 1, gameBoard.y_pos, 0);
            break;
        case 'd':
            updateShapePosition(&gameBoard, gameBoard.x_pos, gameBoard.y_pos + 1, 0);
            break;
        case 'a':
            updateShapePosition(&gameBoard, gameBoard.x_pos, gameBoard.y_pos - 1, 0);
            break;
        case 'w':
            changeShapeMode(&gameBoard);
            break;
        case 'q': exit(1);
        default:
            break;
    }
    printBoard(&gameBoard);
}

void moveShape(int sig) {
    signal(SIGALRM, moveShape);
    alarm(1);

    updateShapePosition(&gameBoard, gameBoard.x_pos + 1, gameBoard.y_pos, 0);
    printBoard(&gameBoard);
}

void startGame(GameBoard* gameBoard) {
}


int main() {
    initializeGameBoard(&gameBoard);
    printBoard(&gameBoard);
    signal(SIGALRM, moveShape);
    alarm(1);


    signal(SIGUSR2, getGameKey);
    while (1) {
        pause();
    }

}
