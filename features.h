#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

#define EIGHTXY_VY false
#define BXNN false
#define FX55_FX65_INCREMENT false

#define STACK_SIZE 16
#define REG_SIZE 16
#define MEM_SIZE 4096
#define INIT_MEM 0x200
#define INST_PER_SEC 700

#define RESET_THRESHOLD 10

#define WIN_NAME "CHIP-8 Emulator"

#define ORIG_WIDTH 64
#define ORIG_HEIGHT 32
#define ORIG_PIXEL_COUNT ORIG_WIDTH * ORIG_HEIGHT

#define LOGFILE stdout

uint32_t WINFLAGS = SDL_WINDOW_VULKAN;
uint32_t RENDFLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

const int SCALE_FACTOR = 16;

const int WIDTH = ORIG_WIDTH * SCALE_FACTOR;
const int HEIGHT = ORIG_HEIGHT * SCALE_FACTOR;
