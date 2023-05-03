//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//code.h hlavičkový soubor pro code_adapt.cpp 
#include "huff_codec.h"
#define BLOCK_SIZE 8

void Code_adapt(string inputFile, string outputFile, uint16_t widthValue, bool model);
void Code_block(uint8_t* block, string outputFile, uint16_t widthValue, bool first);
void heapwork_adapt(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint32_t ignore[]);