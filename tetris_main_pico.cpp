#include "tetris_game.h"
#include "tetris_display.h"
#include "tetris_display_renderers.h"
#include "picosystem.hpp"

using namespace picosystem;

// Wall clock of the last update, for a real delta time
static uint32_t lastUpdateMs = 0;

// PicoSystem-specific input handling
void handleInput() {
    if (pressed(UP)) {
        hardDrop();
    }
    if (pressed(LEFT)) {
        moveTetromino(DIR_LEFT);
    }
    if (pressed(RIGHT)) {
        moveTetromino(DIR_RIGHT);
    }
    if (pressed(Y)) {
        rotateTetromino();
    }
    if (pressed(A)) {
        autoPlace();
    }
    if (pressed(X)) {
        // Toggle pause, or restart once the game is over
        if (game.state == GAME_ACTIVE) {
            game.state = GAME_PAUSED;
        } else if (game.state == GAME_PAUSED) {
            game.state = GAME_ACTIVE;
        } else {
            initGame();
        }
    }
}

// PicoSystem life cycle functions
void init() {
    // Set the PicoSystem renderer as our current renderer
    setDisplayRenderer(getPicoSystemRenderer());

    // Initialize the display
    displayInit();

    // Initialize the game
    initGame();

    lastUpdateMs = time();
}

void update(uint32_t tick) {
    // Handle input
    handleInput();

    // picosystem's loop is vsync locked and drops frames when drawing is slow,
    // so measure the real elapsed time rather than assuming a fixed rate.
    uint32_t now = time();
    // Down and B are held for a soft drop
    updateGame((now - lastUpdateMs) / 1000.0f, button(DOWN) || button(B));
    lastUpdateMs = now;
}

void draw(uint32_t tick) {
    // Draw the game
    drawGame(&game);
}