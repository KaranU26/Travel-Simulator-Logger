CC=g++
CFLAGS=-I
CFLAGS+=-Wall
FILE=LogServer.cpp
LIBS=-lpthread

LogServer: $(FILE)
	$(CC) $(CFLAGS) $(FILE) -o LogServer $(LIBS)

clean:
	rm -f *.o LogServer
	
all: clean LogServer
