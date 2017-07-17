CC = gcc
MAKE = make
LDFLAGS = -lncurses
OBJFILES = main.o draw.o erwin.o network.o scores.o
PREFIX = /usr/local
LOCALEPATH = /usr/local/share/locale
CFLAGS = -Wall -O2 -DPREFIX="\"$(PREFIX)\"" 
VERSION = `grep " VERSION" version.h | sed s/\"//g | sed s/\#define\ VERSION\ //`
LOCALES = de
MAN = doc/man
DISTDIR = tornado-$(VERSION)

###                                                       ###
### You shouldn't need to edit anything beyond this point ###
###                                                       ###

include Makefile.distfiles

all: tornado tornado.6 locales

locales:
	$(MAKE) -C po all

tornado.6:
	$(MAKE) -C $(MAN) all

tornado: $(OBJFILES)
	$(CC) $(LDFLAGS) $(OBJFILES) -o tornado

debug: tornado.6 locales
	gcc -g -ggdb -Wall -ansi -pedantic -o tornado draw.c main.c erwin.c network.c scores.c -lncurses -DPREFIX="\"$(PREFIX)\"" -DLOCALEPATH="\"$(LOCALEPATH)\"" 

static: tornado.6 locales
	gcc -s -O2 -Wall -static -o tornado draw.c main.c erwin.c network.c scores.c -lncurses -DPREFIX="\"$(PREFIX)\"" -DLOCALEPATH="\"$(LOCALEPATH)\""

new: clean tornado tornado.6 locales

clean:
	rm -f tornado *.o
	rm -f tornado-$(VERSION).tar.gz
	$(MAKE) -C po clean
	$(MAKE) -C $(MAN) clean

install: tornado tornado.6 install-locale-data
	@echo "Installing tornado binary in $(PREFIX)/bin..."
	if [ ! -d "$(PREFIX)/bin" ]; then \
	  mkdir -p $(PREFIX)/bin; \
	fi
	install -s tornado $(PREFIX)/bin

	$(MAKE) -C $(MAN) install-man

	if [ -f "/var/games/tornado.scores" ]; then \
	  echo "Skipping install of the highscores file. File exists."; \
	  cat /var/games/tornado.scores | ./convert-highscorefile > score.tmp; \
	  mv -f score.tmp /var/games/tornado.scores; \
	  chmod a+rw-x "/var/games/tornado.scores"; \
	else \
	  if [ ! -d "/var/games" ]; then \
	    mkdir /var/games; \
	  fi; \
	  install tornado.scores /var/games; \
	  chmod a+w-x "/var/games/tornado.scores"; \
	fi

install-locale-data:
	$(MAKE) -C po install-locale-data

uninstall:
	rm -f $(PREFIX)/bin/tornado
	rm -f /var/games/tornado.scores
	$(MAKE) -C $(MAN) uninstall
	$(MAKE) -C po uninstall

tornado-no-locales: tornado tornado.6

tornado-no-locales-install: tornado tornado.6
	if [ ! -d "$(PREFIX)/bin" ]; then \
	  mkdir -p $(PREFIX)/bin; \
	fi
	install -s tornado $(PREFIX)/bin
	if [ ! -d "$(PREFIX)/man/man6" ]; then \
	  mkdir -p $(PREFIX)/man/man6; \
	fi
	install -m 0644 tornado.6 $(PREFIX)/man/man6

	if [ -f "/var/games/tornado.scores" ]; then \
	  echo "Skipping install of the highscores file. File exists."; \
	  cat /var/games/tornado.scores | ./convert-highscorefile > score.tmp; \
	  mv -f score.tmp /var/games/tornado.scores; \
	  chmod a+rw-x "/var/games/tornado.scores"; \
	else \
	  if [ ! -d "/var/games" ]; then \
	    mkdir /var/games; \
	  fi; \
	  install tornado.scores /var/games; \
	  chmod a+w-x "/var/games/tornado.scores"; \
	fi

dist: clean
	mkdir $(DISTDIR)
	cp -R $(DISTFILES) $(DISTDIR)
	tar -czf $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

dist-binary: clean static
	mkdir $(DISTDIR)
	mkdir $(DISTDIR)/man
	mkdir $(DISTDIR)/man/de
	mkdir $(DISTDIR)/man/fr
	mkdir $(DISTDIR)/man/it
	mkdir $(DISTDIR)/man/nl
	mkdir $(DISTDIR)/man/no
	mkdir $(DISTDIR)/man/ru
	mkdir $(DISTDIR)/po
	cp AUTHOR COPYING CREDITS Changelog README INSTALL.binary TODO tornado tornado.scores $(DISTDIR)
	cp doc/man/de/tornado.6 $(DISTDIR)/man/de
	cp doc/man/fr/tornado.6 $(DISTDIR)/man/fr
	cp doc/man/it/tornado.6 $(DISTDIR)/man/it
	cp doc/man/nl/tornado.6 $(DISTDIR)/man/nl
	cp doc/man/no/tornado.6 $(DISTDIR)/man/no
	cp doc/man/ru/tornado.6 $(DISTDIR)/man/ru
	cp doc/man/tornado.6 $(DISTDIR)/man
	cp po/*.mo $(DISTDIR)/po
	cp scripts/install.sh $(DISTDIR)
	tar -czf $(DISTDIR).i386.tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

dist-binary-ppc: clean tornado tornado.6
	mkdir $(DISTDIR)
	cp $(BINARYDISTFILES) $(DISTDIR)
	tar -czf $(DISTDIR).ppc-darwin.tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

dist-slack: clean tornado tornado.6 locales
	mkdir -p $(DISTDIR)/usr/bin
	mkdir -p $(DISTDIR)/usr/share/locale/de/LC_MESSAGES
	mkdir -p $(DISTDIR)/usr/share/locale/fr/LC_MESSAGES
	mkdir -p $(DISTDIR)/usr/share/locale/es/LC_MESSAGES
	mkdir -p $(DISTDIR)/usr/share/locale/pt/LC_MESSAGES
	mkdir -p $(DISTDIR)/usr/share/locale/it/LC_MESSAGES
	mkdir -p $(DISTDIR)/usr/man/man6
	mkdir -p $(DISTDIR)/usr/man/de/man6
	mkdir -p $(DISTDIR)/usr/man/fr/man6
	mkdir -p $(DISTDIR)/usr/doc/$(DISTDIR)
	mkdir -p $(DISTDIR)/install
	cp tornado $(DISTDIR)/usr/bin
	cp po/de.mo $(DISTDIR)/usr/share/locale/de/LC_MESSAGES
	cp po/fr.mo $(DISTDIR)/usr/share/locale/fr/LC_MESSAGES
	cp po/es.mo $(DISTDIR)/usr/share/locale/es/LC_MESSAGES
	cp po/pt.mo $(DISTDIR)/usr/share/locale/pt/LC_MESSAGES
	cp po/it.mo $(DISTDIR)/usr/share/locale/it/LC_MESSAGES
	cp doc/man/tornado.6 $(DISTDIR)/usr/man/man6
	cp doc/man/de/tornado.6 $(DISTDIR)/usr/man/de/man6
	cp doc/man/fr/tornado.6 $(DISTDIR)/usr/man/fr/man6
	cp AUTHOR COPYING CREDITS Changelog README TODO $(DISTDIR)/usr/doc/$(DISTDIR)
	cp scripts/slack-desc $(DISTDIR)/install
	( cd $(DISTDIR) ; tar -czf ../$(DISTDIR)-i386-1.tgz usr install )
