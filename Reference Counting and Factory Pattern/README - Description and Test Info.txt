Reference counting and the factory pattern:
Hey! This project focuses on a example implementation fo the factory design pattern, which allows making
multiple similar data types to be used for similar purposes, by using abstract class to store them.
Each element derived from abstract element also uses reference counting, so that when the array is copied,
the copies share the same data, and can avoid allocating new data, until the copy is changed. This delayed copying
is also known as a lazy approach, and has benefits here, becuase you may never change the data, and never need a new copy.

Main code files:
abstract-element.h
array.cpp
array.h
element-factory.cpp
element-factory.h
element1.cpp
element1.h
element2.cpp
element1.h

If you would like to compile this project, and/ or run the included tests, please follow the below instructions:
1) Open up a command line console inside the folder (such as typing cmd into the file explorer's folder path bar,
in the upper left of the explorer)
2) Type make to compile the code
3) Type make all, or make (and any number 1-16) to run the various tests of different array operations.

The myout1-16 files show the operations done on the array, and if correct, there will be no more output in the console
after the diff line on each test.
