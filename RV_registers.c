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