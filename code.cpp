#include "code.h"

//Funkce která vrací true pokud se hodnota vyskytuje v poli
bool isin(uint16_t elem, const uint16_t* arr, size_t size) 
{
    for (size_t i = 0; i < size; i++) 
    {
        if (arr[i] == elem) 
        {
            return true;
        }
    }
    return false;
}

void heapwork(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint16_t ignore[]) 
{
    //Ukonči tvoření heapu ve správnou chvíli
    if (sizehalfheap-1-poradi < 1)
    {
        return;
    }
    //Ulož si ze seřazené heapy nejmenší prvek a index, kam se bude později ukládat výsledek
    uint32_t extracted_min = heap[heap[0]];
    uint16_t heapinddex = sizehalfheap-poradi-1;

    //Sestav pole, kde budou přidána minima co se mají v ignorovat kvůli správnému sestavení heapu
    uint16_t ignore_minima[256] = {0};

    //Index prvku, který je aktuálním minimem se bude ignorovat a místo minima se vloží index rodiče
    ignore[poradi*2] = heap[0];
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
//Funkce co vypočítá jednotlivé délky na heapu
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
}
//Bubble sort pro řazení dvou polí současně - potřeba zachovat k délkám i indexy
void bubble_sort (uint8_t delky_sorted[], uint8_t indexy_sort_delky[], uint16_t pocethodnot) 
{
    int temp;
    for ( int i = 0; i < pocethodnot; i++ ) 
    {
        for ( int j = 0; j + 1 < pocethodnot - i; j++ ) 
        {
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
//Funkce co vrátí index v poli
uint8_t returnIndex(uint8_t arr[], uint8_t val, uint16_t len)
{
    for(uint16_t i = 0; i <len; i++)
    {
        if(arr[i] == val)
        {
            return i;
        }
    }
    return 0;
}

void Code(string inputFile, string outputFile, uint16_t widthValue)
{
    uint32_t size = widthValue *widthValue ;
    cout << "Size " <<size <<endl;
    cout << "Filename " << inputFile <<endl;
    uint8_t buffer[size];
    uint32_t cetnosti[256] = {0};
    
    //Otevři soubor a do bufferu nahraj všechny hodnoty 0-255
    std::ifstream fin(inputFile, ios::binary);
    while (!fin.eof())
    {
        fin.read(reinterpret_cast<char*>(buffer), size);
    }
    fin.close(); 

    //Procházení četností a uložení toho, kolikrát se dané hodnoty 
    for(uint32_t i = 0; i < size; i++)
    {
        cetnosti[+buffer[i]] = cetnosti[+buffer[i]] + 1 ; 
    }
    uint16_t nenulove_cetnosti = 0;

    //Zjištění kolik je nenulových četnosti (nulové se vynechají)
    for(uint32_t i = 0; i < 256; i++)
    {
        if (cetnosti[i] != 0) nenulove_cetnosti++;
    }


    //Upravení na pouze nenulové hodnoty, uložení indexů kdy index má v sobě odkaz na původní 0-255 hodnotu
    uint32_t cetnosti_beznul[nenulove_cetnosti] = {0};
    uint32_t indexy_beznul[nenulove_cetnosti] = {0};
    uint8_t offset_index = 0;
    cout << "CETTNOSTI ";
    for(uint32_t i = 0; i < 256; i++)
    {   
        cout << cetnosti[i] << ",";
        if (cetnosti[i] != 0)
        {
            cetnosti_beznul[+offset_index] = cetnosti[i];
            indexy_beznul[+offset_index] = i;
            offset_index++;
        }
    }
    cout << endl;
    // uint32_t velikostpole = 7;
    // uint32_t cet_puvodni[velikostpole] = {25,20,13,17,9,11,5}; //{15,12,7,10,5,6,2};
    // uint32_t cet_sort[velikostpole] = {0};
    // uint32_t heap[velikostpole*2] = {0};

    uint16_t velikostpole = nenulove_cetnosti;
    //uint32_t cetnosti[velikostpole] = {10,1,15,7};
    uint32_t cet_sort[velikostpole] = {0};
    uint32_t heap[velikostpole*2] = {0};

    //Uložení četností do druhé poloviny heapu a kopírování pole pro řazení
    for (uint32_t i = 0; i < velikostpole; i++)
    {
            heap[velikostpole+i] =  cetnosti_beznul[i];
            cet_sort[i] =  cetnosti_beznul[i];
    }

    sort(cet_sort, cet_sort + velikostpole);
    //Najdi vždy ity nejmenší prvek, zjisti jeho index a ten ulož na itou pozici v heapu
    for(int i = 0; i < velikostpole; i++)
    {
        uint32_t smallest = cet_sort[i];
        auto index_p = find(cetnosti_beznul, cetnosti_beznul + velikostpole, smallest);
        uint32_t index_value = distance(cetnosti_beznul, index_p);
        heap[i] = index_value+velikostpole;
    }

    uint16_t empty[512] = {0};
    heapwork(heap, velikostpole, 0, empty);
    vypocetdelek(heap, velikostpole);
    uint8_t delky[velikostpole] = {0};
    uint8_t delky_sorted[velikostpole] = {0};
    uint8_t indexy_sort_delky[velikostpole]  = {0};
    for(uint32_t i = 0; i <velikostpole; i++)
    {
        delky[i] = heap[velikostpole+i];
        delky_sorted[i] = heap[velikostpole+i];
        indexy_sort_delky[i] =  indexy_beznul[i];
    }
    bubble_sort(delky_sorted, indexy_sort_delky, velikostpole);

    uint32_t huffmancode[velikostpole] = {0};
    uint16_t delta = 0;
    uint32_t huff_value = 0;
    //cout << indexy_sort_delky[0] << "VALUE " << huff_value <<endl;
    huffmancode[0] = huff_value;
    cout << "DELKY"<<endl;
    for(int i = 0; i <velikostpole; i++)
    {
        cout << +delky_sorted[i] << ",";
    }
    cout << endl;

    for(int i = 1; i <velikostpole; i++)
    {
        delta = delky_sorted[i] - delky_sorted[i-1];
        //cout << "DELTA " << delta << " ";
        huff_value = (huff_value+1)<<delta;
        huffmancode[i] = huff_value;
        //cout << +indexy_sort_delky[i] << " VALUE " << huff_value <<endl;
    }

    uint8_t rozdilne_delky_counter = 0;
    uint16_t rozdilne_delky[velikostpole] = {0};
    uint16_t poctydelek[velikostpole] = {0};
    uint16_t buffer_zacatek_souboru[1 + velikostpole*2] {0};

    //TODO TOTO VYPADA ZE VUBEC NEPOTREBUJES?
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

    uint16_t nenulove_delky_counter = 0;
    uint16_t nenulove_suma= 0;
    uint8_t velikosdtpole_poctydelek=0;
    cout << endl;
    for(uint16_t i = 0; i <velikostpole; i++)
    {   
        cout << poctydelek[i] << ",";
        if(nenulove_suma < velikostpole) velikosdtpole_poctydelek++;

        if(poctydelek[i]!=0){
            nenulove_delky_counter++;
            nenulove_suma = nenulove_suma + poctydelek[i];
        }
    }
    cout << endl;

    uint16_t pole_poctydelek[velikosdtpole_poctydelek] = {0};
    for(uint16_t i = 0; i <velikosdtpole_poctydelek; i++)
    {
        pole_poctydelek[i] = poctydelek[i];
        cout << +pole_poctydelek[i] << ",,";
    }
    cout << endl;
    cout << endl;
    cout << "VELIKOST POLE " <<velikostpole<<endl;
    cout << "neunlove "<<nenulove_delky_counter<<endl;
    cout << "neunlove sumy "<<nenulove_suma<<endl;
    cout << "rozdilne delky "<<+rozdilne_delky_counter<<endl;
    cout << "velikosdtpole_poctydelek "<<+velikosdtpole_poctydelek<<endl;

    ofstream fout("out", ios::binary | ios::trunc);
    fout.write(reinterpret_cast<const char*>(&velikosdtpole_poctydelek), sizeof(velikosdtpole_poctydelek)); 
    fout.write((char*)&pole_poctydelek, sizeof(pole_poctydelek));
    fout.write((char*)&indexy_sort_delky, sizeof(indexy_sort_delky));

    uint8_t bytewrite = 0;
    uint8_t volnebity = 8;
    uint8_t lenght =0;
    uint8_t in = 0;
    bool msb;

    for(uint32_t  i = 0; i< size; i++) //size
    {
        //cout << +buffer[i];
        in = returnIndex(indexy_sort_delky,buffer[i],sizeof(indexy_sort_delky));
        //cout << "in " << +in << endl;
        huff_value = huffmancode[in];
        lenght = delky_sorted[in];
        //std::bitset<32> binary_representation(huff_value);
        //cout << " " << binary_representation << " ";
        huff_value = huff_value << (32-lenght);
        //std::bitset<32> binary_representation2(huff_value);
        //cout << " " << binary_representation2 << " ";
        //cout << " " << huff_value  << "  "<<+lenght << endl;
        for(uint16_t j = 0; j < lenght; j++)
        {
            if(volnebity > 0)
            {   
                msb = huff_value & 0x80000000;
                std::bitset<8> binary_representation(bytewrite);
                std::bitset<32> binary_representation2(huff_value);
                bytewrite = bytewrite << 1;

                if(msb)
                {
                    bytewrite = bytewrite |  0b00000001;
                }
                huff_value = huff_value << 1;
                volnebity--;
                if (volnebity == 0)
                {
                    //cout << " WRITE!!";
                    fout.write(reinterpret_cast<const char*>(&bytewrite), 1); 
                    volnebity = 8;
                    bytewrite  = 0;
                }
                
            }
        } 
    }
    cout << "VOLNE BITY" << +volnebity << endl;
    bytewrite = bytewrite << volnebity;
    std::bitset<8> binary_representation3(bytewrite);
    cout << binary_representation3;
    fout.write(reinterpret_cast<const char*>(&bytewrite), 1);
    volnebity = 8 - volnebity; 
    fout.write(reinterpret_cast<const char*>(&volnebity), 1);
    fout.close();
    
}