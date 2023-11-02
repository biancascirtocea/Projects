/*SCIRTOCEA Bianca Ioana- 311CB*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUPERMAX 100000000
#define DOI 2
#define TREI 3
#define PATRU 4
#define SUTA 100
#define TREIZ 32
#define MAX 255

/*Matricea de pixeli a culorilor*/
typedef struct matrice {
    unsigned char red, green, blue;
}Imag, *Matrice;


typedef struct nod {
    Imag RGB;  // structura culorilor
    struct nod *unu, *doi, *trei, *patru;  // fii
    int niv;
    int tip;
} TNod, *TArb;

typedef struct celula {
  TArb nod;
  struct celula* urm;
} TCelula, *TLista;

typedef struct coada {
    /* adresa primei si ultimei celule */
    TLista inc, sf;
} TCoada;

/*Arbore cu patru fii*/
TArb Constr_Frunza();
TArb ConstrArb(Imag** a, int size, int x, int y, int factor, int nivel);
void DistrugeArb(TArb *adrArbore);

/*Calcul culori*/
unsigned long long Red(Imag** a, int size, int x, int y);
unsigned long long Blue(Imag** a, int size, int x, int y);
unsigned long long Green(Imag** a, int size, int x, int y);
unsigned long long Prag(Imag** a, int size, int x, int y, unsigned long long r,
 unsigned long long g, unsigned long long b);

/*Task 1*/
void Nr_niv(TArb r, int* nivel);
void Nr_nivminim(TArb r, int* max);
int Nr_fr(TArb r);
int Big_zone(TArb r, int size);

/*Task 2*/
void ParcNivel(TArb radacina, TCoada* c, FILE* fout);

/*Task 3*/
void ParcNivel3(TArb* radacina, TCoada* c, FILE* fin);
TArb Val_arb(TCoada *c, FILE* fin, int el_niv);
void ConstrImag3(Matrice** a, TArb r, int size, int x, int y);

/*Functiile pentru Coada*/
TCoada* InitQ();
int ExtrQ(TCoada*c, TArb *x);
int IntrQ(TCoada *c, TArb x);
void DistrQ(TCoada **c);
void AfisareQ(TCoada *c);
