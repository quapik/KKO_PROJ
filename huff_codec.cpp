//Projekt do KKO 2023, VUT FIT, Vojtěch Šíma, xsimav01
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

int main(int argc, char **argv){
    int c;
    string inputFile = "";
    string outputFile = "";
    int widthValue = 0;
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
                widthValue = atoi(optarg);
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


    int size = widthValue*widthValue;
    cout << "Size " <<size <<endl;
    cout << "Filename " << inputFile <<endl;
    vector<uint8_t> buffer(size);
    unsigned char buffer2[size];

    //Open file
    std::ifstream f(inputFile, ios::binary);
    while (!f.eof())
    {
        f.read(reinterpret_cast<char*>(buffer.data()), size);
    }
    f.close(); 
    
    for(int i = 0; i < size; i++)
    {
        uint8_t value = buffer[i]; // Get the value at the current position
        std::cout << "Value: " << static_cast<int>(value) << std::endl;
    }
    


    return 0;
}