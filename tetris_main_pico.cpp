#include "tetris_game.h"
#include "tetris_display.h"
#include "tetris_display_renderers.h"
#include "picosystem.hpp"

using namespace picosystem;

// PicoSystem-specific input handling
void handleInput() {
    // Process PicoSystem button inputs
    if (pressed(A)) {
        rotateTetromino();
    }
    if (pressed(LEFT)) {
        moveTetromino(DIR_LEFT);
    }
    if (pressed(RIGHT)) {
        moveTetromino(DIR_RIGHT);
    }
    if (pressed(DOWN)) {
        moveTetromino(DIR_DOWN);
    }
    if (pressed(B)) {
        // Toggle pause
        if (game.state == GAME_ACTIVE) {
            game.state = GAME_PAUSED;
        } else if (game.state == GAME_PAUSED) {
            game.state = GAME_ACTIVE;
        }
    }
    if (pressed(X) && game.state == GAME_OVER) {
        // Restart game on X button if game is over
        initGame();
    }
}

// PicoSystem delay implementation
void delay_ms(int ms) {
    sleep(ms);
}

// PicoSystem life cycle functions
void init() {
    // Set the PicoSystem renderer as our current renderer
    setDisplayRenderer(getPicoSystemRenderer());
    
    // Initialize the display
    displayInit();

    // Initialize the game
    initGame();
}

void update(uint32_t tick) {
    // Handle input
    handleInput();

    // Update game state (convert tick to seconds)
    updateGame(tick / 1000.0f);
}

void draw(uint32_t tick) {
    // Draw the game
    drawGame(tick / 1000.0f, &game);
}