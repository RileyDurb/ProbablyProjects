/******************************************************************************
filename    abstract-element.h
author      Riley Durbin
DP email    riley.durbin@digipen.edu
course      CS225
section     A
Assignment  4
due date    11/13/2022

Brief Description: Virtual class for having elements inherit from it

******************************************************************************/
#ifndef ABSTRACT_ELEMENT_H
#define ABSTRACT_ELEMENT_H

namespace DigiPen {
  // Base class for array elements
  class AbstractElement 
  {
  public:
    virtual ~AbstractElement(){};
    virtual int Get() const = 0;
    virtual void Print() const = 0;
    virtual void Set(int new_val) = 0;
    virtual AbstractElement* Clone() = 0;
  };
}
#endif