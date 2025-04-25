#include "tetris_display.h"
#include "picosystem.hpp"
#include <cstring>

using namespace picosystem;

// Simple framebuffer and font handling for the picosystem
extern "C" {

// Frame buffer for the display
static uint16_t frameBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

/**
 * Initialize the picosystem display
 */
static void picosystemInit() {
    // PicoSystem's init is handled by the picosystem library
    // Just clear our frame buffer
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        frameBuffer[i] = 0;
    }
}

/**
 * Clear the entire display with a specific color
 */
static void picosystemClear(uint16_t color) {
    // Convert RGB565 to picosystem RGB format (8-bit per channel)
    uint8_t r = ((color >> 11) & 0x1F) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    
    // Use picosystem's clear function
    pen(r, g, b);
    clear();
}

/**
 * Draw a single pixel using picosystem's pixel function
 */
static void picosystemDrawPixel(int x, int y, uint16_t color) {
    // Check if the coordinates are within the display boundaries
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        // Convert RGB565 to picosystem RGB format (8-bit per channel)
        uint8_t r = ((color >> 11) & 0x1F) << 3;
        uint8_t g = ((color >> 5) & 0x3F) << 2;
        uint8_t b = (color & 0x1F) << 3;
        
        // Use picosystem's pixel function
        pen(r, g, b);
        pixel(x, y);
    }
}

/**
 * Draw a line using picosystem's line function
 */
static void picosystemDrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    // Convert RGB565 to picosystem RGB format (8-bit per channel)
    uint8_t r = ((color >> 11) & 0x1F) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    
    // Use picosystem's line function
    pen(r, g, b);
    line(x0, y0, x1, y1);
}

/**
 * Draw a rectangle outline using picosystem's functions
 */
static void picosystemDrawRect(int x, int y, int width, int height, uint16_t color) {
    // Convert RGB565 to picosystem RGB format (8-bit per channel)
    uint8_t r = ((color >> 11) & 0x1F) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    
    // Use picosystem's rect function (not filled)
    pen(r, g, b);
    rect(x, y, width, height);
}

/**
 * Draw a filled rectangle using picosystem's functions
 */
static void picosystemFillRect(int x, int y, int width, int height, uint16_t color) {
    // Convert RGB565 to picosystem RGB format (8-bit per channel)
    uint8_t r = ((color >> 11) & 0x1F) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    
    // Use picosystem's frect function (filled rect)
    pen(r, g, b);
    frect(x, y, width, height);
}

/**
 * Draw text using picosystem's text function
 */
static void picosystemDrawText(int x, int y, const char* text, uint16_t color, uint8_t size) {
    // Convert RGB565 to picosystem RGB format (8-bit per channel)
    uint8_t r = ((color >> 11) & 0x1F) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    
    // Use picosystem's text function
    pen(r, g, b);
    picosystem::text(text, x - (strlen(text) * 4 * size) / 2, y - 4 * size); // Center text horizontally
}

/**
 * Update the display - not needed for picosystem as drawing happens in real-time
 */
static void picosystemUpdate() {
    // Picosystem's display updates automatically in the draw function
    // No explicit update required
}

// Define the PicoSystem renderer
static DisplayRenderer picosystemRenderer = {
    .init = picosystemInit,
    .clear = picosystemClear,
    .drawPixel = picosystemDrawPixel,
    .drawLine = picosystemDrawLine,
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