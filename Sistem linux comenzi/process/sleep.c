#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

unsigned int sleep(unsigned int sec) {
    /*Se introduc structurile necesare pentru nanosleep*/
    struct timespec req = {sec, 0};
    struct timespec rem;

    /*Utilizam nanosleep pana cand trece tot timpul necesar*/
    while (nanosleep(&req, &rem) != 0) {
        /*Daca programul este intrerupt pana sa se termine
        timpul se reapeleaza cu timpul ramas*/
        req = rem;
    }
    return 0;
}
