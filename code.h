//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//code.h hlavičkový soubor pro code.cpp 
#include "huff_codec.h"
bool isin(uint16_t elem, const uint16_t* arr, size_t size);
void heapwork(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint16_t ignore[]);
void vypocetdelek(uint32_t heap[], uint32_t sizehalfheap);
void bubble_sort (uint8_t delky_sorted[], uint8_t indexy_sort_delky[], uint16_t pocethodnot);
uint8_t returnIndex(uint8_t arr[], uint8_t val, uint16_t len);
void Code(string inputFile, string outputFile, uint16_t widthValue);