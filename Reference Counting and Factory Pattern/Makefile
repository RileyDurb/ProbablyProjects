# Provided by my professor, and modified by me, this makefile runs the different tests for the array class using abstract elements, and associated functionality.

PRG=RefcountingAndFactory.exe

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -ansi -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines

OBJECTS0=array.cpp element1.cpp element2.cpp element-factory.cpp
DRIVER0=driver.cpp

OSTYPE := $(shell uname)
#ifeq ($(OSTYPE),Linux)
ifeq ($(OSTYPE),$(filter $(OSTYPE),Linux Darwin))
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16:
	echo "running test$@"
	./$(PRG) $@ >myout$@
	diff out$@ myout$@ $(DIFF_OPTIONS) > difference$@
all: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
mem0 mem1 mem2 mem3 mem4 mem5 mem6 mem7 mem8 mem9 mem10 mem11 mem12 mem13 mem14 mem15 mem16 mem17:
	echo "running memory test $@"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
allMem: mem0 mem1 mem2 mem3 mem4 mem5 mem6 mem7 mem8 mem9 mem10 mem11 mem12 mem13 mem14 mem15 mem16 mem17
drm0 drm1 drm2 drm3 drm4 drm5 drm6 drm7 drm8 drm9 drm10 drm11 drm12 drm13 drm14 drm15 drm16 drm17 drm18:
	@echo "running memory test $@"
	drmemory -batch -no_summary --  ./$(PRG) $(subst drm,,$@) 1>/dev/null 2>difference$@
clean:
	rm -f *.exe *.o
