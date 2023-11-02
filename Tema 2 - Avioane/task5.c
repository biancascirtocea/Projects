#include "./utils.h"
#define DOI 2
#define TREI 3
#define PATRU 4
#define MAX 51

void SolveTask5(void *info, int nr_avioane, int T, int nr_pct_coord, int *X, int *Y, int N) {
    unsigned int viteza[MAX];
    unsigned char cod_avion[MAX][PATRU], directia[MAX];
    unsigned short linie[MAX], coloana[MAX];

    for (int i = 0; i <nr_avioane; i++) {
            linie[i] = *((unsigned short*) info);  // se citesc valorile din pointer exact ca la taskul 1
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

    for (int k = 0; k <= T; k++) {
        int nr = 0;
        for (int i = 0; i < nr_avioane; i++) {
            int col = coloana[i];
            int lin = linie[i];
            unsigned char dir = directia[i];
            unsigned char tip = cod_avion[i][0];
            int ok = 0;
            unsigned int mom = viteza[i] * k;

            /* toate informatiile despre avion se testeaza la timpul T 
            si in momentul mom cand se trece la urmatorul timp avioanele 
            se deplaseaza cu v spatii in directia lor, adica mom+=v */

            if (tip ==  '1') {
                if (dir == 'N') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin-mom && col_obs == col) ok = 1;
                        // se verifica daca exista un obstacol la capul avionului
                        if (lin_obs == lin-mom+2 && col_obs == col) ok = 1;
                        // se verifica daca exista un obstacol in spatiul dintre aripi si coada
                        if (lin_obs == lin-mom+1 && col_obs >= col-2 && col_obs <= col+2) ok = 1;
                        // se verifica daca exista un obstacol in linia aripilor
                        if (lin_obs == lin-mom+3 && col_obs >= col-1 && col_obs <= col+1) ok = 1;
                        // se verifica daca exista un obstacol in coada avionului
                        // cum avionul se deplaseaza inspre N, adica in sus, mom pozitii se scad din linii
                    }
                } else if (dir == 'S') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin+mom && col_obs == col) ok = 1;
                        if (lin_obs == lin+mom-2 && col_obs == col) ok = 1;
                        if (lin_obs == lin+mom-1 && col_obs >= col-2 && col_obs <= col+2) ok = 1;
                        if (lin_obs == lin+mom-3 && col_obs >= col-1 && col_obs <= col+1) ok = 1;
                        // cum avionul se deplaseaza inspre S, adica in jos, mom pozitii se aduna la linii
                    }
                } else if (dir == 'E') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin && col_obs == col+mom) ok = 1;
                        if (lin_obs == lin && col_obs == col+mom-2) ok = 1;
                        if (col_obs == col+mom-1 && lin_obs >= lin-2 && lin_obs <= lin+2) ok = 1;
                        if (col_obs == col+mom-3 && lin_obs >= lin-1 && lin_obs <= lin+1) ok = 1;
                        // cum avionul se deplaseaza inspre E, adica in dreapta, mom pozitii se aduna la coloane
                    }
                } else if (dir == 'W') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin && col_obs == col-mom) ok = 1;
                        if (lin_obs == lin && col_obs == col-mom+2) ok = 1;
                        if (col_obs == col-mom+1 && lin_obs >= lin-2 && lin_obs <= lin+2) ok = 1;
                        if (col_obs == col-mom+3 && lin_obs >= lin-1 && lin_obs <= lin+1) ok = 1;
                        // cum avionul se deplaseaza inspre W, adica in stanga, mom pozitii se scad din coloane
                    }
                }
            } else if (tip == '2') {
                if (dir == 'N') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin-mom && col_obs == col) ok = 1;
                        // se verifica daca exista obstacol in locul capului avionului
                        if (lin_obs == lin-mom+1 && col_obs >= col-1 && col_obs <= col+1) ok = 1;
                        // se verifica daca exista obstacol in corpul dintre cap si aripile avionului
                        if (lin_obs == lin-mom+2 && col_obs >= col-3 && col_obs <= col+3) ok = 1;
                        // se verifica daca exista obstacol in lungimea aripilor
                        if (lin_obs == lin-mom+3 && col_obs == col) ok = 1;
                        // se verifica daca exista obstacol in corpul dintre aripi si coada
                        if (lin_obs == lin-mom+4 && col_obs >= col-2 && col_obs <= col+2) ok = 1;
                        // se verifica daca exista obstacol in coada
                    }
                } else if (dir == 'S') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin+mom && col_obs == col) ok = 1;
                        if (lin_obs == lin+mom-1 && col_obs >= col-1 && col_obs <= col+1) ok = 1;
                        if (lin_obs == lin+mom-2 && col_obs >= col-3 && col_obs <= col+3) ok = 1;
                        if (lin_obs == lin+mom-3 && col_obs == col) ok = 1;
                        if (lin_obs == lin+mom-4 && col_obs >= col-2 && col_obs <= col+2) ok = 1;
                    }
                } else if (dir == 'E') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin && col_obs == col+mom) ok = 1;
                        if (col_obs == col+mom-1 && lin_obs >= lin-1 && lin_obs <= lin+1) ok = 1;
                        if (col_obs == col+mom-2 && lin_obs >= lin-3 && lin_obs <= lin+3) ok = 1;
                        if (lin_obs == lin && col_obs == col+mom-3) ok = 1;
                        if (col_obs == col+mom-4 && lin_obs >= lin-2 && lin_obs <= lin+2) ok = 1;
                    }
                } else if (dir == 'W') {
                    for (int j = 0; j < nr_pct_coord; j++) {
                        int lin_obs = X[j], col_obs = Y[j];
                        if (lin_obs == lin && col_obs == col-mom) ok = 1;
                        if (col_obs == col-mom+1 && lin_obs >= lin-1 && lin_obs <= lin+1) ok = 1;
                        if (col_obs == col-mom+2 && lin_obs >= lin-3 && lin_obs <= lin+3) ok = 1;
                        if (lin_obs == lin && col_obs == col-mom+3) ok = 1;
                        if (col_obs == col-mom+4 && lin_obs >= lin-2 && lin_obs <= lin+2) ok = 1;
                   }
                }
            }
            if (ok == 1) nr++;
        }
        printf("%d: %d\n", k, nr);
    }
}
