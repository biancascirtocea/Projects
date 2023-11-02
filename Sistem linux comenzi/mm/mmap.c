// SPDX-License-Identifier: BSD-3-Clause


#include <errno.h>
#include <internal/syscall.h>
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{	/*Apelam sistemul pentru mmap*/
	long x = syscall(__NR_mmap, (unsigned long)addr, length, prot, flags, fd, offset);
	/*Daca apelul esueaza se seteaza eroarea*/
	if (x < 0) {
		errno = -x;
		return MAP_FAILED;
	}
	return (void*)x;
}

void *mremap(void *old_address, size_t old_size, size_t new_size, int flags)
{
	/*Daca nu exista vechea adresa se returneaza eroarea*/
	if (!old_address) {
		errno = -(*(char *)old_address);
		return MAP_FAILED;
	}
	/*Se face apel la sistem specific pentru mremap*/
	void *x = (void *) syscall(__NR_mremap, old_address, old_size, new_size, flags);

	/*Daca esueaza apelul se seteaza eroarea*/
	if (x == (void*)-1) {
		errno = -(*(int *)x);
		return MAP_FAILED;
	}

	return x;
}

int munmap(void *addr, size_t length)
{
	/*Se face apel la sistem specific lui munmap*/
	int x = syscall(__NR_munmap, addr, length);

	/*Daca esueaza apelul se seteaza eroarea*/
	if (x < 0) {
		errno = -x;
		return -1;
	}
	return 0;
}
