CFLAGS= -std=c++11 -g -O3 -c -Wall -pedantic

.DEFAULT_GOAL := hvm

obj/main.o: main.cpp wing.hpp vortex.hpp
	g++ $(CFLAGS) main.cpp -o obj/main.o

obj/wing.o: wing.cpp wing.hpp vortex.hpp
	g++ $(CFLAGS) wing.cpp -o obj/wing.o

obj/vortex.o:vortex.cpp vortex.hpp
	g++ $(CFLAGS) vortex.cpp -o obj/vortex.o

hvm: obj/main.o  obj/wing.o obj/vortex.o
	g++ -std=c++11 -g obj/*.o -o hvm

clean:
	rm hvm; rm obj/*
