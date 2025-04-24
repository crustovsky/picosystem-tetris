#include "tetris_game.h"
#include "tetris_display.h"

#include "picosystem.hpp"

using namespace picosystem;

// Handle input based on your device's input system
void handleInput() {
    // TODO: Implement input handling for your device
    // Examples:
    // - Check if left button is pressed -> moveTetromino(DIR_LEFT)
    // - Check if right button is pressed -> moveTetromino(DIR_RIGHT)
    // - Check if down button is pressed -> moveTetromino(DIR_DOWN)
    // - Check if rotate button is pressed -> rotateTetromino()
    // - Check if pause button is pressed -> toggle game.state between GAME_ACTIVE and GAME_PAUSED
}

// Main delay function - implement for your device
void delay_ms(int ms) {
    // TODO: Implement delay function for your device
    // This could be a simple busy wait, sleep function, or timer-based delay
}

void init() {
    // Initialise the display
    displayInit();

    // Initialise the game
    initGame();
}

void update(uint32_t tick) {
    handleInput();

    // Update game state
    updateGame(tick);
}

void draw(uint32_t tick) {
    // clear the background
    alpha();
    pen(1, 1, 1);
    clear();

    drawGame(tick, &game);

    pen(10, 10, 10);
    text("TETRIS", 8, 10);

}
