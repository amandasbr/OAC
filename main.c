/*
Universidade de Brasília
Organização e Arquitetura de Computadores
Professor: Ricardo Jacobi
2023/1

Data: 14/05/2023
Amanda Soares Bispo Reis - 200072790

Trabalho simulador RV
*/

#include <stdio.h>
#include <stdint.h>
#include "RV_header.h"

#define CODE_FILE_NAME "test_text.bin"
#define DATA_FILE_NAME "test_data.bin"


const uint32_t DATA_SEGMENT_START = 0X2000;
const uint32_t CODE_SEGMENT_START = 0X00;

int main(){

    load_mem(CODE_FILE_NAME, DATA_FILE_NAME);
    
    run();

    return 0;
}

