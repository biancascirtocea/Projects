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
for (int i = 1; i <= N; i++) {
    int ok = 0;
    if (operatii[i] == '*') {
        result = operand[i-1] * operand[i];
        operand[i-1] = result;
        ok = 1;
    }

    if (operatii[i] == '/') {
        result = operand[i-1] / operand[i];
        operand[i-1] = result;
        ok = 1;
    }

    if (ok == 1) {
        for (int j = i; j <= N-1; j++) {
            operatii[j] = operatii[j+1];
        }
        for (int j = i; j < N-1; j++) {
            operand[j] = operand[j+1];
        }
        N--;
        i--;
    }
}
for (int i = 1; i <= N; i++) {
    int ok = 0;
    if (operatii[i] == '#') {
        result = (operand[i-1] + operand[i]) * (operand[i-1] + operand[i]);
        operand[i-1] = result;
        ok = 1;
    }

    if (ok == 1) {
        for (int j = i; j <= N-1; j++) {
            operatii[j] = operatii[j+1];
        }
        for (int j = i; j < N-1; j++) {
            operand[j] = operand[j+1];
        }
        N--;
        i--;
    }
}
for (int i = 1; i <= N - 1; i++) {
    if (operatii[i] == '+') {
        result = operand[i-1] + operand[i];
        operand[i] = result;
    }

    if (operatii[i] == '-') {
        result = operand[i-1] - operand[i];
        operand[i] = result;
    }
}
printf("%f\n", result);
}

