#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

struct Node{
    int key;
    int height;
    string name;
    Node *lower, *higher;
}*root;

class AVLTree{
    public:    
        AVLTree(){
            root = NULL;
        }
        ~AVLTree(){
            root = destructTree(&root);
        }
        Node* destructTree(Node** node){
            if (*(node)==NULL){
                return NULL;
            } 
            destructTree(&(*node)->lower);
            destructTree(&(*node)->higher);
            delete *(node);
            return NULL;
        }
        int getHeight(Node* node){
            if (node==NULL){
                return 0;
            }
            else return (node->height);
        }
        int getBalance(Node* node){
            if (node==NULL) {
                return 0;
            }
            else return (getHeight(node->lower) - getHeight(node->higher));
        }
        void updateHeight(Node* node){
            if (node!=NULL){
                node->height = (max(getHeight(node->lower), getHeight(node->higher)) + 1);
            }
        }
        void printTree(Node* node, int depth = 0){
            if (node == NULL){
                return;
            }
            printTree(node->higher, depth + 1);
            for (int i = 0; i < depth; i++){
                cout << "    ";
            }
            cout << node->key << " (" << node->name << ")" << endl;
            printTree(node->lower, depth + 1);
        }
        Node* leftRotate(Node* node){
            //cout <<"rotL\n";
            Node* temp = node->higher;
            Node* temp2 = temp->lower;
            temp->lower = node;
            node->higher = temp2;
            updateHeight(node);
            updateHeight(temp);
            return temp;
        }
        Node* rightRotate(Node* node){
            //cout <<"rotR\n";
            Node* temp = node->lower;
            Node* temp2 = temp->higher;
            temp->higher = node;
            node->lower = temp2;
            updateHeight(node);
            updateHeight(temp);
            return temp;
        }
        Node *balanceNode(Node *node){
            if (node == NULL){
                return node;
            }
            int bf = getBalance(node);
            if (bf > 1){
                if (getBalance(node->lower) < 0){
                    node->lower = leftRotate(node->lower);
                }
                return rightRotate(node);
            }
            else if (bf < -1){
                if (getBalance(node->higher) > 0){
                    node->higher = rightRotate(node->higher);
                }
                return leftRotate(node);
            }
            return node;
        }
        Node* addNode(Node* node, int value, string meno){
        if (!(node)){
            node =  new Node;
            node->lower = NULL;
            node->higher = NULL;
            node->name = meno;
            node->key = value;
        }
        else if (value>((node)->key)){
            (node)->higher = addNode(((node)->higher), value, meno);
        }
        else if (value<((node)->key)){
            (node)->lower = addNode(((node)->lower), value, meno);
        }
        else{
            return (node);
        }
        updateHeight(node);
        return balanceNode(node);
        }
        Node* deleteNode(Node* node, int value){
        if (!node){
            return (node);
        }
        if (value < (node->key)){
            (node->lower) = deleteNode((node->lower), value);
        } 
        else if (value > (((node)->key))){
            (node->higher) = deleteNode((node->higher), value);
        } 
        else{
            if (((node->lower) == NULL)&&((node->higher) == NULL)){
                delete (node);
                node=NULL;
            }
            else if (((node->lower) == NULL)&&((node->higher) != NULL)){
                Node* temp = node;
                node = node->higher;
                delete temp;
                temp = NULL;
                return node;
            } 
            else if (((node->higher) == NULL)&&((node->lower) != NULL)){
                Node* temp = node;
                node = (node->lower);
                delete temp;
                temp = NULL;
                return node;
            }
            else{
                Node* temp = (node->higher);
                while ((temp->lower)!=NULL){
                    temp=(temp->lower);
                }
                (node)->key = (temp->key);
                (node->higher) = deleteNode((node->higher), (temp->key));
            }
        }
        updateHeight(node);
        return balanceNode(node);
    }
        string findNode(Node* node, int value){
            if (!node){
                return "No such node";
            }        
            if ((node->key)==value){
                return node->name;
            }
            if ((value < node->key)&&((node->lower)!=NULL)){
                return findNode(node->lower, value);
            } 
            else if ((value > node->key)&&((node->higher)!=NULL)){
                return findNode(node->higher, value);
            }
            else{
                return "No such node";
            }
        }
};

int main(){
    int number, zastav=0;
    char vstup;
    string meno;
    AVLTree *myTree = NULL;
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
                            cout << "Enter number \n";
                            cin >> number;
                            cout << "Enter data \n";
                            cin >> meno;
                            root = myTree->addNode(root, number, meno);
                            break;
                        }
                        case 'd' :{
                            cout << "Enter number \n";
                            cin >> number;
                            root = myTree->deleteNode(root, number);
                            break;
                        }
                        case 's' :{
                            cout << "Enter number \n";
                            cin >> number;
                            cout << myTree->findNode(root, number) << endl;
                            break;
                        }
                        case 'w' :{
                            myTree->printTree(root);
                            break;
                        }
                        case 'q' :{
                            root = myTree->destructTree(&root);
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
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero10.seekg(0);
                            for (int i = 0; i< 10; i++){
                                numero10 >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero10.seekg(0);
                            for (int i = 0; i< 10; i++){
                                numero10 >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero10.seekg(0);
                            gringo10.seekg(0);
                            break;
                        }
                        case 'b' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 100; i++){
                                numero100 >> number;
                                gringo100 >> meno;
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero100.seekg(0);
                            for (int i = 0; i< 100; i++){
                                numero100 >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero100.seekg(0);
                            for (int i = 0; i< 100; i++){
                                numero100 >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero100.seekg(0);
                            gringo100.seekg(0);
                            break;
                        }
                        case 'c' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 1000; i++){
                                numero1k >> number;
                                gringo1k >> meno;
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero1k.seekg(0);
                            for (int i = 0; i< 1000; i++){
                                numero1k >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero1k.seekg(0);
                            for (int i = 0; i< 1000; i++){
                                numero1k >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero1k.seekg(0);
                            gringo1k.seekg(0);
                            break;
                            
                        }
                        case 'd' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 10000; i++){
                                numero10k >> number;
                                gringo10k >> meno;
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero10k.seekg(0);
                            for (int i = 0; i< 10000; i++){
                                numero10k >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero10k.seekg(0);
                            for (int i = 0; i< 10000; i++){
                                numero10k >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero10k.seekg(0);
                            gringo10k.seekg(0);
                            break;
                        }
                        case 'e' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 100000; i++){
                                numero100k >> number;
                                gringo100k >> meno;
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero100k.seekg(0);
                            for (int i = 0; i< 100000; i++){
                                numero100k >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero100k.seekg(0);
                            for (int i = 0; i< 100000; i++){
                                numero100k >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero100k.seekg(0);
                            gringo100k.seekg(0);
                            break;
                        }
                        case 'f' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 1000000; i++){
                                numero1M >> number;
                                gringo1M >> meno;
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero1M.seekg(0);
                            for (int i = 0; i< 1000000; i++){
                                numero1M >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero1M.seekg(0);
                            for (int i = 0; i< 1000000; i++){
                                numero1M >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero1M.seekg(0);
                            gringo1M.seekg(0);
                            break; 
                        }
                        case 'g' :{
                            auto startTime = high_resolution_clock::now();
                            for (int i = 0; i< 10000000; i++){
                                numero10M >> number;
                                gringo10M >> meno;
                                root = myTree->addNode(root, number, meno);
                            }
                            auto insertTime = high_resolution_clock::now();
                            numero10M.seekg(0);
                            for (int i = 0; i< 10000000; i++){
                                numero10M >> number;
                                myTree->findNode(root, number);
                            }
                            auto searchTime = high_resolution_clock::now();
                            numero10M.seekg(0);
                            for (int i = 0; i< 10000000; i++){
                                numero10M >> number;
                                root = myTree->deleteNode(root, number);
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
                            numero10M.seekg(0);
                            gringo10M.seekg(0);
                            break; 
                        }
                        case 'q' :{
                            root = myTree->destructTree(&root);
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