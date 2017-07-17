include ../../../Makefile.inc
VERSION = `grep " VERSION" ../../../version.h | sed s/\"//g | sed s/\#define\ VERSION\ //`

all: tornado.6

tornado.6: tornado.6.de.in
	cat tornado.6.de.in | sed s#PREFIX#$(PREFIX)# | \
	sed s/VERSION/$(VERSION)/ > tornado.6

install-man: tornado.6
	@echo "Installing german manpage ..."
	if [ ! -d "$(PREFIX)/man/de/man6" ]; then \
	  mkdir -p $(PREFIX)/man/de/man6; \
	fi
	install -m 0644 tornado.6 $(PREFIX)/man/de/man6

clean:
	rm -f tornado.6

uninstall:
	rm -f $(PREFIX)/man/de/man6/tornado.6
