#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tetris_display.h"
#include "tetris_game.h"

// Helper function to convert RGB565 to Color structure
Color RGB565toColor(uint16_t color) {
    Color result;
    result.r = ((color >> 11) & 0x1F) << 3;  // 5 bits to 8 bits
    result.g = ((color >> 5) & 0x3F) << 2;   // 6 bits to 8 bits
    result.b = (color & 0x1F) << 3;          // 5 bits to 8 bits
    result.a = 255;                          // Full opacity
    return result;
}

// Helper function to extract RGB values from RGB565 color format
void extractRGB565(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b) {
    if (r) *r = ((color >> 11) & 0x1F) << 3;  // 5 bits to 8 bits
    if (g) *g = ((color >> 5) & 0x3F) << 2;   // 6 bits to 8 bits
    if (b) *b = (color & 0x1F) << 3;          // 5 bits to 8 bits
}

// Tetromino colours in RGB565 format
const uint16_t TETROMINO_COLORS[SHAPE_COUNT + 1] = {
    COLOR_BLACK,  // Empty
    COLOR_CYAN,   // I-shape
    COLOR_YELLOW, // O-shape
    COLOR_PURPLE, // T-shape
    COLOR_GREEN,  // S-shape
    COLOR_RED,    // Z-shape
    COLOR_BLUE,   // J-shape
    COLOR_ORANGE  // L-shape
};

// Current display renderer
DisplayRenderer* currentRenderer = NULL;

// Set the current renderer
void setDisplayRenderer(DisplayRenderer* renderer) {
    currentRenderer = renderer;
}

// Generic display functions that route to the current renderer
void displayInit() {
    if (currentRenderer && currentRenderer->init) {
        currentRenderer->init();
    }
}

void displayClear(uint16_t color) {
    if (currentRenderer && currentRenderer->clear) {
        currentRenderer->clear(color);
    }
}

void displayDrawPixel(int x, int y, uint16_t color) {
    if (currentRenderer && currentRenderer->drawPixel) {
        currentRenderer->drawPixel(x, y, color);
    }
}

void displayDrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    if (currentRenderer && currentRenderer->drawLine) {
        currentRenderer->drawLine(x0, y0, x1, y1, color);
    }
}

void displayDrawRect(int x, int y, int width, int height, uint16_t color) {
    if (currentRenderer && currentRenderer->drawRect) {
        currentRenderer->drawRect(x, y, width, height, color);
    }
}

void displayFillRect(int x, int y, int width, int height, uint16_t color) {
    if (currentRenderer && currentRenderer->fillRect) {
        currentRenderer->fillRect(x, y, width, height, color);
    }
}

void displayDrawText(int x, int y, const char* text, uint16_t color, uint8_t size) {
    if (currentRenderer && currentRenderer->drawText) {
        currentRenderer->drawText(x, y, text, color, size);
    }
}

void displayUpdate() {
    if (currentRenderer && currentRenderer->update) {
        currentRenderer->update();
    }
}

/**
 * Clear the screen for Tetris drawing
 */
void clearScreen() {
    displayClear(COLOR_LIGHT_GRAY);
}

/**
 * Draw the game title
 */
void drawTitle() {
    displayFillRect(0, 0, DISPLAY_WIDTH, 20, COLOR_DARK_GRAY);
    displayDrawText(DISPLAY_WIDTH / 2, 10, "TETRIS", COLOR_WHITE, 2);
}

/**
 * Draw the game board
 */
void drawBoard(const Game* game) {
    // Draw board background
    displayFillRect(BOARD_X, BOARD_Y, BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT, COLOR_BLACK);
    
    // Draw grid lines
    for (int x = 1; x < BOARD_PIXEL_WIDTH / CELL_WIDTH; x++) {
        displayDrawLine(BOARD_X + x * CELL_WIDTH, BOARD_Y, 
                       BOARD_X + x * CELL_WIDTH, BOARD_Y + BOARD_PIXEL_HEIGHT, COLOR_DARK_GRAY);
    }
    
    for (int y = 1; y < BOARD_PIXEL_HEIGHT / CELL_HEIGHT; y++) {
        displayDrawLine(BOARD_X, BOARD_Y + y * CELL_HEIGHT,
                       BOARD_X + BOARD_PIXEL_WIDTH, BOARD_Y + y * CELL_HEIGHT, COLOR_DARK_GRAY);
    }
    
    // Create a temporary board with the current piece
    int tempBoard[BOARD_PIXEL_HEIGHT / CELL_HEIGHT][BOARD_PIXEL_WIDTH / CELL_WIDTH] = {0};
    
    // Copy the main board
    for (int y = 0; y < BOARD_PIXEL_HEIGHT / CELL_HEIGHT; y++) {
        for (int x = 0; x < BOARD_PIXEL_WIDTH / CELL_WIDTH; x++) {
            tempBoard[y][x] = game->board[y][x];
        }
    }
    
    // Add the current piece to the temporary board
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game->currentPiece.blocks[y][x]) {
                int boardX = game->currentPiece.x + x;
                int boardY = game->currentPiece.y + y;
                
                if (boardX >= 0 && boardX < BOARD_PIXEL_WIDTH / CELL_WIDTH && 
                    boardY >= 0 && boardY < BOARD_PIXEL_HEIGHT / CELL_HEIGHT) {
                    tempBoard[boardY][boardX] = game->currentPiece.shape + 1;
                }
            }
        }
    }
    
    // Draw filled cells
    for (int y = 0; y < BOARD_PIXEL_HEIGHT / CELL_HEIGHT; y++) {
        for (int x = 0; x < BOARD_PIXEL_WIDTH / CELL_WIDTH; x++) {
            if (tempBoard[y][x] > 0) {
                uint16_t color = TETROMINO_COLORS[tempBoard[y][x]];
                
                // Fill the cell
                displayFillRect(
                    BOARD_X + x * CELL_WIDTH + 1,
                    BOARD_Y + y * CELL_HEIGHT + 1,
                    CELL_WIDTH - 1,
                    CELL_HEIGHT - 1,
                    color
                );
                
                // Add highlight and shadow for 3D effect
                displayDrawRect(
                    BOARD_X + x * CELL_WIDTH + 1,
                    BOARD_Y + y * CELL_HEIGHT + 1,
                    CELL_WIDTH - 1,
                    CELL_HEIGHT - 1,
                    COLOR_WHITE
                );
            }
        }
    }
    
    // Draw board border
    displayDrawRect(BOARD_X, BOARD_Y, BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT, COLOR_WHITE);
}

/**
 * Draw the next piece preview
 */
void drawNextPiece(const Game* game) {
    // Draw the next piece background and border
    displayFillRect(NEXT_X, NEXT_Y, NEXT_WIDTH, NEXT_HEIGHT, COLOR_BLACK);
    displayDrawRect(NEXT_X, NEXT_Y, NEXT_WIDTH, NEXT_HEIGHT, COLOR_WHITE);
    
    // Draw the "NEXT" label
    displayDrawText(NEXT_X + NEXT_WIDTH / 2, NEXT_Y + NEXT_HEIGHT + 10, "NEXT", COLOR_BLACK, 1);
    
    // Calculate position to centre the piece
    int offsetX = (NEXT_WIDTH - (TETROMINO_SIZE * NEXT_CELL_SIZE)) / 2;
    int offsetY = (NEXT_HEIGHT - (TETROMINO_SIZE * NEXT_CELL_SIZE)) / 2;
    
    // Draw the next piece
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game->nextPiece.blocks[y][x]) {
                uint16_t color = TETROMINO_COLORS[game->nextPiece.shape + 1];
                
                // Fill the cell
                displayFillRect(
                    NEXT_X + offsetX + x * NEXT_CELL_SIZE + 1,
                    NEXT_Y + offsetY + y * NEXT_CELL_SIZE + 1,
                    NEXT_CELL_SIZE - 1,
                    NEXT_CELL_SIZE - 1,
                    color
                );

                // Add highlight and shadow for 3D effect
                displayDrawRect(
                    NEXT_X + offsetX + x * NEXT_CELL_SIZE + 1,
                    NEXT_Y + offsetY + y * NEXT_CELL_SIZE + 1,
                    NEXT_CELL_SIZE - 1,
                    NEXT_CELL_SIZE - 1,
                    COLOR_WHITE
                );
            }
        }
    }
}

/**
 * Draw score, level, and lines cleared
 */
void drawStats(const Game* game) {
    char buffer[16];
    
    // Draw info panel background
    displayFillRect(INFO_X, BOARD_Y, INFO_WIDTH, BOARD_PIXEL_HEIGHT, COLOR_LIGHT_GRAY);
    displayDrawRect(INFO_X, BOARD_Y, INFO_WIDTH, BOARD_PIXEL_HEIGHT, COLOR_DARK_GRAY);
    
    // Draw score
    displayDrawText(INFO_X + INFO_WIDTH / 2, SCORE_LABEL_Y, "SCORE", COLOR_BLACK, 1);
    displayFillRect(INFO_X, SCORE_Y, INFO_WIDTH, SCORE_HEIGHT, COLOR_WHITE);
    snprintf(buffer, sizeof(buffer), "%d", game->score);
    displayDrawText(INFO_X + INFO_WIDTH / 2, SCORE_Y + SCORE_HEIGHT / 2, buffer, COLOR_BLACK, 1);
    
    // Draw level
    displayDrawText(INFO_X + INFO_WIDTH / 2, LEVEL_LABEL_Y, "LEVEL", COLOR_BLACK, 1);
    displayFillRect(INFO_X, LEVEL_Y, INFO_WIDTH, LEVEL_HEIGHT, COLOR_WHITE);
    snprintf(buffer, sizeof(buffer), "%d", game->level);
    displayDrawText(INFO_X + INFO_WIDTH / 2, LEVEL_Y + LEVEL_HEIGHT / 2, buffer, COLOR_BLACK, 1);
    
    // Draw lines cleared
    displayDrawText(INFO_X + INFO_WIDTH / 2, LINES_LABEL_Y, "LINES", COLOR_BLACK, 1);
    displayFillRect(INFO_X, LINES_Y, INFO_WIDTH, LINES_HEIGHT, COLOR_WHITE);
    snprintf(buffer, sizeof(buffer), "%d", game->linesCleared);
    displayDrawText(INFO_X + INFO_WIDTH / 2, LINES_Y + LINES_HEIGHT / 2, buffer, COLOR_BLACK, 1);
}

/**
 * Draw game state messages (GAME OVER, PAUSED)
 */
void drawGameState(const Game* game) {
    if (game->state == GAME_OVER) {
        // Draw semi-transparent overlay
        for (int y = 0; y < BOARD_PIXEL_HEIGHT; y += 2) {
            for (int x = 0; x < BOARD_PIXEL_WIDTH; x += 2) {
                displayFillRect(BOARD_X + x, BOARD_Y + y, 2, 2, COLOR_BLACK);
            }
        }
        
        // Draw game over a message
        displayFillRect(BOARD_X + 20, BOARD_Y + 80, BOARD_PIXEL_WIDTH - 40, 40, COLOR_RED);
        displayDrawRect(BOARD_X + 20, BOARD_Y + 80, BOARD_PIXEL_WIDTH - 40, 40, COLOR_WHITE);
        displayDrawText(BOARD_X + BOARD_PIXEL_WIDTH / 2, BOARD_Y + 100, "GAME OVER", COLOR_WHITE, 2);
    } else if (game->state == GAME_PAUSED) {
        // Draw semi-transparent overlay
        for (int y = 0; y < BOARD_PIXEL_HEIGHT; y += 2) {
            for (int x = 0; x < BOARD_PIXEL_WIDTH; x += 2) {
                displayFillRect(BOARD_X + x, BOARD_Y + y, 2, 2, COLOR_BLACK);
            }
        }
        
        // Draw a pause message
        displayFillRect(BOARD_X + 20, BOARD_Y + 80, BOARD_PIXEL_WIDTH - 40, 40, COLOR_BLUE);
        displayDrawRect(BOARD_X + 20, BOARD_Y + 80, BOARD_PIXEL_WIDTH - 40, 40, COLOR_WHITE);
        displayDrawText(BOARD_X + BOARD_PIXEL_WIDTH / 2, BOARD_Y + 100, "PAUSED", COLOR_WHITE, 2);
    }
}

/**
 * Main draw function
 */
void drawGame(float tick, const Game* game) {
    clearScreen();
    drawTitle();
    drawBoard(game);
    drawStats(game);
    drawNextPiece(game);
    drawGameState(game);
    displayUpdate(); // Update the display
}