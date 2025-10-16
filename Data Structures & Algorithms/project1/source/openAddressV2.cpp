#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <chrono>
using namespace std;
using namespace chrono;

const float maxLoad = 0.75;
const float minLoad = 0.2;
const float chokePoint = 0.3;

class node{
    public:
        string name;
        string value;
    node(string inputName, string inputValue){
        this->name = inputName;
        this->value = inputValue;
    }
    node (){
        name = "deleted";
    }
};

class HashTable {
    public:
        int tableSize;
        int tableChokePoint;
        int elementCount; 
        node** table;
    HashTable(int inputSize){
        tableSize = inputSize;
        table = new node*[tableSize];
        for (int i = 0; i < inputSize; i++){
            table[i] = nullptr;
        }
        elementCount = 0;
        double temp = (chokePoint*tableSize);
        tableChokePoint = round(temp);
    }
    node** destructHashTable(){
        for (int i = 0; i < tableSize; i++){
            delete table[i];
        }
        delete[] table;
        table = new node*[tableSize];
        for (int i = 0; i < tableSize; i++){
            table[i] = nullptr;
        }
        return table;
    }
    int hashit(string name){
        unsigned int offset=8388817, prime = 4194419;
        unsigned int hash = offset;
        for (int i=0; i < name.length() ;i++){
            unsigned int letter = name[i]; 
            hash = (hash ^ letter);
            hash = (hash * prime);
        }
        return (hash);
    }
    void upScaleTable(){
        int newTableSize = (tableSize*2);
        node** tempTable = new node*[newTableSize];
        for (int i = 0; i < newTableSize; i++){
            tempTable[i] = nullptr;
        }
        for (int i = 0; i < tableSize; i++){
            if ((table[i]!=nullptr)&&((table[i]->name) != "deleted")){
                unsigned int index = hashit(table[i]->name);
                if (tempTable[(index%newTableSize)]==nullptr){
                    tempTable[(index%newTableSize)] = new node(table[i]->name, table[i]->value);
                }
                else{
                    unsigned int j = 1;
                    while (tempTable[(index%newTableSize)]!=nullptr){
                        index = (index + (j*j));
                        j++;
                        if ( j >  chokePoint){
                            unsigned int k = 1;
                            while (tempTable[(index%newTableSize)]!=nullptr){
                                index  = (index + k);
                                k++;
                            }    
                        }       
                    }
                    tempTable[(index%newTableSize)]= new node(table[i]->name, table[i]->value);
                }
            }
        }
        delete[] table;
        table = tempTable;
        tableSize = newTableSize;
        return;
    }
    void downScaleTable(){
        int newTableSize = (tableSize/2);
        node** tempTable = new node*[newTableSize];
        for (int i = 0; i < newTableSize; i++){
            tempTable[i] = nullptr;
        }
        for (int i = 0; i < tableSize; i++){
            if ((table[i]!=nullptr)&&((table[i]->name) != "deleted")){
                unsigned int index = hashit(table[i]->name);
                if (tempTable[(index%newTableSize)]==nullptr){
                    tempTable[(index%newTableSize)] = new node(table[i]->name, table[i]->value);
                }
                else{
                    unsigned int j = 1;
                    while (tempTable[(index%newTableSize)]!=nullptr){
                        index = (index + (j*j));
                        j++;
                        if ( j >  chokePoint){
                            unsigned int k = 1;
                            while (tempTable[(index%newTableSize)]!=nullptr){
                                index  = (index + k);
                                k++;
                            }    
                        }       
                    }
                    tempTable[(index%newTableSize)]= new node(table[i]->name, table[i]->value);
                }
            }
        }
        delete[] table;
        table = tempTable;
        tableSize = newTableSize;
        return;
    }
    void addNode(string name, string value){
        unsigned int index = hashit(name);
        if (table[(index%tableSize)]==nullptr){
            table[(index%tableSize)] = new node(name, value);
        }
        else{
            unsigned int j = 1;
            while (table[(index%tableSize)]!=nullptr){
                index = (index + (j*j));
                j++;
                if ( j >  chokePoint){
                    unsigned int k = 1;
                    while (table[(index%tableSize)]!=nullptr){
                        index  = (index + k);
                        k++;
                    }    
                }
            }
            table[(index%tableSize)]= new node(name, value);
        }
        elementCount++;
        double ratio = elementCount ;
        ratio = (ratio / tableSize);
        if (ratio >= maxLoad){
            //cout << "scaling up\n";
            upScaleTable();
        }
        return;
    }
    string findNode(string name){
        unsigned int index = hashit(name);
        unsigned int j = 1;
        if ((table[(index%tableSize)]!=nullptr)&&(table[(index%tableSize)]->name==name)){
            return table[(index%tableSize)]->value;
        }
        else{
            while ((table[(index%tableSize)]!=nullptr)&&(table[(index%tableSize)]->name!=name)){
                index = (index + (j*j));
                j++;
                if ( j >  chokePoint){
                    unsigned int k = 1;
                    while ((table[(index%tableSize)]!=nullptr)&&(table[(index%tableSize)]->name!=name)){
                        index  = (index + k);
                        k++;
                    }    
                }
            }
        }
        if ((table[(index%tableSize)] != nullptr)&&(table[(index%tableSize)]->name == name)){
            return table[(index%tableSize)]->value;
        }
        else{
            return "No such entry";
        }
    }
    bool deleteNode(string name){
        unsigned int index = hashit(name);
        unsigned int j = 1;
        if ((table[(index%tableSize)] != nullptr)&&(table[(index%tableSize)]->name==name)){
            delete table[(index%tableSize)];
            table[(index%tableSize)] = new node();
            elementCount--;
            double ratio = elementCount;
            ratio = (ratio / tableSize);
            if (ratio <= minLoad){
                //cout << "scaledown\n";
                downScaleTable();
            }
            return true;
        }
        else{
            while ((table[(index%tableSize)] != nullptr)&&(table[(index%tableSize)]->name!=name)){
                index = (index + (j*j));
                j++;
                if ( j >  chokePoint){
                    unsigned int k = 1;
                    while ((table[(index%tableSize)]!=nullptr)&&(table[(index%tableSize)]->name!=name)){
                        index  = (index + k);
                        k++;
                    } 
                }
            }
        }
        if ((table[(index%tableSize)] != nullptr)&&(table[(index%tableSize)]->name == name)){
            delete table[(index%tableSize)];
            table[(index%tableSize)] = new node();
            elementCount--;
            double ratio = elementCount;
            ratio = (ratio / tableSize);
            if (ratio <= minLoad){
                //cout << "scaledown\n";
                downScaleTable();
            }
            return true;
        }
        else{
            return false;
        }
    }
    void printTable(){
        for(int i = 0; i < tableSize; i++){
            if (table[i] != nullptr){
                cout << table[i]->name << "  " << table[i]->value << endl;
            }
        }
    }
};



int main(){
    string meno, number;
    char vstup;
    int zastav=0, size;
    ifstream numero10("NUMBERS_10.txt");
    ifstream numero100("NUMBERS_100.txt");
    ifstream numero1k("NUMBERS_1k.txt");
    ifstream numero10k("NUMBERS_10k.txt");
    ifstream numero100k("NUMBERS_100k.txt");
    ifstream numero1M("NUMBERS_1M.txt");
    ifstream numero10M("NUMBERS_10M.txt");
    ifstream gringo10("NAMES_10.txt");
    ifstream gringo100("NAMES_100.txt");
    ifstream gringo1k("NAMES_1k.txt");
    ifstream gringo10k("NAMES_10k.txt");
    ifstream gringo100k("NAMES_100k.txt");
    ifstream gringo1M("NAMES_1M.txt");
    ifstream gringo10M("NAMES_10M.txt");
    while (true){
        cout << "m->MANUAL/a->AUTO/q->QUIT\n";
        cin >> vstup;
        switch(vstup){
            case 'm' :{
                cout << "Enter table size\n";
                cin >> size;
                HashTable hash(size);
                zastav=0;
                cout << "a->ADD/d->DELETE/s->SEARCH/w->WRITEOUT/q->QUIT\n";
                while(zastav==0){
                    cin >> vstup;
                    switch(vstup){
                        case 'a' :{
                            cout << "Enter string \n";
                            cin >> meno;
                            cout << "Enter data \n";
                            cin >> number;
                            hash.addNode(meno, number);
                            break;
                        }
                        case 'd' :{
                            cout << "Enter string \n";
                            cin >> meno;
                            if (hash.deleteNode(meno) == false){
                                cout << "No such node\n";
                            }
                            break;
                        }
                        case 's' :{
                            cout << "Enter string \n";
                            cin >> meno;
                            cout << hash.findNode(meno) << endl;
                            break;
                        }
                        case 'w' :{
                            hash.printTable();
                            break;
                        }
                        case 'q' :{
                            hash.table = hash.destructHashTable();
                            zastav=1;
                            break;
                        }
                        default :{
                            cout << "a->ADD/d->DELETE/s->SEARCH/w->WRITEOUT/q->QUIT\n";
                            break;
                        }
                    }
                }
                break;
            }
            case 'a' :{
                HashTable hash(20);
                zastav=0;
                cout << "a->10/b->100/c->1K/d->10K/e->100K/f->1M/g->10M/q->QUIT\n";
                while(zastav==0){
                    cin >> vstup;
                    switch(vstup){
                        case 'a' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 10; i++){
                                numero10 >> number;
                                gringo10 >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo10.seekg(0);
                            for (int i = 0; i< 10; i++){
                                gringo10 >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo10.seekg(0);
                            for (int i = 0; i< 10; i++){
                                gringo10 >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<microseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " microseconds." << endl;
                            auto search = duration_cast<microseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " microseconds." << endl;
                            auto deletion = duration_cast<microseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " microseconds." << endl;
                            auto duration = duration_cast<microseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " microseconds." << endl;
                            gringo10.seekg(0);
                            numero10.seekg(0);
                            break;
                        }
                        case 'b' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 100; i++){
                                numero100 >> number;
                                gringo100 >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo100.seekg(0);
                            for (int i = 0; i< 100; i++){
                                gringo100 >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo100.seekg(0);
                            for (int i = 0; i< 100; i++){
                                gringo100 >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<microseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " microseconds." << endl;
                            auto search = duration_cast<microseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " microseconds." << endl;
                            auto deletion = duration_cast<microseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " microseconds." << endl;
                            auto duration = duration_cast<microseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " microseconds." << endl;
                            gringo100.seekg(0);
                            numero100.seekg(0);
                            break;
                        }
                        case 'c' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 1000; i++){
                                numero1k >> number;
                                gringo1k >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo1k.seekg(0);
                            for (int i = 0; i< 1000; i++){
                                gringo1k >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo1k.seekg(0);
                            for (int i = 0; i< 1000; i++){
                                gringo1k >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<microseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " microseconds." << endl;
                            auto search = duration_cast<microseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " microseconds." << endl;
                            auto deletion = duration_cast<microseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " microseconds." << endl;
                            auto duration = duration_cast<microseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " microseconds." << endl;
                            gringo1k.seekg(0);
                            numero1k.seekg(0);
                            break;
                            
                        }
                        case 'd' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 10000; i++){
                                numero10k >> number;
                                gringo10k >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo10k.seekg(0);
                            for (int i = 0; i< 10000; i++){
                                gringo10k >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo10k.seekg(0);
                            for (int i = 0; i< 10000; i++){
                                gringo10k >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<microseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " microseconds." << endl;
                            auto search = duration_cast<microseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " microseconds." << endl;
                            auto deletion = duration_cast<microseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " microseconds." << endl;
                            auto duration = duration_cast<microseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " microseconds." << endl;
                            gringo10k.seekg(0);
                            numero10k.seekg(0);
                            break;
                        }
                        case 'e' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 100000; i++){
                                numero100k >> number;
                                gringo100k >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo100k.seekg(0);
                            for (int i = 0; i< 100000; i++){
                                gringo100k >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo100k.seekg(0);
                            for (int i = 0; i< 100000; i++){
                                gringo100k >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<microseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " microseconds." << endl;
                            auto search = duration_cast<microseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " microseconds." << endl;
                            auto deletion = duration_cast<microseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " microseconds." << endl;
                            auto duration = duration_cast<microseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " microseconds." << endl;
                            gringo100k.seekg(0);
                            numero100k.seekg(0);
                            break;
                        }
                        case 'f' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 1000000; i++){
                                numero1M >> number;
                                gringo1M >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo1M.seekg(0);
                            for (int i = 0; i< 1000000; i++){
                                gringo1M >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo1M.seekg(0);
                            for (int i = 0; i< 1000000; i++){
                                gringo1M >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<milliseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " milliseconds." << endl;
                            auto search = duration_cast<milliseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " milliseconds." << endl;
                            auto deletion = duration_cast<milliseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " milliseconds." << endl;
                            auto duration = duration_cast<milliseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " milliseconds." << endl;
                            gringo1M.seekg(0);
                            numero1M.seekg(0);
                            break; 
                        }
                        case 'g' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 10000000; i++){
                                numero10M >> number;
                                gringo10M >> meno;
                                hash.addNode(meno, number);
                            }
                            auto insertTime = high_resolution_clock::now();
                            gringo10M.seekg(0);
                            for (int i = 0; i< 10000000; i++){
                                gringo10M >> meno;
                                hash.findNode(meno);
                            }
                            auto searchTime = high_resolution_clock::now();
                            gringo10M.seekg(0);
                            for (int i = 0; i< 10000000; i++){
                                gringo10M >> meno;
                                hash.deleteNode(meno);
                            }
                            auto endTime = high_resolution_clock::now();
                            auto insertion = duration_cast<milliseconds>(insertTime - startTime).count();
                            cout << "Insert took: " << insertion << " milliseconds." << endl;
                            auto search = duration_cast<milliseconds>(searchTime - insertTime).count();
                            cout << "Search took: " << search << " milliseconds." << endl;
                            auto deletion = duration_cast<milliseconds>(endTime - searchTime).count();
                            cout << "Delete took: " << deletion << " milliseconds." << endl;
                            auto duration = duration_cast<milliseconds>(endTime - startTime).count();
                            cout << "Total time taken: " << duration << " milliseconds." << endl;
                            gringo10M.seekg(0);
                            numero10M.seekg(0);
                            break; 
                        }
                        case 'q' :{
                            hash.table = hash.destructHashTable();
                            zastav=1;
                            break; 
                        }
                        default :{
                            cout << "a->10/b->100/c->1K/d->10K/e->100K/f->1M/g->10M/q->QUIT\n";
                            break;
                        }
                    }
                }
                break;
            }
            case 'q' :{
                return 0;
            }
            default :{
                cout << "m->MANUAL/A->AUTO/q->QUIT\n";
                break;
            }
        }
    }
    return 0;
}

