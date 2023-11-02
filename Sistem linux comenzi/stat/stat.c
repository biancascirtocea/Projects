// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <internal/syscall.h>
#include <fcntl.h>
#include <errno.h>

int stat(const char *restrict path, struct stat *restrict buf)
{
	/*Daca pointerul catre fisier nu e valid se seteaza eroarea*/
	if (!path) {
		errno = -(*path);
		return -1;
	}

	/*Se apeleaza syscall-ul corespunzator functiei stat*/
	int x = syscall(__NR_stat, path, buf);

	/*In caz de esec al apelului de sistem se seteaza eroarea*/
	if (x < 0) {
		errno = -x;
		return -1;
	}
	return 0;
}
