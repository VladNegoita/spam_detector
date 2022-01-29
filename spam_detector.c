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

	fgets(s, NMAX, in);// empty line

	v[email_number].body = 0;
	v[email_number].caps = 0;
	v[email_number].letters = 0;
	v[email_number].invalid_words = 0;
	while (fgets(s, NMAX, in)) {// body
		v[email_number].body += strlen(s);
		v[email_number].caps += caps_number(s);
		v[email_number].letters += letter_number(s);
		strcpy(aux2, s);
		v[email_number].invalid_words += count_invalids(aux2);
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

int is_lower(char c)
{
	return (c >= 'a' && c <= 'z');
}

int is_upper(char c)
{
	return (c >= 'A' && c <= 'Z');
}

int is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

int is_letter(char c)
{
	return (is_upper(c) || is_lower(c));
}

int is_vowel(char c)
{
	if (c == 'a' || c == 'A')
		return 1;
	if (c == 'e' || c == 'E')
		return 1;
	if (c == 'i' || c == 'I')
		return 1;
	if (c == 'o' || c == 'O')
		return 1;
	if (c == 'u' || c == 'U')
		return 1;
	if (c == 'y' || c == 'Y')///phonetic
		return 1;
	return 0;
}

void lower(char *s)
{
	for (int i = 0; i < (int)strlen(s); ++i)
		if (is_upper(s[i]))
			s[i] += ('a' - 'A');
}

int caps_number(char *s)
{
	int cnt = 0;
	for (int i = 0; i < (int)strlen(s); ++i)
		if (is_upper(s[i]))
			++cnt;
	return cnt;
}

int letter_number(char *s)
{
	int cnt = 0;
	for (int i = 0; i < (int)strlen(s); ++i)
		if (is_letter(s[i]))
			++cnt;
	return cnt;
}

int count_invalids(char *s)
{
	int cnt = 0;
	char *p, sep[20] = "\n .,!?;:\"\'()*";
	p = strtok(s, sep);
	while (p) {
		cnt += is_invalid(p);
		p = strtok(NULL, sep);
	}
	return cnt;
}

int is_invalid(char *s)
{
	if (strstr(s, "http"))
		return 1;

	for (int i = 0; i < (int)strlen(s); ++i) {
		if (i > 0 && is_lower(s[i - 1]) && is_upper(s[i]))//aA
			return 1;

		if (i > 1 && s[i - 2] == s[i - 1] && s[i - 1] == s[i])///aaa
			return 1;

		if (i > 3 && !is_vowel(s[i]) && !is_vowel(s[i - 1]))///fsrds
			if (!is_vowel(s[i - 2]) && !is_vowel(s[i - 3]))
				if (!is_vowel(s[i - 4]))
					return 1;

		if (i > 0 && is_letter(s[i - 1]) && !is_letter(s[i]))
			return 1;

		if (i > 0 && !is_letter(s[i - 1]) && is_letter(s[i]))
			return 1;
	}
	return 0;
}

void predict(email *v, int n, keywords dict)
{
	criteria1(v, n);
	criteria2(v, n);
	criteria3(v, n, dict);
	criteria4(v, n);
	criteria5(v, n);

	FILE *out = fopen("prediction.out", "wt");
	for (int i = 0; i < n; ++i) {
		printf("%lf\n", v[i].spam_value);
		if (v[i].spam_value > LIMIT)
			fprintf(out, "%d\n", 1);
		else
			fprintf(out, "%d\n", 0);
	}
	printf("%lf\n", v[2].spam_value);
	fclose(out);
}

void criteria1(email *v, int n)// uppercase
{
	for (int i = 0; i < n; ++i)
		if (v[i].caps > v[i].letters / 2)
			v[i].spam_value = 30.0;
		else
			v[i].spam_value = 50 * (v[i].caps / v[i].letters);
}

void criteria2(email *v, int n)// spammers
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

void criteria3(email *v, int n, keywords dict)// keywords
{
	double avg_body = 0.0;
	for (int i = 0; i < n; ++i)
		avg_body += v[i].body;
	avg_body /= n;

	for (int i = 0; i < n; ++i) {
		int word_count = 0;
		for (int j = 0; j < dict.total; ++j)
			word_count += v[i].keywords[j];
		v[i].spam_value += 7.5 * (avg_body / v[i].body) * word_count;
	}
}

void criteria4(email *v, int n)// body_length
{
	double avg_body = 0.0;
	for (int i = 0; i < n; ++i)
		avg_body += v[i].body;
	avg_body /= n;

	double stdev = 0.0;
	for (int i = 0; i < n; ++i)
		stdev += (v[i].body - avg_body) * (v[i].body - avg_body);
	stdev /= n;
	stdev = sqrt(stdev);

	double range = 2.0;
	for (int i = 0; i < n; ++i)
		if (v[i].body <= avg_body - range * stdev)
			v[i].spam_value += 5;
		else if (v[i].body >= avg_body + range * stdev)
			v[i].spam_value += 5;
}

int max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

void criteria5(email *v, int n)// invalid words
{
	double avg_body = 0.0;
	for (int i = 0; i < n; ++i)
		avg_body += v[i].body;
	avg_body /= n;

	for (int i = 0; i < n; ++i) {
		double aux = (double)max(v[i].invalid_words - 10, 0);
		aux = (aux * (avg_body / v[i].body)) / 5.0;
		v[i].spam_value += aux;
	}
}
