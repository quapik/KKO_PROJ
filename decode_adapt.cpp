//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//decode_adapt.cpp - soubor pro adaptivní dekodovaní dat
#include "decode_adapt.h"
#include "huff_codec.h"
void Decode_adapt(string inputFile, string outputFile, bool model)
{   
    
    uint8_t pocetbloku = 64;
    uint32_t widthValue = 512;
    //uint8_t buffer[256] = {0};
    uint8_t *buffer = new uint8_t[widthValue*widthValue];
    uint8_t *block= new uint8_t[64];
   // uint8_t block[8*8] = {0};

    bool first = true;
    uint8_t pocet_bytu = 0;
    uint8_t pocet_ruznych_kodovych_delek = 0;
    ifstream findecode(inputFile, ios::binary);

    int test_counter = 0;
   
    for(uint32_t j = 0; j < pocetbloku; j++)
    {
        for(uint32_t i = 0; i < pocetbloku; i++)
        {  
            bool first_block = true;
            cout << "test counter " << test_counter <<",";
            test_counter++;
            //Načtení počtu bytů
            findecode.read(reinterpret_cast<char*>(&pocet_bytu), 1);
            //Načtení prvního bytu, který nám říká počet různých velikostí délek -> velikost pole s počty na jednotlivých pozicích
            findecode.read(reinterpret_cast<char*>(&pocet_ruznych_kodovych_delek), 1);
            //cout << "POCET RUZNYCH KODOVYCH DELEK " <<pocet_ruznych_kodovych_delek;
            //uint16_t pole_poctydelek[pocet_ruznych_kodovych_delek];
            //uint16_t *pole_poctydelek = new uint16_t[pocet_ruznych_kodovych_delek];
            uint8_t pole_poctydelek[pocet_ruznych_kodovych_delek]; 
            uint16_t pocetznaku = 0;

            //Načtení pole s délkamy slov - index pole říká délku, hodnota počet slov s touto délkou
            findecode.read(reinterpret_cast<char*>(pole_poctydelek), pocet_ruznych_kodovych_delek);
            cout << "POCET ZNAKU COUNTER " << endl;
            //Pole potřeba projít a posčítat hodnoty prvků pro celkový počet znaků v souboru
            for(uint16_t id = 0; id <pocet_ruznych_kodovych_delek; id++)
            {
                pocetznaku = pocetznaku + pole_poctydelek[id];
                cout << +pole_poctydelek[i] << ",";
            }
            cout << endl;
            cout << "pocet znaku celkem " <<+pocetznaku << endl;


            //Načtení znaků které se vyskytují v souboru a budeou dávány na výstup
            //uint8_t *znaky_decode = new uint8_t[pocetznaku];
            uint8_t znaky_decode[pocetznaku];
            //cout << "POCET ZNAKU " << pocetznaku << endl; 
            findecode.read(reinterpret_cast<char*>(znaky_decode), pocetznaku);
            for(uint32_t id = 0; id < sizeof(znaky_decode); id++)
            {
                cout << +znaky_decode[id] << ",";
            }
            cout << endl;
            
            //Zjištění zbývajících bytů v souboru po odečtění hlavičky
            //uint8_t *decode_bytes = new uint8_t[pocet_bytu];
            uint8_t decode_bytes[pocet_bytu];

        
            //Načtení zbytku souboru
            findecode.read(reinterpret_cast<char*>(decode_bytes), pocet_bytu);
            
            cout << endl;
            cout << "DELKY ";
             for(uint32_t id = 0; id < pocet_ruznych_kodovych_delek; id++)
            {
                cout << +pole_poctydelek[id] << ",";
            }
            cout << endl;

            cout << "BYTY final ";
            for(uint32_t id = 0; id < pocet_bytu; id++)
            {
                cout << +decode_bytes[id] << ",";
            }
            cout << endl;

            //Kolik užitečných bitů bude použito v posledním bytu
            uint8_t pocet_uzitecnych_posledni_byte = decode_bytes[pocet_bytu-1];

            //Vytvoření firstCode a firstSymbol dle přednášky
            uint32_t firstCode[pocet_ruznych_kodovych_delek+1] = {0};
            //uint32_t *firstCode = new uint32_t[pocet_ruznych_kodovych_delek+1];
            uint32_t firstSymbol[pocet_ruznych_kodovych_delek+1] = {0};
           // uint32_t *firstSymbol = new uint32_t[pocet_ruznych_kodovych_delek+1];

            uint32_t c = 0;
            uint32_t s = 0;
            for(uint32_t id = 0; id <pocet_ruznych_kodovych_delek+1; id++)
            {
                firstCode[id] = c;
                firstSymbol[id] = s;
                s = s + (pole_poctydelek[id]);
                c = (c+(pole_poctydelek[id]-1)+1)<<1;
            }

            //delete[] pole_poctydelek;

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
            // delete[] znaky_decode;
            // delete[] decode_bytes;
            // delete[] firstCode;   
            // delete[] firstSymbol;  

            for(uint32_t k = 0; k < 8; k++)
            {
                 for(uint32_t l2 = 0; l2 < 8; l2++)
                 {  
                    //buffer[j*widthValue*8+(i*8)+l];
                    //uint32_t buffindex = j*widthValue*8 + (k*widthValue)+(i*8)+l2;
                    uint32_t buffindex = j*widthValue*8 + (k*widthValue)+(i*8)+l2;
                    buffer[buffindex] = +block[k*8+l2];
                    //cout << "index " << buffindex  << " hodnota " << +block[k*8+l2] << " buff " << +buffer[buffindex] << " poradi " << k*8+l2 << endl;
                 }

            }
            // for(uint32_t kw = 0; kw < 8; kw++)
            // {
            //     for(uint32_t lw = 0; lw < 8; lw++)
            //     {
            //         cout  << +block[kw*8+lw] << " ";
            //     }
            //     cout << endl;
            // }
            // cout << endl;
        }
    }
    ofstream finalout(outputFile, ios::binary | ios::trunc);
    finalout.write(reinterpret_cast<const char*>(buffer), widthValue*widthValue);
    finalout.close();
  
    // for(uint32_t a = 0; a < 256; a++)
    // {
    //     cout << +buffer[a]  << ",";
    // }
    findecode.close();
    delete[] buffer;
    delete[] block;
    return;
}