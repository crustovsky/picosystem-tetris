#include <SDL3/SDL.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tetris_display.h"

// The 240x240 virtual screen is scaled up to this window size
#define WINDOW_SCALE 3

// SDL-specific globals
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static bool sdlInitialized = false;

/**
 * Initialize the SDL3 display
 */
static void sdlInit() {
    if (sdlInitialized) return;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    if (!SDL_CreateWindowAndRenderer("Tetris",
                                     DISPLAY_WIDTH * WINDOW_SCALE,
                                     DISPLAY_HEIGHT * WINDOW_SCALE,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Draw in 240x240 coordinates regardless of the actual window size
    SDL_SetRenderLogicalPresentation(renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    sdlInitialized = true;
}

/**
 * Set the draw colour from an RGB565 value
 */
static void setColor(uint16_t color) {
    Color c = RGB565toColor(color);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

/**
 * Clear the SDL display with a specific color
 */
static void sdlClear(uint16_t color) {
    if (!sdlInitialized) return;

    setColor(color);
    SDL_RenderClear(renderer);
}

/**
 * Draw a rectangle outline on the SDL display
 */
static void sdlDrawRect(int x, int y, int width, int height, uint16_t color) {
    if (!sdlInitialized) return;

    setColor(color);
    SDL_FRect rect = {.x = x, .y = y, .w = width, .h = height};
    SDL_RenderRect(renderer, &rect);
}

/**
 * Draw a filled rectangle on the SDL display
 */
static void sdlFillRect(int x, int y, int width, int height, uint16_t color) {
    if (!sdlInitialized) return;

    setColor(color);
    SDL_FRect rect = {.x = x, .y = y, .w = width, .h = height};
    SDL_RenderFillRect(renderer, &rect);
}

/**
 * Draw text centred on (x, y). SDL's debug font is a fixed 8x8 grid.
 */
static void sdlDrawText(int x, int y, const char* text, uint16_t color) {
    if (!sdlInitialized) return;

    setColor(color);
    SDL_RenderDebugText(renderer, x - strlen(text) * 4.0f, y - 4.0f, text);
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
