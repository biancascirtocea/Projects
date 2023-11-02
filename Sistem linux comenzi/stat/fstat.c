// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <internal/syscall.h>
#include <errno.h>

int fstat(int fd, struct stat *st)
{
	/*Daca nu exista descriptorul de fisier se returneaza eroarea*/
	if (!fd) {
		errno = -fd;
		return -1;
	}

	/*Se apeleaza syscall-ul corespunzator functiei fstat*/
	int x = syscall(__NR_fstat, fd, st);

	/*In caz de esec al apelului de sistem se seteaza eroarea*/
	if (x < 0) {
		errno = -x;
		return -1;
	}
	return 0;
}
