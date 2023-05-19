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

// Function prototypes
uint16_t memRead(uint16_t address);
uint16_t extendSign(uint16_t bits, int bitCount);
void updateFlags(uint16_t regMarker);
void branch(uint16_t instruction);
void add(uint16_t instruction);
void load(uint16_t instruction);
void store(uint16_t instruction);
void jumpRegister(uint16_t instruction);
void bitwiseAnd(uint16_t instruction);
void loadRegister(uint16_t instruction);
void storeRegister(uint16_t instruction);
void bitwiseNot(uint16_t instruction);
void loadIndirect(uint16_t instruction);
void storeIndirect(uint16_t instruction);
void jump(uint16_t instruction);
void loadEffectiveAddr(uint16_t instruction);

int main(int argc, char *argv[])
{
    reg[R_COND] = FL_ZRO;  // Set initial condition flag

    // Set initial value of PC
    // 0x3000 will be the starting point
    uint16_t PC_START = 0x3000;
    reg[R_PC] = PC_START;

    int running = 1;
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