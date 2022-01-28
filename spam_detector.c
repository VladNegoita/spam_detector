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
	v[email_number].from = (char *)malloc((strlen(s) + 1) * sizeof(char));
	strcpy(v[email_number].from, s);

	fgets(s, NMAX, in);//empty line

	v[email_number].body = 0;
	v[email_number].caps = 0;
	while (fgets(s, NMAX, in)) {// body
		v[email_number].body += strlen(s);
		v[email_number].caps += caps_number(s);
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
	for (int i = 0; i < n; ++i) {
		free(v[i].keywords);
		free(v[i].from);
	}
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

int caps_number(char *s)
{
	int cnt = 0;
	for (int i = 0; i < (int)strlen(s); ++i)
		if (s[i] >= 'A' && s[i] <= 'Z')
			++cnt;
	return cnt;
}

void predict(email *v, int n, keywords dict)
{
	criteria1(v, n);
	criteria2(v, n);
	criteria3(v, n, dict);

	FILE *out = fopen("prediction.out", "wt");
	for (int i = 0; i < n; ++i) {
		if (v[i].spam_value > LIMIT)
			fprintf(out, "%d\n", 1);
		else
			fprintf(out, "%d\n", 0);
	}
	fclose(out);
}

void criteria1(email *v, int n)
{
	for (int i = 0; i < n; ++i) {
		if (2 * v[i].caps > v[i].body / 2)
			v[i].spam_value = 30.0;
		else
			v[i].spam_value = 0.0;
	}
}

void criteria2(email *v, int n)
{
	FILE *in = fopen("./data/spammers", "rt");
	if (!in) {
		fprintf(stderr, "spammers list missing");
		exit(-1);
	}

	int known_spammers, *values;
	char s[NMAX + 1], **a;
	fscanf(in, "%d", &known_spammers);
	a = (char **)malloc(known_spammers * sizeof(char *));
	values = (int *)malloc(known_spammers * sizeof(int));

	for (int i = 0; i < known_spammers; ++i) {
		fscanf(in, "%s %d", s, values + i);
		a[i] = (char *)malloc((strlen(s) + 1) * sizeof(char));
		strcpy(a[i], s);
	}

	fclose(in);

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < known_spammers; ++j)
			if (strstr(v[i].from, a[j]))
				v[i].spam_value += values[j];

	for (int i = 0; i < known_spammers; ++i)
		free(a[i]);
	free(a);
	free(values);
}

void criteria3(email *v, int n, keywords dict)
{
	double avg_body = 0.0;
	for (int i = 0; i < n; ++i)
		avg_body += v[i].body;
	avg_body /= n;

	for (int i = 0; i < n; ++i) {
		int word_count = 0;
		for (int j = 0; j < dict.total; ++j)
			word_count += v[i].keywords[j];
		v[i].spam_value += 10 * (avg_body / v[i].body) * word_count;
	}
}
