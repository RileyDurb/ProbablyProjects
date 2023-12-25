/******************************************************************************
filename    element2.cpp
author      Riley Durbin
email       riley.durbin@digipen.edu
date        11/13/2022

Brief Description: Contatins a functions for creating and using an array element
                   that houses an int *

******************************************************************************/
#include "element2.h"


int DigiPen::Element2::total = 0;
int DigiPen::Element2::alive = 0;

DigiPen::Element2::Element2(int _val) 
	: AbstractElement()
	,	p_val(new int(_val)) 
{ 
	// Increment reference counts
	++total; 
	++alive; 
}

DigiPen::Element2::Element2(const Element2& rhs) 
	: AbstractElement(), p_val(new int(*rhs.p_val)) 
{ 
	// Increment reference counts
	++total;
	++alive;
}


DigiPen::Element2::~Element2() {
	delete p_val;
	--alive; // Update reference count
}

DigiPen::AbstractElement* DigiPen::Element2::Clone()
{
	AbstractElement* newElement = new Element2(*(this->p_val)); // Deep copy
	
	return newElement;
}

// Operators //////////////////////////////////////////////////////////////////////////////////////
DigiPen::Element2& DigiPen::Element2::operator=(const Element2& rhs) {
  if (this!=&rhs) {
		delete p_val;
		p_val = new int(*rhs.p_val);
	} 
  return *this;
}

// Getters and setters ////////////////////////////////////////////////////////////////////////////

int DigiPen::Element2::Get() const {
	return *p_val;
}

void DigiPen::Element2::Set(int new_val) { 
	*p_val = new_val; 
}

int DigiPen::Element2::GetTotal() { return total; }

int DigiPen::Element2::GetAlive() { return alive; }
// Output /////////////////////////////////////////////////////////////////////////////////////////

void DigiPen::Element2::Print() const { 
  std::cout << "(" << *p_val << ") "; 
}




