#include "./utils.h"
#define DOI 2
#define TREI 3
#define PATRU 4
#define MAX 51

void SolveTask4(void *info, int nr_avioane, int nr_obstacole, int *x, int *y, int N) {
    unsigned int viteza[MAX];
    unsigned char cod_avion[MAX][PATRU], directia[MAX];
    unsigned short linie[MAX], coloana[MAX];

    for (int i = 0; i <nr_avioane; i++) {  // se citesc exact ca la taskul 1 datele avionului
            linie[i] = *((unsigned short*) info);
            info = info + sizeof(unsigned short);

            coloana[i] =  *((unsigned short*) info);
            info = info + sizeof(unsigned short);

            directia[i] = *((unsigned char *) info);
            info = info + sizeof (unsigned char);

            for (int k = 0 ; k < PATRU ; k++) {
                cod_avion[i][k] = *((unsigned char *) info);
                info = info + sizeof (unsigned char);
            }

            viteza[i] = *((unsigned int *) info);
            info = info + sizeof(unsigned int);
    }

    int nr = 0;
    for (int i = 0; i < nr_avioane; i++) {
        int col = coloana[i];
        int lin = linie[i];
        unsigned char dir = directia[i];
        unsigned char tip = cod_avion[i][0];
        int ok = 1;
        /*in functie de tipul si directia avionului se verifica daca exista cate un obstacol 
        peste care ar putea trece aripile mari ale avionului, precum si cele mici*/

        if (tip ==  '1') {
            if (dir == 'N') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (x[j] <= lin+1) {if (y[j] >= col-DOI && y[j] <= col+DOI) ok = 0;}
                    if (lin+DOI == x[j] && col-1 == y[j]) ok = 0;
                    if (lin+DOI == x[j] && col+1 == y[j]) ok = 0;}
            } else if (dir == 'S') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (x[j] >= lin-1) if (y[j] >= col-DOI && y[j] <= col+DOI) ok = 0;
                    if (lin-DOI == x[j] && col-1 == y[j]) ok = 0;
                    if (lin-DOI == x[j] && col+1 == y[j]) ok = 0;}
            } else if (dir == 'E') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (y[j] >= col-1) if (x[j] >= lin-DOI && x[j] <= lin+DOI) ok = 0;
                    if (lin+1 == x[j] && col-DOI == y[j]) ok = 0;
                    if (lin-1 == x[j] && col-DOI == y[j]) ok = 0;}
            } else if (dir == 'W') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (y[j] <= col+1) if (x[j] >= lin-DOI && x[j] <= lin+DOI) ok = 0;
                    if (lin-1 == x[j] && col+DOI == y[j]) ok = 0;
                    if (lin+1 == x[j] && col+DOI == y[j]) ok = 0;}
            }
        } else if (tip == '2') {
            if (dir == 'N') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (x[j] <= lin+2) if (y[j] >= col-TREI && y[j] <= col+TREI) ok = 0;
                    if (lin+TREI == x[j] && col-1 == y[j]) ok = 0;
                    if (lin+TREI == x[j] && col+1 == y[j]) ok = 0;
                    if (lin+TREI == x[j] && col-DOI == y[j]) ok = 0;
                    if (lin+TREI == x[j] && col+DOI == y[j]) ok = 0;}
            } else if (dir == 'S') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (x[j] >= lin-2) if (y[j] >= col-TREI && y[j] <= col+TREI) ok = 0;
                    if (lin-TREI == x[j] && col-1 == y[j]) ok = 0;
                    if (lin-TREI == x[j] && col+1 == y[j]) ok = 0;
                    if (lin-TREI == x[j] && col-DOI == y[j]) ok = 0;
                    if (lin-TREI == x[j] && col+DOI == y[j]) ok = 0;}
            } else if (dir == 'E') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (y[j] >= col-2) if (x[j] >= lin-TREI && x[j] <= lin+TREI) ok = 0;
                    if (lin+1 == x[j] && col-TREI == y[j]) ok = 0;
                    if (lin+DOI == x[j] && col-TREI == y[j]) ok = 0;
                    if (lin-1 == x[j] && col-TREI == y[j]) ok = 0;
                    if (lin-DOI == x[j] && col-TREI == y[j]) ok = 0;}
            } else if (dir == 'W') {
                for (int j = 0; j < nr_obstacole; j++) {
                    if (y[j] <= col+2) if (x[j] >= lin-TREI && x[j] <= lin+TREI) ok = 0;
                    if (lin+1 == x[j] && col+TREI == y[j]) ok = 0;
                    if (lin+DOI == x[j] && col+TREI == y[j]) ok = 0;
                    if (lin-1 == x[j] && col+TREI == y[j]) ok = 0;
                    if (lin-DOI == x[j] && col+TREI == y[j]) ok = 0;}
            }
        }
        if (ok == 1) nr = nr + 1;
    }
    printf("%d", nr);
}
