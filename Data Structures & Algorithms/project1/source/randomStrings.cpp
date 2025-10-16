#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(time(NULL)); 
    const int numStrings = 10000000; 
    const int stringLength = 8;
    ofstream outFile("NAMES_10M.txt");
    for (int i = 0; i < numStrings; i++) {
        string randomString = "";
        for (int j = 0; j < stringLength; j++) {
            randomString += ('a' + rand() % 26); 
        }
        outFile << randomString << endl;
    }
    outFile.close();
    return 0;
}
