include ../../Makefile.inc
VERSION = `grep " VERSION" ../../version.h | sed s/\"//g | sed s/\#define\ VERSION\ //`

all: tornado.6

tornado.6: tornado.6.in
	cat tornado.6.in | sed s#PREFIX#$(PREFIX)# | \
	sed s/VERSION/$(VERSION)/ > tornado.6
	$(MAKE) -C de tornado.6 all
	$(MAKE) -C fr tornado.6 all
	$(MAKE) -C it tornado.6 all
	$(MAKE) -C ru tornado.6 all
	$(MAKE) -C nl tornado.6 all
	$(MAKE) -C no tornado.6 all

install-man: tornado.6
	@echo "Installing manpage ..."
	if [ ! -d "$(PREFIX)/man/man6" ]; then \
	  mkdir -p $(PREFIX)/man/man6; \
	fi
	install -m 0644 tornado.6 $(PREFIX)/man/man6
	$(MAKE) -C de install-man
	$(MAKE) -C fr install-man
	$(MAKE) -C it install-man
	$(MAKE) -C ru install-man
	$(MAKE) -C nl install-man
	$(MAKE) -C no install-man

clean:
	rm -f tornado.6
	$(MAKE) -C de clean
	$(MAKE) -C fr clean
	$(MAKE) -C it clean
	$(MAKE) -C ru clean
	$(MAKE) -C nl clean
	$(MAKE) -C no clean

uninstall:
	rm -f $(PREFIX)/man/man6/tornado.6
	$(MAKE) -C de uninstall
	$(MAKE) -C fr uninstall
	$(MAKE) -C it uninstall
	$(MAKE) -C ru uninstall
	$(MAKE) -C nl uninstall
	$(MAKE) -C no uninstall
