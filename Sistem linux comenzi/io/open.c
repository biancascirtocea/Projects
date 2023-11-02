// SPDX-License-Identifier: BSD-3-Clause

#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int open(const char *filename, int flags, ...)
{
	/* TODO: Implement open system call. */
	/*Daca pointerul la fisier nu este in regula se seteaza eroarea*/
	if (!filename) {
		errno = EFAULT;
		return -1;
	}
	int fd = 0;
	if (flags && O_CREAT) {
		/*Se preiau informatiile legate de mode, prin intermediul
		functiei cu numar variabil de parametrii*/
		va_list args;
		va_start(args, flags);
		mode_t mode = va_arg(args, mode_t);
		va_end(args);
		/*Se apeleaza sistemul pentru deschiderea fisierului  filename*/
		fd = syscall(__NR_open, filename, flags, mode);
		/*Daca apelul fisierului esueaza se seteaza eroarea*/
		if (fd < 0) {
			errno = -fd;
			return -1;
		}
	} else {
		/*Se face apelul sistemului fara mode si se seteaza eroarea 
		in cazul esecului*/
		fd = syscall(__NR_open, filename, flags);
		if (fd < 0) {
			errno = -fd;
			return -1;
		}
	}
	/*Sereturneaza descriptorul de fisier*/
	return fd;
}
