#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tetris_game.h"

// Global game instance
Game game;

// Tetromino definitions
const bool TETROMINOS[SHAPE_COUNT][TETROMINO_SIZE][TETROMINO_SIZE] = {
    // I-shape
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // O-shape
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // T-shape
    {
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // S-shape
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    },
    // Z-shape
    {
        {0, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // J-shape
    {
        {0, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // L-shape
    {
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    }
};

// Initialise the game
void initGame() {
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Clear the board
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game.board[y][x] = 0;
        }
    }
    
    // Initialise game state
    game.state = GAME_ACTIVE;
    game.score = 0;
    game.level = 1;
    game.linesCleared = 0;
    
    // Create initial pieces
    createNewTetromino(&game.currentPiece);
    createNewTetromino(&game.nextPiece);
}

// Create a new random tetromino
void createNewTetromino(Tetromino* tetromino) {
    tetromino->shape = rand() % SHAPE_COUNT;
    tetromino->rotation = 0;
    tetromino->x = BOARD_WIDTH / 2 - TETROMINO_SIZE / 2;
    tetromino->y = 0;
    
    // Copy tetromino shape from definition
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            tetromino->blocks[y][x] = TETROMINOS[tetromino->shape][y][x];
        }
    }
}

// Check for collision between a tetromino and a given board
static bool collides(const Game* g, Tetromino tetromino) {
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (tetromino.blocks[y][x]) {
                int boardX = tetromino.x + x;
                int boardY = tetromino.y + y;

                // Check bounds
                if (boardX < 0 || boardX >= BOARD_WIDTH ||
                    boardY >= BOARD_HEIGHT) {
                    return true;
                }

                // Check collision with placed blocks (not with empty spaces)
                if (boardY >= 0 && g->board[boardY][boardX]) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Check for collision between current tetromino and board
bool checkCollision(Tetromino tetromino) {
    return collides(&game, tetromino);
}

// Row the current piece would land on if dropped
int ghostDropY(const Game* g) {
    Tetromino temp = g->currentPiece;
    while (!collides(g, temp)) {
        temp.y++;
    }
    return temp.y - 1;
}

// Move the tetromino in the specified direction
bool moveTetromino(Direction dir) {
    // Ignore movement while paused or after game over
    if (game.state != GAME_ACTIVE) {
        return false;
    }

    Tetromino temp = game.currentPiece;

    switch (dir) {
        case DIR_LEFT:
            temp.x--;
            break;
        case DIR_RIGHT:
            temp.x++;
            break;
        case DIR_DOWN:
            temp.y++;
            break;
    }
    
    if (!checkCollision(temp)) {
        game.currentPiece = temp;
        return true;
    }
    
    // If we couldn't move down, place the tetromino
    if (dir == DIR_DOWN) {
        placeTetromino();
    }
    
    return false;
}

// Rotate the tetromino matrix 90 degrees clockwise
void rotateTetrominoMatrix(Tetromino* tetromino) {
    bool temp[TETROMINO_SIZE][TETROMINO_SIZE] = {0};
    
    // Copy rotated blocks to temp
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            temp[x][TETROMINO_SIZE - 1 - y] = tetromino->blocks[y][x];
        }
    }
    
    // Copy back to original
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            tetromino->blocks[y][x] = temp[y][x];
        }
    }
}

// Rotate the current tetromino
bool rotateTetromino() {
    // Ignore rotation while paused or after game over
    if (game.state != GAME_ACTIVE) {
        return false;
    }

    Tetromino temp = game.currentPiece;

    rotateTetrominoMatrix(&temp);
    
    if (!checkCollision(temp)) {
        game.currentPiece = temp;
        game.currentPiece.rotation = (game.currentPiece.rotation + 1) % 4;
        return true;
    }
    
    return false;
}

// Drop the current tetromino straight to its landing row and lock it
void hardDrop() {
    // Ignore drops while paused or after game over
    if (game.state != GAME_ACTIVE) {
        return;
    }

    game.currentPiece.y = ghostDropY(&game);
    placeTetromino();
}

// Place the current tetromino on the board and spawn a new one
void placeTetromino() {
    // Place the current tetromino on the board
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game.currentPiece.blocks[y][x]) {
                int boardX = game.currentPiece.x + x;
                int boardY = game.currentPiece.y + y;
                
                // If placing a block above the board, game over
                if (boardY < 0) {
                    game.state = GAME_OVER;
                    return;
                }
                
                // Mark the cell as filled with the shape type + 1 (0 is empty)
                game.board[boardY][boardX] = game.currentPiece.shape + 1;
            }
        }
    }
    
    // Clear any full lines
    clearLines();
    
    // Set the next piece as current and create new next piece
    game.currentPiece = game.nextPiece;
    createNewTetromino(&game.nextPiece);
    
    // Check if a new piece can be placed
    if (checkCollision(game.currentPiece)) {
        game.state = GAME_OVER;
    }
}

// Clear any full lines and update the score
void clearLines() {
    int linesCleared = 0;
    
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        bool lineFull = true;
        
        // Check if the line is full
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game.board[y][x] == 0) {
                lineFull = false;
                break;
            }
        }
        
        // If a line is full, clear it and move all lines above down
        if (lineFull) {
            linesCleared++;
            
            // Move all lines above down
            for (int moveY = y; moveY > 0; moveY--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    game.board[moveY][x] = game.board[moveY - 1][x];
                }
            }
            
            // Clear the top line
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game.board[0][x] = 0;
            }
            
            // Stay on the same line to check if the moved line is also full
            y--;
        }
    }
    
    // Update score based on the number of lines cleared
    if (linesCleared > 0) {
        // Classic Tetris scoring: more points for clearing multiple lines at once
        static const int lineScores[] = {0, 40, 100, 300, 1200};
        game.score += lineScores[linesCleared] * game.level;
        
        game.linesCleared += linesCleared;
        
        // Update level every 10 lines
        game.level = 1 + (game.linesCleared / 10);
    }
}

// Update game state
void updateGame(float tick) {
    static float accumulator = 0;
    static float moveDownInterval = 1.0f; // seconds
    
    if (game.state != GAME_ACTIVE) {
        return;
    }
    
    // Adjust speed based on level
    float levelSpeed = moveDownInterval - ((float)game.level * 0.05f);
    if (levelSpeed < 0.1f) levelSpeed = 0.1f; // Cap the speed
    
    // Move a piece down automatically
    accumulator += tick;
    if (accumulator >= levelSpeed) {
        moveTetromino(DIR_DOWN);
        accumulator = 0;
    }
    
    // Additional game logic can be added here
    // This would include handling user input, which would
    // call moveTetromino() and rotateTetromino()
}
