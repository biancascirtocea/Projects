/*SCIRTOCEA Bianca Ioana- 311CB*/
#include "arbBC.h"

int main(int argc, char const *argv[]) {
    /*Task 1*/
    if (strcmp(argv[1], "-c1") == 0) {
        /*cel de al doilea argument trebuie sa fie -c1 pentru taskul 1*/

        /*se transforma factorul din argv[2] din char in integer*/
        int factor = atoi(argv[DOI]);
        /*se deschid fisierele*/
        FILE *fin = fopen(argv[TREI], "rb");
        if (!fin) return 0;
        FILE *fout = fopen(argv[PATRU], "wt");
        if (!fout) return 0;

        /*se citesc informatiile despre fisier*/
        char tip[SUTA], carac = 0;
        fscanf(fin, "%s\n", tip);
        int width = 0, height = 0, val_max = 0;
        fscanf(fin, "%d\n", &width);
        fscanf(fin, "%d\n", &height);
        fscanf(fin, "%d", &val_max);
        fscanf(fin, "%c", &carac);

        /*se aloca memoria pentru matricea de pixeli*/
        Imag** a = (Imag**) malloc(sizeof(Imag*) * height);
        if (!a) return 0;

        for (int i = 0; i < width;  i++) {
            a[i] = (Imag*) malloc(sizeof(Imag) * height);
            if (!a[i]) return 0;

            /*se citesc informatiile din fisier pentru matrcie*/
            for (int j = 0; j < height; j++) {
                fread(&a[i][j].red, sizeof(unsigned char), 1, fin);
                fread(&a[i][j].green, sizeof(unsigned char), 1, fin);
                fread(&a[i][j].blue, sizeof(unsigned char), 1, fin);
            }
        }

        int size = height;
        /*se construieste arborele*/
        TArb grid = ConstrArb(a, size, 0, 0, factor, 0);

        /*se calculeaza numarul de niveluri*/
        int nivel = 0;
        Nr_niv(grid, &nivel);
        nivel++;  // se creste cu unu deoarece un nivel este numerotat cu 0
        fprintf(fout, "%d\n", nivel);
        /*se calculeaza numarul de blocuri ramase nevizate, adica nr de frunze*/
        int fr = Nr_fr(grid);
        fprintf(fout, "%d\n", fr);
        /*se calculeaza zona cea mai mare ramasa nedivizata*/
        int latura = Big_zone(grid, size);
        fprintf(fout, "%d\n", latura);

        /*se elibereaza memoria si se inchid fisierele*/
        for (int i = 0; i < width;  i++)
            free(a[i]);
        free(a);
        DistrugeArb(&grid);
        fclose(fin);
        fclose(fout);
    } else if (strcmp(argv[1], "-c2") == 0) {
        /*daca suntem la taskul 2, cel de-al doilea
         argument din terminal trebuie sa fie -c2*/

        /*se transforma factorul din argv[2] din char in integer*/
        int factor = atoi(argv[DOI]);
        /*se deschid fisierele*/
        FILE *fin = fopen(argv[TREI], "rb");
        if (!fin) return 0;
        FILE *fout = fopen(argv[PATRU], "wb");
        if (!fout) return 0;

        /*se citesc informatiile despre fisier*/
        char tip[SUTA], carac = 0;
        fscanf(fin, "%s\n", tip);
        int width = 0, height = 0, val_max = 0;
        fscanf(fin, "%u\n", &width);
        fscanf(fin, "%u\n", &height);
        fscanf(fin, "%d", &val_max);
        fscanf(fin, "%c", &carac);

        /*se aloca memoria pentru matricea de pixeli*/
        Imag** a = (Imag**) malloc(sizeof(Imag*) * height);
        if (!a) return 0;

        for (int i = 0; i < width;  i++) {
            a[i] = (Imag*) malloc(sizeof(Imag) * height);
            if (!a[i]) return 0;

            /*se citesc informatiile din fisier pentru matrcie*/
            for (int j = 0; j< height; j++) {
                fread(&a[i][j].red, sizeof(unsigned char), 1, fin);
                fread(&a[i][j].green, sizeof(unsigned char), 1, fin);
                fread(&a[i][j].blue, sizeof(unsigned char), 1, fin);
            }
        }
        int size = height;
        /*se construieste arborele*/
        TArb grid = ConstrArb(a, size, 0, 0, factor, 0);

        /*se initializeaza o coada*/
        TCoada* c = NULL;
        c = InitQ();
        if (!c) return 1;
        /*se face castul marimii imaginii la unsigned int de la int*/
        unsigned int width2 = (unsigned int) width;

        /*se scrie in fisier marimea imaginii*/
        fwrite(&width2, sizeof(unsigned int), 1, fout);
        /*se parcurge arborele pe niveluri*/
        ParcNivel(grid, c, fout);

        /*se elibereaza memoria si se inchid fisierele*/
        for (int i = 0; i < width;  i++)
            free(a[i]);
        free(a);
        DistrugeArb(&grid);
        DistrQ(&c);
        fclose(fin);
        fclose(fout);
        } else if (strcmp(argv[1], "-d") == 0) {
            /*daca suntem la taskul 3, cel de-al doilea
             argument din terminal trebuie sa fie -d*/

            /*se deschid fisierele*/
            FILE *fin = fopen(argv[DOI], "rb");
            if (!fin) return 0;
            FILE *fout = fopen(argv[TREI], "wb");
            if (!fout) return 0;

            /*se creeaza fisierul ppm cu informatiile sale
            specifice legate de marimea si tipul imaginii*/
            char tipf[PATRU] = "P6";
            fprintf(fout, "%s\n", tipf);
            unsigned int width = 0, val_max = MAX;
            /*se citeste din fisier marimea imaginii dupa se
            scrie informatia in celalalt fisier*/
            fread(&width, sizeof(unsigned int), 1, fin);
            fprintf(fout, "%d %d\n", width, width);
            fprintf(fout, "%d\n", val_max);

            unsigned char red[TREIZ], blue[TREIZ], green[TREIZ];

            /*se initializeaza coada*/
            TCoada* c = NULL;
            c = InitQ();
            if (!c) return 1;

            /*se initializeaza matricea de pizeli a imaginii*/
            Matrice* a = (Matrice*) malloc(sizeof(Matrice) * width);
            if (!a) return 0;
            for (int i = 0; i < width;  i++)
                a[i] = (Imag*) malloc(sizeof(Imag) * width);

            /*se initializeaza arborele cu radacina sa*/
            TArb grid  = Constr_Frunza();
            if (!grid) return 0;

            /*se citesc informatiile radacinii in functie de tipul sau,
             pe care il citim si pe el din fisier*/
            fread(&grid->tip, sizeof(unsigned char), 1, fin);
            if ((int)grid->tip == 0) {
                /*daca nu este fruza toate informatiile legate de culoare sunt 0*/
                grid->RGB.blue = (unsigned char) 0;
                grid->RGB.green = (unsigned char) 0;
                grid->RGB.red = (unsigned char) 0;
                grid->niv = 0;  // nivelul radacinii este 0
            } else {
                /*se citesc informatiile legate de culoare daca este frunza*/
                fread(&grid->RGB.red, sizeof(unsigned char), 1, fin);
                fread(&grid->RGB.green, sizeof(unsigned char), 1, fin);
                fread(&grid->RGB.blue, sizeof(unsigned char), 1, fin);
                grid->niv = 0;  // nivelul radacinii este 0
                }

            /*daca radacina nu este frunza, adica imaginea nu e o singura culoare
            atunci se introduce in coada radacina si apoi se apeleaza functia de parcurgere
            pe nivel pentru a fi creat arborele */
            if (grid->tip == 0) {
                IntrQ(c, grid);
                ParcNivel3(&grid, c, fin);}

            /*se  creeaza imaginea*/
            int width2 = (int) width;
            ConstrImag3(&a, grid, width2, 0, 0);

            /*se scrie in fisier matricea de pixeli creata, adica imaginea din fiser ppm*/
            for (int i = 0; i < width;  i++) {
                for (int j = 0; j < width; j++) {
                    fwrite(&a[i][j].red, sizeof(unsigned char), 1, fout);
                    fwrite(&a[i][j].green, sizeof(unsigned char), 1, fout);
                    fwrite(&a[i][j].blue, sizeof(unsigned char), 1, fout);
                }
            }

            /*se elibereaza memoria si se inchid fisierele*/
            for (int i = 0; i < width;  i++)
                free(a[i]);
            free(a);
            DistrugeArb(&grid);
            DistrQ(&c);
            fclose(fin);
            fclose(fout);
        }
    return 0;
}
