#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    game.hintActive = false;
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

// Row a tetromino would land on if dropped
static int dropY(const Game* g, Tetromino tetromino) {
    while (!collides(g, tetromino)) {
        tetromino.y++;
    }
    return tetromino.y - 1;
}

// Row the current piece would land on if dropped
int ghostDropY(const Game* g) {
    return dropY(g, g->currentPiece);
}

static Tetromino bestPlacement(const Game* g);

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
        if (dir != DIR_DOWN) {
            // The player took over, drop the hint
            game.hintActive = false;
        } else if (game.hintActive) {
            // Falling can change which spots are still reachable
            game.hint = bestPlacement(&game);
        }
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
        game.hintActive = false;
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

// Is a cell filled, treating the walls and floor as filled
static bool filled(int board[BOARD_HEIGHT][BOARD_WIDTH], int x, int y) {
    if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT) return true;
    return y >= 0 && board[y][x];
}

// Score the board left after locking a tetromino, higher is better.
// Features and weights are Pierre Dellacherie's, as tuned for El-Tetris.
static float placementScore(const Game* g, Tetromino t) {
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    memcpy(board, g->board, sizeof board);

    int top = BOARD_HEIGHT, bottom = 0;
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (t.blocks[y][x]) {
                // Locking above the board ends the game
                if (t.y + y < 0) {
                    return -1e9f;
                }
                board[t.y + y][t.x + x] = 1;
                if (t.y + y < top) top = t.y + y;
                if (t.y + y > bottom) bottom = t.y + y;
            }
        }
    }
    float landingHeight = BOARD_HEIGHT - (top + bottom) / 2.0f;

    // Remove full rows by copying the others down
    int lines = 0;
    int dst = BOARD_HEIGHT - 1;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!board[y][x]) full = false;
        }
        if (full) {
            lines++;
        } else {
            memcpy(board[dst--], board[y], sizeof board[y]);
        }
    }
    while (dst >= 0) {
        memset(board[dst--], 0, sizeof board[0]);
    }

    int rowTransitions = 0, colTransitions = 0, holes = 0, wells = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x <= BOARD_WIDTH; x++) {
            if (filled(board, x - 1, y) != filled(board, x, y)) rowTransitions++;
        }
    }
    for (int x = 0; x < BOARD_WIDTH; x++) {
        int wellDepth = 0;
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            bool cell = filled(board, x, y);
            if (filled(board, x, y - 1) != cell) colTransitions++;
            if (!cell && filled(board, x, y - 1)) holes++;
            if (!cell && filled(board, x - 1, y) && filled(board, x + 1, y)) {
                wells += ++wellDepth;
            } else {
                wellDepth = 0;
            }
        }
        if (!filled(board, x, BOARD_HEIGHT - 1)) colTransitions++;
    }

    return -4.500158f * landingHeight + 3.418127f * lines
         - 3.217888f * rowTransitions - 9.348695f * colTransitions
         - 7.899265f * holes - 3.385597f * wells;
}

// Best spot the current piece can reach by rotating in place and sliding
// sideways, as a player would
static Tetromino bestPlacement(const Game* g) {
    Tetromino rotated = g->currentPiece;
    Tetromino best = g->currentPiece;
    float bestScore = -1e30f;

    for (int r = 0; r < 4; r++) {
        for (int x = -TETROMINO_SIZE + 1; x < BOARD_WIDTH; x++) {
            // Slide towards x, giving up if anything is in the way
            Tetromino t = rotated;
            while (t.x != x && !collides(g, t)) {
                t.x += x > t.x ? 1 : -1;
            }
            if (collides(g, t)) {
                continue;
            }

            t.y = dropY(g, t);
            float score = placementScore(g, t);
            if (score > bestScore) {
                bestScore = score;
                best = t;
            }
        }

        rotateTetrominoMatrix(&rotated);
        rotated.rotation = (rotated.rotation + 1) % 4;
        if (collides(g, rotated)) {
            break;
        }
    }

    return best;
}

// The first press shows where the current piece would go, the second press
// drops it there
void autoPlace() {
    if (game.state != GAME_ACTIVE) {
        return;
    }

    if (!game.hintActive) {
        game.hint = bestPlacement(&game);
        game.hintActive = true;
        return;
    }

    game.currentPiece = game.hint;
    placeTetromino();
}

// Place the current tetromino on the board and spawn a new one
void placeTetromino() {
    game.hintActive = false;

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

// Update game state, dropping faster while softDrop is held
void updateGame(float tick, bool softDrop) {
    static float accumulator = 0;
    static float moveDownInterval = 1.0f; // seconds
    
    if (game.state != GAME_ACTIVE) {
        return;
    }
    
    // Adjust speed based on level
    float levelSpeed = moveDownInterval - ((float)game.level * 0.05f);
    if (levelSpeed < 0.1f) levelSpeed = 0.1f; // Cap the speed
    if (softDrop && levelSpeed > 0.05f) levelSpeed = 0.05f;
    
    // Move a piece down automatically
    accumulator += tick;
    if (accumulator >= levelSpeed) {
        moveTetromino(DIR_DOWN);
        accumulator = 0;
    }
}
