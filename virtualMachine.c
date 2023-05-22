#include <stdint.h>
#include <stdio.h>

#define MAX_MEMORY (1 << 16)  // 16-bits (65536 in decimal)

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
    OP_JSR,  // Jump to Subroutine (opCode = 0100)
    OP_AND,  // Bitwise AND (opCode = 0101)
    OP_LDR,  // Load Base + Offset (opCode = 0110)
    OP_STR,  // Store Base + Offset (opCode = 0111)
    OP_RTI,  // Unused (opCode = 1000)
    OP_NOT,  // Bitwise NOT (opCode = 1001)
    OP_LDI,  // Load Indirect (opCode = 1010)
    OP_STI,  // Store Indirect (opCode = 1011)
    OP_JMP,  // Jump (opCode = 1100)
    OP_RES,  // Reserved (Unused) (opCode = 1101)
    OP_LEA,  // Load Effective Address (opCode = 1110)
    OP_TRAP  // Execute Trap (opCode = 1111)
};

// Trap Codes
enum {
    TRAP_GETC = 0x20,   // Read a single character from the keyboard. The character is not echoed onto the console.
    TRAP_OUT = 0x21,    // Write a character in a register to the console display.
    TRAP_PUTS = 0x22,   // Write a string of ASCII characters to the console display.
    TRAP_IN = 0x23,     // Print a prompt on the screen and read a single character from the keyboard.
    TRAP_PUTSP = 0x24,  // Write a string of ASCII characters to the console. (bytes)
    TRAP_HALT = 0x25    // Halt execution and print a message on the console.
};

uint16_t memory[MAX_MEMORY];  // 16-bit memory for VM (64 KB)
uint16_t reg[R_COUNT];        // 16-bit registers
int running = 0;

// Function prototypes
uint16_t memRead(uint16_t address);
uint16_t memWrite(uint16_t addr, uint16_t val);
uint16_t extendSign(uint16_t bits, int bitCount);
void updateFlags(uint16_t regMarker);
void branch(uint16_t instruction);
void add(uint16_t instruction);
void load(uint16_t instruction);
void store(uint16_t instruction);
void jumpToSubroutine(uint16_t instruction);
void bitwiseAnd(uint16_t instruction);
void loadBaseOffset(uint16_t instruction);
void storeBaseOffset(uint16_t instruction);
void bitwiseNot(uint16_t instruction);
void loadIndirect(uint16_t instruction);
void storeIndirect(uint16_t instruction);
void jump(uint16_t instruction);
void loadEffectiveAddr(uint16_t instruction);
void executeTrapCode(uint16_t instruction);
void trapGetc();
void trapOut();
void trapPuts();
void trapIn();
void trapPutsp();
void trapHalt();

int main(int argc, char *argv[])
{
    reg[R_COND] = FL_ZRO;  // Set initial condition flag

    // Set initial value of PC
    // 0x3000 will be the starting point
    uint16_t PC_START = 0x3000;
    reg[R_PC] = PC_START;

    running = 1;
    while (running) {
        // Fetch the instruction from the PC
        uint16_t instruction = memRead(reg[R_PC]++);
        uint16_t opCode = instruction >> 12;  // Get 4 leftmost bits for opCode

        switch (opCode) {
            case OP_BR:
                branch(instruction);
                break;
            case OP_ADD:
                add(instruction);
                break;
            case OP_LD:
                load(instruction);
                break;
            case OP_ST:
                store(instruction);
                break;
            case OP_JSR:
                jumpToSubroutine(instruction);
                break;
            case OP_AND:
                bitwiseAnd(instruction);
                break;
            case OP_LDR:
                loadBaseOffset(instruction);
                break;
            case OP_STR:
                storeBaseOffset(instruction);
                break;
            case OP_RTI:
                abort();  // op code not used, so close program
                break;
            case OP_NOT:
                bitwiseNot(instruction);
                break;
            case OP_LDI:
                loadIndirect(instruction);
                break;
            case OP_STI:
                storeIndirect(instruction);
                break;
            case OP_JMP:
                jump(instruction);
                break;
            case OP_RES:
                abort();  // op code not used, so close program
                break;
            case OP_LEA:
                loadEffectiveAddr(instruction);
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

/*
 * If a value is negative, extend the bits to 16 bits such that the
 * the value remains negative. For example, 1 1111 will become
 * 1111 1111 1111 1111. For positive values, just fill in 0's to get
 * 16 bits (done by casting as a 16-bit int in the parameter).
 *
 * bits: The bits to be converted into a 16-bit int value
 * bitCount: The number of bits contained in the original argument value
 * return: 16-bit int value with respect for Two's Complement
 */
uint16_t extendSign(uint16_t bits, int bitCount)
{
    if ((bits >> (bitCount - 1)) & 1) {
        bits |= (0xFFFF << bitCount);
    }

    return bits;
}

/*
 * Write if a value written to a register is negative, zero, or
 * positive in the condition register.
 *
 * regMarker: Marks the register being examined for the condition flag.
 * return: Void
 */
void updateFlags(uint16_t regMarker)
{
    // Using two'c compliment, a 1 being in the leftmost bit indicates
    // a negative value.
    if (reg[regMarker] >> 15) {
        reg[R_COND] = FL_NEG;
    } else if (reg[regMarker] == 0) {
        reg[R_COND] = FL_ZRO;
    } else {
        reg[R_COND] = FL_POS;
    }
}

/*
 * Branch operation to specify a new set of instructions to begin implementing
 * based on conditions set in the condition register.
 *
 * return: void
 */
void branch(uint16_t instruction)
{
    uint16_t conditionFlag = (instruction >> 9) & 0x7;  // Test bits 11 - 9 for condition check
    // if ((n AND N) OR (z AND Z) OR (p AND P))
    if (conditionFlag & reg[R_COND]) {
        uint16_t pcOffset = extendSign(instruction & 0x1FF, 9);
        reg[R_PC] += pcOffset;  // PC = PC‡ + SEXT(PCoffset9)
    }
}

/*
 * Add operation
 *
 * return: void
 */
void add(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;  // destination register
    uint16_t immFlag = (instruction >> 5) & 0x1;  // indicates if in immediate mode
    uint16_t srcReg1 = (instruction >> 6) & 0x7;  // first number to add (in reg[sr1])
    // If immFlag is 0, use value stored in reg[srcReg2] for second operand.
    // Else, use the 5-bit imm5 value with a sign extension.
    if (!immFlag) {
        uint16_t srcReg2 = instruction & 0x7;  // second number to add (in reg[sr2])
        reg[destReg] = reg[srcReg1] + reg[srcReg2];
    } else {
        uint16_t imm5 = extendSign(instruction & 0x1F, 5);  // second number to add (given 5 bit value)
        reg[destReg] = reg[srcReg1] + imm5;
    }
    updateFlags(destReg);
}

/*
 * Load instruction to move data in memory to a register.
 *
 * return: void
 */
void load(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;
    uint16_t pcOffset = extendSign(instruction & 0x1FF, 9);
    reg[destReg] = memRead(reg[R_PC] + pcOffset);
    updateFlags(destReg);
}

/*
 * Store instruction to move data in a register to memory.
 *
 * return: void
 */
void store(uint16_t instruction)
{
    uint16_t srcReg = (instruction >> 9) & 0x7;
    uint16_t pcOffset = extendSign(instruction & 0x1FF, 9);
    memWrite(reg[R_PC] + pcOffset, reg[srcReg]);
}

/*
 * Jump to subroutine instruction.
 *
 * return: void
 */
void jumpToSubroutine(uint16_t instruction)
{
    uint16_t r7 = reg[R_PC];
    uint16_t addrFlag = (instruction >> 11) & 0x1;
    // If addrFlag is 0, pull address from a register.
    // Else, use the last 11 bits of the instruction as the PC offset
    // to get get the address.
    if (!addrFlag) {
        uint16_t baseReg = (instruction >> 6) & 0x7;
        reg[R_PC] = reg[baseReg];
    } else {
        uint16_t pcOffset = extendSign(instruction & 0x7FF, 11);
        reg[R_PC] += pcOffset;
    }
}

/*
 * Bitwise AND instruction
 *
 * return: void
 */
void bitwiseAnd(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;
    uint16_t srcReg1 = (instruction >> 6) & 0x7;
    uint16_t immFlag = (instruction >> 5) & 0x1;
    // If immFlag is 0, use value is reg[srcReg2] for AND operation.
    // Else, use the provided 5-bit value in the imm5 postion with a sign
    // extension for  the AND operation.
    if (!immFlag) {
        uint16_t srcReg2 = instruction & 0x7;
        reg[destReg] = reg[srcReg1] + reg[srcReg2];
    } else {
        uint16_t imm5 = extendSign(instruction & 0x1F, 5);
        reg[destReg] = reg[srcReg1] + imm5;
    }
    updateFlags(destReg);
}

/*
 * Load Base + Offset Instruction
 *
 * return: void
 */
void loadBaseOffset(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;
    uint16_t baseReg = (instruction >> 6) & 0x7;
    uint16_t offset = extendSign(instruction & 0x3F, 6);
    reg[destReg] = memRead(reg[baseReg] + offset);
    updateFlags(destReg);
}

/*
 * Store base + offset instruction
 *
 * return: void
 */
void storeBaseOffset(uint16_t instruction)
{
    uint16_t srcReg = (instruction >> 9) & 0x7;
    uint16_t baseReg = (instruction >> 6) & 0x7;
    uint16_t offset = extendSign(instruction & 0x3F, 6);
    memWrite(reg[baseReg] + offset, reg[srcReg]);
}

/*
 * Bitwise NOT instruction
 *
 * return: void
 */
void bitwiseNot(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;
    uint16_t srcReg = (instruction >> 6) & 0x7;
    reg[destReg] = ~reg[srcReg];
    updateFlags(destReg);
}

/*
 * Load indirect instruction
 *
 * return: void
 */
void loadIndirect(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;
    uint16_t pcOffset = extendSign(instruction & 0x1FF, 9);
    reg[destReg] = memRead(memRead(reg[R_PC] + pcOffset));
    updateFlags(destReg);
}

/*
 * Store indirect instruction
 *
 * return: void
 */
void storeIndirect(uint16_t instruction)
{
    uint16_t srcReg = (instruction >> 9) & 0x7;
    uint16_t pcOffset = extendSign(instruction & 0x1FF, 9);
    memWrite(reg[R_PC] + pcOffset, reg[srcReg]);
}

/*
 * Jump instruction
 *
 * return: void
 */
void jump(uint16_t instruction)
{
    uint16_t baseReg = (instruction >> 6) & 0x7;
    reg[R_PC] = reg[baseReg];
}

/*
 * Load effective address instruction
 *
 * return: void
 */
void loadEffectiveAddr(uint16_t instruction)
{
    uint16_t destReg = (instruction >> 9) & 0x7;
    uint16_t pcOffset = extendSign(instruction & 0x1FF, 9);
    reg[destReg] = reg[R_PC] + pcOffset;
    updateFlags(destReg);
}

/*
 * Execute the trap code provided in 16-bit instruction (will
 * be contained in bits 7-0).
 *
 * return: void
 */
void executeTrapCode(uint16_t instruction)
{
    uint16_t trapVect = instruction & 0xFF;
    reg[R_R7] = reg[R_PC];
    switch (trapVect) {
        case TRAP_GETC:
            trapGetc();
            break;
        case TRAP_OUT:
            trapOut();
            break;
        case TRAP_PUTS:
            trapPuts();
            break;
        case TRAP_IN:
            // Implement code for IN
            break;
        case TRAP_PUTSP:
            // Implement code for PUTSP
            break;
        case TRAP_HALT:
            // Implement code for HALT
            break;
        default:
            abort();  // End program if unknown trap code is present
            break;
    }
}

/*
 * Read a single character from the keyboard. The character is not echoed onto the
 * console. Its ASCII code is copied into R0. The high eight bits of R0 are cleared.
 *
 * return: void
 */
void trapGetc()
{
    uint16_t inputChar = (uint16_t)getchar();  // High bits are naturally 0
    reg[R_R0] = inputChar;
    updateFlags(R_R0);
}

/*
 * Write a character in R0[7:0] to the console display.
 *
 * return: void
 */
void trapOut()
{
    char c = (char)reg[R_R0];  // Character from R0
    putchar(c);
    fflush(stdout);  // Force output buffer to be outputted to OS
}

/*
 * x22 PUTS Write a string of ASCII characters to the console display. The characters are contained
 * in consecutive memory locations, one character per memory location, starting with
 * the address specified in R0. Writing terminates with the occurrence of x0000 in a
 * memory location.
 *
 * return: void
 */
void trapPuts()
{
    uint16_t *c = memory + reg[R_R0];  // Memory address of where the first char is located
    while (*c) {
        putchar((char)*c);
        c++;  // Move pointer to point to next uint16_t value address (2 bytes)
    }
    fflush(stdout);
}