/******************************************************************************
filename    element1.h
author      Riley Durbin
email       ridurbin0@gmail.com
date        11/13/2022

Brief Description: Contatins a functions for creating and using an array element
                   that houses an int

******************************************************************************/
#ifndef ELEMENT1_H
#define ELEMENT1_H
#include <iostream>
#include "abstract-element.h"


namespace DigiPen {
  class Element1 : public AbstractElement {
    public:
      Element1(int _val);
      Element1(const Element1& rhs); // Shallow copy constructor
      ~Element1();
      AbstractElement* Clone(); // Deep copy clone

      // getters and setters
      int Get() const;
      void Set(int new_val);
      static int GetTotal();
      static int GetAlive();

      // Output
      void Print() const;


    private:
      int val;
      static int total, alive; // Refecence counts
  };
}
#endif
