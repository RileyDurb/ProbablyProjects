# Makefile, template provided by Dimitri Volper, and edited by me, for testing the electric dijkstra function
PRG=gnu.exe

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++11 -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

VALGRIND_OPTIONS=-q --leak-check=full
DIFFLAGS=--strip-trailing-cr -y --suppress-common-lines

OBJECTS0=e-dijkstra.cpp
DRIVER0=driver.cpp

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24:
	@echo "running test$@"
	@echo "should run in less than 100 ms"
	time ./$(PRG) $@ >myout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ myout$@ $(DIFFLAGS)
all short: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
25 26 27 28 29:
	@echo "running test$@"
	@echo "should run under 300 ms for all, likely a lot shorter"
	time ./$(PRG) $@ >myout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ myout$@ $(DIFFLAGS)
all long: 25 26 27 28 29
30 31 32 33 34 35:
	@echo "running test$@"
	@echo "should run under 2000 ms for all, likely a lot shorter"
	time ./$(PRG) $@ >myout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ myout$@ $(DIFFLAGS)
all supalong: 30 31 32 33 34
mem0 mem9:
	@echo "running memory test $@"
	@echo "should run in less than 1500 ms"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
	@echo "lines after this are memory errors"; cat difference$@
clean:
	rm -f *.exe *.o my* difference*
