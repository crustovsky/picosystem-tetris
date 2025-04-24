# Tetris Implementation Documentation

## Overview

This document provides a summary of the Tetris implementation designed for a 240x240 pixel display with 16bpp color depth. The code follows a modular approach with clean separation between game logic and display handling.

## File Structure

The implementation consists of five files:

| File | Description |
|------|-------------|
| `tetris.h` | Game logic definitions and declarations |
| `tetris.c` | Game logic implementation |
| `tetris_display.h` | Display API and drawing function declarations |
| `tetris_display.c` | Implementation of display functions |
| `main.c` | Entry point and game loop |

## Game Components

### Game Logic (`tetris.h` & `tetris.c`)

#### Key Features:
- Standard 10×20 Tetris grid
- 7 classic tetromino shapes (I, O, T, S, Z, J, L)
- Piece movement and rotation with collision detection
- Line clearing and score calculation
- Level progression based on lines cleared
- Game states (active, paused, game over)

#### Core Functions:
- `initGame()` - Initialize the game state
- `update(float tick)` - Update game state based on elapsed time
- `moveTetromino(Direction dir)` - Move the current piece
- `rotateTetromino()` - Rotate the current piece
- `clearLines()` - Clear full lines and update score

### Display Interface (`tetris_display.h` & `tetris_display.c`)

#### Display Layout (240×240):
- Game board area: 150×200 pixels (left side)
- Information panel: 60×200 pixels (right side)
- Title bar: 240×20 pixels (top)

#### Low-Level Display Functions:
- `displayInit()` - Initialize display hardware
- `displayClear()` - Clear the screen
- `displayDrawPixel()` - Set a single pixel
- `displayDrawLine()` - Draw a line (Bresenham algorithm)
- `displayDrawRect()` - Draw a rectangle outline
- `displayFillRect()` - Draw a filled rectangle
- `displayDrawText()` - Render text with specific color and size
- `displayUpdate()` - Send frame buffer to display

#### Tetris-Specific Drawing Functions:
- `clearScreen()` - Clear screen for new frame
- `drawTitle()` - Draw game title
- `drawBoard()` - Draw game board with placed blocks and current piece
- `drawNextPiece()` - Show preview of the next piece
- `drawStats()` - Display score, level, and lines cleared
- `drawGameState()` - Show game over or paused message
- `draw()` - Main drawing function that calls all others

### Main Program (`main.c`)

#### Features:
- Game initialization
- Main game loop running at 40 FPS
- Input handling framework
- Connection between game logic and display rendering

#### Key Functions:
- `handleInput()` - Process user input
- `delay_ms()` - Timing control
- `main()` - Entry point and game loop

## Implementation Notes

### Hardware Abstraction

The implementation uses a clean separation between hardware-specific and game-specific code:

- **Hardware-Specific Areas:** All hardware-dependent code is isolated in clearly marked sections with TODOs in `tetris_display.c` and `main.c`
- **Game Logic:** All game mechanics in `tetris.c` are hardware-independent
- **Drawing Logic:** All drawing functions in `tetris_display.c` use the abstracted display API

### Customization Points

The following areas need to be implemented for specific hardware:

1. **Display Interface:**
   - Hardware initialization
   - Frame buffer management
   - Pixel rendering to physical display
   - Text rendering

2. **Input Handling:**
   - Button/control reading
   - Input mapping to game actions

3. **Timing:**
   - Delay or timing functions for maintaining frame rate

### Memory Usage

- Frame buffer: 240×240×2 bytes (115.2 KB) for 16bpp color
- Game state: Minimal (~1 KB)
- Code size: Small, suitable for embedded applications

### Performance Considerations

- Drawing optimizations for speed (using primitives like `fillRect`)
- Minimal dynamic memory allocation
- Efficient update logic with time-based movement

## Porting Guide

To adapt this implementation to a specific hardware platform:

1. Implement display hardware initialization in `displayInit()`
2. Fill in frame buffer transfer in `displayUpdate()`
3. Implement text rendering in `displayDrawText()`
4. Add input handling in `main.c` based on available controls
5. Implement timing/delay functions in `main.c`

## Compilation

Basic compilation command:
```
gcc -o tetris main.c tetris.c tetris_display.c -I. -std=c99
```

Adjust compiler flags as appropriate for the target platform.
