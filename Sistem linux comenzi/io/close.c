// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int close(int fd)
{
	/* TODO: Implement close(). */
	/*Daca descriptorul este in regula se apeleaza sistemul pentru
	inchiderea fisierului*/
	if (fd >= 0) {
		int x = syscall(__NR_close, fd);
		/*Daca acest apelul esueaza se seteaza eroarea si se returneaza -1*/
		if (x < 0) {
			errno = -x;
			return -1;
		}
		return 0;
	}
	return -1;
}
