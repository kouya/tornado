include ../../../Makefile.inc
VERSION = `grep " VERSION" ../../../version.h | sed s/\"//g | sed s/\#define\ VERSION\ //`

all: tornado.6

tornado.6: tornado.6.nl.in
	cat tornado.6.nl.in | sed s#PREFIX#$(PREFIX)# | \
	sed s/VERSION/$(VERSION)/ > tornado.6

install-man: tornado.6
	@echo "Installing Dutch manpage ..."
	if [ ! -d "$(PREFIX)/man/nl/man6" ]; then \
	  mkdir -p $(PREFIX)/man/nl/man6; \
	fi
	install -m 0644 tornado.6 $(PREFIX)/man/nl/man6

clean:
	rm -f tornado.6

uninstall:
	rm -f $(PREFIX)/man/nl/man6/tornado.6
