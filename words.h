//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#ifndef WORD_HEADER
#define WORD_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 500
#define FILEMAX 30
#define DIM 100

typedef struct {
	char **words;
	int number, total;
} keywords;

void compute_dictionary(keywords *dict);
void free_dictionary(keywords *dict);

#endif
