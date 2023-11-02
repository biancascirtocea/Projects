#include "./utils.h"
#define PATRU 4
#define MAX 51

void SolveTask2(void *info, int nr_avioane, int N, char **mat) {
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

    for (int i = 0; i < nr_avioane; i++) {
        int col = coloana[i];
        int lin = linie[i];
        unsigned char dir = directia[i];
        unsigned char tip = cod_avion[i][0];
        mat[lin][col] = '*';  // pt fiecare avion se completeaza cu * in matrice capul acestuia
        // in functie de tipul si directia lui se completeaza si corpul

        if (tip ==  '1') {
            if (dir == 'N') {
                mat[lin+1][col]= '*';
                mat[lin+1][col+1] = mat[lin+1][col+2] = mat[lin+1][col-1] = mat[lin+1][col-2] = '*';
                mat[lin+2][col] = '*';
                mat[lin+3][col] = mat[lin+3][col-1] = mat[lin+3][col+1] = '*';
            } else if (dir == 'S') {
                mat[lin-1][col]= '*';
                mat[lin-1][col+1] = mat[lin-1][col+2] = mat[lin-1][col-1] = mat[lin-1][col-2] = '*';
                mat[lin-2][col] = '*';
                mat[lin-3][col] = mat[lin-3][col-1] = mat[lin-3][col+1] = '*';
            } else if (dir == 'E') {
                mat[lin][col-1]= '*';
                mat[lin-1][col-1] = mat[lin-2][col-1] = mat[lin+1][col-1] = mat[lin+2][col-1] = '*';
                mat[lin][col-2] = '*';
                mat[lin][col-3] = mat[lin-1][col-3] = mat[lin+1][col-3] = '*';
            } else if (dir == 'W') {
                mat[lin][col+1]= '*';
                mat[lin-1][col+1] = mat[lin-2][col+1] = mat[lin+1][col+1] = mat[lin+2][col+1] = '*';
                mat[lin][col+2] = '*';
                mat[lin][col+3] = mat[lin-1][col+3] = mat[lin+1][col+3] = '*';
            }
        } else if (tip == '2') {
            if (dir == 'N') {
                mat[lin+1][col]= '*';
                mat[lin+1][col+1] = mat[lin+1][col-1] = '*';
                mat[lin+2][col] = mat[lin+2][col+1] = mat[lin+2][col+2] = mat[lin+2][col+3] = mat[lin+2][col-1] = '*';
                mat[lin+2][col-2] = mat[lin+2][col-3] = '*';
                mat[lin+3][col] = '*';
                mat[lin+4][col] = mat[lin+4][col+2] = mat[lin+4][col+1] = mat[lin+4][col-1] = mat[lin+4][col-2] = '*';
            } else if (dir == 'S') {
                mat[lin-1][col]= '*';
                mat[lin-1][col+1] = mat[lin-1][col-1] = '*';
                mat[lin-2][col] = mat[lin-2][col+1] = mat[lin-2][col+2] = mat[lin-2][col+3] = mat[lin-2][col-1] = '*';
                mat[lin-2][col-2] = mat[lin-2][col-3] = '*';
                mat[lin-3][col] = '*';
                mat[lin-4][col] = mat[lin-4][col+2] = mat[lin-4][col+1] = mat[lin-4][col-1] = mat[lin-4][col-2] = '*';
            } else if (dir == 'E') {
                mat[lin][col-1]= '*';
                mat[lin+1][col-1] = mat[lin-1][col-1] = '*';
                mat[lin][col-2] = mat[lin-1][col-2] = mat[lin-2][col-2] = mat[lin-3][col-2] = mat[lin+1][col-2] = '*';
                mat[lin+2][col-2] = mat[lin+3][col-2] = '*';
                mat[lin][col-3] = '*';
                mat[lin][col-4] = mat[lin-1][col-4] = mat[lin-2][col-4] = mat[lin+1][col-4] = mat[lin+2][col-4] = '*';
            } else if (dir == 'W') {
                mat[lin][col+1]= '*';
                mat[lin+1][col+1] = mat[lin-1][col+1] = '*';
                mat[lin][col+2] = mat[lin-1][col+2] = mat[lin-2][col+2] = mat[lin-3][col+2] = mat[lin+1][col+2] = '*';
                mat[lin+2][col+2] = mat[lin+3][col+2] = '*';
                mat[lin][col+3] = '*';
                mat[lin][col+4] = mat[lin-1][col+4] = mat[lin-2][col+4] = mat[lin+1][col+4] = mat[lin+2][col+4] = '*';
            }
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
        printf("%c", mat[i][j]);}
        printf("\n");
    }
}
