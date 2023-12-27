Object Allocator:
Hey! This is my implementation of a custom memory manager. The manager can allocate pages of same-sized objects, and along with the space for
their normal data, these objects can have different kinds of headers to keep track of memory info. Then, the manager can free these objects, and keep 
track of what pages are free, for reusing, and what memory is currently allocated. There are also multiple debug stats that can be enabled, and there is
validation functionality that can be called to check and report on any data that is overrun.

Main code files:
ObjectAllocator.cpp
ObjectAllocator.h

Also, there are some included tests you can run. If you would like to do so, please follow these instructions:

Also, if you'd like to complile it and run the provided test, please follow these instructios.
1) Open up a command line console inside the folder (such as typing cmd into the file explorer's folder path bar,
in the upper left of the explorer)
2) Type make to compile the code
3) Type make all to run all tests, or make (and any number 1-18) to run single tests

Depending on the test, the state of the memory pages after test steps, or debug info, will be output in the myout1-18 files, and if there
is no more output to the console after the diff line of one test, before the next test, then the tests succeded.