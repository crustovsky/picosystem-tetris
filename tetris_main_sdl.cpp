#include "tetris_game.h"
#include "tetris_display.h"
#include "tetris_display_renderers.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <cstdlib> // For exit()

// SDL-specific input handling
void handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            exit(0);
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            // SDL3 uses scancodes directly
            SDL_Scancode scancode = event.key.scancode;
            
            if (scancode == SDL_SCANCODE_LEFT) {
                moveTetromino(DIR_LEFT);
            }
            else if (scancode == SDL_SCANCODE_RIGHT) {
                moveTetromino(DIR_RIGHT);
            }
            else if (scancode == SDL_SCANCODE_DOWN) {
                moveTetromino(DIR_DOWN);
            }
            else if (scancode == SDL_SCANCODE_UP) {
                rotateTetromino();
            }
            else if (scancode == SDL_SCANCODE_P) {
                // Toggle pause
                if (game.state == GAME_ACTIVE) {
                    game.state = GAME_PAUSED;
                } else if (game.state == GAME_PAUSED) {
                    game.state = GAME_ACTIVE;
                }
            }
            else if (scancode == SDL_SCANCODE_R) {
                if (game.state == GAME_OVER) {
                    // Restart game with R key if game is over
                    initGame();
                }
            }
            else if (scancode == SDL_SCANCODE_ESCAPE) {
                exit(0);
            }
        }
    }
}

// SDL delay implementation
void delay_ms(int ms) {
    SDL_Delay(ms);
}

// Main function for SDL version
int main(int argc, char* argv[]) {
    // Set the SDL renderer as our current renderer
    setDisplayRenderer(getSDLRenderer());
    
    // Initialize the display
    displayInit();
    
    // Initialize the game
    initGame();
    
    // Setup timing variables
    uint64_t lastUpdateTime = SDL_GetTicks();
    const int targetFPS = 60;
    const int frameDelay = 1000 / targetFPS;
    
    // Main game loop
    bool quit = false;
    while (!quit) {
        // Track frame start time
        uint64_t frameStart = SDL_GetTicks();
        uint64_t currentTime = frameStart;
        
        // Calculate delta time in seconds
        float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
        lastUpdateTime = currentTime;
        
        // Handle input
        handleInput();
        
        // Update game state
        updateGame(deltaTime);
        
        // Draw the game
        drawGame(deltaTime, &game);
        
        // Cap the frame rate
        uint64_t frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    
    return 0;
}