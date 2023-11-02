// SPDX-License-Identifier: BSD-3-Clause

#include <internal/mm/mem_list.h>
#include <internal/types.h>
#include <internal/essentials.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

void *malloc(size_t size)
{
	int *x = NULL;

	/*Se calculeaza marimea zonei de memorie necesara*/
	int length = size + sizeof(size);

	/*Se aloca memorie folosind mmap si toate flagurile necesare*/
	x = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	/*In caz de esec al apelului de sistem se seteaza eroarea*/
	if (x == MAP_FAILED) {
		errno = -(*x);
		return NULL;
	}

	/*Se memoreaza marimea zonei de memorie alocate*/
	*x = length;

	/*Se returneaza adresa zonei de memorie alocate*/
	return (void *)(&x[1]);
}

void *calloc(size_t nmemb, size_t size)
{
	int *x = NULL;

	/*Se trateaza cazurile speciale*/
	if (nmemb == 0 || size == 0)
		return NULL;

	/*Se calculeaza marimea zonei de memorie necesara*/
	int length = nmemb * (size + 1);

	/*Se aloca memorie folosing mmap si toate flagurile necesare*/
	x = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	/*In caz de esec al apelului de sistem se seteaza eroarea*/
	if (x == MAP_FAILED) {
		errno = -(*x);
		return NULL;
	}

	/*Se memoreaza marimea zonei de memorie alocate*/
	*x = length;

	/*Se initializeaza zona de memorie cu 0*/
	char * y = (char *)(&x[1]);
	for (int i = 0; i < length; i++) {
		y[i] = 0;
	}

	/*Se returneaza adresa zonei de memorie alocate*/
	return (void *)(&x[1]);
}

void free(void *ptr)
{
	/*Daca exista un pointer catre un fisier se elibereaza memoria*/
	if (ptr) {
		/*Se face cast*/
		int *x = (int *)ptr;

		/*Se ajunge in punctul de start/varf al memoriei alocate pentru variabila,
		adica locul unde se memoreaza marimea variabilei*/
		x--;

		/*Se utilizeaza munmap pentru eliberarea memoriei*/
		int rez = munmap(x, *x);

		/*In caz de esec al apelului de sistem se seteaza eroarea*/
		if (rez == -1)
			errno = -(*x);
	}
}

void *realloc(void *ptr, size_t size)
{
	void *x;

	/*Se trateaza cazurile speciale*/
	if (ptr == NULL && size == 0)
		return NULL;
	if (ptr != NULL && size == 0) {
		free(ptr);
		return NULL;
	}

	/*Se aloca memorie pentru noua variabila in functie de existenta 
	pointerul catre vechea variabila*/
	if (!ptr) {
		x = malloc(size);

		/*Daca nu se poate aloca memorie se returneaza NULL*/
		if (!x)
			return ūNULL;
	} else {
		x = malloc(size);
		if (!x)
			return NULL;

		/*Se copiaza continutul variabilei vechi in cea noua*/
		memcpy(x, ptr, size);

		/*Se elibereaza memoria alocata pentru variabila veche*/
		free(ptr);
	}
	return x;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	void *x;

	/*Se trateaza cazurile speciale*/
	if (ptr == NULL && nmemb == 0 && size == 0)
		return NULL;
	if (ptr != NULL && nmemb == 0 && size == 0) {
		free(ptr);
		return NULL;
	}
	/*Se aloca memorie pentru noua variabila in functie de existenta
	pointerul catre vechea variabila*/
	if (!ptr) {
		x = malloc(nmemb * size);

		/*Daca nu se poate aloca memorie se returneaza NULL*/
		if (!x)
			return NULL;
	} else {
		x = malloc(nmemb * size);
		if (!x)
			return NULL;

		/*Se copiaza continutul variabilei vechi in cea noua*/
		memcpy(x, ptr, nmemb * size);

		/*Se elibereaza memoria alocata pentru variabila veche*/
		free(ptr);
	}
	return x;
}
