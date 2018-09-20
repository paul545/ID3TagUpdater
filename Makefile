#
# Makefile 
#

CC = gcc
CFLAGS = -o -Wall -lcrypto -lssl
LFLAGS =  -pthread
LDLIBS = -lssl -lcrypto
LDFLAGS = -L/urs/local/ssl/lib
LIBS = -pthread

all: TagUpdater

TagUpdater: main.c GetWebpage1.c tagReader.c
	$(CC) $(CFLAGS) -c -o GetWebpage1.o GetWebpage1.c
	$(CC) $(CFLAGS) -c -o main.o main.c 
	$(CC) $(CFLAGS) -c -o tagReader.o tagReader.c
	$(CC) $(LFLAGS) -o TagUpdater main.o GetWebpage1.o  tagReader.o $(LDFLAGS) $(LDLIBS)
	
	
clean:
	rm -f main.o GetWebpage1.o tagReader.o TagUpdater
	

