//Projekt do KKO, 5.5.2023, VUT FIT, Vojtěch Šíma, xsimav01
//huff_codec.cpp - hlavní soubor co parsuje arumentu a spouští aplikaci
//./huff_codec -c -w 512 -i df1h.raw -o out
//./huff_codec -d -i out -o out_decoded.raw

#include "huff_codec.h"
#include "decode.h"
#include "code.h"

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
    if(comp) 
    {
        cout << "CODE\n";
        Code(inputFile, outputFile, widthValue);
    }
    if(decomp) 
    {
        cout << "DECODE\n";
        Decode(inputFile, outputFile);
    }
    return 0;
}