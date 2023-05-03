//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//code.cpp - soubor pro adaptivní kodovaní dat
#include "code_adapt.h"
#include "code.h"
void heapwork_adapt(uint32_t heap[], uint32_t sizehalfheap, uint32_t poradi, uint32_t ignore[]) 
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
    uint32_t ignore_minima[256] = {0};

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
    heapwork_adapt(heap,sizehalfheap, poradi+1, ignore);
}


void Code_block(uint8_t* block, string outputFile, uint16_t widthValue,  bool first)
{
    uint32_t cetnosti[256] = {0};

    //Procházení četností a uložení toho, kolikrát se dané hodnoty 
    for(uint8_t i = 0; i < 64; i++)
    {   
        cetnosti[+block[i]] = cetnosti[+block[i]] + 1 ; 
    }
    uint16_t nenulove_cetnosti = 0;

    //Zjištění kolik je nenulových četnosti (nulové se vynechají)
    for(uint16_t i = 0; i < GRAYSCALE_SIZE; i++)
    {   
        if (cetnosti[i] != 0) nenulove_cetnosti++;
    }

  //Upravení na pouze nenulové hodnoty, uložení indexů kdy index má v sobě odkaz na původní 0-255 hodnotu
    uint32_t cetnosti_beznul[nenulove_cetnosti] = {0};
    uint32_t indexy_beznul[nenulove_cetnosti] = {0};
    uint8_t offset_index = 0;

    for(uint32_t i = 0; i < 256; i++)
    {   
        if (cetnosti[i] != 0)
        {
            cetnosti_beznul[+offset_index] = cetnosti[i];
            indexy_beznul[+offset_index] = i;
            offset_index++;
        }
    }

    uint16_t velikostpole = nenulove_cetnosti;
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

    uint32_t empty[512] = {0};

    //Na heapě udělej všechny přesuny a vypočítej délky
    heapwork_adapt(heap, velikostpole, 0, empty);
    vypocetdelek(heap, velikostpole);

    uint8_t delky[velikostpole*2] = {0};
    uint8_t delky_sorted[velikostpole] = {0};
    uint8_t indexy_sort_delky[velikostpole]  = {0};

    for(uint32_t i = 0; i <velikostpole; i++)
    {
        delky[i] = heap[velikostpole+i];
        delky_sorted[i] = heap[velikostpole+i];
        indexy_sort_delky[i] =  indexy_beznul[i];
    }

    //Seřazení obou polí
    bubble_sort(delky_sorted, indexy_sort_delky, velikostpole);
    uint32_t huffmancode[velikostpole] = {0};
    uint16_t delta = 0;
    uint32_t huff_value = 0;

    // Výpočet jednotlivých kódu podle délek
    huffmancode[0] = huff_value;
    for(int i = 1; i <velikostpole; i++)
    {
        delta = delky_sorted[i] - delky_sorted[i-1];
        huff_value = (huff_value+1)<<delta;
        huffmancode[i] = huff_value;
    }

    uint8_t rozdilne_delky_counter = 0;
    uint32_t rozdilne_delky[velikostpole] = {0};
    uint32_t poctydelek[velikostpole] = {0};

    //Zjištění počtu rozdílnách délek
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
    cout << "rozdilne_delky_counter " << +rozdilne_delky_counter <<endl;
    uint16_t nenulove_delky_counter = 0;
    uint16_t nenulove_suma= 0;
    uint8_t velikosdtpole_poctydelek=0;

    //Procházení tak dlouho, dokud nejsou přečteny délky všech znaků
    for(uint16_t i = 0; i <velikostpole; i++)
    {   
        if(nenulove_suma < velikostpole) velikosdtpole_poctydelek++;
        if(poctydelek[i]!=0)
        {
            nenulove_delky_counter++;
            nenulove_suma = nenulove_suma + poctydelek[i];
        }
    }
    //cout << "velikosdtpole_poctydelek " << +velikosdtpole_poctydelek <<endl;
    
    //Uložení pouze prvních N prvků podle velikosti 

    uint16_t pole_poctydelek[velikosdtpole_poctydelek];
    for(uint16_t i = 0; i <velikosdtpole_poctydelek; i++)
    {
        pole_poctydelek[i] = poctydelek[i];
    }

    if (velikosdtpole_poctydelek==1) pole_poctydelek[0] = 1;

    uint8_t lenght = 0;
    uint8_t lenght_sum = 0;
    uint8_t in = 0;
    bool msb;
    int test = 0;
    //Výpočet toho, jak bude výsledký zápis dlouhý v bytech
    for(uint32_t  i = 0; i< BLOCK_SIZE*BLOCK_SIZE; i++) 
    {      
        in = returnIndex(indexy_sort_delky,block[i],sizeof(indexy_sort_delky));
        lenght = delky_sorted[in];
        lenght_sum += lenght;
    }
    lenght_sum = lenght_sum/8 + 2;
    cout << "lenght_sum "<< +lenght_sum << " velikosdtpole_poctydelek " << +velikosdtpole_poctydelek<< endl;
    
    //Zaručení vyprázdnění souboru v prvním případě
    if(first)
    {
        ofstream fout(outputFile, ios::binary | ios::trunc);
        fout.close();
    }

    ofstream fout(outputFile, ios::binary | ios::app);
    
    fout.write(reinterpret_cast<const char*>(&lenght_sum), sizeof(lenght_sum)); 
    fout.write(reinterpret_cast<const char*>(&velikosdtpole_poctydelek), sizeof(velikosdtpole_poctydelek)); 
    fout.write((char*)&pole_poctydelek, sizeof(pole_poctydelek));
    fout.write((char*)&indexy_sort_delky, sizeof(indexy_sort_delky));
    
    uint8_t bytewrite = 0;
    uint8_t volnebity = 8;

    // cout << " velikosdtpole_poctydelek " << +velikosdtpole_poctydelek << endl;
    // for(uint16_t i = 0; i <velikosdtpole_poctydelek; i++)
    // {
    //     cout << +pole_poctydelek[i] <<",";
    // }
    // cout << endl;
    // cout << "indexy " << endl;
    // for(uint16_t i = 0; i <velikostpole; i++)
    // {
    //     cout << + indexy_sort_delky[i] <<",";
    // }
    // // cout << endl;


    // //Osetreni pro pripad, ze se tam vyskystuje pouze jeden znak (jindy to nefunguje, netusim proc)
    // if (velikosdtpole_poctydelek==1)
    // {
    //     for(uint32_t  i = 0; i < BLOCK_SIZE*BLOCK_SIZE; i++) 
    //     {   
    //         volnebity--;
    //         if (volnebity == 0)
    //         {
    //             fout.write(reinterpret_cast<const char*>(&bytewrite), 1); 
    //             volnebity = 8;
    //         }
    //     }
    //     fout.write(reinterpret_cast<const char*>(&bytewrite), 1);
    //     volnebity = 8 - volnebity;
    //     fout.write(reinterpret_cast<const char*>(&volnebity), 1);
    //     return;
    // }


    //Procházej všechny načtené znaky
    for(uint32_t  i = 0; i< BLOCK_SIZE*BLOCK_SIZE; i++) 
    {      
        //Najdi index, najdi kod a jeho delku
        in = returnIndex(indexy_sort_delky,block[i],sizeof(indexy_sort_delky));
        huff_value = huffmancode[in];
        lenght = delky_sorted[in];
        //cout <<"in " <<+in <<" huffvalue " << +huff_value << " Len " << +lenght << endl;
        //Posuň podle délky užitečné bity doprava
        huff_value = huff_value << (32-lenght);
        for(uint16_t j = 0; j < lenght; j++)
        {
            //Vezmi MSB bit a dej ho na LSB bytu, který jsi předtím posunul
            msb = huff_value & 0x80000000;
            bytewrite = bytewrite << 1;

            if(msb) bytewrite = bytewrite |  0b00000001;
            huff_value = huff_value << 1;
            volnebity--;
            if (volnebity == 0)
            {
                fout.write(reinterpret_cast<const char*>(&bytewrite), 1); 
                volnebity = 8;
                bytewrite  = 0;
            }
        } 
    }
    //V předpoledním bytu posuň bity co nejvíce doprava a do posledního bytu napiš kolik jich je užitečných
    bytewrite = bytewrite << volnebity;
    fout.write(reinterpret_cast<const char*>(&bytewrite), 1);
    volnebity = 8 - volnebity;
    fout.write(reinterpret_cast<const char*>(&volnebity), 1);
    fout.close();

}

void Code_adapt(string inputFile, string outputFile, uint16_t widthValue, bool model)
{
    uint32_t size = widthValue * widthValue ;
    uint8_t *buffer = new uint8_t[size];
    uint8_t *block = new uint8_t[8*8];
    bool first = true;
    
    //Otevři soubor a do bufferu nahraj všechny hodnoty 0-255
    std::ifstream fin(inputFile, ios::binary);
    if (fin.is_open()) 
    {
        fin.read(reinterpret_cast<char*>(buffer), size);
    } 
    else 
    {
        cout << "Nepodarilo se otevrit soubor " << inputFile <<endl;
        return;
    }
    fin.close(); 

    uint32_t pocetbloku = (widthValue / 8)+1;
    uint8_t posledniblok_delka = widthValue - (widthValue / 8) * 8 ;
    cout <<"pocetbloku "  <<  pocetbloku << " posledniblok_delka " << +posledniblok_delka <<endl;
    if (posledniblok_delka==0) pocetbloku = pocetbloku - 1;
    for(uint32_t j = 0; j < pocetbloku; j++)
    {
        for(uint32_t i = 0; i < pocetbloku; i++)
        {
            for(uint32_t k = 0; k < 8; k++)
            {
                 for(uint32_t l = 0; l < 8; l++)
                 {
                    block[k*8+l] = buffer[j*widthValue*(8+k)+(i*8)+l];
                    //cout << "block index " <<  k*8+l <<" all index " << i*widthValue*8 + k*widthValue + j*8 + l<< endl;
                 }

            }
            for(uint32_t kw = 0; kw < BLOCK_SIZE; kw++)
            {
                for(uint32_t lw = 0; lw < BLOCK_SIZE; lw++)
                {
                    cout  << +block[kw*BLOCK_SIZE+lw] << " ";
                }
                cout << endl;

            }
            cout << endl;
            cout << endl;
            Code_block(block,outputFile,widthValue, first);
            first = false;
        }
    }

    delete[] buffer;
    delete[] block;
    return;
}
