#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>
using namespace std;
using namespace chrono;

struct Node{
    int key;
    string name;
    Node *lower, *higher, *parent;
}*root;

class SplayTree{
    public:     
        SplayTree(){
            root = nullptr;
        }
        ~SplayTree(){
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
    Node* newNode(int key, string name){
        Node* node = new Node;
        node->key = key;
        node->name = name;
        node->lower = nullptr;
        node->higher = nullptr;
        node->parent = nullptr;
        return node;
    }
    Node* rightRotate(Node* myNode){
        Node* temp = myNode->lower;
        myNode->lower = temp->higher;
        if(temp->higher != nullptr){
            temp->higher->parent = myNode;
        }
        temp->parent = myNode->parent;
        if (myNode->parent == nullptr){
            root = temp;
        }
        else if(myNode == myNode->parent->lower){
            myNode->parent->lower = temp;
        }
        else if (myNode == myNode->parent->higher){
            myNode->parent->higher = temp;
        }
        if (temp != nullptr){
            temp->higher = myNode;
            myNode->parent = temp;
        }
        return temp;
    }
    Node* leftRotate(Node* myNode){
        Node* temp = myNode->higher;
        myNode->higher = temp->lower;
        if (temp->lower != nullptr){
            temp->lower->parent = myNode;
        }
        temp->parent = myNode->parent;
        if (myNode->parent == nullptr){
            root = temp;
        }
        else if(myNode == myNode->parent->lower){
            myNode->parent->lower = temp;
        }
        else if(myNode == myNode->parent->higher){
            myNode->parent->higher = temp;
        }
        if (temp != nullptr){
            temp->lower = myNode;
            myNode->parent = temp;
        }
        return temp;
    }
    Node* splayNode(Node* myNode, Node* rootNode){
        if (myNode->parent == rootNode){
            return myNode;
        }
        while(myNode->parent != rootNode){
            Node* grandParent = myNode->parent->parent;
            Node* Parent = myNode->parent;
            if (grandParent == rootNode){
                if (myNode == Parent->lower){
                    rightRotate(Parent);
                }
                else if (myNode == Parent->higher){
                    leftRotate(Parent);
                }   
            }
            else if((myNode == Parent->lower)&&(Parent == grandParent->lower)){
                grandParent = rightRotate(grandParent);
                Parent = rightRotate(Parent);
            }
            else if((myNode == Parent->higher)&&(Parent == grandParent->higher)){
                grandParent = leftRotate(grandParent);
                Parent = leftRotate(Parent);
            }
            else if((myNode == Parent->lower)&&(Parent == grandParent->higher)){
                Parent = rightRotate(Parent);
               grandParent = leftRotate(grandParent);
            }
            else if((myNode == Parent->higher)&&(Parent == grandParent->lower)){
                Parent = leftRotate(Parent);
                grandParent = rightRotate(grandParent);
            }
        }
        return myNode;
    }
    Node* addNode(Node* myNode, int key, string name){
        if (myNode == nullptr){
            root = newNode(key, name);
            return root;
        }
        Node* temp = myNode;
        while(true){
            if(temp->key == key){
                splayNode(temp, nullptr);
                return root;
            }
            else if(temp->key > key){
                if(temp->lower == nullptr){
                    temp->lower = newNode(key, name);
                    temp->lower->parent = temp;
                    root = splayNode(temp->lower, nullptr);
                    return root;
                }
                else{
                    temp = temp->lower;
                }
            }
            else if(temp->key < key){
                if (temp->higher == nullptr){
                    temp->higher = newNode(key, name);
                    temp->higher->parent = temp;
                    root = splayNode(temp->higher, nullptr);
                    return root;
                }
                else {
                    temp = temp->higher;
                }
            }
        }
    }
    string findNode(Node* myNode, int key){
        if (!myNode){
            return "No such node\n";
        }
        if (myNode->key == key){
            splayNode(myNode, nullptr);
            return myNode->name;
        }
        else if (myNode->key > key){
            return findNode(myNode->lower, key);
        }
        else if (myNode->key < key){
            return findNode(myNode->higher, key);
        }
    }
    Node* deleteNode(Node* myNode, int key){
        if (myNode == nullptr){
            return nullptr;
        }
        else{
            findNode(root, key);
        }
        if (root->key != key){
            return root;
        }
        if((root->lower == nullptr)&&(root->higher == nullptr)){
            delete root;
            return nullptr;
        }
        else if((root->lower == nullptr)&&(root->higher != nullptr)){
            Node* temp = root;
            root = root->higher;
            delete temp;
            root->parent = nullptr;
            return root;
        }
        else if((root->lower != nullptr)&&(root->higher == nullptr)){
            Node* temp = root;
            root = root->lower;
            delete temp;
            root->parent = nullptr;
            return root;
        }
        else if ((root->lower != nullptr)&&(root->higher != nullptr)){
            Node* temp = root->lower;
            while (temp->higher != nullptr){
                temp = temp->higher;
            }
            if (root->lower != temp){
                root->lower = splayNode(temp,root);
            }
            temp->higher = root->higher;
            root->higher->parent = temp;
            Node* temp2 = root;
            root = root->lower;
            delete temp2;
            root->parent = nullptr;
            return root;
        }
    }
    void printTree(Node* node, int depth = 0) {
            if (node == NULL) {
                return;
            }
            printTree(node->higher, depth + 1);
            for (int i = 0; i < depth; i++) {
                cout << "    ";
            }
            cout << node->key << " (" << node->name << ")" << endl;
            printTree(node->lower, depth + 1);
        }
};

int main(){
    int number, zastav=0;
    string meno;
    char vstup;
    SplayTree *myTree = nullptr;
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
                            cout << myTree->findNode(root, number) << endl;;
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


