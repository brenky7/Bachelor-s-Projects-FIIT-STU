#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

string generateRandomNumber(int dlzka) {
    string charset = "0123456789";
    string result = "";
    int charsetLength = charset.length();
    for (int i = 0; i < dlzka; i++) {
        int index = rand() % charsetLength;
        result += charset[index];
    }
    return result;
}

int main() {
    srand(time(NULL)); 
    int pocet = 10000000; 
    int dlzka = 8; 
    ofstream outfile("NUMBERS_10M.txt"); 
    for (int i = 0; i < pocet; i++) {
        string randomString = generateRandomNumber(dlzka);
        outfile << randomString << endl; 
    }
    outfile.close(); 
    return 0;
}
