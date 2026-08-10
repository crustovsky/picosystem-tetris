#include "tetris_game.h"
#include "tetris_display.h"
#include "tetris_display_renderers.h"
#include <SDL3/SDL.h>

// SDL-specific input handling. Returns false when the player wants to quit.
static bool handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        if (event.type != SDL_EVENT_KEY_DOWN) {
            continue;
        }

        switch (event.key.scancode) {
            case SDL_SCANCODE_LEFT:  moveTetromino(DIR_LEFT); break;
            case SDL_SCANCODE_RIGHT: moveTetromino(DIR_RIGHT); break;
            case SDL_SCANCODE_DOWN:  moveTetromino(DIR_DOWN); break;
            case SDL_SCANCODE_UP:    rotateTetromino(); break;
            case SDL_SCANCODE_SPACE:
                if (game.state == GAME_ACTIVE) hardDrop();
                break;
            case SDL_SCANCODE_P:
                if (game.state == GAME_ACTIVE) game.state = GAME_PAUSED;
                else if (game.state == GAME_PAUSED) game.state = GAME_ACTIVE;
                break;
            case SDL_SCANCODE_R:
                initGame();
                break;
            case SDL_SCANCODE_ESCAPE:
                return false;
            default:
                break;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    setDisplayRenderer(getSDLRenderer());
    displayInit();
    initGame();

    const int frameDelay = 1000 / 60;
    uint64_t lastUpdateTime = SDL_GetTicks();

    while (true) {
        uint64_t frameStart = SDL_GetTicks();
        float deltaTime = (frameStart - lastUpdateTime) / 1000.0f;
        lastUpdateTime = frameStart;

        if (!handleInput()) {
            break;
        }
        updateGame(deltaTime);
        drawGame(&game);

        uint64_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    return 0;
}
