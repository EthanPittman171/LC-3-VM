#include <stdint.h>
#include <stdio.h>

#define MAX_MEMORY (1 << 16)  // 16-bits (65536 in decimal)

int main(int argc, char *argv[])
{
    uint16_t memory[MAX_MEMORY];  // 16-bit memory for VM (64 KB)

    return 0;
}