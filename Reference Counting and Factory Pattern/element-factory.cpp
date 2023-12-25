/******************************************************************************
filename    element-factory.cpp
author      Riley Durbin
email  	    ridurbin0@gmail.com
date        11/13/2022

Brief Description: Contatins a function for allocating an array element based
									 on given parameters

******************************************************************************/
#include "element-factory.h"

//this is a concrete factory, so it has to know about all element classes
#include "element1.h"
#include "element2.h"


DigiPen::AbstractElement* DigiPen::ElementFactory::MakeElement(int id,int value) const {
	AbstractElement* p_element = NULL;
	switch (id) {
		case 1: p_element = new Element1(value); break;
		case 2: p_element = new Element2(value); break;
		//case ???: p_element = new Element????(value); break;
	}
	return p_element;
}
