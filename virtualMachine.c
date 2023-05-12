#include <stdint.h>
#include <stdio.h>

#define MAX_MEMORY (1 << 16)  // 16-bits (65536 in decimal)

uint16_t mem_read(uint16_t address);

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

    // Condition Flags
    enum {
        FL_POS = 1 << 0,  // P
        FL_ZRO = 1 << 1,  // Z
        FL_NEG = 1 << 2   // N
    };

    // Instructions
    enum {
        OP_BR,   // Branch (opCode = 0000)
        OP_ADD,  // Add (opCode = 0001)
        OP_LD,   // Load (opCode = 0010)
        OP_ST,   // Store (opCode = 0011)
        OP_JSR,  // Jump Register (opCode = 0100)
        OP_AND,  // Bitwise AND (opCode = 0101)
        OP_LDR,  // Load Register (opCode = 0110)
        OP_STR,  // Store Register (opCode = 0111)
        OP_RTI,  // Unused (opCode = 1000)
        OP_NOT,  // Bitwise NOT (opCode = 1001)
        OP_LDI,  // Load Indirect (opCode = 1010)
        OP_STI,  // Store Indirect (opCode = 1011)
        OP_JMP,  // Jump (opCode = 1100)
        OP_RES,  // Reserved (Unused) (opCode = 1101)
        OP_LEA,  // Load Effective Address (opCode = 1110)
        OP_TRAP  // Execute Trap (opCode = 1111)
    };

    uint16_t memory[MAX_MEMORY];  // 16-bit memory for VM (64 KB)
    uint16_t reg[R_COUNT];        // 16-bit registers

    reg[R_COND] = FL_ZRO;  // Set initial condition flag

    // Set initial value of PC
    // 0x3000 will be the starting point
    uint16_t PC_START = 0x3000;
    reg[R_PC] = PC_START;

    int running = 1;
    while (running) {
        // Fetch the instruction from the PC
        uint16_t instruction = mem_read(reg[R_PC]++);
        uint16_t opCode = instruction >> 12;  // Get 4 leftmost bits for opCode

        switch (opCode) {
            case OP_BR:
                // Implement code for Branch
                break;
            case OP_ADD:
                // Implement code for Add
                break;
            case OP_LD:
                // Implement code for Load
                break;
            case OP_ST:
                // Implement code for Store
                break;
            case OP_JSR:
                // Implement code for Jump Register
                break;
            case OP_AND:
                // Implement code for Bitwise AND
                break;
            case OP_LDR:
                // Implement code for Load Register
                break;
            case OP_STR:
                // Implement code for Store Register
                break;
            case OP_RTI:
                // Unused
                break;
            case OP_NOT:
                // Implement code for Bitwise NOT
                break;
            case OP_LDI:
                // Implement code for Load Indirect
                break;
            case OP_STI:
                // Implement code for Store Indirect
                break;
            case OP_JMP:
                // Implement code for Jump
                break;
            case OP_RES:
                // Implement code for Reserved (Unused)
                break;
            case OP_LEA:
                // Implement code for Load Effective Address
                break;
            case OP_TRAP:
                // Implement code for Execute Trap
                break;
            default:
                // Implement code for a bad op code
                break;
        }
    }

    return 0;
}