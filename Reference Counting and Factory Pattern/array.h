/******************************************************************************
filename    array.h
author      Riley Durbin
email       ridurbin0@gmail.edu
date        11/13/2022

Brief Description: Fuctions for allocating, deallocating, and using an array
	of abstract elements.

******************************************************************************/
#ifndef ARRAY_H
#define ARRAY_H
#include "abstract-element.h"
#include "element-factory.h"
#include <iostream>

namespace DigiPen {

/*
 * Class Array implements a simple array-like data structure.
 * Objects inside the Array have to be derived from AbstractElement.
 */
	class Array {
		public:
			Array(int * array, unsigned int _size, 
					const ElementFactory* _pElementFactory
					);
			Array(const Array& rhs);
			~Array();

			// Getters and setters
			int Get(unsigned int pos) const;
			void Set(int id, int pos, int value);

			// Operators
			Array& operator=(const Array& rhs);

			// Output
			void Print() const;
		private:
			AbstractElement** data;
			unsigned int size;
			ElementFactory const* pElementFactory;
			int * ref_count;

			// helper functions
			void DeleteData();
			void DeepCopy();
	};
}
#endif
