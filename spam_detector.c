//Copyright 2022 311CA Vlad Negoita <vlad1negoita@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include "spam_detector.h"
#include "words.h"

int main(void)
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
		v[i].keywords = (int *)calloc(dictionary.total, sizeof(int));
		read_email(in, i, v, dictionary);
		fclose(in);
	}
	compute_stdev(v, n, dictionary);
	predict(v, n, dictionary);
	free_emails(v, n);
	free_dictionary(&dictionary);
	return 0;
}

void read_email(FILE *in, int email_number, email *v, keywords dict)
{
	char s[NMAX + 1], aux1[NMAX + 1], aux2[NMAX + 1];
	fgets(s, NMAX, in);// date

	fgets(s, NMAX, in);// subject

	fgets(s, NMAX, in);// from

	fgets(s, NMAX, in);//empty line

	while (fgets(s, NMAX, in)) {// body
		v[email_number].body_size += strlen(s);
		strcpy(aux1, s);
		lower(aux1);
		for (int i = 0; i < dict.total; ++i) {
			strcpy(aux2, dict.words[i]);
			lower(aux2);
			v[email_number].keywords[i] += occurences(aux1, aux2);
		}
	}
}

void free_emails(email *v, int n)
{
	for (int i = 0; i < n; ++i)
		free(v[i].keywords);
	free(v);
}

void compute_stdev(email *v, int n, keywords dict)
{
	FILE *out = fopen("statistics.out", "wt");
	for (int i = 0, nrap; i < dict.number; ++i) {
		nrap = 0;
		for (int j = 0; j < n; ++j)
			nrap += v[j].keywords[i];
		double stdev = 0.0, mean = (double)nrap / n;
		for (int j = 0; j < n; ++j)
			stdev += ((v[j].keywords[i] - mean) * (v[j].keywords[i] - mean));
		stdev /= n;
		stdev = sqrt(stdev);
		fprintf(out, "%s %d %.6lf\n", dict.words[i], nrap, stdev);
	}
	fclose(out);
}

int occurences(char *line, char *pattern)
{
	int cnt = 0;
	char *p;
	p = strstr(line, pattern);
	while (p) {
		++cnt;
		p = strstr(p + strlen(pattern), pattern);
	}
	return cnt;
}

void lower(char *s)
{
	for (int i = 0; i < (int)strlen(s); ++i)
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += ('a' - 'A');
}

void predict(email *v, int n, keywords dict)
{

}
