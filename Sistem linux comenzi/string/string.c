// SPDX-License-Identifier: BSD-3-Clause
#define MULT 1000000
#include <string.h>

char *strcpy(char *destination, const char *source)
{
	/*Daca adresele nu sunt valide se returneaza null*/
	if (destination == NULL || source == NULL) return NULL;
	int i = 0;
	/*Se parcurge sursa pana la final si se copie caracter cu
	caracter in destinatie*/
	while (source[i] != '\0') {
		destination[i] = source[i];
		i++;
	}
	/*Se adauga finalul de sir la destinatie*/
	destination[i] = '\0';
	return destination;
}

char *strncpy(char *destination, const char *source, size_t len) {
	/*Daca adresele nu sunt valide se returneaza null*/
	if (destination == NULL || source == NULL) return NULL;
	size_t i = 0;
	/*Se parcurge sursa pana la final sau pana la lungimea data si
	se copie caracter cu caracter in destinatie*/
	while (i < len && source[i]!= '\0') {
		destination[i] = source[i];
		i++;
	}
	/*Se adauga finalul de sir la destinatie pentru restul spatiilor ramase*/
	while (i < len) {
		destination[i] = '\0';
		i++;
	}
	return destination;
}

char *strcat(char *destination, const char *source) {
	/*Daca adresele nu sunt valide se returneaza null*/
	if (destination == NULL || source == NULL) return NULL;
	int i = 0, j = 0;
	/*Se parcurge destinatia pana la final*/
	for (i = 0; destination[i] != '\0'; i++) {}
	/*Se copie sursa la finalul destinatiei*/
	while (source[j] != '\0') {
		destination[i] = source[j];
		i++;
		j++;
	}
	/*Se adauga finalul de sir la destinatie*/
	destination[i] = source[j];

	return destination;
}

char *strncat(char *destination, const char *source, size_t len) {
	/*Daca adresele nu sunt valide se returneaza null*/
	if (destination == NULL || source == NULL) return NULL;
	size_t i = 0, j = 0;
	/*Se parcurge destinatia pana la final*/
	for (i = 0; destination[i] != '\0'; i++) {}
	/*Se copie sursa la finalul destinatiei pana la lungimea data*/
	while (source[j] != '\0' && j < len) {
		destination[i] = source[j];
		i++;
		j++;
	}
	/*Se adauga finalul de sir la destinatie*/
	destination[i] = '\0';
	return destination;
}

int strcmp(const char *str1, const char *str2) {
	/*Se trateaza cazurile speciale de esec*/
	if (str1 == NULL || str2 == NULL) return -1;
	int i = 0, ok = 0;
	/*Se parcurg sirurile in paralel pana la final sau pana cand se gaseste o diferenta*/
	while (str1[i] != '\0' && str2[i] != '\0') {
		/*Daca s-a gasit o diferenta se iese din bucla*/
		if (str1[i] != str2[i]) {
			ok = 1;
			break;
		}
		i++;
	}
	/*Se trateaza cazurile speciale*/
	if (ok == 0 && str1[i] == '\0' && str2[i] == '\0') return 0;
	if (ok == 0 && str1[i] == '\0') return -1;
	if (ok == 0 && str2[i] == '\0') return 1;
	if (ok == 1) {
		if (str1[i] > str2[i]) return 1;
		else
			return -1;
	}
	return -1;
}

int strncmp(const char *str1, const char *str2, size_t len) {
	size_t i = 0, ok = 0;
	/*Se parcurg sirurile in paralel pana la final sau pana cand trecem de
	lungimea de comparat data*/
	while (str1[i] != '\0' && str2[i] != '\0' && i < len) {
		/*Daca s-a gasit o diferenta se iese din bucla*/
		if (str1[i] != str2[i]) {
			ok = 1;
			break;
		}
		i++;
	}
	/*Se trateaza cazurile speciale*/
	if (ok == 0) return 0;
	if (ok == 1) {
		if (str1[i] > str2[i])
			return 1;
		else
			return -1;
	}
	return -1;
}

size_t strlen(const char *str)
{
	size_t i = 0;

	for (; *str != '\0'; str++, i++)
		;

	return i;
}

char *strchr(const char *str, int c)
{
	/*Se parcurge sirul prin intermediul pointerului*/
	while (*str) {
		/*Daca s-a gasit caracterul se returneaza adresa*/
		if (*str == c) return (char*)str;
		str++;
	}

	/*Daca nu s-a gasit caracterul se returneaza NULL*/
	return NULL;
}

char *strrchr(const char *str, int c)
{
	int n = 0;
	/*Se parcurge sirul prin intermediul pointerului pana la final*/
	for (n = 0; str[n] != '\0'; n++) {}

	/*Se parcurge sirul in sens invers pana la caracterul cautat
	si se returneaza*/
	while (n >= 0) {
		if (str[n] == c) return (char *)&str[n];
		n--;
	}
	return NULL;
}

char *strstr(const char *haystack, const char *needle)
{
	int i = 0, j = 0;
	/*Cautam needle pana ajungem la finalul haystackului*/
	while (haystack[i] != '\0') {
		/*Daca nu sunt la fel caracterele se trece la urmatorul*/
		if (haystack[i] != needle[0]) {
			i++;
		}
		/*Daca gasim inceputul needle-ului verificam si restul caracterelor*/
		if (haystack[i] == needle[0]) {
			int k = i;
			/*Verificam restul caracterelor pana terminam needle-ul sau gasim o differenta*/
			while (haystack[k] == needle[j] && needle[j] != '\0') {
				k++;
				j++;
			}
			/*Daca am ajuns la finalul needel-ului returnam adresa de inceput a celui gasim*/
			if (needle[j] == '\0') {
				return (char*)&(haystack[i]);
			} else {
			/*Daca nu am ajuns la final reincepem cautarea*/
				i++;
				j = 0;
			}
		}
	}
	return NULL;
}

char *strrstr(const char *haystack, const char *needle)
{
	int i = 0, j = 0, ok = 0;
	char* start = NULL;
	/*Se parcurge pana la final haystack*/
	while (haystack[i] != '\0') {
		/*Daca nu s-a gasit inceputul de asemanare se trece la urmatorul*/
		if (haystack[i] != needle[0]) {
			i++;
		}
		/*Daca s-a gasit inceputul de asemanare se parcurge sirul*/
		if (haystack[i] == needle[0]) {
			/*Se parcurge sirul pana la finalul needle-ului sau pana cand se gaseste o diferenta,
			utilizand o alta variabila pentru ca in caz de diferenta sa se continuea de urmatorul
			caracter (nu sa se faca i + len)*/
			int k = i;
			while (haystack[k] == needle[j] && needle[j] != '\0') {
				k++;
				j++;
			}
			/*Daca s-a gasit o asemanare se memoreaza adresa de inceput*/
			if (needle[j] == '\0') {
				ok = 1;
				start = (char*)&(haystack[i]);
			}
			i++;
			j = 0;
		}
	}

	/*Daca s-a gasit o asemanare se returneaza adresa de inceput*/
	if (ok == 1) return start;
	else
		return NULL;
}

void *memcpy(void *destination, const void *source, size_t num)
{
	/*Se trateaza cazurile speciale*/
	if (destination == NULL || source == NULL) return NULL;

	/*Se face cast*/
	char* destination_char = (char *) destination;
	const char* source_char = (const char *) source;
	size_t i = 0;

	/*Se copiaza in destinatie sursa caracter cu caracter*/
	do {
		destination_char[i] = source_char[i];
		i++;
	}while (i < num);
	return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
	/*Se trateaza un caz special de invaliditate*/
	if (destination == NULL) return NULL;

	/*Se face cast*/
	char* destination_char = (char *) destination;
	const char* source_char = (const char *) source;
	char dublura[MULT];
	size_t i = 0;

	/*Se trateaza un alt caz de invaliditate*/
	if (destination_char == NULL || source_char == NULL)
		return NULL;

	/*Se copiaza in dublura sursa caracter cu caracter*/
	for (i = 0; i < num; i++) {
		dublura[i] = source_char[i];
	}

	/*Se copiaza in destinatie dublura caracter cu caracter*/
	for (i = 0; i < num; i++) {
		destination_char[i] = dublura[i];
	}
	return destination;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	int ok = 0;
	/*Se face cast*/
	char* ptr1_char = (char *) ptr1;
	const char* ptr2_char = (const char*) ptr2;

	/*Daca pointerii pointeaza la aceeasi zona de memorie se returneaza 0*/
	if (ptr1_char == ptr2_char) {
		return ok;
	} else {
		/*Se parcurg zonele de memorie pana la final sau pana cand se gaseste o diferenta*/
		while (num > 0) {
			num--;
			if (*ptr1_char != *ptr2_char) {
				/*Daca s-a gasit o diferenta se returneaza 1 sau -1 in functie de relatia dintre cei doi*/
				if (*ptr1_char > *ptr2_char)
					ok = 1;
				else
					ok = -1;
				break;
			}
			/*Se trece la urmatorul caracter*/
			ptr1_char++;
			ptr2_char++;
		}
		return ok;
	}
	return -1;
}

void *memset(void *source, int value, size_t num)
{
	/*Se face cast*/
	char* source_char = (char *) source;
	char value_char = (char) value;

	/*Se parcurge zona de memorie si se seteaza valoarea*/
	for (size_t i = 0; i < num; i++) {
		source_char[i] = value_char;
	}
	return source;
}
