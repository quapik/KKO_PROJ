//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//decode_adapt.cpp - soubor pro adaptivní dekodovaní dat
#include "decode_adapt.h"
#include "huff_codec.h"
void Decode_adapt(string inputFile, string outputFile, bool model)
{   
    uint8_t buffer[16*16] = {0};
    uint8_t block[8*8];
    bool first = true;
    uint32_t pocet_bytu = 0;
    uint8_t pocet_ruznych_kodovych_delek = 0;
    std::ifstream findecode(inputFile, ios::binary);
    uint8_t pocetbloku = 2;
    uint16_t widthValue = 16;
    for(uint32_t j = 0; j < pocetbloku; j++)
    {
        for(uint32_t i = 0; i < pocetbloku; i++)
        { 
            //Načtení počtu bytů
            findecode.read(reinterpret_cast<char*>(&pocet_bytu), 1);
             cout <<"pocet_bytu " <<pocet_bytu << endl;
            //Načtení prvního bytu, který nám říká počet různých velikostí délek -> velikost pole s počty na jednotlivých pozicích
            findecode.read(reinterpret_cast<char*>(&pocet_ruznych_kodovych_delek), sizeof(pocet_ruznych_kodovych_delek));
            
            //uint16_t pole_poctydelek[pocet_ruznych_kodovych_delek];
            uint16_t *pole_poctydelek = new uint16_t[pocet_ruznych_kodovych_delek];
            uint16_t pocetznaku = 0;

            //Načtení pole s délkamy slov - index pole říká délku, hodnota počet slov s touto délkou
            findecode.read(reinterpret_cast<char*>(pole_poctydelek), pocet_ruznych_kodovych_delek*2);
            
            //Pole potřeba projít a posčítat hodnoty prvků pro celkový počet znaků v souboru
            for(uint16_t id = 0; id <pocet_ruznych_kodovych_delek; id++)
            {
                pocetznaku = pocetznaku + pole_poctydelek[id];
                //cout << +pole_poctydelek[i] << ",";
            }
            //cout << endl;
            //Načtení znaků které se vyskytují v souboru a budeou dávány na výstup
            uint8_t *znaky_decode = new uint8_t[pocetznaku];
            
            findecode.read(reinterpret_cast<char*>(znaky_decode), pocetznaku);
            //Zjištění zbývajících bytů v souboru po odečtění hlavičky
            uint8_t *decode_bytes = new uint8_t[pocet_bytu];

        
            //Načtení zbytku souboru
            findecode.read(reinterpret_cast<char*>(decode_bytes), pocet_bytu);

            //Kolik užitečných bitů bude použito v posledním bytu
            uint8_t pocet_uzitecnych_posledni_byte = decode_bytes[pocet_bytu-1];

            //Vytvoření firstCode a firstSymbol dle přednášky
            //uint16_t firstCode[pocet_ruznych_kodovych_delek+1] = {0};
            uint16_t *firstCode = new uint16_t[pocet_ruznych_kodovych_delek+1];
            //uin16_t firstSymbol[pocet_ruznych_kodovych_delek+1] = {0};
            uint16_t *firstSymbol = new uint16_t[pocet_ruznych_kodovych_delek+1];

            uint16_t c = 0;
            uint16_t s = 0;
            for(uint16_t id = 0; id <pocet_ruznych_kodovych_delek+1; id++)
            {
                firstCode[id] = c;
                firstSymbol[id] = s;
                s = s + (pole_poctydelek[id]);
                c = (c+(pole_poctydelek[id]-1)+1)<<1;
            }

            delete[] pole_poctydelek;

            uint8_t byteread;
            //Samotné dekódování
            c = 0;
            uint16_t l = 0;
            bool msb_decode;
            uint8_t prev = 0;
            uint8_t diff = 0;
            int counter = 0;
            uint8_t projdibyt = 8;
            //Soubor pro zápis
            
            if(first)
            {
                ofstream finalout(outputFile, ios::binary | ios::trunc);
                finalout.close();
            }
            ofstream finalout(outputFile, ios::binary | ios::app);

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
                        //Zapiš hodnotu znaku do souboru
                        
                        //finalout.write((char*)&(znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]]), 1);
                        //first = false;
                        block[counter] = znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]];
                        //cout <<  +znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]] << "  ";
                        //cout << counter<<endl;
                        counter++;
                        l = 0;
                        c = 0;
                    }
                }  
            }
           
            delete[] znaky_decode;
            delete[] decode_bytes;
            delete[] firstCode;   
            delete[] firstSymbol;  

            for(uint32_t k = 0; k < 8; k++)
            {
                 for(uint32_t l2 = 0; l2 < 8; l2++)
                 {
                    //buffer[j*widthValue*(8+k)+(i*8)+l] = block[k*8+l];
                    buffer[j*widthValue*8 + (k*widthValue)+(i*8)+l2] = block[k*8+l2];
                    cout << "index " << j*widthValue*8 + (k*widthValue)+(i*8)+l2 << " hodnota " << +block[k*8+l2] << " poradi " << k*8+l << endl;
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
    //ofstream finalout(outputFile, ios::binary | ios::trunc);
    //finalout.write(reinterpret_cast<const char*>(buffer), sizeof(buffer));
    findecode.close();
    for(int a = 0; a < 256; a++)
    {
        cout << +buffer[a] << ",";
    }
    cout << endl;
    // //Načtení počtu bytů
    // findecode.read(reinterpret_cast<char*>(&pocet_bytu), sizeof(pocet_bytu));
    // //Načtení prvního bytu, který nám říká počet různých velikostí délek -> velikost pole s počty na jednotlivých pozicích
    // findecode.read(reinterpret_cast<char*>(&pocet_ruznych_kodovych_delek), sizeof(pocet_ruznych_kodovych_delek));
    
    // //uint16_t pole_poctydelek[pocet_ruznych_kodovych_delek];
    // uint16_t *pole_poctydelek = new uint16_t[pocet_ruznych_kodovych_delek];
    // uint16_t pocetznaku = 0;


    // //Načtení pole s délkamy slov - index pole říká délku, hodnota počet slov s touto délkou
    // findecode.read(reinterpret_cast<char*>(pole_poctydelek), pocet_ruznych_kodovych_delek*2);
    
    // //Pole potřeba projít a posčítat hodnoty prvků pro celkový počet znaků v souboru
    // for(uint16_t i = 0; i <pocet_ruznych_kodovych_delek; i++)
    // {
    //     pocetznaku = pocetznaku + pole_poctydelek[i];
    //     //cout << +pole_poctydelek[i] << ",";
    // }
    // //cout << endl;
    // //Načtení znaků které se vyskytují v souboru a budeou dávány na výstup
    // uint8_t *znaky_decode = new uint8_t[pocetznaku];
    
    // findecode.read(reinterpret_cast<char*>(znaky_decode), pocetznaku);
    // //Zjištění zbývajících bytů v souboru po odečtění hlavičky
    // uint8_t *decode_bytes = new uint8_t[pocet_bytu];

 
    // //Načtení zbytku souboru
    // findecode.read(reinterpret_cast<char*>(decode_bytes), pocet_bytu);

    // //Kolik užitečných bitů bude použito v posledním bytu
    // uint8_t pocet_uzitecnych_posledni_byte = decode_bytes[pocet_bytu-1];

    // //Vytvoření firstCode a firstSymbol dle přednášky
    // //uint16_t firstCode[pocet_ruznych_kodovych_delek+1] = {0};
    // uint16_t *firstCode = new uint16_t[pocet_ruznych_kodovych_delek+1];
    // //uin16_t firstSymbol[pocet_ruznych_kodovych_delek+1] = {0};
    // uint16_t *firstSymbol = new uint16_t[pocet_ruznych_kodovych_delek+1];

    // uint16_t c = 0;
    // uint16_t s = 0;
    // for(uint16_t i = 0; i <pocet_ruznych_kodovych_delek+1; i++)
    // {
    //     firstCode[i] = c;
    //     firstSymbol[i] = s;
    //     s = s + (pole_poctydelek[i]);
    //     c = (c+(pole_poctydelek[i]-1)+1)<<1;
    // }

    // delete[] pole_poctydelek;

    // uint8_t byteread;
    // //Samotné dekódování
    // c = 0;
    // uint16_t l = 0;
    // bool msb_decode;
    // uint8_t prev = 0;
    // uint8_t diff = 0;
    // int counter = 0;
    // uint8_t projdibyt = 8;
    // //Soubor pro zápis
    
    // if(first)
    // {
    //     ofstream finalout(outputFile, ios::binary | ios::trunc);
    //     finalout.close();
    // }
    // ofstream finalout(outputFile, ios::binary | ios::app);

    // //Dekódování podle přednášky
    // for(uint32_t i = 0; i < pocet_bytu - 1; i++)
    // {   
    //     byteread = decode_bytes[i];
    //     projdibyt = 8;
    //     //Pokud jsme v posledním bytu, projdi pouze n prvních bitů
    //     if (i == (pocet_bytu-2)) projdibyt = pocet_uzitecnych_posledni_byte;
        
    //     for(uint8_t j = 0; j < projdibyt; j++)
    //     { 
    //         l = l + 1;
    //         //Zjisti hodnotu MSB a posuň hodnoty v čteném bytu
    //         msb_decode = byteread & 0b10000000;
    //         byteread = byteread << 1;

    //         c = (c << 1) + msb_decode;
            
    //         if((c << 1) < firstCode[l + 1 - 1])
    //         {   
    //             //Zapiš hodnotu znaku do souboru
    //             counter++;
    //             finalout.write((char*)&(znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]]), 1);
    //             first = false;
    //             l = 0;
    //             c = 0;
    //         }
    //     }  
    // }
    // delete[] znaky_decode;
    // delete[] decode_bytes;
    // delete[]  firstCode;   
    // delete[]  firstSymbol;   
}