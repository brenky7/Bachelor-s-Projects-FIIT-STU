#include <iostream>
#include <list>
#include <string>
#include <chrono>
#include <fstream>
using namespace std;
using namespace chrono;

struct node{
    string name;
    string value;
};


class HashTable{
    public:
        bool Switch;
        int pocet, maxPocet;
        const float maxLoad = 0.75;
        const float minLoad = 0.2;
        list<node>* table;
    HashTable(int maxPocet){
        pocet = 0;
        (this->maxPocet) = maxPocet;
        table = new list<node>[maxPocet];
    }
    void destructHashTable(){
        for (int i = 0; i< maxPocet; i++){
            table[i].clear();
        }
    }
    int hashit(string name, int maxPocet){
        unsigned int offset=1083019949, prime = 8388817;
        unsigned int hash = offset;
        for (int i=0; i < name.length() ;i++){
            unsigned int letter = name[i]; 
            hash = (hash ^ letter);
            hash = (hash * prime);
        }
        return (hash%maxPocet);
    }
    void resizeTable(){
        if (Switch==false){
            maxPocet = (maxPocet<<1);
            list<node>* temp = new list<node>[maxPocet];
            for (int i = 0; i < (maxPocet>>1); i++) {
                for (auto temp2 = (table[i].begin()); (temp2 != (table[i].end())); temp2++) {
                    int tablePlace = hashit((temp2)->name, maxPocet);
                    temp[tablePlace].push_back(*temp2);
                }
            }
            delete[] table;
            table = temp;
        }
        else{
            maxPocet = (maxPocet>>1);
            list<node>* temp = new list<node>[maxPocet];
            for (int i = 0; i < (maxPocet<<1); i++) {
                for (auto temp2 = (table[i].begin()); (temp2 != (table[i].end())); temp2++) {
                    int tablePlace = hashit((temp2)->name, maxPocet);
                    temp[tablePlace].push_back(*temp2);
                }
            }
            delete[] table;
            table = temp;
        }
    }
    void addNode(string name, string value){
        int tablePlace = hashit(name, maxPocet);
        node temp;
        temp.name = name;
        temp.value = value;
        table[tablePlace].push_back(temp);
        pocet++;
        double ratio = pocet ;
        ratio = (ratio/ maxPocet);
        if (ratio >= maxLoad){
            Switch = false; 
            resizeTable();
        }  
    }
    void deleteNode(string name) {
        if (findNode(name)=="No such entry"){
            cout << "No such entry: " << name <<endl;
            return;
        }
        int tablePlace = hashit(name, maxPocet);
        for (auto it = table[tablePlace].begin(); it != table[tablePlace].end(); it++) {
            if (it->name == name) {
                table[tablePlace].erase(it);
                break;
            }
        }
        pocet--;
        double ratio = pocet ;
        ratio = (ratio/maxPocet);
        if (ratio <= minLoad){
            Switch = true;
            resizeTable();
        }
    }
    string findNode(string name){
        int tablePlace = hashit(name, maxPocet);
        for(auto temp = table[tablePlace].begin(); (temp != table[tablePlace].end()); temp++){
            if (name == ((temp)->name)){
                return temp->value;
            }
        }
        return "No such entry";
    }
    void printTable(){
        for (int i=0; i<maxPocet; i++){
            for (auto temp = table[i].begin(); temp!=table[i].end(); temp++){
                cout << ((temp)->name) << "  " << ((temp)->value) << "  ";
            }
            cout << endl;
        }
    }
};



int main(){
    string meno, number;
    char vstup;
    int zastav=0;
    HashTable hash(20);
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
                            hash.deleteNode(meno);
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
                            hash.destructHashTable();
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
                            hash.destructHashTable();
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
                delete[] hash.table;
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