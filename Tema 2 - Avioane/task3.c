#include "./utils.h"
#define PATRU 4
#define MAX 51
#define TREI 3

/*
sortare crescator dupa tip 1 sau 2
    pentru tipuri egale, descrescator dupa Codul Aeroportului bud<otp
        pentru coduri egale, crescator dupa viteza
*/
typedef struct {  // structura ce contine toate datele avioanelor
            unsigned short linie, coloana;
            int viteza;
            unsigned char directia, tip_avion, cod_aerop[TREI];}inf_avioane;

int comp(const void *p1, const void *p2) {
        const inf_avioane *c1 = (inf_avioane *) p1;
        const inf_avioane *c2 = (inf_avioane *) p2;
        if (c1->tip_avion < c2->tip_avion) return -1;  // compara tipul avionului
        else if (c1->tip_avion > c2->tip_avion) return 1;
        else if (strcmp (c1->cod_aerop, c2->cod_aerop) != 0) return (-1) * strcmp (c1->cod_aerop, c2->cod_aerop);
        // compara codul aeroportului
        else
            return c1->viteza - c2->viteza;}  // compara viteza

void SolveTask3(void *info, int nr_avioane) {
    inf_avioane avion[MAX] = {0};
    for (int i = 0; i <nr_avioane; i++) {
            avion[i].linie = *((unsigned short*) info);
            info = info + sizeof(unsigned short);

            avion[i].coloana =  *((unsigned short*) info);
            info = info + sizeof(unsigned short);

            avion[i].directia = *((unsigned char *) info);
            info = info + sizeof (unsigned char);

            avion[i].tip_avion = *((unsigned char *) info);
            info = info + sizeof (unsigned char);

            for (int k = 0 ; k < TREI ; k++) {
                avion[i].cod_aerop[k] = *((unsigned char *) info);
                info = info + sizeof (unsigned char);
            }

            avion[i].viteza = *((int *) info);
            info = info + sizeof(int);
    }

    qsort( avion, nr_avioane , sizeof(inf_avioane), comp);

    for (int i = 0; i < nr_avioane; i++) {
        printf("(%d, %d)\n", avion[i].linie, avion[i].coloana);
        printf("%c\n", avion[i].directia);
        printf("%c", avion[i].tip_avion);
        for (int k = 0 ; k < TREI ; k++) {
               printf("%c", avion[i].cod_aerop[k]);}
        printf("\n");
        printf("%u\n", avion[i].viteza);
        printf("\n");
    }
}
