// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>
#include <internal/types.h>

int ftruncate(int fd, off_t length)
{
	/* TODO: Implement ftruncate(). */
	/*Daca descriptorul de fisier nu este valid se seteaza eroarea*/
	if (!fd) {
		errno = -fd;
		return -1;
	}
	/*Se apeleaza sistemul pentru ftruncate*/
	off_t x = syscall(__NR_ftruncate, fd, length);
	/*Daca in urma apelului se rezulta o eroare, se seteaza si aceasta*/
	if (x < 0) {
		errno = -x;
		return -1;
	}
	return 0;
}