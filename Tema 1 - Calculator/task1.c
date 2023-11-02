#include <stdio.h>
#define LUNG 100

int main() {
int N = 0;

float operand[LUNG], result = 0;

char operatii[LUNG];

scanf("%d ", &N);

for (int i = 0; i < N; i++)
scanf("%f", &operand[i]);


for (int i=0; i <= N-1 ; i++)
scanf("%c", &operatii[i]);

if (N == 1) result = operand[0];
for (int i = 1, j = 0; i <= N - 1 && j < N-1 ; i++, j++) {
    if (operatii[i] == '+') {
        result = operand[j] + operand[j+1];
        operand[j+1] = result;
    }

    if (operatii[i] == '-') {
        result = operand[j] - operand[j+1];
        operand[j+1] = result;
    }

    if (operatii[i] == '*') {
        result = operand[j] * operand[j+1];
        operand[j+1] = result;
    }

    if (operatii[i] == '/') {
        result = operand[j] / operand[j+1];
        operand[j+1] = result;
    }
}

printf("%f\n", result);
}
