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

bool isin(uint16_t elem, const uint16_t* arr, size_t size) 
{
    for (size_t i = 0; i < size; i++) {
        if (arr[i] == elem) {
            return true;
        }
    }
    return false;
}

void heapwork(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint16_t ignore[]) {

    //Ukonči tvoření heapu ve správnou chvíli
    if (sizehalfheap-1-poradi < 1)
    {
        // cout << "FINAL  ";
        // for(uint32_t i = 0; i < sizehalfheap*2; i++)
        // {
        //     cout << heap[i] << ",";
        //     if (i == sizehalfheap-1) cout << "   ";
        // }
        // cout <<endl;
        return;
    }
    //Ulož si ze seřazené heapy nejmenší prvek a index, kam se bude později ukládat výsledek
    uint32_t extracted_min = heap[heap[0]];
    uint16_t heapinddex = sizehalfheap-poradi-1;

    //Sestav pole, kde budou přidána minima co se mají v ignorovat kvůli správnému sestavení heapu
    uint16_t ignore_minima[256] = {0};

    //Index prvku, který je aktuálním minimem se bude ignorovat a místo minima se vloží index rodiče
    ignore[poradi*2] = heap[0];
    //heap[heapinddex] = INT_MAX; TODO TOTO ASI SMAž
    heap[heap[0]] = heapinddex;

    uint32_t find_min;

    //Hledání minim a uložení indexů na ně do první poloviny heapu
    for(uint16_t i = 0; i <sizehalfheap-1-poradi; i++)
    {    
        find_min  = INT_MAX;
        //Procházení prvků bez toho aktuálního, kam bude později ukládáno (může být ignorován)
        for(uint16_t j = sizehalfheap-1-poradi+1; j < sizehalfheap*2; j++)
        {
            if(heap[j] < find_min)
            {   
                //Pokud už nebylo minimum použito pro řazení / není přepsáno indexy - beru ho
                bool isin1 = isin(j, ignore, 512);
                bool isin2 = isin(j, ignore_minima, 256);
                if (!isin1 && !isin2)
                {
                    find_min = heap[j];
                    heapinddex = j;
                    //cout << "nalezeno minimum " << heap[j] <<  " index " << j << endl;
                }
            }
        }
        //Uložení indexu na itý nejmenší prvek postupně do první poloviny heapu a uložení indexu pro ignorování
        heap[i] = heapinddex;
        ignore_minima[i] = heapinddex;
    }
    //Hledání dalšího nejnižšího prvku (musela být předělána heapa, proto podobný kod podruhý)
    heapinddex = sizehalfheap-poradi-1;
    ignore[poradi*2+1] = heap[0];
    uint32_t extracted_min2 = heap[heap[0]];
    fill(ignore_minima, ignore_minima + 256, 0);

    //Sečtení dvou minim, uložení na konec heapy, odkaz na toto minimum na původní místo druhého minima a také na první místo heapy
    extracted_min = extracted_min + extracted_min2;
    heap[heapinddex] = extracted_min;
    heap[heap[0]] = heapinddex;
    heap[0] = heapinddex;

    for(uint16_t i = 0; i <sizehalfheap-1-poradi; i++)
    {    
        find_min  = INT_MAX;
        for(uint16_t j = sizehalfheap-1-poradi; j < sizehalfheap*2; j++)
        {
            if(heap[j] < find_min)
            {
                bool isin1 = isin(j, ignore, 512);
                bool isin2 = isin(j, ignore_minima, 256);
                if (!isin1 && !isin2)
                {
                    find_min = heap[j];
                    heapinddex = j;   
                }
            }
        }
        heap[i] = heapinddex;
        ignore_minima[i] = heapinddex;
    }
    //Rekurzivní volání a sestavení finální heap    
    heapwork(heap,sizehalfheap, poradi+1, ignore);
}

void vypocetdelek(uint32_t heap[], uint32_t sizehalfheap)
{
    uint32_t root = heap[1];

    for(int i = (sizehalfheap*2)-1; i > sizehalfheap-1; i--)
    {
        uint16_t delka = 1;
        uint16_t index = heap[i];
        while(index != 1)
        {   
            index = heap[index];
            delka++;
        }
    heap[i] = delka;
    }

    // cout << endl;
    // cout << "DELKY  ";
    // for(uint32_t i = 0; i < sizehalfheap*2; i++)
    // {
    //     cout << heap[i] << ",";
    //     if (i == sizehalfheap-1) cout << "   ";
    // }
    // cout <<endl;
}

void bubble_sort (uint32_t delky_sorted[], uint32_t indexy_sort_delky[], uint32_t pocethodnot) 
{
    int temp;
    for ( int i = 0; i < pocethodnot; i++ ) 
        {
            for ( int j = 0; j + 1 < pocethodnot - i; j++ ) 
            {
                // Záměna prvků, pokud je první
                // je větší než druhý.
                if ( delky_sorted[ j ] > delky_sorted [ j + 1 ])  
                {
                    temp = delky_sorted [ j ] ;
                    delky_sorted [ j ] = delky_sorted [ j + 1 ] ;
                    delky_sorted [ j + 1 ] = temp;

                    temp = indexy_sort_delky [ j ] ;
                    indexy_sort_delky [ j ] = indexy_sort_delky [ j + 1 ] ;
                    indexy_sort_delky [ j + 1 ] = temp;
                }
            }
        }
}

int main(int argc, char **argv){
    int argument;
    string inputFile = "";
    string outputFile = "";
    uint16_t widthValue = 0;
    bool comp = false;
    bool decomp = false;
    bool model = false;
    bool adapt = false;
    while ((argument = getopt(argc, argv, "cdmai:o:w:h")) != -1) 
    {
        switch (argument) 
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

    // uint8_t buffer[size];
    // uint32_t cetnosti[256] = {0};
    // float pravdepodobnosti[256] = {0.0};
    
    // std::ifstream fin(inputFile, ios::binary);
    // while (!fin.eof())
    // {
    //     fin.read(reinterpret_cast<char*>(buffer), size);
    // }
    // fin.close(); 

    // ZAPIS DO SOUBORU 
    // ofstream fout(outputFile, ios::binary | ios::trunc);
    // if (!fout.is_open()) {
    //     cout << "Failed to open file for writing!" << endl;
    //     return 1;
    // }
    // fout.write(reinterpret_cast<char*>(buffer), size);
    // fout.close();

    // for(uint32_t i = 0; i < size; i++)
    // {
    //     cetnosti[+buffer[i]] = cetnosti[+buffer[i]] + 1 ; 
    // }

    // uint32_t velikostpole = 7;
    // uint32_t cet_puvodni[velikostpole] = {25,20,13,17,9,11,5}; //{15,12,7,10,5,6,2};
    // uint32_t cet_sort[velikostpole] = {0};
    // uint32_t heap[velikostpole*2] = {0};

    uint32_t velikostpole = 4;
    uint32_t cetnosti[velikostpole] = {10,1,15,7};
    uint32_t cet_sort[velikostpole] = {0};
    uint32_t heap[velikostpole*2] = {0};

    //Uložení četností do druhé poloviny heapu a kopírování pole pro řazení
    for (uint32_t i = 0; i < velikostpole; i++)
    {
            heap[velikostpole+i] = cetnosti[i];
            cet_sort[i] = cetnosti[i];
    }

    sort(cet_sort, cet_sort + velikostpole);
    //Najdi vždy ity nejmenší prvek, zjisti jeho index a ten ulož na itou pozici v heapu
    for(int i = 0; i < velikostpole; i++)
    {
        uint32_t smallest = cet_sort[i];
        auto index_p = find(cetnosti,cetnosti + velikostpole, smallest);
        uint32_t index_value = distance(cetnosti, index_p);
        heap[i] = index_value+velikostpole;
    }
    // cout << endl;
    // cout << "BEGIN  ";
    // for(uint32_t i = 0; i < velikostpole*2; i++)
    // {
    //     cout << heap[i] << ",";
    //     if (i == velikostpole-1) cout << "   ";
    // }
    // cout <<endl;

    uint16_t empty[512] = {0};
    heapwork(heap, velikostpole, 0, empty);
    vypocetdelek(heap, velikostpole);
    uint32_t delky[velikostpole] = {0};
    uint32_t delky_sorted[velikostpole] = {0};
    uint32_t indexy_sort_delky[velikostpole]  = {0};
    for(uint32_t i = 0; i <velikostpole; i++)
    {
        delky[i] = heap[velikostpole+i];
        delky_sorted[i] = heap[velikostpole+i];
        indexy_sort_delky[i] = i;
    }
    bubble_sort(delky_sorted, indexy_sort_delky, velikostpole);

    uint32_t huffmancode[velikostpole] = {0};
    uint16_t delta = 0;
    uint32_t huff_value = 0;
    //cout << indexy_sort_delky[0] << "VALUE " << huff_value <<endl;
    huffmancode[0] = huff_value;

    for(int i = 1; i <velikostpole; i++)
    {
        delta = delky_sorted[i] - delky_sorted[i-1];
        huff_value = (huff_value+1)<<delta;
        huffmancode[i] = huff_value;
        //cout << indexy_sort_delky[i] << "VALUE " << huff_value <<endl;
    }

    uint16_t rozdilne_delky_counter = 0;
    uint16_t rozdilne_delky[velikostpole] = {0};
    uint16_t poctydelek[velikostpole] = {0};
    uint16_t buffer_zacatek_souboru[1 + velikostpole*2] {0};
    for(uint32_t i = 0; i <velikostpole; i++)
    {
        bool isdelkain = isin(delky_sorted[i], rozdilne_delky, velikostpole);
        poctydelek[delky_sorted[i]-1]++;
        if(!isdelkain)
        {
            rozdilne_delky_counter++;
            rozdilne_delky[i] = delky_sorted[i]; 
        }

    }
    buffer_zacatek_souboru[0] = rozdilne_delky_counter;
    for(uint32_t i = 0; i <velikostpole; i++)
    {   
        buffer_zacatek_souboru[1+i] = poctydelek[i];
        buffer_zacatek_souboru[1+i+velikostpole] = indexy_sort_delky[i];
    }

    //DECODE
    cout << "\nDECODE\n";
    uint16_t decode_N = buffer_zacatek_souboru[0];
    
    //CREATE FIRST SYMBOL AND FIRST CODE
    uint16_t firstCode[decode_N+1] = {0};
    uint16_t firstSymbol[decode_N+1] = {0};

    int c = 0;
    int s = 0;
    for(int i = 0; i <decode_N+1; i++)
    {
        firstCode[i] = c;
        firstSymbol[i] = s;
        s = s + (buffer_zacatek_souboru[i+1]);
        c = (c+(buffer_zacatek_souboru[i+1]-1)+1)<<1;
    }
    int testinpuit = 0b110110011110;

    
    //DECODE function
    uint16_t cd = 0;
    uint16_t l = 0;
      for(int i = 0; i<12; i++)
      {
        l = l+1;
        bool  bit = testinpuit & (1 << (12 - i - 1));
        cd = (cd<<1) + bit;

        if((cd<<1)<firstCode[l+1-1])
        {
            //TOTO JSOU INDEXY na to co ukazuje při tom poslání
            cout << firstSymbol[l-1] + cd -firstCode[l-1] << ",";
            l =0;
            cd= 0;
        }

      }


    return 0;

}