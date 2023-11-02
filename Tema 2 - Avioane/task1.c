#include "./utils.h"
#define PATRU 4
#define MAX 51

void SolveTask1(void *info, int nr_avioane) {
    unsigned int viteza[MAX];
    unsigned char cod_avion[MAX][PATRU], directia[MAX];
    unsigned short linie[MAX], coloana[MAX];

    for (int i = 0; i <nr_avioane; i++) {
            linie[i] = *((unsigned short*) info);  // salvam din pointer un variabila de tip short
            info = info + sizeof(unsigned short);
            // in pointer se trece mai departe la spatiul de momorie alocat dupa tipul short

            coloana[i] =  *((unsigned short*) info);  // se continua in acelasi stil
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
        printf("(%d , %d)\n", linie[i], coloana[i]);
        printf("%c\n", directia[i]);
        for (int k = 0 ; k < PATRU ; k++) {
               printf("%c", cod_avion[i][k]);}
        printf("\n");
        printf("%u\n", viteza[i]);
        printf("\n");
    }
}
