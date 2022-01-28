//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "words.h"

void compute_dictionary(keywords *dict)
{
	FILE *in = fopen("./data/keywords", "rt");
	if (!in) {
		fprintf(stderr, "Non-existing keywords");
		exit(-1);
	}

	fscanf(in, "%d", &dict->number);
	dict->total = dict->number;
	dict->words = (char **)malloc(dict->number * sizeof(char *));
	if (!dict->words) {
		fprintf(stderr, "Mem error");
		exit(-1);
	}

	char s[NMAX];
	for (int i = 0; i < dict->number; ++i) {
		fscanf(in, "%s", s);
		dict->words[i] = (char *)malloc((strlen(s) + 1) * sizeof(char));
		strcpy(dict->words[i], s);
	}
	fclose(in);

	in = fopen("additional_keywords", "rt");
	if (!in) {
		fprintf(stderr, "Non-existing additional_keywords");
		free_dictionary(dict);
		exit(-1);
	}

	int additional_words;
	fscanf(in, "%d", &additional_words);
	dict->total = dict->number + additional_words;
	char **aux;
	aux = (char **)realloc(dict->words, (dict->total) * sizeof(char *));
	if (!aux) {
		fprintf(stderr, "Mem error");
		free_dictionary(dict);
		exit(-1);
	}
	dict->words = aux;

	for (int i = dict->number; i < dict->total; ++i) {
		fscanf(in, "%s", s);
		dict->words[i] = (char *)malloc((strlen(s) + 1) * sizeof(char));
		strcpy(dict->words[i], s);
	}

	fclose(in);
}

void free_dictionary(keywords *dict)
{
	for (int i = 0; i < dict->total; ++i)
		free(dict->words[i]);
	free(dict->words);
}
