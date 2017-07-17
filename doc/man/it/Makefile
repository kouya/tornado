include ../../../Makefile.inc
VERSION = `grep " VERSION" ../../../version.h | sed s/\"//g | sed s/\#define\ VERSION\ //`

all: tornado.6

tornado.6: tornado.6.it.in
	cat tornado.6.it.in | sed s#PREFIX#$(PREFIX)# | \
	sed s/VERSION/$(VERSION)/ > tornado.6

install-man: tornado.6
	@echo "Installing Italian manpage ..."
	if [ ! -d "$(PREFIX)/man/it/man6" ]; then \
	  mkdir -p $(PREFIX)/man/it/man6; \
	fi
	install -m 0644 tornado.6 $(PREFIX)/man/it/man6

clean:
	rm -f tornado.6

uninstall:
	rm -f $(PREFIX)/man/it/man6/tornado.6
