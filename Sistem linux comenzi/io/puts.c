// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>

int puts(const char *s) {
    int nr = 0;
    /*Se face o copie a lui s pentru a fi parcurs si a se calcula lungimea sa*/
    const char *p = s;
    while (*p) {
        nr++;
        p++;
    }

    /*Se scrie in buffer stringul s*/
    int x  = write(1, s, nr);

    /*Daca nu s-a scris tot stringul se returneaza eroarea*/
    if (x < 0) {
        errno = -x;
        return 1;
    }

    /*Se scrie in buffer un caracter de newline*/
    x  = write(1, "\n", 1);

    /*Daca nu s-a scris caracterul se returneaza eroarea*/
    if (x < 0) {
        errno = -x;
        return 1;
    }
    return 0;
}
