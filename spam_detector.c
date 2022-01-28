//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include "spam_detector.h"
#include "words.h"

#define NMAX 1000
#define FILEMAX 50

int main()
{
	keywords dictionary;
	compute_dictionary(&dictionary);

	int n = -2;
	DIR *d;
	d = opendir("./data/emails/");
	if (!d) {
		fprintf(stderr, "%s\n", "Invalid Path");
		exit(-1);
	}

	struct dirent *dir;
	dir = readdir(d);
	while (dir) {
		++n;
		dir = readdir(d);
	}
	closedir(d);

	email *v = (email *)malloc(n * sizeof(email));
	FILE *in;
	for (int i = 0; i < n; ++i) {
		char file_name[FILEMAX];
		sprintf(file_name, "./data/emails/%d", i);
		in = fopen(file_name, "rt");
		if (!in) {
			fprintf(stderr, "error email number: %d", i);
			exit(-1);
		}
		read_email(in, i, v);
		fclose(in);
	}
	free(v);
	return 0;
}

void read_email(FILE *in, int email_number, email *v)
{
	char s[NMAX + 1];
	fgets(s, NMAX, in);// date

	fgets(s, NMAX, in);// subject

	fgets(s, NMAX, in);// from

	fgets(s, NMAX, in);// empty line

	while (!feof(in)) {// body
		fgets(s, NMAX, in);
	}
}
