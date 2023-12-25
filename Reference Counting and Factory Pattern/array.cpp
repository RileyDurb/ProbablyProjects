/******************************************************************************
filename    array.cpp
author      Riley Durbin
email       ridurbin0@gmail.edu
date        11/13/2022

Brief Description: Fuctions for allocating, deallocating, and using an array
	of abstract elements.

******************************************************************************/
#include "array.h"
// if Factory is used correctly, there will be no more includes

// Helper functions ///////////////////////////////////////////////////////////////////////////////
void DigiPen::Array::DeepCopy()
{
	AbstractElement ** shallowData = data; // Saves pointer to shallow data
	--(*ref_count); // Old data will no longer be referenced by this
	data = new AbstractElement*[size]; // Allocates it's own array
	ref_count = new int(1); // Allocates its own referece count

	for (unsigned int i = 0; i < size; ++i)
	{
		// Deep copies using overriden clone function
		data[i] = shallowData[i]->Clone();
	}
}
void DigiPen::Array::DeleteData()
{
	for (unsigned int i = 0; i < size; ++i)
	{
		delete data[i]; // Overloaded destructor will be called for each
	}

	// deletes remaining data
	delete[] data;
	delete ref_count;
}

// Construction/ destruction //////////////////////////////////////////////////////////////////////
DigiPen::Array::Array(int * array, 
		unsigned int _size, 
		const DigiPen::ElementFactory* _pElementFactory
		) 
: data(new AbstractElement*[_size]),
  size(_size),
	pElementFactory(_pElementFactory),
	ref_count(new int(1))
{
	for ( unsigned int i=0; i<size; ++i ) {
		//create Element with id = 1 (that is Element1 )
		data[i] = pElementFactory->MakeElement( 1, array[i] );
	}
}

// Shallow copies, and increments reference count
DigiPen::Array::Array(const Array& rhs)
	: data(rhs.data)
	, size(rhs.size)
	, pElementFactory(rhs.pElementFactory)
	, ref_count(rhs.ref_count)
{
	++(*ref_count);
}

DigiPen::Array::~Array()
{
	--(*ref_count); // Updates reference count

	if ((*ref_count) <= 0) // If no more references
	{
		DeleteData(); // Data is not needed, delete it
	}
}
// Getters and setters ////////////////////////////////////////////////////////////////////////////
int DigiPen::Array::Get(unsigned int pos) const
{
	return data[pos]->Get(); // Uses overidden get function
}

void DigiPen::Array::Set( int id, int pos, int value ) { 
	if (*ref_count == 1)
	{
		// Only one reference, just replace data
		delete data[pos];
		data[pos] = pElementFactory->MakeElement(id, value); // make sure is used 
	}
	else
	{
		DeepCopy(); // modifying data with multiple references - need our own copy
		delete data[pos];
		data[pos] = pElementFactory->MakeElement(id, value); // make sure is used 
	}
}
// Operators //////////////////////////////////////////////////////////////////////////////////////
DigiPen::Array& DigiPen::Array::operator=(const Array& rhs)
{
	if (this->data != rhs.data) // if data is not the same
	{
		--(*ref_count); // Update reference count of current data
		if (*ref_count == 0) // if no more references
		{
			DeleteData(); // Current data no longer needed, delete it
		}

		// Shallow copy new data
		size = rhs.size;
		pElementFactory = rhs.pElementFactory;
		ref_count = rhs.ref_count;
		data = rhs.data;

		++(*this->ref_count); // Update new reference count
	}

	return *this;
}
// Output /////////////////////////////////////////////////////////////////////////////////////////
void DigiPen::Array::Print() const {
	for (unsigned int i=0;i<size;++i) data[i]->Print(); 
	std::cout << std::endl;
}


