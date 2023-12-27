/******************************************************************************
filename    PRNG.h
author      Dimitri Volper

Brief Description: Provided by my professor, contains functions for the random
    used in the driver of tests for the various features of a custom memory manager.
    // Original code: http://remus.rutgers.edu/~rhoads/Code/random2.c

******************************************************************************/
//---------------------------------------------------------------------------
#ifndef PRNGH
#define PRNGH
//---------------------------------------------------------------------------

namespace Digipen
{
  namespace Utils
  {
    unsigned rand(void);              // returns a random 32-bit integer
    void srand(unsigned, unsigned);   // seed the generator
    int Random(int low, int high);    // range
  }
}
#endif
