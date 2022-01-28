//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#ifndef SPAM_HEADER
#define SPAM_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

typedef struct
{
	int *keywords;
	int body_size;
	double spam_value;
} email;

void read_email(FILE *in, int email_number, email *v);

#endif