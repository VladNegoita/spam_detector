//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#ifndef SPAM_HEADER
#define SPAM_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include "words.h"

#define NMAX 500
#define FILEMAX 30
#define DIM 100

typedef struct {
	int *keywords;
	int body_size;
	double spam_value;
	char *from;
} email;

void read_email(FILE *in, int email_number, email *v, keywords dict);
void compute_stdev(email *v, int n, keywords dict);
void free_emails(email *v, int n);
int occurences(char *line, char *pattern);
void lower(char *s);
void predict(email *v, int n, keywords dict);

#endif
