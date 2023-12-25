/******************************************************************************
filename    element1.cpp
author      Riley Durbin
email       ridurbin0@gmail.com
date        11/13/2022

Brief Description: Contatins a functions for creating and using an array element
                   that houses an int

******************************************************************************/
#include "element1.h"

int DigiPen::Element1::total = 0;
int DigiPen::Element1::alive = 0;

DigiPen::Element1::Element1(int _val)
 : AbstractElement()
 , val(_val)
{
  // Update reference counts
  ++total; 
  ++alive;
}

DigiPen::Element1::Element1(const Element1& rhs) 
	: AbstractElement(), val(rhs.val) 
{ 
  // Update reference counts
  ++total;
  ++alive;
}

DigiPen::Element1::~Element1() { --alive; }

DigiPen::AbstractElement* DigiPen::Element1::Clone()
{
	AbstractElement* newElement = new Element1(this->val); // Deep copy
	
	return newElement;
}

// Getters and setters ////////////////////////////////////////////////////////////////////////////
int DigiPen::Element1::Get() const { return val; }

void DigiPen::Element1::Set(int new_val) { val=new_val; }

int DigiPen::Element1::GetTotal() { return total; }

int DigiPen::Element1::GetAlive() { return alive; }

// Output /////////////////////////////////////////////////////////////////////////////////////////

void DigiPen::Element1::Print() const { 
  std::cout << "[" << Get() << "] "; 
}

