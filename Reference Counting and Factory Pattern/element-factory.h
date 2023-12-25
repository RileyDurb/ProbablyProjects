/******************************************************************************
filename    element-factory.h
author      Riley Durbin
email  	    ridurbin0@gmail.com
date        11/13/2022

Brief Description: Contatins a function for allocating an array element of 
	different types, based on given parameters

******************************************************************************/
#ifndef ELEMENT_FACTORY_H
#define ELEMENT_FACTORY_H
#include "abstract-element.h"

namespace DigiPen {
	class ElementFactory {
		public:
			// Creates an element of type matching the id, with given value
			AbstractElement* MakeElement(int id,int value) const;
	};
}
#endif
