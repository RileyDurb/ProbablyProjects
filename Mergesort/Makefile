# Makefile provided by Dimitri Volper, and edited by me, for testing the mergesort function

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++11 -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Wunused-parameter

PRG=gnu

MSC=cl
MSCFLAGS=/EHa /W4 /Za /Zc:forScope /nologo /D_CRT_SECURE_NO_DEPRECATE /D"_SECURE_SCL 0" /O2i /GL

VALGRIND_OPTIONS=-q --leak-check=full
DIFFLAGS=--strip-trailing-cr

OBJECTS0=mergesort.cpp
DRIVER0=driver.cpp

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
msc0:
	$(MSC) /Fe$@.exe           $(DRIVER0) $(OBJECTS0) $(MSCFLAGS)
0:
	@echo "running test$@"
	time ./$(PRG) $@ > myout$@
#	watchdog 1000 
	diff $(DIFFLAGS) out0 myout0
mem0:
	time $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
#	watchdog 8000 valgrind 
clean:
	rm -f *gcno *gcda *gcov *.exe *.o *.obj *.tds difference* myout*
