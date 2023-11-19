// SPDX-License-Identifier: BSD-3-Clause

#include "osmem.h"
#include "block_meta.h"
#include "printf.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <limits.h>

/*Definurile sunt luate din teoria oferita din cadrul temei*/
#define MMAP_THRESHOLD (128 * 1024)
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define PAGE_SIZE 4096

#define METADATA_SIZE	(sizeof(struct block_meta))
#define HEADER_SIZE (ALIGN(METADATA_SIZE))

#define SPLIT_THRESHOLD (HEADER_SIZE + ALIGN(1))

/* Structuri pentru a retine inceputul si sfarsitul listei dublu inlantuite */
struct block_meta *heap_start;
struct block_meta *heap_end;
struct block_meta *start_mmap;
struct block_meta *end_mmap;

/*Functia pentru prealocarea memoriei*/
void initial_alloc(size_t size)
{
	/*Se aloca memorie de dimensiunea size cu functia sbrk si*/
	/*daca e cazul se returneaza eroarea respectiva*/
	heap_start = (struct block_meta *) sbrk(size);
	DIE(heap_start == (void *) -1, "couldn't create a new heap area");


	/*Se initializeaza campurile structurii block_meta,*/
	/*pentru inceputul listei dublu inlanuite*/
	struct block_meta *block = (struct block_meta *)heap_start;

	block->status = STATUS_FREE;
	block->size = ALIGN(MMAP_THRESHOLD) - ALIGN(sizeof(struct block_meta));
	block->next = NULL;
	block->prev = NULL;
	/*Se initializeaza si finalul listei dublu inlantuite*/
	heap_end = block;
}

/*Functia coalesce, pentru combinarea blocurilor free*/
void combine(struct block_meta *block)
{
	/*Se parcurge lista dublu inlanuita pana la final*/
	for (; block != NULL; block = block->next) {
		/*Cat timp exista doua blocuri una langa alta cu statusul free*/
		while (block->next != NULL && block->next->status == STATUS_FREE && block->status == STATUS_FREE) {
			/*Se aduna sizeurile, se modifica legaturile si se modifica sfarsitul listei daca e cazul*/
			block->size += block->next->size + ALIGN(sizeof(struct block_meta));
			if (block->next == heap_end)
				heap_end = block;

			if (block->next == end_mmap)
				end_mmap = block;

			if (block->next != NULL)
				block->next->prev = block;

			block->next = block->next->next;
		}
	}
}

/*Functia pentru redistribuirea memoriei in calupuri mai mici*/
struct block_meta *split_block(struct block_meta *block, size_t size)
{
	size_t rest_size = block->size - size - ALIGN(sizeof(struct block_meta));

	/*Se verifica daca exista spatiu suficient pentru a se face split*/
	if (block->size >= size + SPLIT_THRESHOLD && rest_size >= 1) {
		/*Se creaza un nou bloc cu size-ul ramas*/
		struct block_meta *another_blk = (struct block_meta *) ((char *) block + size + ALIGN(sizeof(struct block_meta)));

		/*Se modifica campurile structurii*/
		another_blk->next = block->next;
		another_blk->size = block->size - size - ALIGN(sizeof(struct block_meta));
		another_blk->status = STATUS_FREE;
		another_blk->prev = block;

		/*Se modifica legaturile*/
		if (block->next != NULL)
			block->next->prev = another_blk;

		block->next = another_blk;
		block->size = size;
	}
	block->status = STATUS_ALLOC;
	return block;
}

/*Functie pentru cautarea blocului cu dimensiunea potrivita*/
struct block_meta *find_best_block(size_t size)
{
	struct block_meta *block = heap_start;
	struct block_meta *best_block = NULL;
	size_t min_size = ULONG_MAX;

	/*Se parcurge lista dublu inlantuita si se cauta blocul*/
	/*cu statusul free cu dimensiunea cea mai potrivita*/
	for (; block != NULL; block = block->next) {
		if (block->size >= size && block->size < min_size && block->status == STATUS_FREE) {
			min_size = block->size;
			best_block = block;
		}
	}

	/*Se returneaza blocul gasit*/
	if (best_block != NULL)
		return best_block;
	else
		return NULL;
}

/*Functie pentru alocarea cu sbrk*/
struct block_meta *alloc_heap(size_t size)
{
	size_t new_size = ALIGN(size);
	/*Se apeleaza functia combine pentru a elimina orice*/
	/*impartiri inutile ale memoriei*/
	combine(heap_start);
	combine(start_mmap);
	/*Se cauta blocul ideal de memorie pentru dimensiunea size*/
	struct block_meta *block = find_best_block(ALIGN(size));

	/*Daca nu am gasit niciun bloc disponibil necesitatilor actuale*/
	if (block == NULL) {
		/*Verificam daca blocul de memorie de la finalul listei*/
		/*dublu inlantuite este ocupat*/
		if (heap_end->status == STATUS_FREE) {
			/*Alocam restul de memorie necesara la finalul listei*/
			block = sbrk(ALIGN(size - heap_end->size));
			DIE(block == (void *) -1, "couldn't create a new heap area");

			/*Modificam campurile structurii block_meta*/
			block = heap_end;
			heap_end->status = STATUS_ALLOC;
			block->size = block->size + ALIGN(size - heap_end->size);
		} else {
			/*Altfel alocam pentru tot size-ul memorie*/
			/*si returnam mesaj de eroare daca e nevoie*/
			block = sbrk(ALIGN(size) + HEADER_SIZE);
			DIE(block == (void *) -1, "couldn't create a new heap area");

			/*Modificam campurile structurii block_meta*/
			block->next = NULL;
			block->size = new_size;

			/*Modificam finalul listei dublu inlantuite*/
			heap_end->next = block;
			block->prev = heap_end;
			heap_end = block;
		}
	} else if (block->size >= new_size + SPLIT_THRESHOLD) {
		/*Daca am gasit un bloc disponibil, dar acesta este mai mare decat*/
		/*dimensiunea necesara, il impartim in doua apeland functia split*/
		block->status = STATUS_ALLOC;
		split_block(block, ALIGN(size));
	}
	/*Schimbam statusul blocului*/
	block->status = STATUS_ALLOC;

	return block;
}

/*Functie pentru alocarea cu mmap*/
struct block_meta *alloc_mmap(size_t size)
{
	size_t new_size = ALIGN(size);

	/*Se aloca memorie cu functia mmap*/
	void *ptr = (void *)mmap(NULL, new_size + ALIGN(sizeof(struct block_meta)),
	PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	/*Se returneaza mesaj de eroare daca e cazul*/
	if (ptr == (void *)-1) {
		DIE(ptr == (void *)-1, "mmap failed for __os_malloc__");
		return NULL;
	}

	/*Se modifica campurile structurii block_meta*/
	struct block_meta *block = (struct block_meta *) ptr;

	block->status = STATUS_MAPPED;
	block->size = new_size;
	block->next = NULL;

	/*Se adauga la finalul listei dublu inlantuite, iar daca el este*/
	/*primul element de tip mmap se modifica ai inceputul si finalul*/
	if (end_mmap != NULL) {
		end_mmap->next = block;
		block->prev = end_mmap;
		end_mmap = block;
	} else {
		start_mmap = block;
		end_mmap = block;
		block->prev = NULL;
		block->next = NULL;
	}

	return block;
}

/*Functie pentru alocarea memoriei*/
void *os_malloc(size_t size)
{
	size_t new_size = ALIGN(size);
	/*Se verifica daca size-ul este mai mic decat 0*/
	if (size <= 0)
		return NULL;

	struct block_meta *block = NULL;
	/*Se verifica daca size-ul este mai mic decat pragul de alocare cu mmap*/
	if (new_size + HEADER_SIZE < MMAP_THRESHOLD) {
		/*Se verifica daca exista memorie prealocata*/
		if (heap_start == NULL)
			initial_alloc(ALIGN(MMAP_THRESHOLD));
		/*Se apeleaza functia de alocare cu sbrk*/
		block = alloc_heap(size);
	} else {
		/*Se apeleaza functia de alocare cu mmap*/
		if (ALIGN(size) + HEADER_SIZE >= MMAP_THRESHOLD)
			block = alloc_mmap(size);
	}

	if (block == NULL)
		return NULL;
	/*Se returneaza adresa de inceput a blocului, tinand cont de padding*/
	return (void *)(block + 1);
}

/*Functie pentru eliberarea memoriei*/
void os_free(void *ptr)
{
	if (ptr == NULL)
		return;
	/*Se apeleaza functia combine pentru a combina blocurile free*/
	combine(heap_start);
	struct block_meta *first = heap_start;
	struct block_meta *end = NULL;

	/*Se cauta blocul de memorie in lista dublu inlantuita*/
	for (; first != NULL; first = first->next) {
		/*Daca s-a gasit blocul de memorie se modifica statusul si se combina blocurile free*/
		if (first + 1 == (struct block_meta *) ptr) {
			first->status = STATUS_FREE;
			combine(heap_start);
			return;
		}
	}

	struct block_meta *next = start_mmap;
	/*Se cauta blocul de memorie in lista dublu inlantuita pt mmap*/
	for (next = start_mmap; next != NULL; next = next->next) {
		/*Daca s-a gasit blocul de memorie */
		if (next + 1 == (struct block_meta *) ptr) {
			/*Se verifica daca el se afla pe prima pozitie*/
			/*Daca da se actualizeaza sfarsitul gasit pana acum, altfel inceputul listei*/
			if (end != NULL)
				end->next = next->next;
			else
				start_mmap = next->next;

			/*Se verifica daca blocul de memorie se afla la finalul listei*/
			if (next->next == NULL)
				end_mmap = end;
			/*Se elibereaza memoria cu functia munmap*/
			int err = munmap(next, next->size + HEADER_SIZE);

			DIE(err == -1, "munmap failed for __os_free__");
			return;
		}
		/*Se actualizeaza sfarsitul gasit pana acum*/
		end = next;
	}
}

/*Functie pentru alocarea cu calloc*/
void *os_calloc(size_t nmemb, size_t size)
{
	struct block_meta *block = NULL;
	char *ptr = NULL;

	/*Se verifica datele si se returneaza mesaj de eroare daca este cazul*/
	if (nmemb == 0 || size == 0) {
		return NULL;
		DIE(nmemb == 0 || size == 0, "calloc failed for __os_calloc__");
	}

	/*Se calculeaza dimensiunea necesara*/
	size_t new_size = nmemb * size;

	/*Se verifica daca se poate aloca cu sbrk*/
	if (ALIGN(new_size) + ALIGN(sizeof(struct block_meta)) < (unsigned int) sysconf(_SC_PAGESIZE)) {
		/*Se verifica daca exista memorie prealocata*/
		if (heap_start == NULL)
			initial_alloc(ALIGN(MMAP_THRESHOLD));
		/*Se apeleaza functia de alocare cu sbrk*/
		block = alloc_heap(new_size);
		/*Se returneaza mesaj de eroare daca este cazul*/
		if (!block) {
			return NULL;
			DIE(!block, "calloc failed for __os_calloc__");
		}
		/*Se seteaza paddingul*/
		ptr = (char *) block + ALIGN(sizeof(struct block_meta));
	} else {
		/*Se apeleaza functia de alocare cu mmap*/
		block = alloc_mmap(new_size);
		/*Se returneaza mesaj de eroare daca este cazul*/
		if (!block) {
			return NULL;
			DIE(!block, "calloc failed for __os_calloc__");
		}
		/*Se seteaza paddingul*/
		ptr = (char *) block + ALIGN(sizeof(struct block_meta));
	}

	/*Se seteaza toate valorile pe 0*/
	memset(ptr, 0, ALIGN(new_size));
	return (void *) ptr;
}

/*Functie pentru realocarea memoriei*/
void *os_realloc(void *ptr, size_t size)
{
	/*Daca pointerul pentru realloc este null, se aloca memoria normal cu malloc*/
	if (ptr == NULL)
		return os_malloc(size);

	/*Se verifica datele, iar daca este cazul se returneaza*/
	/*mesaj de eroare si se elibereaza memoria initiala*/
	if (size == 0) {
		os_free(ptr);
		return NULL;
		DIE(size == 0, "realloc failed for __os_realloc__");
	}

	if (ptr == (void *) 0)
		return os_malloc(size);

	/*Se apeleaza functia combine pentru a combina blocurile free*/
	combine(heap_start);
	struct block_meta *block = NULL;
	struct block_meta *find = heap_start;
	/*Se cauta blocul de memorie in lista dublu inlantuita*/
	for (; find != NULL && block == NULL; find = find->next) {
		if ((struct block_meta *)ptr == find + 1)
			block = find;
	}
	/*Se cauta blocul de memorie in lista dublu inlantuita pt mmap*/
	if (block == NULL) {
		find = start_mmap;
		for (; find != NULL && block == NULL; find = find->next) {
			if ((struct block_meta *)ptr  == find + 1)
				block = find;
		}
	}

	/*Se returneaza NULL daca nu s-a gasit blocul de memorie*/
	if (block == NULL)
		return NULL;
	/*Se returneaza NULL daca blocul de memorie gasit este free*/
	if (block->status == STATUS_FREE)
		return NULL;

	/*Se verifica daca blocul este alocat cu mmap sau sbrk in fucntie de status*/
	if (block->status == STATUS_ALLOC) {
		/*Daca noul size e mai mic decat cel initial se face split*/
		if (ALIGN(size) < block->size) {
			split_block(block, ALIGN(size));
		} else if (ALIGN(size) > block->size) {
			/*Daca noul size e mai mare decat cel initial se verifica daca*/
			/*urmatorul bloc este free si daca da le imbinam*/
			if (block->next && block->next->status == STATUS_FREE
			&& block->next->size + HEADER_SIZE + block->size >= ALIGN(size)) {
				block->size = block->size + block->next->size + HEADER_SIZE;
				block->next = block->next->next;
			} else if (block == heap_end) {
				/*Daca blocul este ultimul din lista dublu inlantuita*/
				/*se apeleaza functia sbrk pentru a aloca mai multa memorie*/
				sbrk(ALIGN(size) - block->size);
				block->size = ALIGN(size);
			} else {
				/*Altfel se aloca memorie cu malloc*/
				void *new_ptr = os_malloc(size);
				/*Se returneaza mesaj de eroare daca este cazul*/
				if (new_ptr == NULL) {
					DIE(new_ptr == NULL, "realloc failed for __os_realloc__");
					return NULL;
				}
				/*Se copiaza datele si se elibereaza memoria initiala*/
				memmove(new_ptr, ptr, block->size);
				os_free(ptr);
				/*Se returneaza adresa de inceput a blocului, tinand cont de padding*/
				block = (struct block_meta *)new_ptr - 1;
			}
		}
	} else if (block->status == STATUS_MAPPED) {
		/*Daca blocul este alocat cu mmap se apeleaza*/
		/*functia malloc de mai sus, pentru a se folosi mmap*/
		void *new_ptr = os_malloc(size);

		/*Se returneaza mesaj de eroare daca este cazul*/
		if (new_ptr == NULL) {
			DIE(new_ptr == NULL, "realloc failed for __os_realloc__");
			return NULL;
		}

		/*Se copiaza datele si se elibereaza memoria initiala*/
		memmove(new_ptr, ptr, ALIGN(size));
		os_free(ptr);
	block = (struct block_meta *) new_ptr - 1;
	}
	/*Se returneaza adresa de inceput a blocului, tinand cont de padding*/
	return (void *)(block + 1);
}
