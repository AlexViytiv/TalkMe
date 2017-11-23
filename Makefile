C=gcc
CFLAGS=-Wall -O0 -ggdb
LIBS=`pkg-config --cflags --libs gtk+-2.0`

IDIR=include
SDIR=src
BDIR=bin

INCLUDE= -I$(IDIR)/server -I$(IDIR)/client -I$(IDIR)/common -I$(IDIR)

INSTALL=/usr/local/bin

all: $(BDIR)/talkme_login $(BDIR)/talkme_client $(BDIR)/talkme_server

$(BDIR)/talkme_login: $(SDIR)/talkme.c
	$(C) -o $@ $^ $(INCLUDE) $(CFLAGS) $(LIBS)

$(BDIR)/talkme_client:  $(SDIR)/client/gclient.c $(SDIR)/client/client.c
	$(C) -o $@ $^ $(INCLUDE) $(CFLAGS) $(LIBS)

$(BDIR)/talkme_server: $(SDIR)/server/server.c $(SDIR)/server/db-worker.c
	$(C) -o $@ $^ $(INCLUDE) $(CFLAGS)

.PHONY: clean install uninstall

install:
	sudo mkdir -p $(INSTALL)
	sudo install -D $(BDIR)/talkme_login $(INSTALL)
	sudo install -D $(BDIR)/talkme_client $(INSTALL)
	sudo install -D $(BDIR)/talkme_server $(INSTALL)

uninstall:
	sudo rm $(INSTALL)/talkme_login
	sudo rm $(INSTALL)/talkme_client
	sudo rm $(INSTALL)/talkme_server

clean:
	rm -f $(BDIR)/*

