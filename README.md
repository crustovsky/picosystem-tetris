# Vibe Coded Tetris

A Tetris implementation for Raspberry Pi Pico with PicoSystem from Pimoroni, with cross-platform support for SDL.

<img src="./tetris_layout.svg">

## Overview

This document provides a summary of the Tetris implementation designed for a 240x240 pixel display with 16bpp color depth. The code follows a modular approach with clean separation between game logic and display handling.

## Architecture

The project is structured to allow for multiple display targets:

- **Game Logic**: Contained in `tetris_game.c/h` - handles all the game mechanics
- **Display Interface**: Contained in `tetris_display.c/h` - provides a unified drawing API and game-specific rendering 
- **Renderers**: Platform-specific implementations
  - PicoSystem: `tetris_display_picosystem.cpp` - PicoSystem implementation
  - SDL: `tetris_display_sdl.c` - Desktop implementation using SDL3

## File Structure

The implementation consists of the following files:

| File | Description |
|------|-------------|
| `tetris_game.h` | Game logic definitions and declarations |
| `tetris_game.c` | Game logic implementation |
| `tetris_display.h` | Display API and drawing function declarations |
| `tetris_display.c` | Implementation of display functions and renderer interface |
| `tetris_display_renderers.h` | Header defining renderer functions |
| `tetris_display_picosystem.cpp` | PicoSystem-specific rendering implementation |
| `tetris_display_sdl.c` | SDL3-specific rendering implementation |
| `tetris_main_pico.cpp` | Entry point and game loop for PicoSystem |
| `tetris_main_sdl.cpp` | Entry point and game loop for SDL |

## Game Components

### Game Logic (`tetris_game.h` & `tetris_game.c`)

#### Key Features:
- Standard 10×20 Tetris grid
- 7 classic tetromino shapes (I, O, T, S, Z, J, L)
- Piece movement and rotation with collision detection
- Line clearing and score calculation
- Level progression based on lines cleared
- Game states (active, paused, game over)

#### Core Functions:
- `initGame()` - Initialize the game state
- `updateGame(float tick)` - Update game state based on elapsed time
- `moveTetromino(Direction dir)` - Move the current piece
- `rotateTetromino()` - Rotate the current piece
- `clearLines()` - Clear full lines and update score

### Display Interface (`tetris_display.h` & `tetris_display.c`)

#### Display Layout (240×240):
- Game board area: 150×200 pixels (left side)
- Information panel: 60×200 pixels (right side)
- Title bar: 240×20 pixels (top)

#### Renderer Interface:
The display system is now abstracted through a renderer interface:
```c
typedef struct {
    void (*init)();
    void (*clear)(uint16_t color);
    void (*drawPixel)(int x, int y, uint16_t color);
    void (*drawLine)(int x0, int y0, int x1, int y1, uint16_t color);
    void (*drawRect)(int x, int y, int width, int height, uint16_t color);
    void (*fillRect)(int x, int y, int width, int height, uint16_t color);
    void (*drawText)(int x, int y, const char* text, uint16_t color, uint8_t size);
    void (*update)();
} DisplayRenderer;
```

#### Tetris-Specific Drawing Functions:
- `clearScreen()` - Clear screen for new frame
- `drawTitle()` - Draw game title
- `drawBoard()` - Draw game board with placed blocks and current piece
- `drawNextPiece()` - Show preview of the next piece
- `drawStats()` - Display score, level, and lines cleared
- `drawGameState()` - Show game over or paused message
- `drawGame()` - Main drawing function that calls all others

### Main Programs (`tetris_main_pico.cpp` & `tetris_main_sdl.cpp`)

#### PicoSystem Main (`tetris_main_pico.cpp`):
- PicoSystem initialization
- PicoSystem input handling
- PicoSystem lifecycle functions (init, update, draw)

#### SDL Main (`tetris_main_sdl.cpp`):
- SDL initialization
- SDL input handling
- SDL main loop with timing control

## Building

### For PicoSystem

To build for PicoSystem:

```bash
mkdir build_pico
cd build_pico
cmake .. -DUSE_PICOSYSTEM=ON
make
```

### For SDL (Desktop)

To build for desktop using SDL:

```bash
mkdir build_sdl
cd build_sdl
cmake ..
make
```

## Controls

### PicoSystem
- **Left/Right/Down**: Move piece
- **A**: Rotate piece
- **B**: Pause/Unpause
- **X**: Restart after game over

### SDL
- **Arrow Keys**: Move and rotate piece
- **P**: Pause/Unpause
- **R**: Restart after game over
- **ESC**: Quit

## Development

This project uses an abstraction layer for display rendering to make it easy to port to different platforms. The display renderer interface is defined in `tetris_display.h` and includes functions for basic drawing operations.

To add support for a new platform:
1. Create a new renderer implementation file (e.g., `tetris_display_myplatform.c`)
2. Implement the `DisplayRenderer` interface functions for your platform
3. Add a function to get your renderer (e.g., `getMyPlatformRenderer()`)
4. Update `tetris_display_renderers.h` to expose your new renderer
5. Update CMakeLists.txt to include your platform-specific build options

## Memory Usage

- Frame buffer: 240×240×2 bytes (115.2 KB) for 16bpp color
- Game state: Minimal (~1 KB)
- Code size: Small, suitable for embedded applications

## Performance Considerations

- Drawing optimizations for speed (using primitives like `fillRect`)
- Minimal dynamic memory allocation
- Efficient update logic with time-based movement