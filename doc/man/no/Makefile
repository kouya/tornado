include ../../../Makefile.inc
VERSION = `grep " VERSION" ../../../version.h | sed s/\"//g | sed s/\#define\ VERSION\ //`

all: tornado.6

tornado.6: tornado.6.no.in
	cat tornado.6.no.in | sed s#PREFIX#$(PREFIX)# | \
	sed s/VERSION/$(VERSION)/ > tornado.6

install-man: tornado.6
	@echo "Installing Norwegian manpage ..."
	if [ ! -d "$(PREFIX)/man/no/man6" ]; then \
	  mkdir -p $(PREFIX)/man/no/man6; \
	fi
	install -m 0644 tornado.6 $(PREFIX)/man/no/man6

clean:
	rm -f tornado.6

uninstall:
	rm -f $(PREFIX)/man/no/man6/tornado.6
