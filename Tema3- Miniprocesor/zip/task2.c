#include <stdio.h>
#define NINE 9
#define EIGHT 8
#define FOUR 4
#define THREE 3
#define SIZE 32
#define MAX 16

int main() {
unsigned int inst = 0;

scanf("%u", &inst);  // se citeste secventa codificata

unsigned int N = 0 , Dim = 0;
char op[SIZE];
unsigned int dup = inst;
int nr = 0;

int n = SIZE - THREE;
// se calculeaza numarul de pozitii cu care o sa se shifteze
// numarul maxim de biti pt unsigned int 32

N = dup >> n;
N++;

for (int k = 1; k <= N; k++) {
    n--;  // trecem la urmatorul bit
    unsigned int opt1 = (dup >> n) & 1;  // verificam daca bitul este setat sau nu si ii salvam valoarea
    n--;
    unsigned int opt2 = (dup >> n) & 1;

    if (opt1 ==  0 && opt2 == 0) op[k] = '+';
    if (opt1 ==  0 && opt2 == 1) op[k] = '-';
    if (opt1 ==  1 && opt2 == 0) op[k] = '*';
    if (opt1 ==  1 && opt2 == 1) op[k] = '/';
}

unsigned int i = EIGHT;
for (int j = 1; j <= 4 ; j++) {
    n--;
    unsigned int val = (dup >> n) & 1;  // verificam bitii de pe pozitiile pentru Dim
    if (val == 1) Dim = Dim + i;
    i = i / 2;
}
Dim++;

unsigned short num[MAX], numar[MAX];
unsigned short val = (N + 1) * Dim;
unsigned short nr2 = val / MAX;
if ( val % MAX != 0) nr2++;  // se calculeaza numarul de secvente citite de la tastatura

for(int i = 1; i <= nr2 ; i++) {
    scanf("%hd", &num[i]);
}

unsigned int y = MAX / Dim;  // numar de numere de decodificat dintr-o secventa citita de la tastatura
int j = 0, k = 1, p = 1, poz = 0;

char c = op[1];

for(int i = 1 ; i <= nr2 ; i++) {
    poz = MAX - Dim;  // se calculeaza dimensiunea cu care sa shiftam la dreapta ca sa ne ramana numarul
    numar[p] = num[i] >> poz;  // se shifteaza num[i] astfel incat sa ne ramana doar nuamrul necesar
    num[i] = num[i] << Dim;  // se sterge codul pentru numarul gasit
    // si se aduce pe prima pozitie codul numarului urmator
    p++;
    for (int j = 2 ; j <= y ; j++) {
            numar[p] = num[i] >> poz;  // se continua la fel
            p++;
            num[i] = num[i] << Dim;
        }
    }

unsigned int rez = numar[1];
p = 2;

op[1] = c;

for(int m = 1 ; m <= N ; m++) {
    y = numar[p];
    p++;
    if (op[m] == '+') rez = rez + y;
    if (op[m] == '-') rez = rez - y;
    if (op[m] == '*') rez = rez * y;
    if (op[m] == '/') rez = rez / y;
}

printf("%d\n", rez);


return 0;
}
