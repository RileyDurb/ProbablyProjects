# Provided by my professor, Dimitri Volper, and edited by me makefile for compiling and running tests on my custom memory manager
FLAGS_OPT=-O3
LAGS_OPT=-O3

CFLAGS_DEBUG=-g

CXX=g++

CFLAGS=-std=c++11

CFLAGS+=

CFLAGS := $(CFLAGS_OPT) $(CFLAGS)
LDFLAGS := -flto -fwhole-program
OBJDIR = obj


TARGET_FILE = ObjectAllocator


default: 
	$(CXX) $(CFLAGS) $(CFLAGS+) $(FLAGS_OPT) -o $(TARGET_FILE) driver.cpp ObjectAllocator.cpp PRNG.cpp  

1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18:

	./$(TARGET_FILE) $@ > myout$@
	diff myout$@ out$@ > diff$@ --strip-trailing-cr
all: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
run:
	./$(TARGET_FILE) > debug.txt

clean:
	-rm -rf $(OBJDIR) $(TARGET_FILE) myout* diff*

diff:
	diff debug.txt output-sample-zeros-ec-LP64.txt --strip-trailing-cr
	
memchk:
	valgrind -q --leak-check=full --show-reachable=yes --tool=memcheck ./$(TARGET_FILE)