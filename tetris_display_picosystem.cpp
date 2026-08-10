#include "tetris_display.h"
#include "picosystem.hpp"

using namespace picosystem;

extern "C" {

/**
 * Set the PicoSystem pen from an RGB565 value.
 * PicoSystem uses 4 bits per channel, so each component is shifted down.
 */
static void setPen(uint16_t color) {
    uint8_t r, g, b;
    extractRGB565(color, &r, &g, &b);
    pen(r >> 4, g >> 4, b >> 4, 0xF);
}

/**
 * Initialize the picosystem display - handled by the picosystem library
 */
static void picosystemInit() {
}

/**
 * Clear the entire display with a specific color
 */
static void picosystemClear(uint16_t color) {
    setPen(color);
    clear();
}

/**
 * Draw a rectangle outline
 */
static void picosystemDrawRect(int x, int y, int width, int height, uint16_t color) {
    setPen(color);
    rect(x, y, width, height);
}

/**
 * Draw a filled rectangle
 */
static void picosystemFillRect(int x, int y, int width, int height, uint16_t color) {
    setPen(color);
    frect(x, y, width, height);
}

/**
 * Draw text centred on (x, y). The default font is variable width, so ask
 * picosystem how wide the string actually is rather than assuming 8px cells.
 */
static void picosystemDrawText(int x, int y, const char* text, uint16_t color) {
    setPen(color);
    int32_t w = 0, h = 0;
    measure(text, w, h);
    picosystem::text(text, x - w / 2, y - h / 2);
}

/**
 * Update the display - the picosystem library flips the buffer for us
 */
static void picosystemUpdate() {
}

// Define the PicoSystem renderer
static DisplayRenderer picosystemRenderer = {
    .init = picosystemInit,
    .clear = picosystemClear,
    .drawRect = picosystemDrawRect,
    .fillRect = picosystemFillRect,
    .drawText = picosystemDrawText,
    .update = picosystemUpdate
};

// Function to get the PicoSystem renderer
DisplayRenderer* getPicoSystemRenderer() {
    return &picosystemRenderer;
}

} // extern "C"
