#include <stdio.h>
#define Trick 7
#define Treat 11

int main() {
    int N = 0;

    printf("Introduceti valoarea numarului natural N:\n");

    scanf("%d", &N);

    for ( int i = 1; i <= N; i++ ) {
         if ( i % Trick == 0 && i % Treat == 0 ) printf("TrickOrTreat");
         else if ( i % Treat == 0 ) printf("Treat");
         else if ( i % Trick == 0 ) printf("Trick");
         else
            printf("%d", i);

         if ( i != N ) printf(" ");
    }

return 0;
}

