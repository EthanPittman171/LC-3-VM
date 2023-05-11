#include <stdint.h>
#include <stdio.h>

#define MAX_MEMORY (1 << 16)  // 16-bits (65536 in decimal)

int main(int argc, char *argv[])
{
    // Registers
    enum {
        R_R0,    // General Register 0
        R_R1,    // General Register 1
        R_R2,    // General Register 2
        R_R3,    // General Register 3
        R_R4,    // General Register 4
        R_R5,    // General Register 5
        R_R6,    // General Register 6
        R_R7,    // General Register 7
        R_PC,    // Program Counter
        R_COND,  // Flag Register
        R_COUNT  // Number of Registers
    };

    uint16_t memory[MAX_MEMORY];  // 16-bit memory for VM (64 KB)
    uint16_t reg[R_COUNT];        // 16-bit registers

    return 0;
}