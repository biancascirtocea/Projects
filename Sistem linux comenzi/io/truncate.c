// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>
#include <internal/types.h>

int truncate(const char *path, off_t length)
{
	/* TODO: Implement truncate(). */
	/*Daca pointerul path nu este valid se seteaza eroarea*/
	if (!path) {
		errno = -(*path);
		return -1;
	}
	/*Se apeleaza sistemul pentru truncate*/
	off_t x = syscall(__NR_truncate, path, length);
	/*Daca in urma apelului se rezulta o eroare, se seteaza si aceasta*/
	if (x < 0) {
		errno = -x;
		return -1;
	}
	return 0;
}
