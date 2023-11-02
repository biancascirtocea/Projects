#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
    /*Se Realizeaza un apel a sistemului pentru nanosleep*/
    int x = syscall(__NR_nanosleep, rqtp, rmtp);

    /*Daca apelul a esuat se returneaza eroarea*/
    if (x != 0) {
        errno = -x;
        return 1;
    }
    return 0;
}

