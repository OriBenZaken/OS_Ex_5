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
#define FLIP 'w'
#define LEFT 'a'
#define RIGHT 'd'
#define DOWN 's'
#define QUIT 'q'

typedef struct GameBoard {
    char board[BOARD_SIZE][BOARD_SIZE];
    int x_pos;  // x (row) position of the shape
    int y_pos;  // y (column0 position of the shape
    int shapeMode;
} GameBoard;

GameBoard gameBoard;

/**
 * Initializes an empty game board.
 * @param gameBoard Game board
 */
void clearBoard(GameBoard* gameBoard) {
    int i, j;
    for (i = 0; i < BOARD_SIZE - 1; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (j == 0 || j == BOARD_SIZE - 1) {
                // fill side borders with '*'
                gameBoard->board[i][j] = '*';
                continue;
            }
            gameBoard->board[i][j] = ' ';
        }
    }

    // fill bottom border with '*'
    for (j = 0; j < BOARD_SIZE; j++) {
        gameBoard->board[BOARD_SIZE - 1][j] = '*';
    }
}

void updateShapePosition(GameBoard* gameBoard, int x, int y, int flipped);

/**
 * Initializes GameBoard struct. Brings the Tetris game to start mode.
 * @param gameBoard
 */
void initializeGameBoard(GameBoard* gameBoard) {
    gameBoard->shapeMode = HORIZONTAL;
    gameBoard->x_pos = 0;
    gameBoard->y_pos = BOARD_SIZE / 2 - 1;
    clearBoard(gameBoard);
    updateShapePosition(gameBoard, gameBoard->x_pos, gameBoard->y_pos, 0);

}

/**
 * Clears the shape from the game board.
 * @param gameBoard Game board
 */
void clearShape(GameBoard* gameBoard) {
    if (gameBoard->shapeMode == HORIZONTAL) {
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos] = ' ';
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos + 1] = ' ';
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos + 2] = ' ';
    } else {
        // Vertical
        gameBoard->board[gameBoard->x_pos][gameBoard->y_pos] = ' ';
        gameBoard->board[gameBoard->x_pos + 1][gameBoard->y_pos] = ' ';
        gameBoard->board[gameBoard->x_pos + 2][gameBoard->y_pos] = ' ';
    }
}

/**
 * Clears the previous the shape from the board and draws a new one.
 * @param gameBoard Game board
 * @param x x position of the new shape.
 * @param y y position of the new shape.
 * @param flipped 0 - shape was not flipped before the update, 1 - else
 */
void updateShapePosition(GameBoard* gameBoard, int x, int y, int flipped) {
    /* check that the new position is not crossing the board borders*/
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

    // erase previous shape if it wasn't flipped before calling this function.
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
        // Vertical
        gameBoard->board[x][y] = '-';
        gameBoard->board[x + 1][y] = '-';
        gameBoard->board[x + 2][y] = '-';

        if (x + 2 >= BOARD_SIZE - 1) {
            initializeGameBoard(gameBoard);
        }
    }
}

/**
 * prints the game board.
 * @param gameBoard Game board
 */
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

/**
 * Do the FLIP. changes the shape mode from HORIZONTAL to VERTICAL or the oppisite.
 * @param gameBoard Game board
 */
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
        /* flip from vertical to horizontal will cause the shape to cross the right game border.*/
        if (y + 2 >= BOARD_SIZE - 1) {
           y = BOARD_SIZE - 4;
            /* flip from vertical to horizontal will cause the shape to cross the left game border.*/
        } else if (y < 1) {
            y = 1;
        }
        gameBoard->shapeMode = HORIZONTAL;
        updateShapePosition(gameBoard, x, y, 1);
    }
}

/**
 * Gets a game key from the user and perform the suitable operation.
 * @param sig
 */
void getGameKey(int sig) {
    signal(SIGUSR2, getGameKey);

    char ch;
    ch = (char)getchar();
    switch (ch) {
        case DOWN:
            updateShapePosition(&gameBoard, gameBoard.x_pos + 1, gameBoard.y_pos, 0);
            break;
        case RIGHT:
            updateShapePosition(&gameBoard, gameBoard.x_pos, gameBoard.y_pos + 1, 0);
            break;
        case LEFT:
            updateShapePosition(&gameBoard, gameBoard.x_pos, gameBoard.y_pos - 1, 0);
            break;
        case FLIP:
            changeShapeMode(&gameBoard);
            break;
        case QUIT: exit(1);
        default:
            break;
    }
    printBoard(&gameBoard);
}

/**
 * Moves the shape one step down every 1 second.
 * @param sig
 */
void moveShape(int sig) {
    signal(SIGALRM, moveShape);
    alarm(1);

    updateShapePosition(&gameBoard, gameBoard.x_pos + 1, gameBoard.y_pos, 0);
    printBoard(&gameBoard);
}

/**
 * starts the Tetris game.
 * @param gameBoard Game board.
 */
void startGame(GameBoard* gameBoard) {
    initializeGameBoard(gameBoard);
    printBoard(gameBoard);
    signal(SIGALRM, moveShape);
    alarm(1);

    signal(SIGUSR2, getGameKey);
    while (1) {
        // wait until the process gets a siganl.
        pause();
    }
}


int main() {
    startGame(&gameBoard);
}