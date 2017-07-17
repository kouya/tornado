include ../Makefile.inc
VERSION = `echo VERSION | cpp -P -include ../version.h | tr -d '" ' | grep -v '^$$'`

all: locales

locales: de.po fr.po pt.po es.po it.po ru.po nl.po no.po
	msgfmt de.po -o de.mo
	msgfmt fr.po -o fr.mo
	msgfmt pt.po -o pt.mo
	msgfmt es.po -o es.mo
	msgfmt it.po -o it.mo
	msgfmt ru.po -o ru.mo
	msgfmt nl.po -o nl.mo
	msgfmt no.po -o no.mo

install-locale-data: de.mo fr.mo pt.mo es.mo it.mo ru.mo nl.mo no.po
	if [ ! -d "$(LOCALEPATH)/de/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/de/LC_MESSAGES/; \
	fi
	install -m 0644 de.mo $(LOCALEPATH)/de/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/fr/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/fr/LC_MESSAGES/; \
	fi
	install -m 0644 fr.mo $(LOCALEPATH)/fr/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/pt/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/pt/LC_MESSAGES/; \
	fi
	install -m 0644 pt.mo $(LOCALEPATH)/pt/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/es/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/es/LC_MESSAGES/; \
	fi
	install -m 0644 es.mo $(LOCALEPATH)/es/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/it/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/it/LC_MESSAGES/; \
	fi
	install -m 0644 it.mo $(LOCALEPATH)/it/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/ru/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/ru/LC_MESSAGES/; \
	fi
	install -m 0644 ru.mo $(LOCALEPATH)/ru/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/nl/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/nl/LC_MESSAGES/; \
	fi
	install -m 0644 nl.mo $(LOCALEPATH)/nl/LC_MESSAGES/tornado.mo
	if [ ! -d "$(LOCALEPATH)/no/LC_MESSAGES/" ]; then \
	  mkdir -p $(LOCALEPATH)/no/LC_MESSAGES/; \
	fi
	install -m 0644 no.mo $(LOCALEPATH)/no/LC_MESSAGES/tornado.mo

clean:
	rm -f *.mo

uninstall:
	rm -f $(LOCALEPATH)/de/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/fr/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/pt/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/es/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/it/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/ru/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/nl/LC_MESSAGES/tornado.mo
	rm -f $(LOCALEPATH)/no/LC_MESSAGES/tornado.mo
