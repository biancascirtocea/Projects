/*SCIRTOCEA Bianca-Ioana - 311CB*/
#include "lista2.h"

int main() {
    /*Se deschid fisierele*/
    FILE *fin = fopen("tema1.in", "rt");
    FILE *fout = fopen("tema1.out", "wt");

    TBanda* banda = NULL;
    TLista2 L = NULL;

    /*Se initializeaza stivele*/
    TStiva vf_undo = InitS();
    TStiva vf_redo = InitS();

    L = InitLista2();   /* creeaza santinela */
    if (!L) return 0;
    banda = InitBanda(L);  /* creeaza banda*/
    if (!banda) return 0;

    int nr = 0;
    fscanf(fin, "%d", &nr);  // Se citeste numarul de comenzi citite din fisier

    TCoada* com = InitQ();  // Se initializeaza coada pentru comenzi

    int i = 0;
    for (i = 0; i <= nr; i++) {
        char c[SUTA], x[SUTA];
        fgets(c, sizeof(c), fin);  // Se citeste din fisier pana la enter

        /*Se adauga comenzile specifice in coada */
        if (strncmp(c, "WRITE", CINCI) == 0) IntrQ(com, c);
        if (strncmp(c, "MOVE_LEFT_CHAR", TREIZ) == 0) IntrQ(com, c);
        else if (strncmp(c, "MOVE_LEFT\n", NOUA) == 0) IntrQ(com, c);
        if (strncmp(c, "MOVE_RIGHT_CHAR", CINCIZ) == 0) IntrQ(com, c);
        else if (strncmp(c, "MOVE_RIGHT\n", NOUA) == 0) IntrQ(com, c);
        if (strncmp(c, "INSERT_LEFT", NOUA) == 0) IntrQ(com, c);
        if (strncmp(c, "INSERT_RIGHT", NOUA) == 0) IntrQ(com, c);

        /*Se executa comenzile de afisare daca se intalnesc*/
        if (strncmp(c, "SHOW_CURRENT\n", ZECE) == 0) SHOW_CURRENT(banda, fout);
        else if (strncmp(c, "SHOW\n", PATRU) == 0) SHOW(banda, fout);

        /*Se executa comenzile de UNDO  si REDO*/
        if (strncmp(c, "UNDO\n", PATRU) == 0) UNDO(banda, &vf_undo, &vf_redo);
        if (strncmp(c, "REDO\n", PATRU) == 0) REDO(banda, &vf_redo, &vf_undo);

        if (strncmp(c, "EXECUTE", SAPTE) == 0) {
            int ok = ExtrQ(com, x);  // Se extrag din coada comenzile si se executa

            if (strncmp(x, "WRITE", CINCI) == 0) {
                    WRITE_C(banda, x[SASE]);
                    TLista2 el = NULL;
                    /*Se golesc stivele*/
                    while (vf_redo!= NULL) Pop(&vf_redo, &el);
                    while (vf_undo!= NULL) Pop(&vf_undo, &el);}

            if (strncmp(x, "MOVE_LEFT_CHAR", TREIZ) == 0) MOVE_LEFT_CHAR(banda, x[CINCIZ], fout);
            else if (strncmp(x, "MOVE_LEFT\n", NOUA) == 0)
                       {TLista2 el = banda->crt;
                        Push(&vf_undo, banda->crt);

                        /*Se salveaza in stiva UNDO adresa curentului pana sa se schime*/
                        MOVE_LEFT(banda);
                        if (el == banda->crt) UNDO(banda, &vf_undo, &vf_redo);}

            if (strncmp(x, "MOVE_RIGHT_CHAR", CINCIZ) == 0) MOVE_RIGHT_CHAR(banda, x[SASEZ]);
            else if (strncmp(x, "MOVE_RIGHT\n", NOUA) == 0)
                        {Push(&vf_undo, banda->crt);
                        /*Se salveaza in stiva UNDO adresa curentului pana sa se schime*/
                        MOVE_RIGHT(banda);}

            if (strncmp(x, "INSERT_LEFT", NOUA) == 0) INSERT_LEFT_CHAR(banda, x[DOIZ], fout);
            if (strncmp(x, "INSERT_RIGHT", NOUA) == 0) INSERT_RIGHT_CHAR(banda, x[TREIZ]);
        }
    }

    /*Eliberare memorie*/
    DistrQ(&com);
    DistrugeS(&vf_redo);
    DistrugeS(&vf_undo);
    DistrugeLista2(&(banda->L));
    free(banda);

    /*Inchidere fisiere*/
    fclose(fin);
    fclose(fout);

    return 0;
}
