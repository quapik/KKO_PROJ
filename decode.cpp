//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//decode.cpp - soubor pro dekodovaní dat
#include "decode.h"
void Decode(string inputFile, string outputFile, bool model, bool adapt)
{
    //Zjištění velikosti celého souboru, což bude potřeba pro dopočítání velikostí
    std::ifstream findecodesize(inputFile, std::ios::ate | std::ios::binary );
    std::streampos fsize = findecodesize.tellg();
    findecodesize.close();

    uint8_t pocet_ruznych_kodovych_delek = 0;
    std::ifstream findecode(inputFile, ios::binary);

    //Načtení prvního bytu, který nám říká počet různých velikostí délek -> velikost pole s počty na jednotlivých pozicích
    findecode.read(reinterpret_cast<char*>(&pocet_ruznych_kodovych_delek), sizeof(pocet_ruznych_kodovych_delek));
    
    //uint16_t pole_poctydelek[pocet_ruznych_kodovych_delek];
    uint16_t *pole_poctydelek = new uint16_t[pocet_ruznych_kodovych_delek];
    uint16_t pocetznaku = 0;


    //Načtení pole s délkamy slov - index pole říká délku, hodnota počet slov s touto délkou
    findecode.read(reinterpret_cast<char*>(pole_poctydelek), pocet_ruznych_kodovych_delek*2);
    
    //Pole potřeba projít a posčítat hodnoty prvků pro celkový počet znaků v souboru
    for(uint16_t i = 0; i <pocet_ruznych_kodovych_delek; i++)
    {
        pocetznaku = pocetznaku + pole_poctydelek[i];
        cout << +pole_poctydelek[i] << ",";
    }
    cout << endl;
    //Načtení znaků které se vyskytují v souboru a budeou dávány na výstup
    //uint8_t znaky_decode[pocetznaku];
    uint8_t *znaky_decode = new uint8_t[pocetznaku];
    
    findecode.read(reinterpret_cast<char*>(znaky_decode), pocetznaku);
    //Zjištění zbývajících bytů v souboru po odečtění hlavičky
    uint32_t pocet_bytu =  static_cast<uint32_t>(fsize)-(1+pocet_ruznych_kodovych_delek*2+pocetznaku);
    //cout << pocet_bytu;

    //uint8_t decode_bytes[pocet_bytu]= {0};
    uint8_t *decode_bytes = new uint8_t[pocet_bytu];

 
    //Načtení zbytku souboru
    findecode.read(reinterpret_cast<char*>(decode_bytes), pocet_bytu);
    findecode.close(); 

    //Kolik užitečných bitů bude použito v posledním bytu
    uint8_t pocet_uzitecnych_posledni_byte = decode_bytes[pocet_bytu-1];

    //Vytvoření firstCode a firstSymbol dle přednášky
    //uint16_t firstCode[pocet_ruznych_kodovych_delek+1] = {0};
    uint16_t *firstCode = new uint16_t[pocet_ruznych_kodovych_delek+1];
    //uin16_t firstSymbol[pocet_ruznych_kodovych_delek+1] = {0};
    uint16_t *firstSymbol = new uint16_t[pocet_ruznych_kodovych_delek+1];

    uint16_t c = 0;
    uint16_t s = 0;
    for(uint16_t i = 0; i <pocet_ruznych_kodovych_delek+1; i++)
    {
        firstCode[i] = c;
        firstSymbol[i] = s;
        s = s + (pole_poctydelek[i]);
        c = (c+(pole_poctydelek[i]-1)+1)<<1;
    }

    delete[] pole_poctydelek;

    uint8_t byteread;
    //Samotné dekódování
    c = 0;
    uint16_t l = 0;
    bool msb_decode;
    bool first = true;
    uint8_t prev = 0;
    uint8_t diff = 0;
    int counter = 0;
    uint8_t projdibyt = 8;
    //Soubor pro zápis
    ofstream finalout(outputFile, ios::binary | ios::trunc);
        //Dekódování podle přednášky
        // cout << "pocet bytu " << pocet_bytu << endl;
        // cout << "pocet bytu_all " << pocet_bytu + (1+pocet_ruznych_kodovych_delek*2+pocetznaku) << endl;
        for(uint32_t i = 0; i < pocet_bytu - 1; i++)
        {   
            byteread = decode_bytes[i];
            projdibyt = 8;
            //Pokud jsme v posledním bytu, projdi pouze n prvních bitů
            if (i == (pocet_bytu-2)) projdibyt = pocet_uzitecnych_posledni_byte;
            
            for(uint8_t j = 0; j < projdibyt; j++)
            { 
                l = l + 1;
                //Zjisti hodnotu MSB a posuň hodnoty v čteném bytu
                msb_decode = byteread & 0b10000000;
                byteread = byteread << 1;

                c = (c << 1) + msb_decode;
               
                if((c << 1) < firstCode[l + 1 - 1])
                {   
                    //Zapiš hodnotu znaku do souboru
                    if(model)
                    {   
                        if(first)
                        {   
                            prev = znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]];
                            first = false;
                            counter++; 
                            cout << "FIRST " << +prev <<endl;
                        }
                        else
                        {   
                            diff = znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]];
                            //cout << "diff " << +diff << " ";
                            //cout << +prev << " ";
                            prev = prev + diff;
                            //cout << +prev << " " <<endl;
                            counter++; 
                            cout << counter << " ";
                        }
                    //28 30 29 31

                    //finalout.write((char*)&prev, 1);
                    finalout.put(prev);
                    }
                    else
                    {   
                        counter++;
                        finalout.write((char*)&(znaky_decode[firstSymbol[l-1] + c -firstCode[l-1]]), 1);
                    }
                    l = 0;
                    c = 0;
                }
            }  
        }
    cout << "counter" <<counter;
    delete[] znaky_decode;
    delete[] decode_bytes;
    delete[]  firstCode;   
    delete[]  firstSymbol;  
    finalout.close();    
}