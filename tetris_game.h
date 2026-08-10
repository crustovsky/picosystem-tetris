#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Game constants
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define TETROMINO_SIZE 4

// Game states
typedef enum {
    GAME_ACTIVE,
    GAME_PAUSED,
    GAME_OVER
} GameState;

// Tetromino shapes
typedef enum {
    I_SHAPE,
    O_SHAPE,
    T_SHAPE,
    S_SHAPE,
    Z_SHAPE,
    J_SHAPE,
    L_SHAPE,
    SHAPE_COUNT
} TetrominoShape;

// Direction for movement
typedef enum {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_DOWN
} Direction;

// Tetromino structure
typedef struct {
    TetrominoShape shape;
    int x, y;
    int rotation; // 0, 1, 2, or 3 (0, 90, 180, 270 degrees)
    bool blocks[TETROMINO_SIZE][TETROMINO_SIZE];
} Tetromino;

// Game structure
typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    Tetromino currentPiece;
    Tetromino nextPiece;
    GameState state;
    int score;
    int level;
    int linesCleared;
} Game;

// Global game instance
extern Game game;

// Game functions
void initGame();
void updateGame(float tick);
bool moveTetromino(Direction dir);
bool rotateTetromino();
void hardDrop();
void createNewTetromino(Tetromino* tetromino);
bool checkCollision(Tetromino tetromino);
void placeTetromino();
void clearLines();
void rotateTetrominoMatrix(Tetromino* tetromino);

// Row the current piece would land on if dropped (used to draw the ghost)
int ghostDropY(const Game* game);

#ifdef __cplusplus
}
#endif


#endif /* TETRIS_GAME_H */
