//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//decode_adapt.cpp - soubor pro adaptivní dekodovaní dat
#include "decode_adapt.h"
#include "huff_codec.h"
void Decode_adapt(string inputFile, string outputFile, bool model)
{   
    uint8_t pocetbloku = 64;
    uint32_t widthValue = 512;
    uint8_t *buffer = new uint8_t[widthValue*widthValue];
    uint8_t *block= new uint8_t[64];

    bool first = true;
    uint8_t pocet_bytu = 0;
    uint8_t pocet_ruznych_kodovych_delek = 0;
    ifstream findecode(inputFile, ios::binary);

    for(uint32_t j = 0; j < pocetbloku; j++)
    {
        for(uint32_t i = 0; i < pocetbloku; i++)
        {  
            bool first_block = true;
            //Načtení počtu bytů
            findecode.read(reinterpret_cast<char*>(&pocet_bytu), 1);
            //Načtení prvního bytu, který nám říká počet různých velikostí délek -> velikost pole s počty na jednotlivých pozicích
            findecode.read(reinterpret_cast<char*>(&pocet_ruznych_kodovych_delek), 1);
            uint8_t pole_poctydelek[pocet_ruznych_kodovych_delek]; 
            uint16_t pocetznaku = 0;

            //Načtení pole s délkamy slov - index pole říká délku, hodnota počet slov s touto délkou
            findecode.read(reinterpret_cast<char*>(pole_poctydelek), pocet_ruznych_kodovych_delek);
            //Pole potřeba projít a posčítat hodnoty prvků pro celkový počet znaků v souboru
            for(uint16_t id = 0; id <pocet_ruznych_kodovych_delek; id++)
            {
                pocetznaku = pocetznaku + pole_poctydelek[id];
            }


            //Načtení znaků které se vyskytují v souboru a budeou dávány na výstup
            uint8_t znaky_decode[pocetznaku];
            findecode.read(reinterpret_cast<char*>(znaky_decode), pocetznaku);
  
            //Zjištění zbývajících bytů v souboru po odečtění hlavičky
            uint8_t decode_bytes[pocet_bytu];

            //Načtení zbytku souboru
            findecode.read(reinterpret_cast<char*>(decode_bytes), pocet_bytu);

            //Kolik užitečných bitů bude použito v posledním bytu
            uint8_t pocet_uzitecnych_posledni_byte = decode_bytes[pocet_bytu-1];

            //Vytvoření firstCode a firstSymbol dle přednášky
            uint32_t firstCode[pocet_ruznych_kodovych_delek+1] = {0};
            uint32_t firstSymbol[pocet_ruznych_kodovych_delek+1] = {0};

            uint32_t c = 0;
            uint32_t s = 0;
            for(uint32_t id = 0; id <pocet_ruznych_kodovych_delek+1; id++)
            {
                firstCode[id] = c;
                firstSymbol[id] = s;
                s = s + (pole_poctydelek[id]);
                c = (c+(pole_poctydelek[id]-1)+1)<<1;
            }

            uint8_t byteread;
            //Samotné dekódování
            c = 0;
            uint32_t l = 0;
            bool msb_decode;
            uint8_t prev = 0;
            uint8_t diff = 0;
            uint8_t counter = 0;
            uint8_t projdibyt = 8;

            //Dekódování podle přednášky
            for(uint32_t id = 0; id < pocet_bytu - 1; id++)
            {   
                byteread = decode_bytes[id];
                projdibyt = 8;
                //Pokud jsme v posledním bytu, projdi pouze n prvních bitů
                if (id == (pocet_bytu-2)) projdibyt = pocet_uzitecnych_posledni_byte;
                
                for(uint8_t jd = 0; jd < projdibyt; jd++)
                { 
                    l = l + 1;
                    //Zjisti hodnotu MSB a posuň hodnoty v čteném bytu
                    msb_decode = byteread & 0b10000000;
                    byteread = byteread << 1;

                    c = (c << 1) + msb_decode;
                    
                    if((c << 1) < firstCode[l + 1 - 1])
                    {   
                        if (counter < 64)
                        {
                            //Zapiš hodnotu znaku do bufferu
                            if(model)
                            {   
                                if(first_block)
                                {   
                                    prev = znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]];
                                    first_block = false;
                                }
                                else
                                {   
                                    diff = znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]];
                                    prev = prev + diff;
                                }
                            block[counter] = prev;
                            }
                            else
                            {   
                                block[counter] = znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]];
                            }
                            counter++;
                        }
                        l = 0;
                        c = 0;
                    }
                }  
            }


            for(uint32_t k = 0; k < 8; k++)
            {
                 for(uint32_t l2 = 0; l2 < 8; l2++)
                 {  
                    uint32_t buffindex = j*widthValue*8 + (k*widthValue)+(i*8)+l2;
                    buffer[buffindex] = +block[k*8+l2];
                 }

            }
        }
    }
    ofstream finalout(outputFile, ios::binary | ios::trunc);
    finalout.write(reinterpret_cast<const char*>(buffer), widthValue*widthValue);
    finalout.close();
  
    findecode.close();
    delete[] buffer;
    delete[] block;
    return;
}