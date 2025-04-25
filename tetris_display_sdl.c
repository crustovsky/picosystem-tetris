#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tetris_display.h"

// SDL-specific globals
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool sdlInitialized = false;

/**
 * Initialize the SDL3 display
 */
static void sdlInit() {
    if (sdlInitialized) return;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    
    // Create window
    window = SDL_CreateWindow("Tetris", DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }
    
    // Create renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    
    sdlInitialized = true;
}

/**
 * Convert RGB565 color to SDL_Color using the shared helper function
 */
static SDL_Color RGB565toRGB(uint16_t color) {
    SDL_Color rgb;
    Color c = RGB565toColor(color);
    rgb.r = c.r;
    rgb.g = c.g;
    rgb.b = c.b;
    rgb.a = c.a;
    return rgb;
}

/**
 * Clear the SDL display with a specific color
 */
static void sdlClear(uint16_t color) {
    if (!sdlInitialized) return;
    
    SDL_Color rgb = RGB565toRGB(color);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
    SDL_RenderClear(renderer);
}

/**
 * Draw a single pixel on the SDL display
 */
static void sdlDrawPixel(int x, int y, uint16_t color) {
    if (!sdlInitialized) return;
    
    SDL_Color rgb = RGB565toRGB(color);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
    SDL_RenderPoint(renderer, x, y);
}

/**
 * Draw a line on the SDL display
 */
static void sdlDrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    if (!sdlInitialized) return;
    
    SDL_Color rgb = RGB565toRGB(color);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
    SDL_RenderLine(renderer, x0, y0, x1, y1);
}

/**
 * Draw a rectangle outline on the SDL display
 */
static void sdlDrawRect(int x, int y, int width, int height, uint16_t color) {
    if (!sdlInitialized) return;
    
    SDL_Color rgb = RGB565toRGB(color);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
    
    SDL_FRect rect = {.x = x, .y = y, .w = width, .h = height};
    SDL_RenderRect(renderer, &rect);
}

/**
 * Draw a filled rectangle on the SDL display
 */
static void sdlFillRect(int x, int y, int width, int height, uint16_t color) {
    if (!sdlInitialized) return;
    
    SDL_Color rgb = RGB565toRGB(color);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
    
    SDL_FRect rect = {.x = x, .y = y, .w = width, .h = height};
    SDL_RenderFillRect(renderer, &rect);
}

/**
 * Draw text on the SDL display using SDL_RenderDebugText
 */
static void sdlDrawText(int x, int y, const char* text, uint16_t color, uint8_t size) {
    if (!sdlInitialized) return;
    
    SDL_Color rgb = RGB565toRGB(color);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
    
    // Calculate position for centered text
    // SDL_RenderDebugText uses a fixed-width font of 8x8 pixels per character
    int textWidth = strlen(text) * 8;  // Each character is 8 pixels wide
    
    // Center the text (calculate the starting position)
    float posX = x - (textWidth / 2.0f);
    float posY = y - 4.0f;  // Center vertically (8 pixels tall, so offset by 4)
    
    // Use SDL's built-in debug text rendering
    SDL_RenderDebugText(renderer, posX, posY, text);
}

/**
 * Update the SDL display
 */
static void sdlUpdate() {
    if (!sdlInitialized) return;
    
    SDL_RenderPresent(renderer);
}

/**
 * Clean up SDL resources when done
 */
static void sdlCleanup() {
    if (!sdlInitialized) return;
    
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    sdlInitialized = false;
}

// Define the SDL renderer
static DisplayRenderer sdlRenderer = {
    .init = sdlInit,
    .clear = sdlClear,
    .drawPixel = sdlDrawPixel,
    .drawLine = sdlDrawLine,
    .drawRect = sdlDrawRect,
    .fillRect = sdlFillRect,
    .drawText = sdlDrawText,
    .update = sdlUpdate
};

// Function to get the SDL renderer
DisplayRenderer* getSDLRenderer() {
    return &sdlRenderer;
}

// Register cleanup function at exit
static void __attribute__((constructor)) sdlSetupCleanup() {
    atexit(sdlCleanup);
}