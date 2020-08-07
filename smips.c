// Assignment 2: smips
// by Hayley Gayfer (z5312096)
// date started: 28/7/2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <ctype.h>

#define MAXLINE 9
#define REG_1 21
#define REG_2 16
#define REG_EQ 11
#define INST 26

#define ADD 32
#define SUB 34
#define AND 36
#define OR 37
#define MUL 2
#define SLT 42
#define BEQ 4
#define BNE 5
#define ADDI 8
#define SLTI 10
#define ANDI 12
#define ORI 13
#define LUI 15

#define PRINT_INT 1
#define EXIT 10
#define PRINT_CHAR 11

#define SYSCALL 12

#define TRUE 0
#define FALSE 1

typedef int32_t Register;
typedef int32_t Instruction;
typedef int Syscall;

// unique MIPS instruction; e.g. Syscall, immediate or register commands
// includes command, registers / immediate value used in instruction
typedef struct _instruction {
    Syscall call;
    Instruction command;
    Register used_registers[3];
    int16_t immediate_val;
} *Instruct;

// array of instructions that forms the program
typedef struct _instruction_list {
    Instruct mips_instructions[1000];
    int num_instructions;
} *Instruct_List;

// Function definitions
Instruct_List interpret_reg_instruct(Instruction instruct, Instruct_List l);
Instruct_List interpret_imm_instruct(Instruction instruct, Instruct_List l);
Instruction hex_to_binary(char *hex);
Instruct_List newInstruction(Instruct_List l, Register reg_1, Register reg_2, Register reg_eq, uint32_t immediate, Instruction command, int call);
int is_valid(Instruction code);

int main (int argc, char *argv[]) {

    char *file = argv[1];

    FILE *fh = fopen(file, "r");

    if (fh == NULL) {
        printf("Error num: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    // initialise all 32 registers as 0
    Register registers[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // initialise list of instructions
    Instruct_List i_list = malloc(sizeof(struct _instruction_list));
    i_list->num_instructions = 0;

    char line[MAXLINE];

    // translate MIPS instructions and 'load'
    while (fgets(line, sizeof(line), fh)) {
        if (line[0] == '\n' || line[0] == '\0') {
            if ((fgets(line, sizeof(line), fh)) == NULL) {
                break;
            }

        }

        // convert mips hex code to binary
        Instruction mips_com = hex_to_binary(line);

        if (is_valid(mips_com) == FALSE) {
            printf("%s:%d: invalid instruction code: %s\n", file, i_list->num_instructions + 1, line);
            return 1;
        }

        // Syscall
        if (mips_com == SYSCALL) {
            i_list = newInstruction(i_list, 0, 0, 0, 0, 0, SYSCALL);
        // command uses only registers (first 6 bytes are 0, with exception of mul)
        } else if (mips_com >> 26 == 0 || mips_com >> 26 == 28) {
            i_list = interpret_reg_instruct(mips_com, i_list);
        // command loads immediate value 
        } else {
            i_list = interpret_imm_instruct(mips_com, i_list);
        }
    }

    fclose(fh);
    // FILE CLOSED

    // print the program
    printf("Program\n");
    for (int i = 0; i < i_list->num_instructions; i++) {
        printf("%3d: ", i);

        // Syscall: print integer
        if (i_list->mips_instructions[i]->call == SYSCALL) {
            printf("syscall");
        }

        int16_t immediate = i_list->mips_instructions[i]->immediate_val;
        Register reg_1 = i_list->mips_instructions[i]->used_registers[0];
        Register reg_2 = i_list->mips_instructions[i]->used_registers[1];
        Register reg_eq = i_list->mips_instructions[i]->used_registers[2];

        switch (i_list->mips_instructions[i]->command) {
            // add $reg_eq, $reg_1, $reg_2
            case ADD: 
                printf("add  $%d, $%d, $%d", reg_eq, reg_1, reg_2);
                break;
            // sub $reg_eq, $reg_1, $reg_2
            case SUB: 
                printf("sub  $%d, $%d, $%d", reg_eq, reg_1, reg_2);
                break;
            // and $reg_eq, $reg_1, $reg_2  
            case AND: 
                printf("and  $%d, $%d, $%d", reg_eq, reg_1, reg_2);
                break;
            // or $reg_eq, $reg_1, $reg_2
            case OR: 
                printf("or   $%d, $%d, $%d", reg_eq, reg_1, reg_2);
                break;
            // slt $reg_eq, $reg_1, $reg_2
            case SLT: 
                printf("slt  $%d, $%d, $%d", reg_eq, reg_1, reg_2);
                break;
            // mul $reg_eq, $reg_1, $reg_2
            case MUL: 
                printf("mul  $%d, $%d, $%d", reg_eq, reg_1, reg_2);
                break;
            // beq $reg_1, $reg_2, immediate
            case BEQ: 
                printf("beq  $%d, $%d, %d", reg_1, reg_2, immediate);
                break;
            // bne $reg_1, $reg_2, immediate
            case BNE: 
                printf("bne  $%d, $%d, %d", reg_1, reg_2, immediate);
                break;
            // addi $reg_2, $reg_1, immediate
            case ADDI: 
                printf("addi $%d, $%d, %d", reg_2, reg_1, immediate);
                break;
            // slti $reg_2, $reg_1, immediate
            case SLTI: 
                printf("slti $%d, $%d, %d", reg_2, reg_1, immediate);
                break;
            // andi $reg_2, $reg_1, immediate
            case ANDI: 
                printf("andi $%d, $%d, %d", reg_2, reg_1, immediate);
                break;
            // ori $reg_2, $reg_1, immediate
            case ORI: 
                printf("ori  $%d, $%d, %d", reg_2, reg_1, immediate);
                break;
            // ori $reg_2, immediate
            case LUI: 
                printf("lui  $%d, %d", reg_2, immediate);
                break;
        }
        printf("\n");
    }

    printf("Output\n");

    // 'run' the program
    for (int prog_counter = 0; prog_counter < i_list->num_instructions; prog_counter++) {
        registers[0] = 0;

        if (i_list->mips_instructions[prog_counter]->call == SYSCALL) {
            // Syscall: print integer
            if (registers[2] == PRINT_INT) {
                printf("%d", registers[4]);
            // Syscall: exit
            } else if (registers[2] == EXIT) {
                break;
            // Syscall: print character
            } else if (registers[2] == PRINT_CHAR) {
                printf("%c", registers[4]);
            } else {
                printf("Unknown system call: %d\n", registers[2]);
                prog_counter = i_list->num_instructions;
                break;
            }
        }

        int16_t immediate = i_list->mips_instructions[prog_counter]->immediate_val;
        Register reg_1 = i_list->mips_instructions[prog_counter]->used_registers[0];
        Register reg_2 = i_list->mips_instructions[prog_counter]->used_registers[1];
        Register reg_eq = i_list->mips_instructions[prog_counter]->used_registers[2];

        switch (i_list->mips_instructions[prog_counter]->command) {
            // add $reg_eq, $reg_1, $reg_2
            case ADD: 
                registers[reg_eq] = registers[reg_1] + registers[reg_2];
                break;
            // sub $reg_eq, $reg_1, $reg_2
            case SUB: 
                registers[reg_eq] = registers[reg_1] - registers[reg_2];
                break;
            // and $reg_eq, $reg_1, $reg_2  
            case AND: 
                registers[reg_eq] = registers[reg_1] & registers[reg_2];
                break;
            // or $reg_eq, $reg_1, $reg_2
            case OR: 
                registers[reg_eq] = registers[reg_1] | registers[reg_2];
                break;
            // slt $reg_eq, $reg_1, $reg_2
            case SLT: 
                if (registers[reg_1] < registers[reg_2]) 
                    registers[reg_eq] = 1;
                else
                    registers[reg_eq] = 0;
                break;
            // mul $reg_eq, $reg_1, $reg_2
            case MUL: 
                registers[reg_eq] = registers[reg_1] * registers[reg_2];
                break;
            // beq $reg_1, $reg_2, immediate
            case BEQ: 
                if (registers[reg_1] == registers[reg_2]) {
                    prog_counter += (immediate - 1);
                }
                break;
            // bne $reg_1, $reg_2, immediate
            case BNE: 
                if (registers[reg_1] != registers[reg_2]) {
                    prog_counter += (immediate - 1);
                } 
                break;
            // addi $reg_2, $reg_1, immediate
            case ADDI: 
                registers[reg_2] = registers[reg_1] + immediate;
                break;
            // slti $reg_2, $reg_1, immediate
            case SLTI: 
                registers[reg_2] = (registers[reg_1] < immediate);
                break;
            // andi $reg_2, $reg_1, immediate
            case ANDI: 
                registers[reg_2] = registers[reg_1] & immediate;
                break;
            // ori $reg_2, $reg_1, immediate
            case ORI: 
                registers[reg_2] = registers[reg_1] | immediate;
                break;
            // ori $reg_2, immediate
            case LUI:
                registers[reg_2] = (immediate << 16);
                break;
        }
    }

    printf("Registers After Execution\n");

    for (int i = 1; i < 32; i++) {
        if (registers[i] != 0) {
            if (i >= 10) {
                printf("$%d = %d\n", i, registers[i]);
            } else {
                printf("$%d  = %d\n", i, registers[i]);
            }
        }
    }

    return 0;
}

// seperate non-immediate instruction into : command... reg_1... reg2... reg_eq
Instruct_List interpret_reg_instruct(Instruction instruct, Instruct_List l) {
    uint32_t mask = 31;
    Register reg_1 = (instruct >> REG_1) & mask;
    Register reg_2 = (instruct >> REG_2) & mask;
    Register reg_eq = (instruct >> REG_EQ) & mask;

    uint32_t inst_mask = 2047;
    Instruction inst = instruct & inst_mask;

    l = newInstruction(l, reg_1, reg_2, reg_eq, 0, inst, -1);

    return l;
}

// seperate immediate instruction into : command... reg_1... reg2... immediate value
Instruct_List interpret_imm_instruct(Instruction instruct, Instruct_List l) {
    uint32_t mask = 31;
    Instruction inst = (instruct >> INST) & mask;

    Register reg_1 = (instruct >> REG_1) & mask;
    Register reg_2 = (instruct >> REG_2) & mask;

    uint32_t imm_mask = 65535;
    int32_t immediate = instruct & imm_mask;

    l = newInstruction(l, reg_1, reg_2, -1, immediate, inst, -1);

    return l;
}

Instruct_List newInstruction(Instruct_List l, Register reg_1, Register reg_2, Register reg_eq, uint32_t immediate, Instruction command, int call) {
    Instruct inst = malloc(sizeof(*inst));

    inst->call = call;
    inst->command = command;
    inst->used_registers[0] = reg_1;
    inst->used_registers[1] = reg_2;
    inst->used_registers[2] = reg_eq;
    inst->immediate_val = immediate;

    l->mips_instructions[l->num_instructions] = inst;

    // DEBUGGING
    /*printf("----------------------------------------------\n");
    printf("Instruction: %d\ncall: %d\ncommand: %d\nReg1: %d\n", l->num_instructions, inst->call, inst->command, inst->used_registers[0]);
    printf("Reg2: %d\nRegEq: %d\nImmediate: %d\n", inst->used_registers[1], inst->used_registers[2], inst->immediate_val);*/
    l->num_instructions++;

    return l;
}

// convert hex string mips command to binary format
Instruction hex_to_binary(char *hex) {
    int i = 0;
    Instruction bin_code = 0;

    while (hex[i] != '\0' && hex[i] != '\n') {
        if (hex[i] >= 'A' && hex[i] <= 'Z') hex[i] = tolower(hex[i]);
        switch (hex[i]) {
            case '1':
                bin_code |= 1;
                break;
            case '2':
                bin_code |= 2;
                break;
            case '3':
                bin_code |= 3;
                break;
            case '4':
                bin_code |= 4;
                break;
            case '5':
                bin_code |= 5;
                break;
            case '6':
                bin_code |= 6;
                break;
            case '7':
                bin_code |= 7;
                break;
            case '8':
                bin_code |= 8;
                break;
            case '9':
                bin_code |= 9;
                break;    
            case 'a':
                bin_code |= 10;
                break;
            case 'b':
                bin_code |= 11;
                break;
            case 'c':
                bin_code |= 12;
                break;
            case 'd':
                bin_code |= 13;
                break;
            case 'e':
                bin_code |= 14;
                break;
            case 'f':
                bin_code |= 15;
                break;    
        }
        i++;
        if (hex[i] != '\0' && hex[i] != '\n') bin_code <<= 4;
    }

    return bin_code;
}

int is_valid(Instruction code) {
    if (code == SYSCALL) {
        return TRUE;
    }
    if (code >> 26 == 0 || code >> 26 == 28) {
        code &= 63;
    } else {
        code >>= 26;
    }
    switch (code) {
        case ADD: break;
        case SUB: break;
        case AND: break;
        case OR: break;
        case SLT: break;
        case MUL: break;
        case BEQ: break;
        case BNE: break;
        case ADDI: break;
        case SLTI: break;
        case ANDI: break;
        case ORI: break;
        case LUI: break;
        default:
            return FALSE;
    }
    return TRUE;
}

