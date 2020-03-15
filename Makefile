CC=g++
OBJS= server server.o digraph.o
CFLAGS=-c -Wall -O2
PROGRAM=prog

PROGRAM: $(OBJS)

server: server.o digraph.o
	$(CC) server.o digraph.o -o server

server.o: server.cpp
	$(CC) server.cpp -o server.o $(CFLAGS)


digraph.o: digraph.cpp
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)


clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)
