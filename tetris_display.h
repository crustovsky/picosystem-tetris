#ifndef TETRIS_DISPLAY_H
#define TETRIS_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "tetris.h" // For Game structure definition

// Display dimensions
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

// Game board dimensions and position
#define BOARD_X 10
#define BOARD_Y 25
#define BOARD_WIDTH 150
#define BOARD_HEIGHT 200
#define CELL_WIDTH 15
#define CELL_HEIGHT 10

// Next piece preview dimensions and position
#define NEXT_X 175
#define NEXT_Y 30
#define NEXT_WIDTH 50
#define NEXT_HEIGHT 50
#define NEXT_CELL_SIZE 10

// Score, level, and lines positions
#define SCORE_LABEL_Y 110
#define SCORE_Y 115
#define SCORE_HEIGHT 20

#define LEVEL_LABEL_Y 150
#define LEVEL_Y 155
#define LEVEL_HEIGHT 20

#define LINES_LABEL_Y 190
#define LINES_Y 195
#define LINES_HEIGHT 20

#define INFO_X 175
#define INFO_WIDTH 50
#define TEXT_HEIGHT 20

// Colors in RGB565 format (16-bit)
#define COLOR_BLACK 0x0000      // 0, 0, 0
#define COLOR_WHITE 0xFFFF      // 255, 255, 255
#define COLOR_GRAY 0x8410       // 128, 128, 128
#define COLOR_DARK_GRAY 0x4208  // 64, 64, 64
#define COLOR_LIGHT_GRAY 0xC618 // 192, 192, 192
#define COLOR_BLUE 0x001F       // 0, 0, 255
#define COLOR_RED 0xF800        // 255, 0, 0
#define COLOR_GREEN 0x07E0      // 0, 255, 0
#define COLOR_CYAN 0x07FF       // 0, 255, 255
#define COLOR_MAGENTA 0xF81F    // 255, 0, 255
#define COLOR_YELLOW 0xFFE0     // 255, 255, 0
#define COLOR_ORANGE 0xFD20     // 255, 128, 0
#define COLOR_PURPLE 0x8010     // 128, 0, 128

// Display functions for your device - implement these in tetris_display.c
void displayInit();
void displayClear(uint16_t color);
void displayDrawPixel(int x, int y, uint16_t color);
void displayDrawLine(int x0, int y0, int x1, int y1, uint16_t color);
void displayDrawRect(int x, int y, int width, int height, uint16_t color);
void displayFillRect(int x, int y, int width, int height, uint16_t color);
void displayDrawText(int x, int y, const char* text, uint16_t color, uint8_t size);
void displayUpdate();

// Tetris drawing functions
void clearScreen();
void drawTitle();
void drawBoard(Game* game);
void drawNextPiece(Game* game);
void drawStats(Game* game);
void drawGameState(Game* game);
void draw(float tick, Game* game);

#endif /* TETRIS_DISPLAY_H */
