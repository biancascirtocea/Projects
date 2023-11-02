/*SCIRTOCEA Bianca-Ioana - 311CB*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SUTA 100
#define NOUA 9
#define CINCI 5
#define TREI 3
#define ZECE 10
#define PATRU 4
#define CINCIZ 15
#define SASEZ 16
#define SAPTE 7
#define SASE 6
#define TREIZ 13
#define DOIZ 12

/* definire lista dublu inlantuita cu santinela, stiva si coada */

typedef struct celula2 {
  char info;
  struct celula2 *pre, *urm;
} TCelula2, *TLista2;

typedef struct banda2 {
  TLista2 L;
  TLista2 crt;
} TBanda;

typedef struct celula {
  char info[SUTA];
  struct celula* urm;
} TCelula, *TLista;

typedef struct coada {
  TLista inc, sf;  // adresa primei si ultimei celule
} TCoada;

typedef struct celst {
  TLista2 info;
  struct celst *urm;
} TCelulaStiva, *TStiva;

TLista2 AlocCelula2(char x);
TLista2 InitLista2();

TBanda* InitBanda(TLista2 L);

/*Antetul functiilor pentru coada*/
TCoada* InitQ();
int IntrQ(TCoada *c, char x[SUTA]);
int ExtrQ(TCoada *c, char *x);
void AfisareQ(TCoada *c);

/*Antetul functiilor pentru stiva*/
TStiva InitS(void);
int Pop(TStiva* vf, TLista2* ax);
int Push(TStiva* vf, TLista2 x);
void AfisareS(TStiva c);

void UNDO(TBanda* banda, TStiva* vf_undo, TStiva* vf_redo);
void REDO(TBanda* banda, TStiva* vf_redo, TStiva* vf_undo);

void INSERT_LEFT_CHAR(TBanda* banda, char x, FILE * fout);
void INSERT_RIGHT_CHAR(TBanda* banda, char x);

void MOVE_LEFT(TBanda* banda);
void MOVE_RIGHT(TBanda* banda);

void MOVE_LEFT_CHAR(TBanda* banda, char x, FILE * fout);
void MOVE_RIGHT_CHAR(TBanda* banda, char x);

void WRITE_C(TBanda* banda, char x);

void SHOW(TBanda* banda, FILE* fout);
void SHOW_CURRENT(TBanda* banda, FILE* fout);

void DistrQ(TCoada **c);
void DistrugeS(TStiva* vf);
void DistrugeLista2(TLista2* aL);
