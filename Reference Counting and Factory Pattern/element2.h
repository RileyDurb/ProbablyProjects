/******************************************************************************
filename    element2.h
author      Riley Durbin
email       riley.durbin@digipen.edu
date        11/13/2022

Brief Description: Contatins a functions for creating and using an array element
                   that houses an int *

******************************************************************************/
#ifndef ELEMENT2_H
#define ELEMENT2_H
#include <iostream>
#include "abstract-element.h"

namespace DigiPen {
  class Element2 : public AbstractElement {
    public:
      Element2(int _val);
      Element2(const Element2& rhs); // Shallow copy constructor
      ~Element2();
      AbstractElement* Clone(); // Deep copy clone

      // Getters and setters
      int Get() const;
      void Set(int new_val);
      static int GetTotal();
      static int GetAlive();

      // Operators
      Element2& operator=(const Element2& rhs);

      // Output
      void Print() const;

    private:
      int * p_val;
      static int total, alive; // Reference counts
  };
}
#endif
