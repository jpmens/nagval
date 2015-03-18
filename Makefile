CC=gcc
CFLAGS= -Wall -Werror
LIBS="-L/usr/local/lib"
EXTRALIBS="-lpthread"

all: nagval nagval.8 README.md

txt: txt.c
	$(CC) $(CFLAGS) txt.c -o txt -lval-threads -lsres -lcrypto

nagval: nagval.c Makefile
	$(CC) $(CFLAGS) nagval.c -o nagval $(LIBS) -lval-threads -lsres -lcrypto -lssl $(EXTRALIBS)

nagval.8: nagval.pandoc
	pandoc -s -w man nagval.pandoc -o nagval.8

README.md: nagval.pandoc
	pandoc -w markdown nagval.pandoc -o README.md
