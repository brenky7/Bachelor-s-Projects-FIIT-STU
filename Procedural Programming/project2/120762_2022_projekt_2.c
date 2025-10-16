#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct zaznam{
    char ID[20];
    char meno[20];
    char modul[20];
    char velicina[20];      //deklaracia struktury, kazda hodnota ma rovnaku dlzku pre jednoduchsie narabanie pri nasledovnych fciach
    char hodnota[20];
    char cas[20];
    char datum[20];
    struct zaznam *dalsi; // pointer na dalsiu strukturu rovnakeho typu
}ZAZNAM;


void fcia_n(FILE **f, int *pn, int *pocetzaznamov, ZAZNAM **zaciatok){
    *f=fopen("dataloger_V2.txt","r");
    if (!*f) printf("Zaznamy neboli nacitane!"); // ak sa nezdari otvorit subor, vypise podla zadania message
    if (*pn==0){        // ak este nebol linklist vyrobeny, tak ho vyrobi
        int pocetriadkov=0;
        char SSS[20], x[40];
        while (fgets(x, 40, *f)!=NULL){            // najprv zisti pocet zaznamov
            pocetriadkov++;
        }
        *pocetzaznamov=(pocetriadkov/8);
        rewind(*f);
        *zaciatok =malloc(sizeof(ZAZNAM));
        ZAZNAM *aktual= *zaciatok;
        ZAZNAM *aktual2 = NULL; 
        for (int i=0; i< *pocetzaznamov; i++){      // potom ich postupne nahadze do struktury, tu pripoji na linklist
            for (int j=0; j<8;j++){
                if (j==0){
                    fgets(SSS, 20, *f);
                }
                else if (j==1){
                    fgets(aktual->ID, 20, *f);
                }
                else if (j==2){
                    fgets(aktual->meno, 20, *f);
                }
                else if (j==3){
                    fgets(aktual->modul, 10, *f);
                }
                else if (j==4){
                    fgets(aktual->velicina, 10, *f);
                }
                else if (j==5){
                    fgets(aktual->hodnota, 20, *f);
                }
                else if (j==6){
                    fgets(aktual->cas, 10, *f);
                }
                else if (j==7){
                    fgets(aktual->datum, 20, *f);
                }
            }
            if (i== (*pocetzaznamov-1)){    // hodnotu dalsi poslednej struktury nastavi na NULL
                aktual->dalsi=NULL;
            }
            else{
                aktual2=malloc(sizeof(ZAZNAM));
                aktual->dalsi=aktual2;
                aktual2->dalsi=NULL;
            }
            aktual = aktual2;
        }
        rewind(*f);
        if ((*zaciatok)!=NULL){     // ak sa nacitanie zadari, vypise podla zadania message 
            printf("Nacitalo sa %d zaznamov.\n", *pocetzaznamov);
        }
        *pn=1;
    }    
    else{
        ZAZNAM *aktual= *(zaciatok), *aktual2; // ak uz linklist urobeny bol, tak ho premaze a zavola sa znovu
        while(aktual!=NULL){
            aktual2=aktual;
            aktual=aktual->dalsi;
            free(aktual2);
        }
        *pn=0;
        fcia_n(f, pn, pocetzaznamov, zaciatok);
    }
}

void fcia_v(int pocetzaznamov, ZAZNAM **zaciatok){
    ZAZNAM *aktual= *zaciatok;      //ak ma linklist nejake hodnoty, tka ich vypise, nic zlozite
    int i=1;
    if ((*zaciatok)!=NULL){
        while (aktual!=NULL){
            printf("%d:\n",i); i++;
            printf("ID cislo mer. osoby: %s", aktual->ID);
            printf("Meno osoby: %s", aktual->meno);
            printf("Mer. modul: %s", aktual->modul);
            printf("Typ mer. veliciny: %s", aktual->velicina);
            printf("Hodnota: %s", aktual->hodnota);
            printf("Cas merania: %s", aktual->cas);
            printf("Datum: %s", aktual->datum);
            aktual=aktual->dalsi;
            putchar('\n');
        }
    }
}

void fcia_r(ZAZNAM **zaciatok, int pocetzaznamov){
    ZAZNAM *p1=(ZAZNAM*)malloc(sizeof(ZAZNAM)), *p1b=(ZAZNAM*)malloc(sizeof(ZAZNAM));
    ZAZNAM *p2=(ZAZNAM*)malloc(sizeof(ZAZNAM)), *p2b=(ZAZNAM*)malloc(sizeof(ZAZNAM));       //Na polovicu hotova, funguje prehadzovanie prvkov ked jeden z nich je na prvom mieste v zozname,
    ZAZNAM *temp=(ZAZNAM*)malloc(sizeof(ZAZNAM));                                           //nefunguje prehadzovanie prvkov vnutri zoznamu, nemal som uz cas to poriesit
    ZAZNAM *aktual= (*zaciatok);
    int c1,c2;
    scanf("%d %d", &c1, &c2);
    if ((c1==1)||(c2==1)){
        if (c2==1){
            int temp = c1;
            c1=c2;
            c2=temp;
        }
        if (c1==1){
            for (int i=1; i<c2-1; i++){
                aktual=aktual->dalsi;
            }
            p1=aktual->dalsi;
            p2=p1->dalsi;
            p1->dalsi=(*zaciatok)->dalsi;
            (*zaciatok)->dalsi=p2;
            aktual->dalsi=(*zaciatok);
            (*zaciatok)=p1;
        }
    }
    else if((c1>pocetzaznamov)||(c2>pocetzaznamov)){
        printf("Zly vstup\n");
    }
    else{
        for (int i=1; i<c1-1; i++){
                aktual=aktual->dalsi;
        }
        p1=aktual;
        p1b=p1->dalsi->dalsi;
        aktual=(*zaciatok);
        for (int i=1; i<c2-1; i++){
                aktual=aktual->dalsi;
        }
        p2=aktual;
        p2b=p2->dalsi->dalsi;
        p2->dalsi=p1b;
        temp=p1;
        p1=p2;
        p1->dalsi=p2b;
        p2=temp;
    }

}

void fcia_z(ZAZNAM **zaciatok){
    ZAZNAM *aktual= (*zaciatok);        //Nefunguje, tento kod som pisal davno na zaciatku, predtym nez som vobec tusil ako tieto veci funguju
    long ID=0, ID2=0;                   //Nemal som uz cas ju znovu robit
    scanf("%ld", &ID);
    while(strtol((*zaciatok)->ID, NULL, 10)==ID){
        (*zaciatok)=(*zaciatok)->dalsi;
    }
    aktual= (*zaciatok);
    while(aktual->dalsi!=NULL){
        if(strtol(aktual->dalsi->ID, NULL, 10)==ID){
            ZAZNAM *vymazac=(ZAZNAM*)malloc(sizeof(ZAZNAM));
            vymazac=aktual->dalsi;
            aktual->dalsi=vymazac->dalsi;
            free(vymazac);
        }
        else{
            aktual=aktual->dalsi;
        }
    }
    
}

void fcia_p(ZAZNAM **zaciatok, int *pocetzaznamov){
    ZAZNAM *aktual=(*zaciatok);                         // Funguje na 99%
    int miesto=0; char temp[20];                        // jediny detail je ze berie meno iba bez medzery (tj. peto/fero/...)
    ZAZNAM *novy=(ZAZNAM*)malloc(sizeof(ZAZNAM));       // nevedel som to nijako opravit, ziadne permutacie gets() a scanf() {napr.: scanf(" %[^\n]%*c", str) alebo scanf("%[^\n]s",str)} toto nezmenili.
    scanf("%d", &miesto);
    if (miesto>(*pocetzaznamov)){
        while (aktual->dalsi!=NULL){
            aktual=aktual->dalsi;
        }
        scanf("%s", novy->ID);
        scanf("%s", novy->meno);
        scanf("%s", novy->modul);
        scanf("%s", novy->velicina);
        scanf("%s", novy->hodnota);
        scanf("%s", novy->cas);
        scanf("%s", novy->datum);
        novy->ID[strlen(novy->ID)]='\n';
        novy->meno[strlen(novy->meno)]='\n';
        novy->modul[strlen(novy->modul)]='\n';
        novy->velicina[strlen(novy->velicina)]='\n';
        novy->hodnota[strlen(novy->hodnota)]='\n';
        novy->cas[strlen(novy->cas)]='\n';
        novy->datum[strlen(novy->datum)]='\n';
        novy->dalsi=NULL;
        aktual->dalsi=novy;
        *(pocetzaznamov)+=1;
    }
    else if(miesto==1){
        scanf("%s", novy->ID);
        scanf("%s", novy->meno);
        scanf("%s", novy->modul);
        scanf("%s", novy->velicina);
        scanf("%s", novy->hodnota);
        scanf("%s", novy->cas);
        scanf("%s", novy->datum);
        novy->ID[strlen(novy->ID)]='\n';
        novy->meno[strlen(novy->meno)]='\n';
        novy->modul[strlen(novy->modul)]='\n';
        novy->velicina[strlen(novy->velicina)]='\n';
        novy->hodnota[strlen(novy->hodnota)]='\n';
        novy->cas[strlen(novy->cas)]='\n';
        novy->datum[strlen(novy->datum)]='\n';
        novy->dalsi=(*zaciatok);
        (*zaciatok)=novy;
        *(pocetzaznamov)+=1;
    }
    else{
        for (int i=1; i<miesto-1; i++){
            aktual=aktual->dalsi;
        }
        scanf("%s", novy->ID);
        scanf("%s", novy->meno);
        scanf("%s", novy->modul);
        scanf("%s", novy->velicina);
        scanf("%s", novy->hodnota);
        scanf("%s", novy->cas);
        scanf("%s", novy->datum);
        novy->ID[strlen(novy->ID)]='\n';
        novy->meno[strlen(novy->meno)]='\n';
        novy->modul[strlen(novy->modul)]='\n';
        novy->velicina[strlen(novy->velicina)]='\n';
        novy->hodnota[strlen(novy->hodnota)]='\n';
        novy->cas[strlen(novy->cas)]='\n';
        novy->datum[strlen(novy->datum)]='\n';
        novy->dalsi=aktual->dalsi;
        aktual->dalsi=novy;
        *pocetzaznamov+=1;
    }
    
}

void fcia_u(ZAZNAM **zaciatok, int pocetzaznamov){
    ZAZNAM *prvy=(*zaciatok);
    ZAZNAM *aktual=(*zaciatok)->dalsi;              // s tymto som sa natrapil najviac, stale nefunguje
    while(aktual!=NULL){                            // nakolko obycajny bubblesort po opakovanych pokusoch stale nerobil to co mal, skusil som z hlavy vymysliet vlastny algoritmus
        if(strcmp(aktual->datum,prvy->datum)<0){       
            prvy=aktual;
        }
        else if(strcmp(aktual->datum,prvy->datum)==0){      // tento algoritmus najprv najde prvok, kt. ma byt na prvom mieste (*prvy), tj. s najstarsim datumoma casom
            if (strcmp(aktual->cas,prvy->cas)<0){
                prvy=aktual;
            }
        }
        aktual=aktual->dalsi;
    }
    if((*zaciatok)!=prvy){
        aktual=(*zaciatok);                     // potom ho vyberie z tela linklistu a prilepi na zaciatok linklistu (ak uz je prvy tak nerobi nic lebo netreba)
        while(aktual->dalsi!= prvy){
            aktual=aktual->dalsi;
        }
        aktual->dalsi=prvy->dalsi;
        prvy->dalsi=(*zaciatok);
        (*zaciatok)= prvy;
    }
    ZAZNAM *zaciatok2= prvy, *aktual2=prvy, *novy=NULL;
    aktual=prvy->dalsi; int novypocet=1, nasielrovnaky=0; // potom ide triedit tak ze vytvori druhy linklist, do neho postupne pridava prvkyz prveho tak, aby boli zoradene
    while (novypocet!=pocetzaznamov){        // tento cyklus ma fungovat tak ze kym nieje naplneny druhy linklist (neboli zobrate vsetky prvky z prveho linklistu do druheho) tak opakuje telo cyklu
        while(aktual!=NULL){
            if ((strncmp(aktual->datum,aktual2->datum,8)==0)&&(strncmp(aktual->cas,aktual2->cas,4)==0)){
                novy=aktual;
                novypocet++;
                nasielrovnaky=1;        // tuto najprv zoberie vsetky prvky s rovnakym datumom a casom a prilepi ich do druheho linklistu
            }                           // toto este funguje
            else{
                novy=NULL;
            }
            aktual=aktual->dalsi;
            aktual2->dalsi=novy;
            if (novy!=NULL){
                aktual2=aktual2->dalsi;

            }
        }
        if (nasielrovnaky==0){
            aktual=prvy->dalsi;
            while (aktual!=NULL){           //ak uz nenasiel ziadny duplicitny datum a cas, najde dalsi najstarsi a prida ho do druheho linklistu
                                            // k nakodeniu tohto som uz nedostal, lebo dalsia cast nefunguje


                aktual=aktual->dalsi;
                nasielrovnaky=1;
            }
        }
        aktual=prvy;
        while (aktual!=NULL){                   //aby cyklus daval zmysel, tak z prveho zoznamu, z ktoreho vyberam prvky, ich musim nasledne aj zmazat, aby som mohol telo cyklu zasa opakovat, to ma robit tento while
            aktual2=prvy;
            for (int i=0; i<novypocet; i++){
                if (aktual->dalsi==aktual2){        // toto uz nefunguje
                    ZAZNAM *vymaz=aktual->dalsi;
                    aktual->dalsi=vymaz->dalsi;
                    free(vymaz);
                }
                aktual2=aktual2->dalsi;
            }
            aktual=aktual->dalsi;
        }
        nasielrovnaky=0;
    }
    (*zaciatok)=zaciatok2;      // uplne na konci prehodi hodnotu zaciatku s ktorou sa pracuje vo zvysku programu na hodnotu druheho, usporiadaneho linklistu. idealne potom by ten prvy neusporiadany linklist este free-ol.
}

void fcia_h(ZAZNAM *zaciatok){
    ZAZNAM *aktual=(ZAZNAM*)malloc(sizeof(ZAZNAM));     //funguje tak jak ma
    aktual = zaciatok;                      // zoberie zadany modul, prejde cely link list, zakazdym ked sa zadany modul rovna tomu v zazname tak vypise zaznam
    char modul[20];             
    int nasiel=0;
    scanf("%s", modul);
    while(aktual!=NULL){
        if (strncmp(aktual->modul,modul,3)==0){
            nasiel++;
            printf("%d:\n",nasiel);
            printf("ID cislo mer. osoby: %s", aktual->ID);
            printf("Meno osoby: %s", aktual->meno);
            printf("Mer. modul: %s", aktual->modul);
            printf("Typ mer. veliciny: %s", aktual->velicina);
            printf("Hodnota: %s", aktual->hodnota);
            printf("Cas merania: %s", aktual->cas);
            printf("Datum: %s", aktual->datum);
        }
        aktual=aktual->dalsi;
    }
    if (nasiel==0){         // ak ziaden nenasiel, podla zadania vypise message
        printf("Pre meraci modul %s nie su zaznamy.\n", modul);
    }
}

void fcia_k(ZAZNAM **zaciatok){
    ZAZNAM *aktual=(*zaciatok);     //vymaze postupne cely linklist
    ZAZNAM *vymaz=(ZAZNAM*)malloc(sizeof(ZAZNAM));
    while(aktual!=NULL){
        vymaz=aktual;
        aktual=aktual->dalsi;
        free(vymaz);
    }
    (*zaciatok)=NULL;
}

int main(){
    FILE *f=NULL;           //pyta vstupy na fcie, potom vola fcie, pri fcii k skonci, to iste co v prvom projekte
    int pocetzaznamov=0, pn=0;
    char vstup;
    ZAZNAM *zaciatok;
    while (1){
        scanf("%c",&vstup);
        switch (vstup){
            case 'n' :{
                fcia_n(&f, &pn, &pocetzaznamov, &zaciatok);
                break;
            }
            case 'v' :{
                fcia_v(pocetzaznamov, &zaciatok);
                break;
            }
            case 'z' :{
                fcia_z(&zaciatok);
                break;
            }
            case 'p' :{
                fcia_p(&zaciatok, &pocetzaznamov);
                break;
            }
            case 'u' :{
                fcia_u(&zaciatok, pocetzaznamov);
                break;
            }
            case 'h' :{
                fcia_h(zaciatok);
                break;
            }
            case 'r' :{
                fcia_r(&zaciatok, pocetzaznamov);
                break;
            }
            case 'k' :{
                fcia_k(&zaciatok);
                return 0;
                break;
            }
        }

    }
    return 0;
}