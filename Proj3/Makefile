CC=gcc

all: MirrorServer ContentServer MirrorInitiator

MirrorServer:
	$(CC) -o MirrorServer MirrorServer.c -lpthread

ContentServer:
	$(CC) -o ContentServer ContentServer.c -lpthread

MirrorInitiator:
	$(CC) -o MirrorInitiator MirrorInitiator.c



clean:
	rm -f MirrorServer
	rm -f ContentServer
	rm -f MirrorInitiator

.PHONY: all clean
