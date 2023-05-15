//
// Created by Ricardo Jacobi on 18/11/22.
// Modified By Amanda Reis on 14/05/2023
//

#include "RV_header.h"
//
// banco de registradores do RV
//

int32_t breg[32];

//
// registradores especiais
//

uint32_t	pc,				// contador de programa
            sp,				// stack pointer
            gp,				// global pointer
            ri;				// registrador de intrucao


//
// campos das instrucoes RISCV
//
// Formato R:  | funct7       | rs2 | rs1 | funct3      | rd | opcode |
// Formato I:  |    imm[11:0]       | rs1 | funct3      | rd | opcode |
// Formato S:  | imm[11:5]    | rs2 | rs1 | imm[4:0]    | rd | opcode |
// Formato SB: | imm[12-11:5] | rs2 | rs1 | imm[4:1-11] | rd | opcode |
// Formato U:  |               imm[31:12]               | rd | opcode |
// Formato UJ: | imm[20] imm[10:1] imm[11] imm[19:12]   | rd | opcode |

uint32_t	opcode,		// codigo da operacao
            rs1,		// indice registrador rs
            rs2,		// indice registrador rt
            rd,			// indice registrador rd
            shamt,		// deslocamento
            funct3,		// campos auxiliares
            funct7;		// constante instrucao tipo J

int32_t		imm12_i,	// constante 12 bits
            imm12_s,				// constante 12 bits
            imm13,					// constante 13 bits
            imm20_u,				// constante 20 bis mais significativos
            imm21,					// constante 21 bits
            imm32;                  // constante de 32 bits: o imediado da instrução

char* instr_str[39];

void build_dic() {
    instr_str[I_add]	= "ADD";	instr_str[I_addi] = "ADDi";	instr_str[I_and] = "AND";
    instr_str[I_andi]	= "ANDi";	instr_str[I_auipc] = "AUIPC";
    instr_str[I_beq]	= "BEQ";	instr_str[I_bge] = "BGE";
    instr_str[I_bgeu]	= "BGEU";	instr_str[I_blt] = "BLT";	instr_str[I_bltu] = "BLTU";
    instr_str[I_bne]	= "BNE";
    instr_str[I_jal]	= "JAL";	instr_str[I_jalr] = "JALR";
    instr_str[I_lb]		= "LB";		instr_str[I_lbu] = "LBU";	instr_str[I_lh] = "LH";
    instr_str[I_lhu]	= "LHU";	instr_str[I_lui] = "LUI";	instr_str[I_lw] = "LW";
    instr_str[I_or]		= "OR";		instr_str[I_ori] = "ORi";
    instr_str[I_sb]		= "SB";		instr_str[I_sh] = "SH";		instr_str[I_sll] = "SLL";
    instr_str[I_slt]	= "SLT";	instr_str[I_slli] = "SLLi";
    instr_str[I_slti]	= "SLTi";	instr_str[I_sltiu] = "SLTIU";
    instr_str[I_sltu]	= "SLTU";	instr_str[I_srl] = "SRL";
    instr_str[I_sra]	= "SRA";	instr_str[I_srli] = "SRLi";
    instr_str[I_srai]	= "SRAi";	instr_str[I_sub] = "SUB";
    instr_str[I_sw]		= "SW";		instr_str[I_xor] = "XOR";	instr_str[I_xori] = "XORi";
    instr_str[I_ecall]  = "ECALL";  instr_str[I_nop]  = "NOP";
}

enum INSTRUCTIONS ins_code;
