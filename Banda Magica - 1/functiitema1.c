/*SCIRTOCEA Bianca-Ioana - 311CB*/
#include "lista2.h"

/* Aloca un element de tip TCelula2 si returneaza pointerul aferent */
TLista2 AlocCelula2(char x) {
    TLista2 aux = (TLista2) malloc(sizeof(TCelula2));
    if (!aux) {
        return NULL;
    }

    aux->info = x;
    aux->pre = aux->urm = NULL;

    return aux;
}

/*Se adauga un element in lista*/
void Add(TLista2 *rez ) {
    char x = '#';
    TLista2 aux = AlocCelula2(x);
    if (!aux) return;
    aux->pre = (*rez);
    (*rez)->urm = aux;
    aux->urm = NULL;
}

/* Creeaza santinela pentru lista folosita */
TLista2 InitLista2() {
    TLista2 aux = (TLista2) malloc(sizeof(TCelula2));
    if (!aux) {
        return NULL;
    }

    aux->info = 0;                  /* informatia din santinela are valoarea 0 */
    aux->pre = NULL;
    aux->urm = NULL;
    Add(&aux);

    return aux;
}

/*Alocare banda*/
TBanda* InitBanda(TLista2 L) {
    TBanda* banda = (TBanda*)malloc(sizeof(TBanda));
    banda->L = L;
    banda->crt = L->urm;
    return banda;
}

/* creeaza coada vida cu elemente de dimensiune d;
anumite implementari pot necesita si alti parametri */
TCoada* InitQ() {
  TCoada* c = NULL;          /* spatiu pentru descriptor coada */
  c = (TCoada*)malloc(sizeof(TCoada));
  if (!c) return NULL;                  /* nu exista spatiu -> "esec" */

  c->inc = c->sf = NULL;
  return c;          /* intoarce adresa descriptorului cozii */
}

/*Initializare stiva*/
TStiva InitS(void) {
    return NULL;
}

/* adauga element la sfarsitul cozii */
int IntrQ(TCoada *c, char x[SUTA]) {
    TLista aux = NULL;
    aux = (TLista)malloc(sizeof(TCelula));  // aloca o noua celula
    if (!aux) return 0;  // alocare imposibila -> "esec"

    snprintf(aux->info, strlen(x)+1, "%s", x);
    aux->urm = NULL;

    if (c->sf != NULL) c->sf->urm = aux;  // coada nevida -> leaga celula dupa ultima din coada
    else
        c->inc = aux;  // coada vida -> noua celula se afla la inceputul cozii

    /* actualizeaza sfarsitul cozii */
    c->sf = aux;
    return 1;  // operatie reusita -> "succes"
}

/* extrage primul element din coada la adresa ae */
int ExtrQ(TCoada *c, char *x) {
    TLista aux = NULL;
    if (c->inc == NULL || c->sf == NULL) return 0;
    else
        {if (c->inc == c->sf) {
            snprintf(x, strlen(c->inc->info)+1, "%s", c->inc->info);
            aux = c->inc;
            c->inc = NULL;
            c->sf = NULL;
            free(aux);
            } else {snprintf(x, strlen(c->inc->info)+1, "%s", c->inc->info);
                aux = c->inc;
                c->inc = c->inc->urm;
                free(aux);}
            return 1;
        }
}

/*Se adauga element in stiva*/
int Push(TStiva* vf, TLista2 x) {
  TStiva aux = (TStiva)malloc(sizeof(TCelulaStiva));
  if (!aux) return 0;

  aux->info = x; aux->urm = NULL;
  aux->urm = (*vf);
  (*vf) = aux;

  return 1;
}

/*Se sterge element din lista*/
int Pop(TStiva* vf, TLista2* ax) {
  TStiva aux = NULL;
  if ((*vf) == NULL) return 0;

  (*ax) = (*vf)->info;
  aux = *vf;
  (*vf) = aux->urm;

  free(aux);

  return 1;
}

void MOVE_LEFT(TBanda* banda) {
    /*Verifica daca degetul se afla pe prima pozitie dupa santinela*/
    if (banda->crt->pre != (banda)->L) banda->crt = banda->crt->pre;
}

void MOVE_RIGHT(TBanda* banda) {
    TLista2 p = banda->crt;

    /* Se verifica daca degetul se afla la finalul listei, pentru a se adauga o noua celula*/
    if (p->urm == NULL)  Add(&banda->crt);
    banda->crt = banda->crt->urm;
}

void MOVE_LEFT_CHAR(TBanda* banda, char x, FILE * fout) {
    TLista2 p = banda->crt, aux = NULL;
    if (banda->crt->info == x) return;

    for (aux = p; aux != banda->L; aux=aux->pre)
        if (aux->info == x) {
            banda->crt = aux;
            return;
        }

    /*Daca nu se gaseste elementul si 
    nu se incheie functia in for se afiseaza mesajul de eroare*/
    fprintf(fout, "ERROR\n");
}

void MOVE_RIGHT_CHAR(TBanda* banda, char x) {
    TLista2 p = banda->crt, aux = NULL;
    if (banda->crt->info == x) return;

    for (aux = p->urm; aux != NULL; aux = aux->urm, banda->crt = banda->crt->urm)
        if (aux->info == x) {
            banda->crt = banda->crt->urm;
            return;
        }

    /*Daca se iese din for inseamna ca nu s-a gasit elementul*/
    Add(&banda->crt);  // Se adauga o noua celula
    banda->crt = banda->crt->urm;  // Degetul se muta pe noua celula
}

void INSERT_RIGHT_CHAR(TBanda* banda, char x) {
    TLista2 p = banda->crt, aux = NULL;

    if (p->urm != NULL) {
        aux = AlocCelula2(x);
        if (!aux) return;

        /*Se schimba legaturile pentru a se insera noua celula*/
        aux->urm = p->urm;
        aux->pre = p;
        p->urm = aux;
        aux->urm->pre = aux;}
    else
        Add(&p);

    banda->crt = banda->crt->urm;
    banda->crt->info = x;
}

void INSERT_LEFT_CHAR(TBanda* banda, char x, FILE *fout) {
    TLista2 p = banda->crt, aux = NULL;

    /*Daca degetul se afla la primul element dupa santinela apare mesajul de eroare */
    if (p == banda->L->urm) {
        fprintf(fout, "ERROR\n");
        return; }

    aux = AlocCelula2(x);
    if (!aux) return;

    /*Se schimba legaturile pentru a se insera noua celula*/
    aux->urm = p;
    aux->pre = p->pre;
    p->pre->urm = aux;
    aux->urm->pre = aux;

    banda->crt = banda->crt->pre;  // Se modifica pozitia degetului
}

/*Actualizarea caracterului curent*/
void WRITE_C(TBanda* banda, char x) {
    TLista2 p = banda->crt;
    p->info = x;
}

/* Afisare continut lista */
void SHOW(TBanda* banda, FILE* fout) {
    TLista2 p = banda->L->urm;
    while (p != NULL) {         /* parcurgere lista */
        if (p == banda->crt) fprintf(fout, "|");  // elemetul la care se afla degetul
        fprintf(fout, "%c", p->info);
        if (p == banda->crt) fprintf(fout, "|");  // elemetul la care se afla degetul
        p = p->urm;
    }
    fprintf(fout, "\n");
}

/*Afisare element curent*/
void SHOW_CURRENT(TBanda* banda, FILE* fout) {
    fprintf(fout, "%c\n", banda->crt->info);
}

void UNDO(TBanda* banda, TStiva* vf_undo, TStiva* vf_redo) {
  TLista2 el = NULL;
  el = banda->crt;
  Push(vf_redo, el);  // adaugam pointerul pozitiei curente in stiva de redo
  Pop(vf_undo, &el);  // eliminam pointerul pozitiei curente din stiva undo pentru a il utiliza
  banda->crt = el;  // pozitia degetului devine vechea pozitie, cea preluata din stiva undo
}

void REDO(TBanda* banda, TStiva* vf_redo, TStiva* vf_undo) {
  TLista2 el = NULL;
  el = banda->crt;
  Push(vf_undo, el);  // adaugam pointerul pozitiei curente in stiva de undo
  Pop(vf_redo, &el);  // eliminam pointerul pozitiei curente din stiva redo pentru a il utiliza
  banda->crt = el;  // pozitia degetului pe banda se schimba  cu cea din stiva redo
}

/* distruge coada - elibereaza memoria */
void DistrQ(TCoada **c) {
  TLista p = NULL, aux = NULL;
  p = (*c)->inc;
  while (p) {
    aux = p;
    p = p->urm;
    free(aux);
  }
  free(*c);
  *c = NULL;
}

/* distruge stiva - elibereaza memoria */
void DistrugeS(TStiva* vf) {
  TStiva aux = NULL;
  while (*vf) {
    aux = *vf;
    (*vf) = (*vf)->urm;
    free(aux);
  }
}

/* Distrugere lista - elibereaza memoria */
void DistrugeLista2(TLista2 *aL) {
    TLista2 p = (*aL)->urm, aux = NULL;
    while (p != NULL) {         /* distrugere elementele listei */
        aux = p;
        p = p->urm;
        free(aux);
    }

    free(*aL);                  /* distrugere santinela */
    *aL = NULL;
}
