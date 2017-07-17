CC = gcc
MAKE = make
LDFLAGS = -lncurses
OBJFILES = main.o draw.o erwin.o network.o scores.o
PREFIX = /usr/local
LOCALEPATH = /usr/local/share/locale
CFLAGS = -Wall -O2 -DPREFIX="\"$(PREFIX)\"" -DLOCALEPATH="\"$(LOCALEPATH)\""
LOCALES = de
MAN = doc/man
