#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fcia_v(FILE**f1, int*pv, int*pn, char ***pole_ID, char ***pole_modul, char ***pole_velicina, 
    char ***pole_hodnota, char ***pole_cas, char ***pole_datum, int *ppocetzaznamov){
    if (*f1==NULL){
        *f1=fopen("dataloger.txt","r");
    }
    if(!*f1){
        printf("Neotvoreny Subor\n");
    }
    if ((*f1!=NULL) || (*pv==1)){
        if (*pn==0){
            char x[40];
            int riadok = 0;
            while(fgets(x,40,*f1)!=NULL){    
                if (riadok==7) riadok=0;
                if (riadok==0){
                    printf("ID cislo mer. osoby: ");
                }
                if (riadok==1){
                    printf("Mer modul: ");
                }
                if (riadok==2){
                    printf("Typ mer. veliciny: ");
                }
                if (riadok==3){
                    printf("Hodnota: ");
                }
                if (riadok==4){
                    printf("Cas merania: ");
                }
                if (riadok==5){
                    printf("Datum: ");
                }
                printf("%s",x);
                riadok++; 
            }
            *pv=1; 
            rewind(*f1); 

        }
        else{
            for (int i=0; i<*ppocetzaznamov; i++){
                printf("ID cislo mer. osoby: ");
                printf("%s \n",(*pole_ID)[i]);
                printf("Mer modul: ");
                printf("%s \n",(*pole_modul)[i]);
                printf("Typ mer. veliciny: ");
                printf("%s \n",(*pole_velicina)[i]);
                printf("Hodnota: ");
                printf("%s \n",(*pole_hodnota)[i]);
                printf("Cas merania: ");
                printf("%s \n",(*pole_cas)[i]);
                printf("Datum: ");
                printf("%s \n",(*pole_datum)[i]);
                putchar('\n');
            }

        }
    }
}
void fcia_n(FILE**f1, int*pv, int*pn, char ***pole_ID, char ***pole_modul, char ***pole_velicina, 
    char ***pole_hodnota, char ***pole_cas, char ***pole_datum, int *ppocetzaznamov){
    if (*pv==0){
        printf("Neotvoreny subor\n");
    }
    else if(*pn==0){
        char x[40];
        int pocetriadkov = 0;
        while(fgets(x,40,*f1)!=NULL){ 
            pocetriadkov++;
        }
        *ppocetzaznamov=(pocetriadkov+1)/7;
        rewind(*f1);
        *pole_ID = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_modul = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_velicina = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_hodnota = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_cas = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_datum = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        for (int i = 0; i < *ppocetzaznamov; i++){
            (*pole_ID)[i]=calloc(40,sizeof(char*));
            (*pole_modul)[i]=calloc(40,sizeof(char*));
            (*pole_velicina)[i]=calloc(40,sizeof(char*));
            (*pole_hodnota)[i]=calloc(40,sizeof(char*));
            (*pole_cas)[i]=calloc(40,sizeof(char*));
            (*pole_datum)[i]=calloc(40,sizeof(char*));
        }
       for (int j = 0; j < *ppocetzaznamov; j++){
           
            fscanf(*f1, "%s", x);
            strcpy((*pole_ID)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_modul)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_velicina)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_hodnota)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_cas)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_datum)[j],x);
            fgetc(*f1);
        }
        rewind(*f1); 
        *pn=1;  
    }
    else if(*pn==1){
        char x[40];
        for(int i=0;i<*ppocetzaznamov;i++){
            free((*pole_ID)[i]);
            free((*pole_modul)[i]);
            free((*pole_velicina)[i]);
            free((*pole_hodnota)[i]);
            free((*pole_datum)[i]);
            free((*pole_cas)[i]);
        }
        free(*pole_ID);
        free(*pole_modul);
        free(*pole_velicina);
        free(*pole_hodnota);
        free(*pole_datum);
        free(*pole_cas);
        *pole_ID = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_modul = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_velicina = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_hodnota = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_cas = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        *pole_datum = (char**)calloc(*ppocetzaznamov,sizeof(char*));
        for (int i = 0; i < *ppocetzaznamov; i++){
            (*pole_ID)[i]=calloc(40,sizeof(char*));
            (*pole_modul)[i]=calloc(40,sizeof(char*));
            (*pole_velicina)[i]=calloc(40,sizeof(char*));
            (*pole_hodnota)[i]=calloc(40,sizeof(char*));
            (*pole_cas)[i]=calloc(40,sizeof(char*));
            (*pole_datum)[i]=calloc(40,sizeof(char*));
        }
       for (int j = 0; j < *ppocetzaznamov; j++){
           
            fscanf(*f1, "%s", x);
            strcpy((*pole_ID)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_modul)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_velicina)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_hodnota)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_cas)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((*pole_datum)[j],x);
            fgetc(*f1);
        }
        rewind(*f1); 
    }
    
}
void fcia_o(FILE**f1, int*pv, int*pn, char ***pole_modul, char ***pole_velicina, 
    char ***pole_hodnota, char ***pole_cas, char ***pole_datum, int *ppocetzaznamov){
    if (*pv==0){
        printf("Neotvoreny subor\n");
    }
    else if (*pn==1){
        char modul[5]; char velicina[4];
        char **pole_cas_2, **pole_datum_2, **pole_modul_2, **pole_velicina_2, **pole_hodnota_2;
        int pocetzaznamovmodulu=0;
        scanf("%s %s", modul, velicina);
        for (int i=0; i< *ppocetzaznamov; i++){
            if((strncmp((*pole_modul)[i], modul, 3)==0)&&(strncmp((*pole_velicina)[i], velicina,3)==0)){
                pocetzaznamovmodulu++;
            } 
        }
        pole_modul_2 = (char**)calloc(pocetzaznamovmodulu,sizeof(char*));
        pole_velicina_2 = (char**)calloc(pocetzaznamovmodulu,sizeof(char*));
        pole_cas_2 = (char**)calloc(pocetzaznamovmodulu,sizeof(char*));
        pole_hodnota_2 = (char**)calloc(pocetzaznamovmodulu,sizeof(char*));
        pole_datum_2 = (char**)calloc(pocetzaznamovmodulu,sizeof(char*));
        for (int i=0; i<pocetzaznamovmodulu; i++){
            (pole_cas_2)[i]=calloc(20,sizeof(char*));
            (pole_datum_2)[i]=calloc(20,sizeof(char*));
            (pole_hodnota_2)[i]=calloc(20,sizeof(char*));
            (pole_velicina_2)[i]=calloc(20,sizeof(char*));
            (pole_modul_2)[i]=calloc(20,sizeof(char*));
        }
        int j=0;
        for (int i=0; i< *ppocetzaznamov; i++){
            if((strncmp((*pole_modul)[i], modul,3)==0)&&(strncmp((*pole_velicina)[i], velicina,8)==0)){
                (pole_cas_2)[j]=(*pole_cas)[i];
                (pole_datum_2)[j]=(*pole_datum)[i];
                (pole_hodnota_2)[j]=(*pole_hodnota)[i];
                (pole_velicina_2)[j]=(*pole_velicina)[i];
                (pole_modul_2)[j]=(*pole_modul)[i];
                j++;
            } 
        }
        for (int i=0; i < pocetzaznamovmodulu-1; i++){
            for (int j=0; j<pocetzaznamovmodulu-i-1; j++){
                long datum,datum2;
                sscanf(pole_datum_2[j],"%ld",&datum);
                sscanf(pole_datum_2[j+1],"%ld",&datum2);
                if (datum>datum2){
                    long temp;
                    sscanf(pole_datum_2[j],"%ld",&temp);
                    sscanf(pole_datum_2[j+1],"%s",pole_datum_2[j]);
                    sprintf(pole_datum_2[j+1],"%ld",temp);
                    char temp2[6];
                    strcpy(temp2, pole_cas_2[j]);
                    sscanf(pole_cas_2[j+1],"%s",pole_cas_2[j]);
                    sprintf(pole_cas_2[j+1],"%s",temp2);
                    float temp3;
                    sscanf(pole_hodnota_2[j],"%f",&temp3);
                    sscanf(pole_hodnota_2[j+1],"%s",pole_hodnota_2[j]);
                    sprintf(pole_hodnota_2[j+1],"%f",temp3);
                    char temp4[5];
                    strcpy(temp4, pole_modul_2[j]);
                    sscanf(pole_modul_2[j+1],"%s",pole_modul_2[j]);
                    sprintf(pole_modul_2[j+1],"%s",temp4);
                    char temp5[4];
                    strcpy(temp5,pole_velicina_2[j]);
                    sscanf(pole_velicina_2[j+1],"%s",pole_velicina_2[j]);
                    sprintf(pole_velicina_2[j+1],"%s",temp5);
                } 
            }
        }
        for (int i=0; i < pocetzaznamovmodulu; i++){
            printf("%s %s %s %s %s\n",pole_modul_2[i], pole_velicina_2[i], pole_datum_2[i], pole_cas_2[i], pole_hodnota_2[i]);
        }  
        free(pole_cas_2);  
        free(pole_datum_2); 
        free(pole_hodnota_2); 
        free(pole_modul_2); 
        free(pole_velicina_2); 
    }    
    else{
        char **pole_cas_3, **pole_datum_3, **pole_hodnota_3, **pole_velicina_3, **pole_modul_3, **pole_ID_3; //**pole_cas_4, **pole_datum_4, **pole_hodnota_4, **pole_velicina_4, **pole_modul_4;
        char x[40];
        int ppocetriadkov = 0,pppocetzaznamov=0;
        while(fgets(x,40,*f1)!=NULL){ 
            ppocetriadkov++;
        }
        pppocetzaznamov=(ppocetriadkov+1)/7;
        rewind(*f1);
        pole_ID_3 = (char**)calloc(pppocetzaznamov,sizeof(char*));
        pole_modul_3 = (char**)calloc(pppocetzaznamov,sizeof(char*));
        pole_velicina_3 = (char**)calloc(pppocetzaznamov,sizeof(char*));
        pole_hodnota_3 = (char**)calloc(pppocetzaznamov,sizeof(char*));
        pole_cas_3 = (char**)calloc(pppocetzaznamov,sizeof(char*));
        pole_datum_3 = (char**)calloc(pppocetzaznamov,sizeof(char*));
        for (int i = 0; i < pppocetzaznamov; i++){
            (pole_ID_3)[i]=calloc(40,sizeof(char*));
            (pole_modul_3)[i]=calloc(40,sizeof(char*));
            (pole_velicina_3)[i]=calloc(40,sizeof(char*));
            (pole_hodnota_3)[i]=calloc(40,sizeof(char*));
            (pole_cas_3)[i]=calloc(40,sizeof(char*));
            (pole_datum_3)[i]=calloc(40,sizeof(char*));
        }
       for (int j = 0; j < pppocetzaznamov; j++){  
            fscanf(*f1, "%s", x);
            strcpy((pole_ID_3)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((pole_modul_3)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((pole_velicina_3)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((pole_hodnota_3)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((pole_cas_3)[j],x);
            fscanf(*f1, "%s", x);
            strcpy((pole_datum_3)[j],x);
            fgetc(*f1);
        }
        rewind(*f1); 
        char modul[5],velicina[4];
        int ppocetzaznamovmodulu=0;
        scanf("%s %s",modul,velicina);
        for (int i=0; i< pppocetzaznamov; i++){
            if((strncmp(pole_modul_3[i], modul,3)==0)&&(strncmp(pole_velicina_3[i], velicina,8)==0)){
                ppocetzaznamovmodulu++;
            } 
        }
        char **pole_modul_4 = (char**)calloc(ppocetzaznamovmodulu,sizeof(char*));
        char **pole_velicina_4 = (char**)calloc(ppocetzaznamovmodulu,sizeof(char*));
        char **pole_cas_4 = (char**)calloc(ppocetzaznamovmodulu,sizeof(char*));
        char **pole_hodnota_4 = (char**)calloc(ppocetzaznamovmodulu,sizeof(char*));
        char **pole_datum_4 = (char**)calloc(ppocetzaznamovmodulu,sizeof(char*));
        for (int i=0; i<ppocetzaznamovmodulu; i++){
            (pole_cas_4)[i]=calloc(20,sizeof(char*));
            (pole_datum_4)[i]=calloc(20,sizeof(char*));
            (pole_hodnota_4)[i]=calloc(20,sizeof(char*));
            (pole_velicina_4)[i]=calloc(20,sizeof(char*));
            (pole_modul_4)[i]=calloc(20,sizeof(char*));
        }
        int j=0;
        for (int i=0; i< pppocetzaznamov; i++){
            if((strncmp(pole_modul_3[i], modul,3)==0)&&(strncmp(pole_velicina_3[i], velicina,8)==0)){
                (pole_cas_4)[j]=pole_cas_3[i];
                (pole_datum_4)[j]=pole_datum_3[i];
                (pole_hodnota_4)[j]=pole_hodnota_3[i];
                (pole_velicina_4)[j]=pole_velicina_3[i];
                (pole_modul_4)[j]=pole_modul_3[i];
                j++;
            } 
        }
        for (int i=0; i < ppocetzaznamovmodulu-1; i++){
            for (int j=0; j<ppocetzaznamovmodulu-i-1; j++){
                long datum,datum2;
                sscanf(pole_datum_4[j],"%ld",&datum);
                sscanf(pole_datum_4[j+1],"%ld",&datum2);
                if (datum>datum2){
                    long temp;
                    sscanf(pole_datum_4[j],"%ld",&temp);
                    sscanf(pole_datum_4[j+1],"%s",pole_datum_4[j]);
                    sprintf(pole_datum_4[j+1],"%ld",temp);
                    char temp2[6];
                    strcpy(temp2, pole_cas_4[j]);
                    sscanf(pole_cas_4[j+1],"%s",pole_cas_4[j]);
                    sprintf(pole_cas_4[j+1],"%s",temp2);
                    float temp3;
                    sscanf(pole_hodnota_4[j],"%f",&temp3);
                    sscanf(pole_hodnota_4[j+1],"%s",pole_hodnota_4[j]);
                    sprintf(pole_hodnota_4[j+1],"%f",temp3);
                    char temp4[5];
                    strcpy(temp4, pole_modul_4[j]);
                    sscanf(pole_modul_4[j+1],"%s",pole_modul_4[j]);
                    sprintf(pole_modul_4[j+1],"%s",temp4);
                    char temp5[4];
                    strcpy(temp5,pole_velicina_4[j]);
                    sscanf(pole_velicina_4[j+1],"%s",pole_velicina_4[j]);
                    sprintf(pole_velicina_4[j+1],"%s",temp5);
                } 
            }
        }
        for (int i=0; i < ppocetzaznamovmodulu; i++){
            printf("%s %s %s %s %s\n",pole_modul_4[i], pole_velicina_4[i], pole_datum_4[i], pole_cas_4[i], pole_hodnota_4[i]);
        }
        free(pole_cas_4);
        free(pole_datum_4);
        free(pole_hodnota_4);
        free(pole_modul_4);
        free(pole_velicina_4);
        free(pole_cas_3);
        free(pole_datum_3);
        free(pole_hodnota_3);
        free(pole_modul_3);
        free(pole_velicina_3);
    }
}
void fcia_c(FILE**f1,int *pv){
    if (*pv==0){
        printf("Neotvoreny subor\n");
    }
    else{
        int spravne=1;
        int riadok=0;
        char x[40];
        while(fgets(x,40,*f1)!=NULL){    
            if (riadok==7) riadok=0;
            if (riadok==0){
                int delitelne=0;
                long cislo;
                sscanf(x,"%ld",&cislo);
                if (cislo%11==0) {
                    delitelne=1;
                } 
                if ((strlen(x)!=12)||(delitelne==0)){ 
                    printf("Nekorektne  zadany  vstup:  ID  cislo  mer.  osoby.\n");
                    spravne=0;
                } 
            }
            if (riadok==1){
                int velke=0;
                for (int i=65;i<=90;i++){
                    if (x[0]==i) velke=1;
                }
                int index=0;
                if (((x[2]=='0')||(x[2]=='1')||(x[2]=='2')||(x[2]=='3')||(x[2]=='4')||(x[2]=='5')||(x[2]=='6')||(x[2]=='7')||(x[2]=='8')||(x[2]=='9'))&&((x[1]=='0')||(x[1]=='1')||(x[1]=='2')||(x[1]=='3')||(x[1]=='4')||(x[1]=='5')||(x[1]=='6')||(x[1]=='7')||(x[1]=='8')||(x[1]=='9'))) index=1; 
                if ((velke==0)||(strlen(x)!=5)||(index==0)){
                    printf("Nekorektne  zadany  vstup: Mer modul.\n");
                    spravne=0;
                }    
            }
            if (riadok==2){
                int velke=0;
                if ((x[0]==65)||(x[0]==82)||(x[0]==85)) velke=1;
                int index=0;
                if ((x[1]=='1')||(x[1]=='2')||(x[1]=='4')) index=1;
                if ((velke==0)||(index==0)){
                    printf("Nekorektne  zadany  vstup: Typ mer. veliciny.\n");
                    spravne=0;
                }
            }
            if (riadok==4){
                int hodina=0;
                if (x[0]=='0'){
                    if ((x[1]=='0')||(x[1]=='1')||(x[1]=='2')||(x[1]=='3')||(x[1]=='4')||(x[1]=='5')||(x[1]=='6')||(x[1]=='7')||(x[1]=='8')||(x[1]=='9')) hodina=1;
                }
                if (x[0]=='1'){
                    if ((x[1]=='0')||(x[1]=='1')||(x[1]=='2')||(x[1]=='3')||(x[1]=='4')||(x[1]=='5')||(x[1]=='6')||(x[1]=='7')||(x[1]=='8')||(x[1]=='9')) hodina=1;
                }
                if (x[0]=='2'){
                    if ((x[1]=='0')||(x[1]=='1')||(x[1]=='2')||(x[1]=='3')) hodina=1; 
                }
                int minuta=0;
                if (((x[2]=='0')||(x[2]=='1')||(x[2]=='2')||(x[2]=='3')||(x[2]=='4')||(x[2]=='5'))&&((x[3]=='0')||(x[3]=='1')||(x[3]=='2')||(x[3]=='3')||(x[3]=='4')||(x[3]=='5')||(x[3]=='6')||(x[3]=='7')||(x[3]=='8')||(x[3]=='9'))) minuta=1;
                if ((strlen(x)!=6)||(minuta==0)||(hodina==0)){
                    printf("Nekorektne  zadany  vstup: Cas merania.\n");
                    spravne=0;
                }
            }
            if (riadok==5){
                int den=0;
                if (x[6]=='0'){
                    if ((x[7]=='0')||(x[7]=='1')||(x[7]=='2')||(x[7]=='3')||(x[7]=='4')||(x[7]=='5')||(x[7]=='6')||(x[7]=='7')||(x[7]=='8')||(x[7]=='9')) den=1;
                }
                if (x[6]=='1'){
                    if ((x[7]=='0')||(x[7]=='1')||(x[7]=='2')||(x[7]=='3')||(x[7]=='4')||(x[7]=='5')||(x[7]=='6')||(x[7]=='7')||(x[7]=='8')||(x[7]=='9')) den=1;
                }
                if (x[6]=='2'){
                    if ((x[7]=='0')||(x[7]=='1')||(x[7]=='2')||(x[7]=='3')||(x[7]=='4')||(x[7]=='5')||(x[7]=='6')||(x[7]=='7')||(x[7]=='8')||(x[7]=='9')) den=1;
                }
                if (x[6]=='3'){
                    if ((x[7]=='0')||(x[7]=='1')) den = 1;
                }    
                if ((strlen(x)!=10)||(den==0)){
                    printf("Nekorektne  zadany  vstup: Datum.\n");
                    printf("%d\n",den);
                    spravne=0;
                }
            }
            riadok++; 
        }
        if (spravne==1) printf("Data su korektne\n");
    }
}
void fcia_s(int *pn, int *ppocetzaznamov, char ***pole_modul, char ***pole_velicina, 
    char ***pole_hodnota, char ***pole_cas, char ***pole_datum){
    FILE *f2;
    f2 = fopen("vystup_S.txt","w");
    if (*pn==0){
        printf("Polia nie su vytvorene\n");
    }
    else{
        char modul[5],velicina[4];
        int pocetzaznamovmodulu2=0;
        scanf("%s %s", modul, velicina);
        for (int i=0; i< *ppocetzaznamov; i++){
            if((strncmp((*pole_modul)[i], modul,3)==0)&&(strncmp((*pole_velicina)[i], velicina,8)==0)){
                pocetzaznamovmodulu2++;  
            } 
        }
        if (pocetzaznamovmodulu2==0){
            printf("Pre dany vstup neexistuju zaznamy\n");
            fprintf(f2,"-");
            rewind(f2);
            fclose(f2);
        }
        else if (pocetzaznamovmodulu2>0){
            char **pole_cas_5, **pole_datum_5, **pole_hodnota_5;
            pole_cas_5 = (char**)calloc(pocetzaznamovmodulu2,sizeof(char*));
            pole_hodnota_5 = (char**)calloc(pocetzaznamovmodulu2,sizeof(char*));
            pole_datum_5 = (char**)calloc(pocetzaznamovmodulu2,sizeof(char*));
            for (int i=0; i<pocetzaznamovmodulu2; i++){
                (pole_cas_5)[i]=calloc(20,sizeof(char*));
                (pole_datum_5)[i]=calloc(20,sizeof(char*));
                (pole_hodnota_5)[i]=calloc(20,sizeof(char*));
            }
            int k=0;
            for (int i=0; i< *ppocetzaznamov; i++){
                if((strcmp((*pole_modul)[i], modul)==0)&&(strcmp((*pole_velicina)[i], velicina)==0)){
                    (pole_cas_5)[k]=(*pole_cas)[i];
                    (pole_datum_5)[k]=(*pole_datum)[i];
                    (pole_hodnota_5)[k]=(*pole_hodnota)[i];
                    k++;
                } 
            }
            for (int i=0; i < pocetzaznamovmodulu2-1; i++){
                for (int j=0; j<pocetzaznamovmodulu2-i-1; j++){
                    long datum,datum2;
                    sscanf(pole_datum_5[j],"%ld",&datum);
                    sscanf(pole_datum_5[j+1],"%ld",&datum2);
                    if (datum>datum2){
                        long temp;
                        sscanf(pole_datum_5[j],"%ld",&temp);
                        sscanf(pole_datum_5[j+1],"%s",pole_datum_5[j]);
                        sprintf(pole_datum_5[j+1],"%ld",temp);
                        char temp2[6];
                        strcpy(temp2, pole_cas_5[j]);
                        sscanf(pole_cas_5[j+1],"%s",pole_cas_5[j]);
                        sprintf(pole_cas_5[j+1],"%s",temp2);
                        float temp3;
                        sscanf(pole_hodnota_5[j],"%f",&temp3);
                        sscanf(pole_hodnota_5[j+1],"%s",pole_hodnota_5[j]);
                        sprintf(pole_hodnota_5[j+1],"%f",temp3);
                    } 
                }
            }
            for (int i=0;i<pocetzaznamovmodulu2;i++){
                fprintf(f2,"%s%s     %s\n",(pole_datum_5)[i],(pole_cas_5)[i], (pole_hodnota_5)[i]);
            }
            rewind(f2);
            fclose(f2); 
            free(pole_hodnota_5);
            free(pole_cas_5);
            free(pole_datum_5);
            printf("Pre dany vstup  je vytvoreny  txt subor\n");
        }
    }

}
void fcia_h(int *pn, char ***pole_velicina, char ***pole_hodnota, int *ppocetzaznamov){
    if (*pn==0){
        printf("Polia  nie  su  vytvorene\n");
    }
    else{
        int interval[57];
        for (int i=0;i<57;i++){
            interval[i]=0;
        }
        int pocetnost=0;
        char velicina[4];
        scanf("%s",velicina);
        for (int i=0;i<*ppocetzaznamov;i++){
            if (strncmp((*pole_velicina)[i],velicina,8)==0){
                pocetnost++;
            }
        }
        float *pole_hodnota_2=calloc(pocetnost,sizeof(float));
        int k=0;
        for (int i=0;i<*ppocetzaznamov;i++){
            if (strncmp((*pole_velicina)[i],velicina,8)==0){
                float temp = atof((*pole_hodnota)[i]);
                pole_hodnota_2[k]=temp;
                k++;
            } 
        }
        for (int i=0; i<pocetnost;i++){
            for (int j=0;j<*ppocetzaznamov;j++){
                if (((pole_hodnota_2[i])>(j*5))&&((pole_hodnota_2[i])<=(5*(j+1))))interval[j]++;
            }
        }
        printf("    %s      pocetnost\n",velicina);
        for (int i=0;i<*ppocetzaznamov;i++){
            if (interval[i]>0)printf("( %d.0- %d.0>     %d\n",i*5,(i+1)*5,interval[i]);
        }
        free(pole_hodnota_2);
    }
}
void fcia_z(int *pn, char ***pole_ID, char ***pole_modul, char ***pole_velicina, 
    char ***pole_hodnota, char ***pole_cas, char ***pole_datum, int *ppocetzaznamov){
    if (*pn==0){
        printf("Polia nie su vytvorene\n");
    }
    else{
        /*char id[10];
        int pocet=0;
        scanf("%s",id);
        for (int i=0;i<*ppocetzaznamov;i++){
            if(strcmp((*pole_ID)[i],id)==0)
            pocet++;
        }
        printf("%d\n",pocet);*/
        





    }
}
void fcia_r(int *pn, char ***pole_cas, int *ppocetzaznamov){
    if (*pn==0){
        printf("Polia neboli vytvorene\n");
    }
    else{
        char (**pole_hodiny)=(char**)calloc(*ppocetzaznamov,sizeof(char*));
        for (int i=0;i<*ppocetzaznamov;i++){
            pole_hodiny[i]=calloc(150,sizeof(char));
        }
        char x[6];
        int uzmam=0;
        int estenemam=0;
        int g=0;
        for (int i=0;i<*ppocetzaznamov;i++){
            strcpy(x,(*pole_cas)[i]);
            for (int j=0;j<*ppocetzaznamov;j++){
                if ((pole_hodiny[j][0]==x[0])&&(pole_hodiny[j][1]==x[1]))uzmam=1;
                else estenemam=1;
            }
            if ((estenemam==1)&&(uzmam==0)){
                pole_hodiny[g][0]=x[0];
                pole_hodiny[g][1]=x[1];
                pole_hodiny[g][2]=':';  
                g++;
            }
            else{
                estenemam=0;
                uzmam=0;
            }
        }
        for (int i=0; i < g-1; i++){
            for (int j=0; j<g-i-1; j++){
                int hodina1,hodina2;
                sscanf(pole_hodiny[j],"%d",&hodina1);
                sscanf(pole_hodiny[j+1],"%d",&hodina2);
                if (hodina1>hodina2){
                    int temp;
                    char temp2[3];
                    sscanf(pole_hodiny[j],"%d",&temp);
                    sscanf(pole_hodiny[j+1],"%s",pole_hodiny[j]);
                    sprintf(pole_hodiny[j+1],"%d",temp);
                    pole_hodiny[j+1][2]=':';
                }
            }
        }
        int g2=3;
        for (int i=0;i<g;i++){
            for (int j=0;j<(*ppocetzaznamov);j++){
                if (((*pole_cas)[j][0]==pole_hodiny[i][0])&&((*pole_cas)[j][1]==pole_hodiny[i][1])){   
                    pole_hodiny[i][g2]=(*pole_cas)[j][2];
                    pole_hodiny[i][g2+1]=(*pole_cas)[j][3];
                    pole_hodiny[i][g2+2]=',';
                    g2=g2+3; 
                }
                else{
                    g2=3;
                }
            }   
        }
        for (int i=0;i<g;i++){
            printf("%s\n",pole_hodiny[i]);
        }    
    }
}
void fcia_j(char ***pole_modul, char ***pole_velicina, char ***pole_hodnota, int *ppocetzaznamov){
    char velicina[4], modul[5], modul2[5];
    float amin=1000; float amax=0;
    scanf("%s %s %s", modul, modul2, velicina);
    int apocetzaznamovmodulu=0, bpocetzaznamovmodulu=0;
    for (int i=0; i< *ppocetzaznamov; i++){
        if((strncmp((*pole_modul)[i], modul, 3)==0)&&(strncmp((*pole_velicina)[i], velicina,3)==0)){
            apocetzaznamovmodulu++;
            float temp;
            sscanf((*pole_hodnota)[i],"%f",&temp);
            if (temp<amin) amin=temp; 
            float temp2;   
            sscanf((*pole_hodnota)[i],"%f",&temp2);
            if (temp2>amax) amax=temp2;
        } 
    }
    float bmin=1000; float bmax=0;
    for (int i=0; i< *ppocetzaznamov; i++){
        if((strncmp((*pole_modul)[i], modul2, 3)==0)&&(strncmp((*pole_velicina)[i], velicina,3)==0)){
            bpocetzaznamovmodulu++;
            float temp;
            sscanf((*pole_hodnota)[i],"%f",&temp);
            if (temp<bmin) bmin=temp; 
            float temp2;   
            sscanf((*pole_hodnota)[i],"%f",&temp2);
            if (temp2>bmax) bmax=temp2;
        } 
    }
    char **pole_hodnota_a = (char**)calloc(apocetzaznamovmodulu,sizeof(char*));
    for (int i=0; i<apocetzaznamovmodulu;i++){
        (pole_hodnota_a)[i]=calloc(50,sizeof(char*));
    }
    char **pole_hodnota_b = (char**)calloc(bpocetzaznamovmodulu,sizeof(char*));
    for (int i=0; i<bpocetzaznamovmodulu;i++){
        (pole_hodnota_b)[i]=calloc(50,sizeof(char*));
    }
    int j1=0;
    for (int i=0; i< *ppocetzaznamov; i++){
        if((strncmp((*pole_modul)[i], modul, 3)==0)&&(strncmp((*pole_velicina)[i], velicina,3)==0)){
            (pole_hodnota_a)[j1]=(*pole_hodnota)[i];
            j1++;
        }
    }
    int j2=0;
    for (int i=0; i< *ppocetzaznamov; i++){
        if((strncmp((*pole_modul)[i], modul2, 3)==0)&&(strncmp((*pole_velicina)[i], velicina,3)==0)){
            (pole_hodnota_b)[j2]=(*pole_hodnota)[i];
            j2++;
        }
    }
    if (apocetzaznamovmodulu>0){
    for (int i=0; i < apocetzaznamovmodulu-1; i++){
        for (int j=0; j<apocetzaznamovmodulu-i-1; j++){
            float hodnota1,hodnota2;
            sscanf(pole_hodnota_a[j],"%f",&hodnota1);
            sscanf(pole_hodnota_a[j+1],"%fv",&hodnota2);
            if (hodnota1>hodnota2){
                float temp3;
                sscanf(pole_hodnota_a[j],"%f",&temp3);
                sscanf(pole_hodnota_a[j+1],"%s",pole_hodnota_a[j]);
                sprintf(pole_hodnota_a[j+1],"%f",temp3);
            } 
        }
    }
    }
    if (bpocetzaznamovmodulu>0){
        for (int i=0; i < bpocetzaznamovmodulu-1; i++){
        for (int j=0; j<bpocetzaznamovmodulu-i-1; j++){
            float hodnota1,hodnota2;
            sscanf(pole_hodnota_b[j],"%f",&hodnota1);
            sscanf(pole_hodnota_b[j+1],"%fv",&hodnota2);
            if (hodnota1>hodnota2){
                float temp3;
                sscanf(pole_hodnota_b[j],"%f",&temp3);
                sscanf(pole_hodnota_b[j+1],"%s",pole_hodnota_b[j]);
                sprintf(pole_hodnota_b[j+1],"%f",temp3);
            } 
        }
    }
    }
    float q10=0, q90=0;
    int aq10=0, bq10=0, aq90=0, bq90=0;
    q10=((0.1)*apocetzaznamovmodulu);
    q90=((0.9)*apocetzaznamovmodulu);
    int pomoc1=0; pomoc1 = (int)q10;
    if (q10>(pomoc1+0.5)){
        aq10=pomoc1+1;
    }
    else{
        aq10=pomoc1;
    }
    int pomoc2=0; pomoc2 = (int)q90;
    if (q90>(pomoc2+0.5)){
        aq90=pomoc2+1;
    }
    else{
        aq90=pomoc2;
    }
    q10=((0.1)*bpocetzaznamovmodulu);
    q90=((0.9)*bpocetzaznamovmodulu);
    pomoc1 = (int)q10;
    if (q10>(pomoc1+0.5)){
        bq10=pomoc1+1;
    }
    else{
        bq10=pomoc1;
    }
    pomoc2 = (int)q90;
    if (q90>(pomoc2+0.5)){
        bq90=pomoc2+1;
    }
    else{
        bq90=pomoc2;
    }

    //printf("pomoc: %d\n",pomoc1);
    //printf("q10: %f %f\n",q10, q90);
    //printf("kvartil: %d %d\n",aq10, aq90);
    //printf("kvartil: %d %d\n",bq10, bq90);
    /*for (int i=0;i<bpocetzaznamovmodulu;i++){
        printf("%s\n",pole_hodnota_b[i]);
    }
    for (int i=0;i<apocetzaznamovmodulu;i++){
        printf("%s\n",pole_hodnota_a[i]);
    }*/
    printf("%s\t%f\t(%s)\t%f\t(%s)\n",modul, amin, pole_hodnota_a[aq10], amax, pole_hodnota_a[aq90]);
    printf("%s\t%f\t(%s)\t%f\t(%s)\n",modul2, bmin, pole_hodnota_b[bq10], bmax, pole_hodnota_b[bq90]);
    float kvartila1, kvartila9, kvartilb1, kvartilb9;
    sscanf(pole_hodnota_a[aq10],"%f",&kvartila1);
    sscanf(pole_hodnota_a[aq90],"%f",&kvartila9);
    sscanf(pole_hodnota_b[bq10],"%f",&kvartilb1);
    sscanf(pole_hodnota_b[bq90],"%f",&kvartilb9);
    float rozdiel1, rozdiel2; 
    rozdiel1=(kvartila1-kvartilb1);
    rozdiel2=(kvartila9-kvartilb9);
    if((apocetzaznamovmodulu>0)&&(bpocetzaznamovmodulu>0)){
        if ((rozdiel1<2)&&(rozdiel1>(-2))&&(rozdiel2<2)&&(rozdiel2>(-2))){
            printf("%s a %s su v standardnej odchylke\n",modul, modul2);
        }
        else{
            printf("%s a %s prekracuju limit povoleneho rozmedzia\n",modul, modul2);
        }
    }
    else{
        printf("%s a %s sa nemozu porovnavat\n", modul, modul2);
    }
}
void fcia_x(){

}
int main(){
    FILE *f1=NULL;
    int v=0,n=0;
    char pismenko;
    int pocetzaznamov=0;
    char **pole_ID, **pole_modul, **pole_velicina, **pole_hodnota, **pole_cas, **pole_datum;
    while (1){
        scanf("%c", &pismenko);
        switch(pismenko){
            case 'v' : {
                fcia_v(&f1,&v,&n, &pole_ID, &pole_modul, &pole_velicina, &pole_hodnota, &pole_cas, &pole_datum, &pocetzaznamov) ;
                break;
            }
            case 'n' : {
                fcia_n(&f1,&v,&n, &pole_ID, &pole_modul, &pole_velicina, &pole_hodnota, &pole_cas, &pole_datum, &pocetzaznamov);
                break;
            }
            case 'o' : {
                fcia_o(&f1,&v,&n, &pole_modul, &pole_velicina, &pole_hodnota, &pole_cas, &pole_datum, &pocetzaznamov);
                break;
            }
            case 'c' : {
                fcia_c(&f1,&v);
                break;
            }
            case 's' : {
                fcia_s(&n,&pocetzaznamov,&pole_modul, &pole_velicina, &pole_hodnota, &pole_cas, &pole_datum);
                break;
            }
            case 'h': {
                fcia_h(&n,&pole_velicina, &pole_hodnota, &pocetzaznamov);
                break;
            }
            case 'z' : {
                fcia_z(&n, &pole_ID, &pole_modul, &pole_velicina, &pole_hodnota, &pole_cas, &pole_datum, &pocetzaznamov);
                break;
            }
            case 'r' : {
                fcia_r(&n, &pole_cas, &pocetzaznamov);
                break;
            }
            case 'j' : {
                fcia_j(&pole_modul, &pole_velicina, &pole_hodnota, &pocetzaznamov);
                break;
            }
            case 'x' : {
                fcia_x();
                break;
            }
            case 'k' : {
                if (v==0||n==0){
                    return 0;
                }
                else{
                    for(int i=0;i<pocetzaznamov;i++){
                        free(pole_ID[i]);
                        free(pole_modul[i]);
                        free(pole_velicina[i]);
                        free(pole_hodnota[i]);
                        free(pole_datum[i]);
                        free(pole_cas[i]);
                    }
                    free(pole_ID);
                    free(pole_modul);
                    free(pole_velicina);
                    free(pole_hodnota);
                    free(pole_datum);
                    free(pole_cas);
                    fclose(f1);
                    return 0;
                }
            }
        }
    }
}