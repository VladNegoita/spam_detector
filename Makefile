# Copyright 2020 Darius Neatu <neatudarius@gmail.com>

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=spam_detector

build: $(TARGETS)

spam_detector: spam_detector.c words.c
	$(CC) $(CFLAGS) spam_detector.c words.c -o spam_detector

pack:
	zip -FSr 311CA_VladNegoita_Tema4.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
