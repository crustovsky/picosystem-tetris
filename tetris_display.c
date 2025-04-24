#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tetris_display.h"
#include "tetris_game.h"

// Tetromino colors in RGB565 format
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

// Frame buffer for the display
static uint16_t frameBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

// Font data structure (simplified)
typedef struct {
    const uint8_t* data;
    uint8_t width;
    uint8_t height;
} Font;

// Simplified small and large font declarations
static Font smallFont = { NULL, 5, 7 };
static Font largeFont = { NULL, 8, 12 };

/**
 * Initialize the display hardware
 */
void displayInit() {
    // TODO: Initialize your display hardware
    // - Configure SPI/I2C/parallel interface
    // - Set up display controller registers
    // - Configure resolution, color mode, etc.
    // - Turn on the display
    
    // Clear the frame buffer
    displayClear(COLOR_BLACK);
}

/**
 * Clear the entire display with a specific color
 */
void displayClear(uint16_t color) {
    // Fill the frame buffer with the specified color
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        frameBuffer[i] = color;
    }
    
    // TODO: If your display has a hardware clear function, call it here
}

/**
 * Draw a single pixel at the specified coordinates with the given color
 */
void displayDrawPixel(int x, int y, uint16_t color) {
    // Check if the coordinates are within the display boundaries
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        // Set the pixel in the frame buffer
        frameBuffer[y * DISPLAY_WIDTH + x] = color;
    }
}

/**
 * Draw a line from (x0,y0) to (x1,y1) with the given color
 * Implementation of Bresenham's line algorithm
 */
void displayDrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    
    while (1) {
        displayDrawPixel(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * Draw a rectangle outline with the specified dimensions and color
 */
void displayDrawRect(int x, int y, int width, int height, uint16_t color) {
    // Draw the four sides of the rectangle
    displayDrawLine(x, y, x + width - 1, y, color);                 // Top
    displayDrawLine(x, y + height - 1, x + width - 1, y + height - 1, color); // Bottom
    displayDrawLine(x, y, x, y + height - 1, color);                // Left
    displayDrawLine(x + width - 1, y, x + width - 1, y + height - 1, color);  // Right
}

/**
 * Draw a filled rectangle with the specified dimensions and color
 */
void displayFillRect(int x, int y, int width, int height, uint16_t color) {
    // Ensure coordinates are within display boundaries
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if (x + width > DISPLAY_WIDTH) {
        width = DISPLAY_WIDTH - x;
    }
    if (y + height > DISPLAY_HEIGHT) {
        height = DISPLAY_HEIGHT - y;
    }
    
    // Draw nothing if the rectangle is outside the display
    if (width <= 0 || height <= 0) {
        return;
    }
    
    // Fill the rectangle in the frame buffer
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            frameBuffer[j * DISPLAY_WIDTH + i] = color;
        }
    }
    
    // TODO: If your display has a hardware fill function, call it here
}

/**
 * Draw text at the specified coordinates with the given color and size
 * Size: 1 = small font, 2 = large font
 */
void displayDrawText(int x, int y, const char* text, uint16_t color, uint8_t size) {
    // TODO: Implement text rendering based on your font system
    
    // Select the font based on size
    Font* font = (size > 1) ? &largeFont : &smallFont;
    
    // Draw each character in the string
    int cursor_x = x;
    int cursor_y = y - font->height / 2; // Center text vertically
    
    while (*text) {
        // TODO: Implement character rendering
        // This example just draws a placeholder rectangle for each character
        
        displayFillRect(cursor_x, cursor_y, font->width, font->height, COLOR_BLACK);
        displayDrawRect(cursor_x, cursor_y, font->width, font->height, color);
        
        // Move cursor to the next character position
        cursor_x += font->width + 1;
        
        // Move to the next character
        text++;
    }
}

/**
 * Update the display with the content in the frame buffer
 */
void displayUpdate() {
    // TODO: Send the frame buffer to the display
    // This depends on your specific display hardware interface
    
    // For SPI display example:
    // 1. Set the display address window to full screen
    // 2. Start the data transmission
    // 3. Send all pixels from the frame buffer
    // 4. End the transmission
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
void drawBoard(Game* game) {
    // Draw board background
    displayFillRect(BOARD_X, BOARD_Y, BOARD_WIDTH, BOARD_HEIGHT, COLOR_BLACK);
    
    // Draw grid lines
    for (int x = 1; x < BOARD_WIDTH / CELL_WIDTH; x++) {
        displayDrawLine(BOARD_X + x * CELL_WIDTH, BOARD_Y, 
                       BOARD_X + x * CELL_WIDTH, BOARD_Y + BOARD_HEIGHT, COLOR_DARK_GRAY);
    }
    
    for (int y = 1; y < BOARD_HEIGHT / CELL_HEIGHT; y++) {
        displayDrawLine(BOARD_X, BOARD_Y + y * CELL_HEIGHT,
                       BOARD_X + BOARD_WIDTH, BOARD_Y + y * CELL_HEIGHT, COLOR_DARK_GRAY);
    }
    
    // Create a temporary board with the current piece
    int tempBoard[BOARD_HEIGHT / CELL_HEIGHT][BOARD_WIDTH / CELL_WIDTH] = {0};
    
    // Copy the main board
    for (int y = 0; y < BOARD_HEIGHT / CELL_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH / CELL_WIDTH; x++) {
            tempBoard[y][x] = game->board[y][x];
        }
    }
    
    // Add the current piece to the temporary board
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game->currentPiece.blocks[y][x]) {
                int boardX = game->currentPiece.x + x;
                int boardY = game->currentPiece.y + y;
                
                if (boardX >= 0 && boardX < BOARD_WIDTH / CELL_WIDTH && 
                    boardY >= 0 && boardY < BOARD_HEIGHT / CELL_HEIGHT) {
                    tempBoard[boardY][boardX] = game->currentPiece.shape + 1;
                }
            }
        }
    }
    
    // Draw filled cells
    for (int y = 0; y < BOARD_HEIGHT / CELL_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH / CELL_WIDTH; x++) {
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
    displayDrawRect(BOARD_X, BOARD_Y, BOARD_WIDTH, BOARD_HEIGHT, COLOR_WHITE);
}

/**
 * Draw the next piece preview
 */
void drawNextPiece(Game* game) {
    // Draw next piece background and border
    displayFillRect(NEXT_X, NEXT_Y, NEXT_WIDTH, NEXT_HEIGHT, COLOR_BLACK);
    displayDrawRect(NEXT_X, NEXT_Y, NEXT_WIDTH, NEXT_HEIGHT, COLOR_WHITE);
    
    // Draw "NEXT" label
    displayDrawText(NEXT_X + NEXT_WIDTH / 2, NEXT_Y + NEXT_HEIGHT + 10, "NEXT", COLOR_BLACK, 1);
    
    // Calculate position to center the piece
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
void drawStats(Game* game) {
    char buffer[16];
    
    // Draw info panel background
    displayFillRect(INFO_X, BOARD_Y, INFO_WIDTH, BOARD_HEIGHT, COLOR_LIGHT_GRAY);
    displayDrawRect(INFO_X, BOARD_Y, INFO_WIDTH, BOARD_HEIGHT, COLOR_DARK_GRAY);
    
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
void drawGameState(Game* game) {
    if (game->state == GAME_OVER) {
        // Draw semi-transparent overlay
        for (int y = 0; y < BOARD_HEIGHT; y += 2) {
            for (int x = 0; x < BOARD_WIDTH; x += 2) {
                displayFillRect(BOARD_X + x, BOARD_Y + y, 2, 2, COLOR_BLACK);
            }
        }
        
        // Draw game over message
        displayFillRect(BOARD_X + 20, BOARD_Y + 80, BOARD_WIDTH - 40, 40, COLOR_RED);
        displayDrawRect(BOARD_X + 20, BOARD_Y + 80, BOARD_WIDTH - 40, 40, COLOR_WHITE);
        displayDrawText(BOARD_X + BOARD_WIDTH / 2, BOARD_Y + 100, "GAME OVER", COLOR_WHITE, 2);
    } else if (game->state == GAME_PAUSED) {
        // Draw semi-transparent overlay
        for (int y = 0; y < BOARD_HEIGHT; y += 2) {
            for (int x = 0; x < BOARD_WIDTH; x += 2) {
                displayFillRect(BOARD_X + x, BOARD_Y + y, 2, 2, COLOR_BLACK);
            }
        }
        
        // Draw pause message
        displayFillRect(BOARD_X + 20, BOARD_Y + 80, BOARD_WIDTH - 40, 40, COLOR_BLUE);
        displayDrawRect(BOARD_X + 20, BOARD_Y + 80, BOARD_WIDTH - 40, 40, COLOR_WHITE);
        displayDrawText(BOARD_X + BOARD_WIDTH / 2, BOARD_Y + 100, "PAUSED", COLOR_WHITE, 2);
    }
}

/**
 * Main draw function
 */
void draw(float tick, Game* game) {
    clearScreen();
    drawTitle();
    drawBoard(game);
    drawNextPiece(game);
    drawStats(game);
    drawGameState(game);
    displayUpdate(); // Update the display
}