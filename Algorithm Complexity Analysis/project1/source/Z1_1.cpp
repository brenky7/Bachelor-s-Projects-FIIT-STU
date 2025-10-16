#include <iostream>
#include <string>
#include <algorithm>
using namespace std;
const int n = 7;


// struktura jobu
struct job{
    int id;
    int deadline;
    int profit;
};


// insert sort, zoradi joby podla profitu, vracia pole deadlinov
int* sort(job* arr){

    // vytvorim si pomocne premenne
    int i, key, key2, key3, j;

    // zoradim joby podla profitu
    for (i = 1; i < n; i++) {
        key = arr[i].profit;
        key2 = arr[i].deadline;
        key3 = arr[i].id;
        j = i - 1;
        while (arr[j].profit <= key && j >= 0) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1].profit = key;
        arr[j + 1].deadline = key2;
        arr[j + 1].id = key3;
    }

    // vytvorim si pole deadlinov
    int* deadlines = new int[n];

    // naplnim pole deadlinov
    for (i = 0; i < n; i++){
        deadlines[i] = arr[i].deadline;
    }
    return deadlines;
}


// najde splnitelnu sekvenciu, ak nenajde, vrati sekvenciu bez noveho jobu
string isFeasible(string sequence, string deadlines){

    // vytvorim si kopiu sekvencie, ktoru budem modifikovat
    string sequence2 = sequence;
    sequence2.erase(sequence2.length() -1);

    // prejdem vsetky permutacie sekvencie s novym jobom
    do{
        string output;

        // prejdem vsetky joby v momentalnej permutacii
        for(int i = 0; i < sequence.length(); i++){
            char s = sequence[i];
            int d = deadlines[((s - '0') - 1)] - '0';

            // ak je deadline vacsi ako deadline jobu, pridam ho do vystupu
            if(d >= (i + 1)){
                output += sequence[i];
            }
        }

        // ak je dlzka vystupu rovna dlzke sekvencie, nasiel som splnitelnu sekvenciu
        if(output.length() == sequence.length()){
            return output;
        }
    } while(next_permutation(sequence.begin(), sequence.end()));

    // ak som nenasiel splnitelnu sekvenciu, vratim povodnu sekvenciu bez noveho jobu
    return sequence2;
}


// vypise optimalnu sekvenciu naplanovania jobov
string schedule(int* deadlines){

    // premenim pole deadlinov na string
    string Deadlines;
    for (int i =0; i < n; i++){
        Deadlines += to_string(deadlines[i]);
    }

    // pridam prvy job do sekvencie
    string sequence_final = "1", sequence_temp;

    // postupne pridavam dalsie joby do sekvencie, ak je to mozne
    for (int i = 2; i <= n; i++){
        sequence_temp = sequence_final + to_string(i);
        sequence_final = isFeasible(sequence_temp, Deadlines);
    }
    return sequence_final;
}


int main(){

    // vytvorim si pole jobov
    // ak chcete testovat na inej mnozine treba zemnit aj const n na zaciatku suboru
    job arr[] = {
            {1,2, 40},
            {2,4, 15},
            {3,3, 60},
            {4,2, 20},
            {5,3, 10},
            {6,1, 45},
            {7,1,55}
    };

    // zoradim joby podla profitu, vytvorim pole deadlinov pre planovaci algoritmus
    int* deadlines = sort(arr);

    // vypisem joby zoradene podla profitu
    cout << "Array of jobs sorted by profit: " << endl;
    for(auto & i : arr){
        cout << "Job id: " << i.id << ", Deadline: " << i.deadline << ", Profit: " << i.profit << endl;
    }

    // vytvorim sekvenciu jobov podla planovacieho algoritmu
    string sequence_raw = schedule(deadlines);

    // prepisem optimalnu sekvenciu jobov podla ich id
    string sequence_final = "\nThe optimal sequence of jobs is:\n";
    for(char i : sequence_raw){
       int j = arr[((i - '0') - 1)].id;
       sequence_final += "Job id: ";
       sequence_final += to_string(j);
       sequence_final += "\n";
    }

    // vypisem optimalnu sekvenciu jobov
    cout << sequence_final << endl;

}