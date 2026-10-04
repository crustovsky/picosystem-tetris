#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
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

void displayDrawText(int x, int y, const char* text, uint16_t color) {
    if (currentRenderer && currentRenderer->drawText) {
        currentRenderer->drawText(x, y, text, color);
    }
}

void displayUpdate() {
    if (currentRenderer && currentRenderer->update) {
        currentRenderer->update();
    }
}

/**
 * Draw one block. The 1px inset leaves the black background showing through,
 * which gives the grid for free.
 */
static void drawBlock(int px, int py, int size, uint16_t color) {
    displayFillRect(px + 1, py + 1, size - 2, size - 2, color);
}

/**
 * Draw the game board, the auto place marker, the landing ghost and the
 * falling piece
 */
void drawBoard(const Game* game) {
    displayFillRect(BOARD_X, BOARD_Y, BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT, COLOR_BLACK);

    // Placed blocks
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x] > 0) {
                drawBlock(BOARD_X + x * CELL_SIZE, BOARD_Y + y * CELL_SIZE,
                          CELL_SIZE, TETROMINO_COLORS[game->board[y][x]]);
            }
        }
    }

    // Outline the last auto placed piece so it is clear where it went
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->autoPlaced[y][x]) {
                displayDrawRect(BOARD_X + x * CELL_SIZE + 1, BOARD_Y + y * CELL_SIZE + 1,
                                CELL_SIZE - 2, CELL_SIZE - 2, COLOR_WHITE);
            }
        }
    }

    // Ghost outline showing where the piece will land
    int ghost = ghostDropY(game);
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game->currentPiece.blocks[y][x] && ghost + y >= 0) {
                displayDrawRect(BOARD_X + (game->currentPiece.x + x) * CELL_SIZE + 1,
                                BOARD_Y + (ghost + y) * CELL_SIZE + 1,
                                CELL_SIZE - 2, CELL_SIZE - 2, COLOR_DARK_GRAY);
            }
        }
    }

    // Falling piece
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game->currentPiece.blocks[y][x] && game->currentPiece.y + y >= 0) {
                drawBlock(BOARD_X + (game->currentPiece.x + x) * CELL_SIZE,
                          BOARD_Y + (game->currentPiece.y + y) * CELL_SIZE,
                          CELL_SIZE, TETROMINO_COLORS[game->currentPiece.shape + 1]);
            }
        }
    }

    displayDrawRect(BOARD_X - 1, BOARD_Y - 1,
                    BOARD_PIXEL_WIDTH + 2, BOARD_PIXEL_HEIGHT + 2, COLOR_DARK_GRAY);
}

/**
 * Draw the next piece preview, centred on the piece's own bounding box so
 * the empty rows in the 4x4 definition don't push it off-centre.
 */
void drawNextPiece(const Game* game) {
    displayFillRect(NEXT_BOX_X, NEXT_BOX_Y, NEXT_BOX_SIZE, NEXT_BOX_SIZE, COLOR_BLACK);
    displayDrawRect(NEXT_BOX_X, NEXT_BOX_Y, NEXT_BOX_SIZE, NEXT_BOX_SIZE, COLOR_DARK_GRAY);
    displayDrawText(PANEL_CX, NEXT_BOX_Y - 12, "NEXT", COLOR_GRAY);

    // Bounding box of the filled cells
    int minX = TETROMINO_SIZE, maxX = -1, minY = TETROMINO_SIZE, maxY = -1;
    for (int y = 0; y < TETROMINO_SIZE; y++) {
        for (int x = 0; x < TETROMINO_SIZE; x++) {
            if (game->nextPiece.blocks[y][x]) {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }
    }
    if (maxX < 0) return;

    int originX = NEXT_BOX_X + (NEXT_BOX_SIZE - (maxX - minX + 1) * NEXT_CELL_SIZE) / 2;
    int originY = NEXT_BOX_Y + (NEXT_BOX_SIZE - (maxY - minY + 1) * NEXT_CELL_SIZE) / 2;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (game->nextPiece.blocks[y][x]) {
                drawBlock(originX + (x - minX) * NEXT_CELL_SIZE,
                          originY + (y - minY) * NEXT_CELL_SIZE,
                          NEXT_CELL_SIZE, TETROMINO_COLORS[game->nextPiece.shape + 1]);
            }
        }
    }
}

/**
 * Draw one label/value pair in the side panel
 */
static void drawStat(int y, const char* label, int value) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", value);
    displayDrawText(PANEL_CX, y, label, COLOR_GRAY);
    displayDrawText(PANEL_CX, y + STAT_VALUE_DY, buffer, COLOR_WHITE);
}

/**
 * Draw score, level, and lines cleared
 */
void drawStats(const Game* game) {
    drawStat(SCORE_Y, "SCORE", game->score);
    drawStat(LEVEL_Y, "LEVEL", game->level);
    drawStat(LINES_Y, "LINES", game->linesCleared);
}

/**
 * Draw game state messages (GAME OVER, PAUSED)
 */
void drawGameState(const Game* game) {
    if (game->state == GAME_ACTIVE) {
        return;
    }

    const int height = 26;
    int y = BOARD_Y + (BOARD_PIXEL_HEIGHT - height) / 2;
    bool over = (game->state == GAME_OVER);

    displayFillRect(BOARD_X, y, BOARD_PIXEL_WIDTH, height, over ? COLOR_RED : COLOR_BLUE);
    displayDrawRect(BOARD_X, y, BOARD_PIXEL_WIDTH, height, COLOR_WHITE);
    displayDrawText(BOARD_X + BOARD_PIXEL_WIDTH / 2, y + height / 2,
                    over ? "GAME OVER" : "PAUSED", COLOR_WHITE);
}

/**
 * Main draw function
 */
void drawGame(const Game* game) {
    displayClear(COLOR_BG);
    drawBoard(game);
    drawNextPiece(game);
    drawStats(game);
    drawGameState(game);
    displayUpdate();
}
