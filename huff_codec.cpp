//Projekt do KKO 2023, VUT FIT, Vojtěch Šíma, xsimav01
//./huff_codec -c -w 512 -i df1h.raw -o out.txt

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdint> 
#include <algorithm>
#include <limits.h>
using namespace std;

void heapwork(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint16_t ignore[]) {

   uint32_t extracted_min = heap[heap[0]];
   uint32_t extracted_min2 = heap[heap[1]];
   uint16_t heapinddex = sizehalfheap-poradi-1;
   ignore[poradi*2] = heap[0];
   ignore[poradi*2+1] = heap[1];
   extracted_min = extracted_min + extracted_min2;
   heap[sizehalfheap-poradi-1] = extracted_min;
   heap[heap[0]] = heapinddex;
   heap[heap[1]] = heapinddex;
   heap[0] = heapinddex;

   extracted_min  = INT_MAX;
   for(uint16_t i = 0; i <sizehalfheap-1-poradi; i++)
   {    
        for(uint16_t j = sizehalfheap-1-poradi; j < sizehalfheap*2; j++)
        {
            if(heap[j] < extracted_min)
            {
            uint16_t *index = find(ignore, ignore + 512, heap[j]);
            uint16_t *end = ignore + 512;
            if (index == end)
            {
                extracted_min = heap[j];
                heapinddex = j;
            }
            }
        }
        heap[i] = heapinddex;
   }
}

// uint32_t findmin(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint32_t ignore[], uint32_t ignorerazeni[])
// {
//     for(int i = sizehalfheap-1-poradi; < sizehalfheap*2; i++)
//     {
        
//     }
// }

int main(int argc, char **argv){
    int c;
    string inputFile = "";
    string outputFile = "";
    uint16_t widthValue = 0;
    bool comp = false;
    bool decomp = false;
    bool model = false;
    bool adapt = false;
    while ((c = getopt(argc, argv, "cdmai:o:w:h")) != -1) 
    {
        switch (c) 
        {
            case 'c':
                comp = true;
                break;
            case 'd':
                decomp = true;
                break;
            case 'm':
                model = true;
                break;
            case 'a':
                adapt = true;
                break;
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'w':
                widthValue = (uint16_t)atoi(optarg);
                break; 
            case 'h':
                cout << "-c volba režimu, aplikace bude vstupní soubor komprimovat" << endl;
                cout << "-d volba režimu, aplikace bude vstupní soubor dekoprimovat " << endl;
                cout << "-m aktivuje model (viz bod b) zadání) pro předzpracování vstupních dat " << endl;
                cout << "-a aktivuje režim adaptivního skenování obrazu (viz bod b) zadání); jinak je použito sekvenční skenování v horizontálním směru (tzn tak, jak jsou za sebou uložena data) " << endl;
                cout << "-i <ifile> název vstupního souboru. Dle režimu činnosti aplikace, viz přepínače –c/-d výše, se jedná buď o data určená ke kompresi nebo dekompresi "<< endl;
                cout << "-o <ofile> název výstupního souboru. Dle režimu činnosti aplikace, viz přepínače –c/-d výše, se jedná buď o zkomprimovaná či dekomprimovaná data " << endl;
                cout << "-w <width_value> udává šířku obrazu, přičemž platí vztah width_value >= 1. Parametr se zadává pouze v případě komprese, aby aplikace věděla, jak široký obraz je. Pro potřeby dekomprese je nutné uložit všechny potřebné informace do výstupního souboru" << endl;
                cout << "-h vypíše nápovědu na standardní výstup a ukončí se" << endl;
                return 0;
                break; 
        }
     }
    if (!comp && !decomp && !model && !adapt) 
    {
        cout << "Nebyl zadán žádný z přepínačů -c -d -m -a" << endl;
        return 0;
    }

    if (inputFile == "" || outputFile == "")
    {
        if (inputFile == "")  cout << "Nezadán soubor pro vstup" <<endl;
        else  cout << "Nezadán soubor pro výstup" <<endl;
        return 0;
    }

    if ((comp) && (widthValue < 1))
    {
        cout << "Chybně zadaný argument -w" <<endl;
        return 0;
    }

    uint32_t size = widthValue *widthValue ;
   
    cout << "Size " <<size <<endl;
    cout << "Filename " << inputFile <<endl;

    uint8_t buffer[size];
    uint32_t cetnosti[256] = {0};
    float pravdepodobnosti[256] = {0.0};
    //Open file

    /*
    std::ifstream fin(inputFile, ios::binary);
    while (!fin.eof())
    {
        fin.read(reinterpret_cast<char*>(buffer), size);
    }
    fin.close(); 

    // ZAPIS DO SOUBORU 
    // ofstream fout(outputFile, ios::binary | ios::trunc);
    // if (!fout.is_open()) {
    //     cout << "Failed to open file for writing!" << endl;
    //     return 1;
    // }
    // fout.write(reinterpret_cast<char*>(buffer), size);
    // fout.close();


    int q = 0;
    int sum = 0;
    for(uint32_t i = 0; i < size; i++)
    {
        cetnosti[+buffer[i]] = cetnosti[+buffer[i]] + 1 ; 
        q++;
        sum = sum + (+buffer[i]);
    }

    uint32_t test = 0;
    float fl = 0.0;
    for(uint16_t i = 0; i < 256; i++)
    {
        // std::cout << i << " " << cetnosti[i] << std::endl;
        // cout << i <<"  "<< +test << " + " << (cetnosti[i]);
        // test = test +  (cetnosti[i]);
        // cout <<" = " <<test<<endl;
        pravdepodobnosti[i] =  cetnosti[i] / (float)size;
        fl = fl + pravdepodobnosti[i];
        cout << pravdepodobnosti[i] << endl;
    }
    cout << "f  " << fl;
    //cout << "\navg " << sum/size;

    */

    uint32_t sizetest = 7;
    uint32_t cet_puvodni[sizetest] = {25,20,13,17,9,11,5};
    uint32_t cet_sort[sizetest] = {0};
    uint32_t heap[sizetest*2] = {0};

    //Uložení četností do druhé poloviny heapu a kopírování pole pro řazení
    for (uint32_t i = 0; i < sizetest; i++)
    {
            heap[sizetest+i] = cet_puvodni[i];
            cet_sort[i] = cet_puvodni[i];
    }

    sort(cet_sort, cet_sort + sizetest);
    
    //Najdi vždy ity nejmenší prvek, zjisti jeho index a ten ulož na itou pozici v heapu
    for(int i = 0; i < sizetest; i++)
    {
        uint32_t smallest = cet_sort[i];
        auto index_p = find(cet_puvodni,cet_puvodni + sizetest, smallest);
        uint32_t index_value = distance(cet_puvodni, index_p);
        heap[i] = index_value+sizetest;
    }

    for(uint32_t i = 0; i < sizetest*2; i++)
    {
        cout << heap[i] << ",";
    }
    cout <<endl;

    uint16_t empty[512] = {0};
    heapwork(heap, sizetest, 0, empty);
    cout << "FCE\n";
    for(uint32_t i = 0; i < sizetest*2; i++)
    {
        cout << heap[i] << ",";
    }
    cout <<endl;


    return 0;
}




