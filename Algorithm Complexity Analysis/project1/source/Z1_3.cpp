#include <iostream>
#include <limits>
using namespace std;
const int n = 3;
const int INF = numeric_limits<int>::max();


void printMatrix(const int* cost_matrix){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << *(cost_matrix + i * n + j) << " ";
        }
        cout << endl;
    }
    cout << endl;
}


int** copy_matrix(const int* cost_matrix) {
    int** copy = new int*[n];
    for (int i = 0; i < n; ++i) {
        copy[i] = new int[n];
        for (int j = 0; j < n; ++j) {
            copy[i][j] = *(cost_matrix + i * n + j);
        }
    }
    return copy;
}

void greedy_algorithm(int* cost_matrix){

    //vytvorim si pole assignments, ktore bude obsahovat indexy priradenych jobov
    int* assignments = new int[n];
    for (int i = 0; i < n; i++) {

        //najdem najmensiu cenu v riadku a priradim ju do pola assignments - greedy vlastnost
        int min_cost = INF;
        for (int j = 0; j < n; j++) {
            if (*(cost_matrix + i * n + j) != 0 && *(cost_matrix + i * n + j) <= min_cost) {
                assignments[i] = j;
                min_cost = *(cost_matrix + i * n + j);
            }
        }

        //vynulujem stlpec, v ktorom sa nachadza priradeny job
        for (int k = 0; k < n; k++) {
            *(cost_matrix + k * n + assignments[i]) = 0;
        }

        //vypisem vysledok
        cout << "The " << i + 1 << ". person is assigned the " << assignments[i] + 1 << ". job." << endl;
    }
}


// pomocna funkcia pre dynamic_programming_algorithm
void cover_columns(int* dp, int index){
    for (int i = 0; i < n; i++){
        *(dp + i * n + index) = INF;
    }
}


void dynamic_programming_algorithm(int* cost_matrix) {

    // vytvorim maticu dp a naplnim ju nekonecnymi hodnotami
    int dp[n][n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dp[i][j] = INF;
        }
    }

    // naplnim prvy riadok hodnotami z cost_matrix
    for (int j = 0; j < n; j++) {
        dp[0][j] = *(cost_matrix + 0 * n + j);
    }

    // naplnim zvysne riadky suctom cien z cost_matrix a min hodnotou z predchadzajuceho riadku
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (k != j) {
                    dp[i][j] = min(dp[i][j], *(cost_matrix + i * n + j) + dp[i - 1][k]);
                }
            }
        }
    }

    // v poslednom riadku najdem najmensiu hodnotu a jej index zapisem do pola assignments
    int* assignments = new int[n];
    int min_index = static_cast<int>(min_element(dp[n - 1], dp[n - 1] + n) - dp[n - 1]);
    assignments[n - 1] = min_index;
    cover_columns(&dp[0][0], min_index);

    // dany job uz je priradeny, takze v matici dp prekryjem stlpec, v ktorom sa nachadza nekonecnymi hodnotami
    for (int i = n - 2; i >= 0; i--) {
        min_index = static_cast<int>(min_element(dp[i], dp[i] + n) - dp[i]);
        assignments[i] = min_index;
        cover_columns(&dp[0][0], min_index);
    }

    // Vypisem vysledok
    for (int i = 0; i < n; i++) {
        cout << "Person " << i + 1 << " is assigned to Job " << assignments[i] + 1 << endl;
    }

    // vycistim pamat
    delete[] assignments;
}


int main(){
    int cost_matrix[n][n] = {
            {10, 5, 5},
            {2, 4, 10},
            {5, 1, 7},
    };

    int** cost_matrix_copy = copy_matrix(&cost_matrix[0][0]);

    cout << "Greedy algorithm assignments:" << endl;
    greedy_algorithm(&cost_matrix[0][0]);
//    cout << "Matrix after greedy algorithm:" << endl;
//    printMatrix(&cost_matrix[0][0]);
    cout << endl;
    cout << "Dynamic Programming Optimal Assignments:" << endl;
    dynamic_programming_algorithm(*cost_matrix_copy);
//    cout << "Matrix after dynamic programming algorithm:" << endl;
//    printMatrix(*cost_matrix_copy);
    return 0;
}


