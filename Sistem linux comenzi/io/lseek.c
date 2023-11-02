// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>
#include <internal/types.h>

off_t lseek(int fd, off_t offset, int whence)
{
	/*Se apeleaza syscall-ul corespunzator functiei lseek*/
	off_t x = syscall(__NR_lseek, fd, offset, whence);

	/*In caz de esec al apelului de sistem se seteaza eroarea*/
	if (x < 0) {
		errno = -x;
		return -1;
	}
	return x;
}
