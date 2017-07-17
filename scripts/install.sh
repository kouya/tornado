#!/bin/sh

PREFIX="/usr/local"
LOCALEPATH="/usr/local/share/locale"

if [ ! -d "$PREFIX/bin" ]; then
  mkdir -p $PREFIX/bin;
fi
install -s tornado $PREFIX/bin

if [ -f "/var/games/tornado.scores" ]; then
  echo "Skipping install of the highscores file. File exists.";
else
  if [ ! -d "/var/games" ]; then
    mkdir /var/games;
  fi;
  install tornado.scores /var/games;
  chmod a+w-x "/var/games/tornado.scores";
fi

echo "Installing manpages ..."
if [ ! -d "$PREFIX/man/man6" ]; then
  mkdir -p $PREFIX/man/man6;
fi
install -m 0644 man/tornado.6 $PREFIX/man/man6
if [ ! -d "$PREFIX/man/de/man6" ]; then
  mkdir -p $PREFIX/man/de/man6;
fi
install -m 0644 man/de/tornado.6 $PREFIX/man/de/man6
if [ ! -d "$PREFIX/man/it/man6" ]; then
  mkdir -p $PREFIX/man/it/man6;
fi
install -m 0644 man/it/tornado.6 $PREFIX/man/it/man6
if [ ! -d "$PREFIX/man/fr/man6" ]; then
  mkdir -p $PREFIX/man/fr/man6;
fi
install -m 0644 man/fr/tornado.6 $PREFIX/man/fr/man6
if [ ! -d "$PREFIX/man/nl/man6" ]; then
  mkdir -p $PREFIX/man/nl/man6;
fi
install -m 0644 man/nl/tornado.6 $PREFIX/man/nl/man6
if [ ! -d "$PREFIX/man/no/man6" ]; then
  mkdir -p $PREFIX/man/no/man6;
fi
install -m 0644 man/no/tornado.6 $PREFIX/man/no/man6
if [ ! -d "$PREFIX/man/ru/man6" ]; then
  mkdir -p $PREFIX/man/ru/man6;
fi
install -m 0644 man/ru/tornado.6 $PREFIX/man/ru/man6

echo "Installing translations ..."
if [ ! -d "$LOCALEPATH/de/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/de/LC_MESSAGES/;
fi
install -m 0644 po/de.mo $LOCALEPATH/de/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/fr/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/fr/LC_MESSAGES/;
fi
install -m 0644 po/fr.mo $LOCALEPATH/fr/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/pt/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/pt/LC_MESSAGES/;
fi
install -m 0644 po/pt.mo $LOCALEPATH/pt/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/es/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/es/LC_MESSAGES/;
fi
install -m 0644 po/es.mo $LOCALEPATH/es/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/it/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/it/LC_MESSAGES/;
fi
install -m 0644 po/it.mo $LOCALEPATH/it/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/nl/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/nl/LC_MESSAGES/;
fi
install -m 0644 po/nl.mo $LOCALEPATH/nl/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/no/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/no/LC_MESSAGES/;
fi
install -m 0644 po/no.mo $LOCALEPATH/no/LC_MESSAGES/tornado.mo
if [ ! -d "$LOCALEPATH/ru/LC_MESSAGES/" ]; then
    mkdir -p $LOCALEPATH/ru/LC_MESSAGES/;
fi
install -m 0644 po/ru.mo $LOCALEPATH/ru/LC_MESSAGES/tornado.mo
