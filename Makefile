CC=gcc
BINNAME=threadracer

all:
	$(CC) threadracer.c -o $(BINNAME) -lpthread

clean:
	rm -rf $(BINNAME)
