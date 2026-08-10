#ifndef TETRIS_DISPLAY_H
#define TETRIS_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "tetris_game.h" // For Game structure definition

// Display dimensions
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

// Board: square cells. 10x20 cells at 11px = 110x220, centred vertically.
#define CELL_SIZE 11
#define BOARD_PIXEL_WIDTH (BOARD_WIDTH * CELL_SIZE)   // 110
#define BOARD_PIXEL_HEIGHT (BOARD_HEIGHT * CELL_SIZE) // 220
#define BOARD_X 6
#define BOARD_Y 10

// Side panel: everything that is not the board
#define PANEL_X 126
#define PANEL_WIDTH 108
#define PANEL_CX (PANEL_X + PANEL_WIDTH / 2)

// Next-piece preview (also square cells)
#define NEXT_CELL_SIZE 11
#define NEXT_BOX_SIZE (TETROMINO_SIZE * NEXT_CELL_SIZE + 2) // 46
#define NEXT_BOX_X (PANEL_X + (PANEL_WIDTH - NEXT_BOX_SIZE) / 2)
#define NEXT_BOX_Y 28

// Stat rows: label above value
#define SCORE_Y 92
#define LEVEL_Y 132
#define LINES_Y 172
#define STAT_VALUE_DY 14

// Colours in RGB565 format (16-bit)
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_BG 0x2104         // 32, 32, 32 - page background
#define COLOR_GRAY 0x8410       // 128, 128, 128
#define COLOR_DARK_GRAY 0x4208  // 64, 64, 64
#define COLOR_LIGHT_GRAY 0xC618 // 192, 192, 192
#define COLOR_BLUE 0x001F
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW 0xFFE0
#define COLOR_ORANGE 0xFD20
#define COLOR_PURPLE 0x8010

// Colour structure to use RGBA components directly
typedef struct {
    uint8_t r;  // Red component (0-255)
    uint8_t g;  // Green component (0-255)
    uint8_t b;  // Blue component (0-255)
    uint8_t a;  // Alpha component (0-255)
} Color;

// Helper function to convert RGB565 to Color structure
Color RGB565toColor(uint16_t color);

// Helper function to extract RGB values from RGB565 color format
void extractRGB565(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b);

// Display renderer interface - to be implemented by platform-specific renderers
typedef struct {
    // Initialize display
    void (*init)();
    // Clear the display with a specific color
    void (*clear)(uint16_t color);
    // Draw a rectangle outline
    void (*drawRect)(int x, int y, int width, int height, uint16_t color);
    // Draw a filled rectangle
    void (*fillRect)(int x, int y, int width, int height, uint16_t color);
    // Draw text, centred on (x, y)
    void (*drawText)(int x, int y, const char* text, uint16_t color);
    // Update display (flush buffer to screen)
    void (*update)();
} DisplayRenderer;

// Current display renderer
extern DisplayRenderer* currentRenderer;

// Function to set the current renderer
void setDisplayRenderer(DisplayRenderer* renderer);

// Generic display functions that route to the current renderer
void displayInit();
void displayClear(uint16_t color);
void displayDrawRect(int x, int y, int width, int height, uint16_t color);
void displayFillRect(int x, int y, int width, int height, uint16_t color);
void displayDrawText(int x, int y, const char* text, uint16_t color);
void displayUpdate();

// Tetris drawing functions (platform-independent)
void drawBoard(const Game* game);
void drawNextPiece(const Game* game);
void drawStats(const Game* game);
void drawGameState(const Game* game);
void drawGame(const Game* game);

#ifdef __cplusplus
}
#endif

#endif /* TETRIS_DISPLAY_H */
