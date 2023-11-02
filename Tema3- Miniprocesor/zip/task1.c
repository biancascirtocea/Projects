#include <stdio.h>
#define NINE 9
#define EIGHT 8
#define FOUR 4
#define THREE 3
#define TWO 2
#define SIZE 32

int main() {
unsigned int inst = 0;

scanf("%u", &inst);  // se citeste secventa codificata

unsigned int N = 0 , Dim = 0;
char op[NINE];
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

printf("%d ", N);
for(int k = 1; k <= N; k++ )
printf("%c ", op[k]);
printf("%d\n", Dim);

return 0;
}
