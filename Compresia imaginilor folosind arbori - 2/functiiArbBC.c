/*SCIRTOCEA Bianca Ioana- 311CB*/
#include "arbBC.h"

/* Aloca un element de tip Arb si returneaza pointerul aferent */
TArb Constr_Frunza() {
    TArb aux = (TArb) malloc(sizeof(TNod));
    if (!aux)
        return NULL;
    aux->tip = 1;  // tipul este frunza intial
    /*fii sunt nuli*/
    aux->unu = aux->doi = aux->trei = aux->patru = NULL;

    return aux;
}

unsigned long long Red(Imag** a, int size, int x, int y) {
    /*se calculeaza culoarea red cu formula data pentru pozitiile x si y*/
    unsigned long long s = 0;
    for (int i = x; i < x+size; i++)
        for (int j = y; j < y+size; j++) {
            s += (unsigned long long) a[i][j].red;
        }
    unsigned long long p = size * size;
    return s / p;
}
unsigned long long Blue(Imag** a, int size, int x, int y) {
    /*se calculeaza culoarea red cu formula data pentru pozitiile x si y*/
    unsigned long long s = 0;
    for (int i = x; i < x+size; i++)
        for (int j = y; j < y+size; j++) {
            s += (unsigned long long) a[i][j].blue;
        }
    unsigned long long p = size * size;
    return s / p;
}
unsigned long long Green(Imag** a, int size, int x, int y) {
    /*se calculeaza culoarea red cu formula data pentru pozitiile x si y*/
    unsigned long long s = 0;
    for (int i = x; i < x+size; i++)
        for (int j = y; j < y+size; j++) {
            s += (unsigned long long) a[i][j].green;
        }
    unsigned long long p = size * size;
    return s / p;
}
unsigned long long Prag(Imag** a, int size, int x, int y,
 unsigned long long r, unsigned long long g, unsigned long long b) {
    /*se calculeaza culoarea pragul zonei respective
    cu formula data pentru pozitiile x si y*/
    unsigned long long s = 0;
    for (int i = x; i < x+size; i++) {
        for (int j = y; j < y+size; j++) {
            s += (r-(unsigned long long)a[i][j].red) * (r-(unsigned long long)a[i][j].red);
            s += (g-(unsigned long long)a[i][j].green) * (g-(unsigned long long)a[i][j].green);
            s += (b-(unsigned long long)a[i][j].blue) * (b-(unsigned long long)a[i][j].blue);
        }
    }
    return s / (3 * size * size);
}
TArb ConstrArb(Imag** a, int size, int x, int y, int factor, int nivel) {
    unsigned long long r = Red(a, size, x, y);
    unsigned long long g = Green(a, size, x, y);
    unsigned long long b = Blue(a, size, x, y);
    unsigned long long mean_val = Prag(a, size,  x, y, r, g, b);
    /*se calculeaza informatiile despre culoare a zonei date*/
    TArb nod  = Constr_Frunza();  // se construieste frunza
    if (!nod) return NULL;

    if (mean_val <= factor) {
        /*daca pragul calculat este mai mic sau egal decat factorul dat,
        atunci zona nu mai trebuie sa fie impartita si se completeza informatiile*/
        nod->RGB.blue = (unsigned char) Blue(a, size, x, y);
        nod->RGB.green = (unsigned char)Green(a, size, x, y);
        nod->RGB.red = (unsigned char) Red(a, size, x, y);
        nod->niv = nivel;
    } else {
        /*altfel se imparte zona in alte patru zone ce vor devenii fii arborelui, nivelul creste,
         iar pozitionarea matricei se schimba in functie de zona*/
        nod->tip = 0;
        nod->niv = nivel;
        nod->unu = ConstrArb(a, size/2, x, y, factor, nivel+1);
        nod->patru = ConstrArb(a, size/2, x + size/2, y, factor, nivel+1);
        nod->trei = ConstrArb(a, size/2, x + size/2, y + size/2, factor, nivel+1);
        nod->doi = ConstrArb(a, size/2, x, y + size/2, factor, nivel+1);
    }
    return nod;
}
void Nr_nivminim(TArb r, int* max) {
    if (!r) {
        return;
    }
    /*maximul se actualizeaza de fiecare data cand
     gasim recursiv o frunza cu un nivel mai mic decat el,
     astfel se calculeaza nivelul minim al unei frunze*/
    Nr_nivminim(r->unu, max);
    Nr_nivminim(r->doi, max);
    Nr_nivminim(r->trei, max);
    Nr_nivminim(r->patru, max);
    if (r->unu == NULL && r->doi == NULL && r->trei == NULL && r->patru == NULL)
       if (r->niv < (*max)) (*max) = r->niv;
}
void Nr_niv(TArb r, int *nivel) {
    if (r == NULL) {
        return;
    }
    /*nivel se actualizeaza de fiecare data cand
     gasim recursiv o frunza cu un nivel mai mare decat el,
     astfel se calculeaza nivelul maxim al unei frunze*/
    Nr_niv(r->unu, nivel);
    Nr_niv(r->doi, nivel);
    Nr_niv(r->trei, nivel);
    Nr_niv(r->patru, nivel);
if (r->unu == NULL && r->doi == NULL && r->trei == NULL && r->patru == NULL) {
    if (r->niv > (*nivel)) (*nivel) = r->niv;}
}
int Nr_fr(TArb r) {
    int contor = 0;
    if (r == NULL) {
        return 0;
    }

    /*se aduna recursiv toate frunzele gasite de pe toate ramurile*/
    contor += Nr_fr(r->unu);
    contor += Nr_fr(r->doi);
    contor += Nr_fr(r->trei);
    contor += Nr_fr(r->patru);
if (r->tip == 1)    // de fiecare data cand gasim o frunza se incrementeaza cu unu
contor++;

    return contor;
}
int Big_zone(TArb r, int size) {
    int max = SUPERMAX;
    Nr_nivminim(r, &max);  // se calculeaza nivelul minim pe care se gaseste o frunza
    /*acea frunza reprezinta in matrice zona cea mai mare nedivizata*/
    int imp = 1;
    for (int i = 1; i <= max; i++) {
        imp *= 2;  // se creeaza numarul la care s-a impartit matricea pana la prima zona mare
    }
    /*se imparte marimea imaginii la numarul de impartiri  la doi a ei pana la prima frunza*/
    return size/imp;
}

void ParcNivel(TArb radacina, TCoada* c, FILE* fout) {
    if (radacina == NULL)
        return;
    TArb aux = NULL;

    /*se introduce in coada radacina*/
    IntrQ(c, radacina);

    while (c != NULL) {
        /*cat timp coada este nevida se extrage un element din coada*/
        if (!ExtrQ(c, &aux)) return;
        if (aux->tip == 0) {
            /*daca elementul extras nu este de tip frunza se scrie in fisier doar tipul
            apoi se introduc in coada fii sai*/
            fwrite(&aux->tip, sizeof(unsigned char), 1, fout);
            IntrQ(c, aux->unu);
            IntrQ(c, aux->doi);
            IntrQ(c, aux->trei);
            IntrQ(c, aux->patru);
        } else {
            /*daca elementul extras est de tip frunza
             se scriu datele sale in fisier conform enuntului*/
            unsigned char str = (unsigned char) aux->tip;
            fwrite(&str, sizeof(unsigned char), 1, fout);
            fwrite(&aux->RGB.red, sizeof(unsigned char), 1, fout);
            fwrite(&aux->RGB.green, sizeof(unsigned char), 1, fout);
            fwrite(&aux->RGB.blue, sizeof(unsigned char), 1, fout);
        }
    }
}

TArb Val_arb(TCoada *c, FILE* fin, int el_niv) {
        TArb fiu  = Constr_Frunza();
        if (!fiu) return NULL;
        /*se creeaza un fiu al alborelui*/

        unsigned char tip = 0, red[TREIZ], blue[TREIZ], green[TREIZ];
        fread(&tip, sizeof(unsigned char), 1, fin);
        /*se citeste din fisier tipul*/
        if ((int)tip == 0) {
            /*daca nu este frunza se actualizeaza datele cu 0 legate de culoare, se creste cu 1
            nivelul (deoarece cel de dinainte era al tatalui) si se introduce in coada
            pentru a fi reapelat mai tarziu*/
            fiu->RGB.blue = (unsigned char) 0;
            fiu->RGB.green = (unsigned char) 0;
            fiu->RGB.red = (unsigned char) 0;
            fiu->niv = el_niv + 1;
            fiu->tip = tip;
            IntrQ(c, fiu);
        } else {
            /*altfel, daca e frunza se citesc datele din fisier legate de culoare si se atribuie*/
            fread(&red, sizeof(unsigned char), 1, fin);
            fread(&green, sizeof(unsigned char), 1, fin);
            fread(&blue, sizeof(unsigned char), 1, fin);
            fiu->RGB.blue = (*blue);
            fiu->RGB.green = (*green);
            fiu->RGB.red = (*red);
            fiu->niv = el_niv + 1;  // se creste nivelul la fel
            fiu->tip = tip;  // si se scrie tipul sau de frunza
            }
            return fiu;  // se returneaza fiul
}

void ParcNivel3(TArb* radacina, TCoada* c, FILE* fin) {
    if (radacina == NULL)
        return;
    TArb aux = NULL;
    int el_niv = 0;
    /*cat timp coada nu e vida*/
    while (c->inc != NULL) {
        /*se extrage un element din coada*/
        if (!ExtrQ(c, &aux)) return;
        /*se verifica daca nivelul elementului e la fel ca cel al celui de dinainte,
        daca nu e se actualizeaza, inseamna ca am trecut pe urmatorul nivel*/
        if (aux->niv != el_niv) el_niv = aux->niv;
        /*se apeleaza pentru fiecare fiu sa i se scrie informatiile*/
        aux->unu = Val_arb(c, fin, el_niv);
        aux->doi = Val_arb(c, fin, el_niv);
        aux->trei = Val_arb(c, fin, el_niv);
        aux->patru = Val_arb(c, fin, el_niv);
    }
}

void ConstrImag3(Matrice** a, TArb r, int size, int x, int y) {
    /*daca tipul elementului este frunza, atunci putem completa
     matricea de la pozitia x si y pana la marginea zonei respective */
    if (r->tip == 1) {
        for (int i = x; i < x + size; i++) {
            for (int j = y; j < y + size; j++) {
                (*a)[i][j].red = r->RGB.red;
                (*a)[i][j].green = r->RGB.green;
                (*a)[i][j].blue = r->RGB.blue;
            }
        }
    } else {
        /*daca nu este frunza atunci marimea zonei se imparte la doi si se
        reapelaza functie pentru fiecare noua zona in parte, cu size/2 si cu
        pozitiile lui x si y deplasate in functie de pozitie*/
        int p = size/2;
        if (r->unu) ConstrImag3(a, r->unu, p, x, y);
        if (r->doi) ConstrImag3(a, r->doi, p, x, y + p);
        if (r->trei) ConstrImag3(a, r->trei, p, x + p, y + p);
        if (r->patru) ConstrImag3(a, r->patru, p, x + p, y);
    }
}


/*Functii pentru coada*/
/* creeaza coada vida cu elemente de dimensiune d; anumite implementari
 pot necesita si alti parametri */
TCoada* InitQ() {
    TCoada* c = NULL;          /* spatiu pentru descriptor coada */
    c = (TCoada*)malloc(sizeof(TCoada));
    if (!c) return NULL;                  /* nu exista spatiu -> "esec" */

    c->inc = c->sf = NULL;
    return c;          /* intoarce adresa descriptorului cozii */
}
/* adauga element la sfarsitul cozii */
int IntrQ(TCoada *c, TArb x) {
    TLista aux = NULL;
    aux = (TLista)malloc(sizeof(TCelula));      /* aloca o noua celula */
    if (!aux) return 0;             /* alocare imposibila -> "esec" */
    aux->nod = x;

    if (c->sf != NULL) /* coada nevida */
        c->sf->urm = aux; /* -> leaga celula dupa ultima din coada */
    else /* coada vida */
        c->inc = aux; /* -> noua celula se afla la inceputul cozii */
    c->sf = aux; /* actualizeaza sfarsitul cozii */
    return 1; /* operatie reusita -> "succes" */
}
/* extrage primul element din coada la adresa ae */
int ExtrQ(TCoada *c, TArb *x) {
    TLista aux = NULL;
    if (c->inc == NULL) {
        return 0;
    } else {
        if (c->inc == c->sf) {
            *x = c->inc->nod;
            aux = c->inc;
            c->inc = NULL;
            c->sf = NULL;
            free(aux);
        } else {
            *x = c->inc->nod;
            aux = c->inc;
            c->inc = c->inc->urm;
            free(aux);
        }
    }
    return 1;
}
/* distruge coada */
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
/* afisare elementele cozii */
void AfisareQ(TCoada *c) {
  TLista p = NULL;
  /* coada vida */
  if (c->inc == NULL) {
    printf("Coada vida\n");
    return;
  }
  printf("Elementele cozii: ");
  for (p = c->inc; p != NULL; p = p->urm)
    printf("%d ", p->nod->RGB.red);
  printf("\n");
}



/* Dezalocare memorie arbore */
void DistrugeArb(TArb* adrArbore) {
if (!(*adrArbore)) {
    return;
}

DistrugeArb(&(*adrArbore)->unu);
DistrugeArb(&(*adrArbore)->doi);
DistrugeArb(&(*adrArbore)->trei);
DistrugeArb(&(*adrArbore)->patru);

free(*adrArbore);
}
