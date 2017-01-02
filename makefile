CC=g++
EXE=hvm
CFLAGS= -std=c++11 -g -O3 -c -Wall -pedantic

.DEFAULT_GOAL := $(EXE)

obj/main.o: main.cpp wing.hpp vortex.hpp
	$(CC) $(CFLAGS) main.cpp -o obj/main.o

obj/wing.o: wing.cpp wing.hpp vortex.hpp
	$(CC) $(CFLAGS) wing.cpp -o obj/wing.o

obj/vortex.o:vortex.cpp vortex.hpp
	$(CC) $(CFLAGS) vortex.cpp -o obj/vortex.o

$(EXE): obj/main.o  obj/wing.o obj/vortex.o
	$(CC) -std=c++11 -g obj/*.o -o $(EXE)

clean:
	rm $(EXE); rm obj/*
