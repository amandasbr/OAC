//
// Created by Ricardo Jacobi on 18/11/22.
// Modified By Amanda Reis on 14/05/2023
//
#include "RV_header.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//
//Memória
//
int32_t mem[MEM_SIZE];

char stop_prg;
//
// Initial values for registers
//

void init() {
    pc = 0;
    ri = 0;
    stop_prg = 0;
    sp = 0x3ffc;
    gp = 0x1800;
    build_dic();
}

void fetch() {
    ri = lw(pc, 0);
    pc = pc + 4;
}

void decode() {
    int32_t tmp;

    opcode = ri & 0x7F;
    rs2 = (ri >> 20) & 0x1F;
    rs1 = (ri >> 15) & 0x1F;
    rd = (ri >> 7) & 0x1F;
    shamt = (ri >> 20) & 0x1F;
    funct3 = (ri >> 12) & 0x7;
    funct7 = (ri >> 25);
    imm12_i = ((int32_t) ri) >> 20;
    tmp = get_field(ri, 7, 0x1f);
    imm12_s = set_field(imm12_i, 0, 0x1f, tmp);
    imm13 = imm12_s;
    imm13 = set_bit(imm13, 11, imm12_s & 1);
    imm13 = imm13 & ~1;
    imm20_u = ri & (~0xFFF);
    imm21 = (int32_t) ri >> 11;            // estende sinal
    tmp = get_field(ri, 12, 0xFF);        // le campo 19:12
    imm21 = set_field(imm21, 12, 0xFF, tmp);    // escreve campo em imm21
    tmp = get_bit(ri, 20);                // le o bit 11 em ri(20)
    imm21 = set_bit(imm21, 11, tmp);            // posiciona bit 11
    tmp = get_field(ri, 21, 0x3FF);
    imm21 = set_field(imm21, 1, 0x3FF, tmp);
    imm21 = imm21 & ~1;                    // zero bit 0
    ins_code = get_instr_code(opcode, funct3, funct7);
}

int load_mem(const char *codeFile, const char *dataFile) {
    FILE *codeFilePtr, *dataFilePtr;
    int *memoryPtr = mem + (CODE_SEGMENT_START >> 2);
    int size = 0;

    codeFilePtr = fopen(codeFile, "rb");
    if(!codeFilePtr){
        printf("Arquivo nao encontrado!");
        return -1;
    }
    else{
        while (!feof(codeFilePtr)){
            fread(memoryPtr, 4, 1, codeFilePtr);
            memoryPtr++;
            size++;
        }
        fclose(codeFilePtr);
    }

    memoryPtr = mem + (DATA_SEGMENT_START >> 2);
    dataFilePtr = fopen(dataFile, "rb");
    if(!dataFilePtr){
        printf("Arquivo nao encontrado!");
        return -1;
    }
    else{
        while (!feof(dataFilePtr)){
            fread(memoryPtr, 4, 1, dataFilePtr);
            memoryPtr++;
            size++;
        }
        fclose(dataFilePtr);
    }
    return size;
}


int32_t get_imm32(enum FORMATS iformat) {
    switch (iformat) {
        case RType:     return 0;
        case IType:     return imm12_i;
        case SType:     return imm12_i;
        case SBType:    return imm13;
        case UType:     return imm20_u;
        case UJType:    return imm21;
        default:        return 0;
    }
}

enum FORMATS get_i_format(uint32_t opcode, uint32_t funct3, uint32_t funct7) {
    switch (opcode) {
        case 0x33 :
            return RType;
        case 0x03:
        case 0x13:
        case 0x67:
        case 0x73:
            return IType;
        case 0x23 :
            return SType;
        case 0x63 :
            return SBType;
        case 0x37 :
            return UType;
        case 0x6F:
        case 0x17 :
            return UJType;
        case 0x00:
            if (funct3 == 0 && funct7 == 0)
                return NOPType;
            else
                return NullFormat;
        default:
            printf("Undefined Format");
            return NullFormat;
    }
}

enum INSTRUCTIONS get_instr_code(uint32_t opcode, uint32_t funct3, uint32_t funct7) {
    switch (opcode) {
        case LUI:
            return I_lui;
        case AUIPC:
            return I_auipc;
        case BType:
            switch (funct3) {
                case BEQ3:
                    return I_beq;
                case BNE3:
                    return I_bne;
                case BLT3:
                    return I_blt;
                case BGE3:
                    return I_bge;
                case BLTU3:
                    return I_bltu;
                case BGEU3:
                    return I_bgeu;
            }
            break;
        case ILType:
            switch (funct3) {
                case LB3:
                    return I_lb;
                case LH3:
                    return I_lh;
                case LW3:
                    return I_lw;
                case LBU3:
                    return I_lbu;
                default:
                    break;
            }
            break;
        case JAL:
            return I_jal;
        case JALR:
            return I_jalr;
        case StoreType:
            switch (funct3) {
                case SB3:
                    return I_sb;
                case SH3:
                    return I_sh;
                case SW3:
                    return I_sw;
                default:
                    break;
            }
            break;
        case ILAType:
            switch (funct3) {
                case ADDI3:
                    return I_addi;
                case ORI3:
                    return I_ori;
                case ANDI3:
                    return I_andi;
                case XORI3:
                    return I_xori;
                case SLTI3:
                    return I_slti;
                case SLTIU3:
                    return I_sltiu;
                case SLLI3:
                    return I_slli;
                case SRI3:
                    if (funct7 == SRLI7) return I_srli;
                    else return I_srai;
                default:
                    break;
            }
            break;
        case RegType:
            switch (funct3) {
                case ADDSUB3:
                    if (funct7 == SUB7) return I_sub;
                    else return I_add;
                case SLL3:
                    return I_sll;
                case SLT3:
                    return I_slt;
                case SLTU3:
                    return I_sltu;
                case XOR3:
                    return I_xor;
                case OR3:
                    return I_or;
                case AND3:
                    return I_and;
                case SR3:
                    if (funct7 == SRA7) return I_sra;
                    else return I_srl;
                default:
                    break;
            }
            break;
        case ECALL:
            return I_ecall;
        default:
            printf("\n\nInstrucao Invalida (PC = %08x RI = %08x)\n", pc, ri);
            break;
    }
    return I_nop;
}



/*Amanda Soares Bispo Reis
Funções implementadas*/

void execute(){
    enum INSTRUCTIONS instruct = get_instr_code(opcode, funct3, funct7);

    switch (instruct) {
        case I_lui:
            lui();
            return;
        case I_auipc:
            auipc();
            return;
        
        case I_beq:
            beq();
            return;
        case I_bne:
            bne();
            return;
        case I_blt:
            blt();
            return;
        case I_bge:
            bge();
            return;
        case I_bltu:
            bltu();
            return;
        case I_bgeu:
            bgeu();
            return;
    
        case I_lb:
            breg[rd] = lb(get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;
        case I_lh:
            breg[rd] = lh(get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;
        case I_lw:
            breg[rd] = lw(get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;
        case I_lbu:
            breg[rd] = lbu(get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;

        case I_jal:
            return jal();
        case I_jalr:
            return jalr();
      
        case I_sb:
            sb(breg[rs1],get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;
        case I_sh:
            sb(breg[rs1],get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;
        case I_sw:
            sw(breg[rs1],get_imm32(get_i_format(opcode, funct3, funct7)), breg[rs1]);
            return;

        case I_addi:
            addi();
            return;
        case I_ori:
            ori();
            return;
        case I_andi:
            andi();
            return;
        case I_xori:
            xori();
            return;
        case I_slli:
            slli();
            return;
        case I_srli:
            srli();
            return;
        case I_srai:
            srai();
            return;

        case I_sub:
            sub();
            return;
        case I_add:
            add();
            return;
        case I_slt:
            slt();
            return;
        case I_sltu:
            sltu();
            return;
        case I_xor:
            xor();
            return;
        case I_or:
            or();
            return;
        case I_and:
            and();
            return;

        case I_ecall:
            ecall();
            return;

        default:
            return;
    }

}
void step(){
    fetch();
    decode();
    execute();
    breg[ZERO] = 0x00;
}
void run(){
     init();
    while ((pc < DATA_SEGMENT_START) && !stop_prg)
        step();
}

void add(){
    breg[rd]=breg[rs2]+breg[rs1];
}
void addi(){
    breg[rd]=breg[rs1] + get_imm32(get_i_format(opcode, funct3, funct7)); 
}
void and(){
    breg[rd]=breg[rs1] & breg[rs2];
}
void andi(){
    breg[rd]=breg[rs1] & get_imm32(get_i_format(opcode, funct3, funct7));
}
void auipc(){
    breg[rd]= pc + get_imm32(get_i_format(opcode, funct3, funct7))-4;
}
void beq(){
    if(breg[rs1]==breg[rs2]){
        pc+= get_imm32(get_i_format(opcode, funct3, funct7))-4;
    }
}
void bne(){
    if(breg[rs1]!=breg[rs2]){
        pc+= get_imm32(get_i_format(opcode, funct3, funct7))-4;
    }
}
void bge(){
    if(breg[rs1]>=breg[rs2]){
        pc+= get_imm32(get_i_format(opcode, funct3, funct7))-4;
    }
}
void bgeu(){
    if((uint32_t)breg[rs1]>=(uint32_t)breg[rs2]){
        pc+= get_imm32(get_i_format(opcode, funct3, funct7))-4;
    }
}
void blt(){
     if(breg[rs1]<=breg[rs2]){
        pc+= get_imm32(get_i_format(opcode, funct3, funct7))-4;
    }
}
void bltu(){
     if((uint32_t)breg[rs1]<=(uint32_t)breg[rs2]){
        pc+= get_imm32(get_i_format(opcode, funct3, funct7))-4;
    }
}
void jal(){
     if(rd)
        breg[rd] = pc;

    pc += get_imm32(get_i_format(opcode, funct3, funct7)) - 4;
}
void jalr(){
    if(rd)
        breg[rd] = pc;

    pc = breg[rs1] + get_imm32(get_i_format(opcode, funct3, funct7));
}
void or(){
    breg[rd] = breg[rs1] | breg[rs2];
}

void lui(){
     breg[rd] = get_imm32(get_i_format(opcode, funct3, funct7)) & 0xFFFFF000;
}
void slt(){
    if(breg[rs1] < breg[rs2])
        breg[rd] = 1;
    else
        breg[rd] = 0;
}
void sltu(){
    if((uint32_t)breg[rs1] < (uint32_t)breg[rs2])
        breg[rd] = 1;
    else
        breg[rd] = 0;
}
void ori(){
    breg[rd] = breg[rs1] | get_imm32(get_i_format(opcode, funct3, funct7));
}
void slli(){
    breg[rd] = breg[rs1] << get_imm32(get_i_format(opcode, funct3, funct7));
}
void srai(){
    breg[rd] = breg[rs1] >> get_imm32(get_i_format(opcode, funct3, funct7));
}
void srli(){
    breg[rd] = (uint32_t)breg[rs1] >> get_imm32(get_i_format(opcode, funct3, funct7));
}
void sub(){
    breg[rd] = breg[rs1] - breg[rs2];
}
void xor(){
    breg[rd] = breg[rs1] ^ breg[rs2];
}
void xori(){
    breg[rd] = breg[rs1] ^ get_imm32(get_i_format(opcode, funct3, funct7));
}

void ecall(){
     switch (breg[A7]){
    case 1:
        printf("%d", breg[A0]);
        break;
    
    case 4:
        for(int i = breg[A0]; lb(i,0) != '\0'; i++)
            printf("%c", lb(i,0));
        break;
    
    case 10:
        stop_prg = 1;
        break;

    default:
        break;
    }
}


//funções de acesso à memória
int32_t lb(uint32_t address, int32_t kte){
    if ((address+kte)/4 > MEM_SIZE){
		//inválido
		return 0;
	}
	
	char *pos = (char*)mem;

	//(int32_t)pos[(address+kte)]
	return (int32_t)pos[(address+kte)];
}

int32_t lh(uint32_t address, int32_t kte){

	if ((address+kte)/4 > MEM_SIZE){
		//inválido
		return 0;
	}
	
	int16_t *pos = (int16_t*)mem;

	//(int32_t)pos[(address+kte)]
	return (int32_t)pos[(address+kte)];
}



int32_t lw(uint32_t address, int32_t kte){
    if ((address+kte)%4 != 0 || (address+kte)/4 > MEM_SIZE){
		//inválido!
		return 0;
	}

	//mem[(address+kte)/4]

	return mem[(address+kte)/4];
}

int32_t lbu(uint32_t address, int32_t kte){
    if ((address+kte)/4 > MEM_SIZE){
		//inválido
		return 0;
	}
	
	char *pos = (char*)mem;
	int32_t aux = (int32_t)pos[(address+kte)];
	
	if(aux < 0){
		aux = 0xffffffff - aux;
		aux = 0xff - aux; // operação para zerar primeiros bits
		//aux = aux & 0x00000011; // máscara de dados
	}
	return aux;
}

void sb(uint32_t address, int32_t kte, int8_t dado){
    if ((address+kte)/4 > MEM_SIZE){
		//inválido
		return;
	}
	char *pos = (char*)mem;

	pos[(address+kte)] = dado;
	return;
}

void sw(uint32_t address, int32_t kte, int32_t dado){
    if ((address+kte)%4 != 0 || (address+kte)/4 > MEM_SIZE){
		//inválido
		return;
	}

	mem[(address+kte)/4] = dado;

	return;
}