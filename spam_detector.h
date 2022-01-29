//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#ifndef SPAM_HEADER
#define SPAM_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include "words.h"

#define NMAX 50000
#define FILEMAX 30
#define DIM 100
#define LIMIT 36

typedef struct {
	int *keywords;
	int body, caps, invalid_words, letters;
	double spam_value;
	char *from;
} email;

void read_email(FILE *in, int email_number, email *v, keywords dict);
void compute_stdev(email *v, int n, keywords dict);
void free_emails(email *v, int n);
int occurences(char *line, char *pattern);
void lower(char *s);
void predict(email *v, int n, keywords dict);
int caps_number(char *s);
void criteria1(email *v, int n);
void criteria2(email *v, int n);
void criteria3(email *v, int n, keywords dict);
void criteria4(email *v, int n);
void criteria5(email *v, int n);
int count_invalids(char *s);
int is_invalid(char *s);
int is_lower(char c);
int is_upper(char c);
int is_digit(char c);
int is_vowel(char c);
int is_letter(char c);
int letter_number(char *s);

#endif
