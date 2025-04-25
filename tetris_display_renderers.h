#ifndef TETRIS_DISPLAY_RENDERERS_H
#define TETRIS_DISPLAY_RENDERERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tetris_display.h"

// Function to get the PicoSystem renderer
DisplayRenderer* getPicoSystemRenderer();

// Function to get the SDL renderer
DisplayRenderer* getSDLRenderer();

#ifdef __cplusplus
}
#endif

#endif /* TETRIS_DISPLAY_RENDERERS_H */