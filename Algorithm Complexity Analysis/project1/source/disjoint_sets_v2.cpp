#include <iostream>
#include <string>
using namespace std;


// struktura uzla pre binarny strom
struct set {
    string value;
    set* parent;
    set* left;
    set* right;
    // konstruktor
    explicit set(const string& val) : value(val), parent(nullptr), left(nullptr), right(nullptr) {}
};


// vytvori jednoprvkovu mnozinu
set* make_set(const string& x) {
    return new set(x);
}


// najde mnozinu, v ktorej sa nachadza prvok x
set* find(const string& x, set** sets) {
    for (int i = 0; !sets[i]->value.empty(); i++) {
        set* current = sets[i];
        while (current != nullptr) {
            if (current->value == x) {
                return current;
            } else if (x < current->value) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
    }
    return nullptr;
}


// spoji dve mnoziny
void merge(set* x, set* y) {

    // pomocne pointre na spajanie
    set* p_x = find(x->value, &x);
    set* p_y = find(y->value, &y);

    if (p_x != p_y) {
        // jeden sa stane rodicom druheho, podla hodnoty
        if (p_x->value < p_y->value) {
            set* temp = p_y->left;  // ulozi momentalny lavy podstrom p_y
            p_y->left = p_x;        // zmeni lavy podstrom p_y na p_x
            p_x->parent = p_y;
            p_x->right = temp;      // zmeni pravy podstrom p_x na povodny lavy podstrom p_y
            if (temp != nullptr) {
                temp->parent = p_x;  // ak neni nullptr, zmeni parenta na p_x
            }
        }
        // to iste len z druhej strany
        else {
            set* temp = p_x->right;
            p_x->left = p_y;
            p_y->parent = p_x;
            p_y->left = temp;
            if (temp != nullptr) {
                temp->parent = p_y;
            }
        }
    }
}


// vrati najmensi prvok v mnozine, potrebne pre planovaci algoritmus
int small(set* set) {
    while (set->left != nullptr) {
        set = set->left;
    }
    return stoi(set->value);
}


// pomocna funkcia na vypis stromu, pouzival som pri debugovani
void print_set(set* root) {
    if (root != nullptr) {
        print_set(root->left);    // Print left subtree
        cout << root->value << " ";  // Print root value
        print_set(root->right);   // Print right subtree
    }
}
