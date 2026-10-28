# CHIP-8 Emulator

A CHIP-8 emulator/interpreter written in C++, built from scratch to understand how CPUs, memory, and low-level instruction execution work.

![Platform](https://img.shields.io/badge/platform-macOS-lightgrey)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Build](https://img.shields.io/badge/build-CMake-informational)
[![Build Status](https://github.com/seervii/chip8-emulator/actions/workflows/build.yml/badge.svg)](https://github.com/seervii/chip8-emulator/actions/workflows/build.yml)

## What is CHIP-8?

CHIP-8 is an interpreted programming language from the mid-1970s, originally designed to make it easier to write games for 8-bit microcomputers. Rather than being a real physical CPU, it's a virtual machine — meaning it defines a set of instructions (opcodes) that get executed by a program (an emulator/interpreter) rather than actual hardware.

This project implements a complete CHIP-8 interpreter, including all 34 original instructions, capable of running classic CHIP-8 ROMs such as Pong, Tetris, and Space Invaders clones.

## Features

- Full implementation of all 34 CHIP-8 instructions
- Accurate memory management (4KB address space)
- Font rendering system (hexadecimal characters 0-F)
- ROM loading from binary `.ch8` files
- SDL2-based graphics rendering (64x32 display, scalable)
- Keyboard input mapped to the original 16-key hexadecimal keypad
- Adjustable emulation speed via command-line arguments

## Tech Stack

- **Language:** C++17
- **Build System:** CMake
- **Graphics/Input:** SDL2
- **Platform tested:** macOS

## Getting Started

### Prerequisites

Install CMake and SDL2 via Homebrew:

```bash
brew install cmake sdl2
```

### Build

```bash
git clone https://github.com/seervii/chip8-emulator.git
cd chip8-emulator
mkdir build
cd build
cmake ..
cmake --build .
```

### Run

```bash
./chip8-emulator <Scale> <Delay> <ROM Path>
```

**Example:**
```bash
./chip8-emulator 10 3 ../roms/pong.ch8
```

- `Scale` — window size multiplier (e.g., 10 = 640x320 window)
- `Delay` — cycle delay in milliseconds, controls emulation speed
- `ROM Path` — path to the `.ch8` ROM file to load

## Architecture Overview

| Component | Description |
|---|---|
| Memory | 4KB (4096 bytes) address space |
| Registers | 16 general-purpose 8-bit registers (V0-VF) |
| Stack | 16-level stack for subroutine calls |
| Display | 64x32 monochrome pixel display |
| Timers | Delay timer and sound timer, both counting down at 60Hz |
| Keypad | 16-key hexadecimal input |

ROM instructions load into memory starting at address `0x200`, since addresses below that are reserved for the interpreter and font data.

## Acknowledgments

- [Austin Morlan's CHIP-8 Emulator Guide](https://austinmorlan.com/posts/chip8_emulator/) — primary reference for this implementation