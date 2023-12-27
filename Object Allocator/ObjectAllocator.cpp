/******************************************************************************
filename    ObjectAllocator.cpp
author      Riley Durbin
email    	ridurbin0@gmail.com
date    	1/17/2022

Brief Description: Contains structure for managing memory, and the functions
				   for the public interface that uses it.

******************************************************************************/


#include "ObjectAllocator.h"
#include "string.h" // memset

// P
/* What am I doing ?
Creating an object allocator that knows
- How many pages it can allocate
- How big it's objects are

So that it can then be used to allocate objects of that size, and free them, in it's pages
Done in a linked list, with
- A list of all pages
-  A list of free objects
- Later, completely free pages will be freed and taken off the list

it will also add padding to these objects, of a certain size, so that it can validate if memory has been overwritten

And, maybe something about the page boundaries it gets?


Questions: how to cast objects from a bage
What's the deal with boundaries?
*/


// Creates the ObjectManager per the specified values
// Throws an exception if the construction fails. (Memory allocation problem)
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
{
	mConfig = config;
	mStats.ObjectSize_ = ObjectSize;
	numAllocations = 0; // Initializes to no allocations
	PageList_ = NULL;
	FreeList_ = NULL;

	// Calculates page size
	unsigned int paddingPerObject = mConfig.PadBytes_ * 2;

	// Calclates page size from object and header size info
	mStats.PageSize_ = ((config.ObjectsPerPage_ *
		(mStats.ObjectSize_ + paddingPerObject + mConfig.HBlockInfo_.size_)) + sizeof(void *));
	
	if (mStats.PagesInUse_ < mConfig.MaxPages_ || mConfig.MaxPages_ == 0) // If memory left, or memory unlimited
	{
		allocate_new_page(); // Allocates the first page
	}
	else
	{
		throw OAException(OAException::E_NO_PAGES, "ObjectAllocator: No logical memory available");
	}

}

// Destroys the ObjectManager (never throws, unlike me)
ObjectAllocator::~ObjectAllocator()
{
	while (PageList_)
	{
		if (mConfig.HBlockInfo_.type_ == OAConfig::hbExternal) // If uising external header
		{
			// Handles external header data

			unsigned char* pageSpot = reinterpret_cast<unsigned char*>(PageList_); // Brings to the start of the page memory
			pageSpot += sizeof(GenericObject*); // Moves past page list ptr

			for (unsigned int i = 0; i < mConfig.ObjectsPerPage_; i++) // For each object on the page
			{
				pageSpot += mConfig.HBlockInfo_.size_; // Moves past header

				pageSpot += mConfig.PadBytes_; // Moves past left pad bytes

				if (!is_on_freelist(pageSpot)) // If not freed already
				{
					deallocate_external_header(pageSpot);
				}

				pageSpot += mStats.ObjectSize_; // Moves to end of object

				pageSpot += mConfig.PadBytes_; // Moves past right pad bytes
			}
		}

		GenericObject* temp = PageList_->Next;
		delete [] PageList_; // Frees the page
		PageList_ = temp;
	}
}


// Take an object from the free list and give it to the client (simulates new)
// Throws an exception if the object can't be allocated. (Memory allocation problem)
void* ObjectAllocator::Allocate(const char* label)
{
	if (mConfig.UseCPPMemManager_) // If bypassing the memory manager
	{
		void * newObj = NULL;

		try // Tries allocating new object
		{
			newObj = new char[mStats.ObjectSize_];
		}
		catch (std::bad_alloc&) // Error message thrown on allocation fail
		{
			throw OAException(OAException::E_NO_MEMORY, "allocate_new_page: No system memory available");
		}

		// Updates allocation stats
		mStats.Allocations_++;
		mStats.ObjectsInUse_++;
		if (mStats.MostObjects_ < mStats.ObjectsInUse_)
		{
			mStats.MostObjects_ = mStats.ObjectsInUse_;
		}


		return newObj;
	}
	else // If using the memory manager
	{
		GenericObject* returnObj = grab_free_object();
		if (returnObj) //  There was a free object
		{
			if (mConfig.DebugOn_)
			{
				// Sets memory to allocated debug values
				memset(returnObj, ALLOCATED_PATTERN, mStats.ObjectSize_);

			}

			// Updates allocation stats
			mStats.Allocations_++;
			mStats.ObjectsInUse_++;
			if (mStats.MostObjects_ < mStats.ObjectsInUse_)
			{
				mStats.MostObjects_ = mStats.ObjectsInUse_;
			}

			// Header calculations
			if (mConfig.HBlockInfo_.type_ == OAConfig::hbExternal)
			{
				allocate_and_set_external_header(returnObj, label);
			}
			if (mConfig.HBlockInfo_.type_ == OAConfig::hbBasic || mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
			{
				// Sets the allocation number in the header
				set_allocation_num(returnObj, true);

				set_allocated_flag(returnObj, true);
			}
			if (mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
			{
				// Increases specific object count in the extended header
				increase_use_count(returnObj);
			}

			return returnObj; // Returns the found free object
		}
		else // No free objects
		{
			if (mStats.PagesInUse_ < mConfig.MaxPages_ || mConfig.MaxPages_ == 0) // If memory left, or memory unlimited
			{
				allocate_new_page();

				returnObj = grab_free_object();

				if (mConfig.DebugOn_)
				{
					// Sets memory to allocated debug values
					memset(returnObj, ALLOCATED_PATTERN, mStats.ObjectSize_);
				}


				// Updates allocation stats
				mStats.Allocations_++; // Allocation count
				mStats.ObjectsInUse_++; // Used object count
				if (mStats.MostObjects_ < mStats.ObjectsInUse_) // Highest amount of objects used at once count
				{
					mStats.MostObjects_ = mStats.ObjectsInUse_;
				}

				// Header calculations
				if (mConfig.HBlockInfo_.type_ == OAConfig::hbExternal)
				{
					allocate_and_set_external_header(returnObj, label);
				}
				if (mConfig.HBlockInfo_.type_ == OAConfig::hbBasic || mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
				{
					// Sets the allocation number in the header
					set_allocation_num(returnObj, true);

					set_allocated_flag(returnObj, true);
				}
				if (mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
				{
					// Increases specific object count in the extended header
					increase_use_count(returnObj);
				}

				return returnObj;
			}
			else
			{
				throw OAException(OAException::E_NO_PAGES, "Allocate: No logical memory available");
			}
		}
	}
	
}

// Returns an object to the free list for the client (simulates delete)
// Throws an exception if the the object can't be freed. (Invalid object)
void ObjectAllocator::Free(void* Object)
{
	if (mConfig.UseCPPMemManager_) // If bypassing the memory manager
	{
		GenericObject* obj = reinterpret_cast<GenericObject*>(Object);

		delete [] obj;

		// Updates deallocation related stats
		mStats.Deallocations_++;
		mStats.ObjectsInUse_--;
	}
	else // If using the memory manager
	{
		if (mConfig.DebugOn_)
		{
			// Checks for bad boundary
			if (validate_object_boundary(Object) == false)
			{
				throw OAException(OAException::E_BAD_BOUNDARY, "Free: Object to be freed is on a bad boundary");
			}

			// Checks padding
			if (validate_object_padding(Object) == false)
			{
				throw OAException(OAException::E_CORRUPTED_BLOCK, "Free: Object to be freed has corrupted padding");
			}

			// Checks for double free
			if (is_on_freelist(Object) == true)
			{
				throw OAException(OAException::E_MULTIPLE_FREE, "Free: Multiple free attempted");
			}

			// Sets object to free debug pattern
			memset(Object, FREED_PATTERN, mStats.ObjectSize_);
		}

		if (mConfig.HBlockInfo_.type_ == OAConfig::hbExternal)
		{
			deallocate_external_header(Object);
		}
		else if (mConfig.HBlockInfo_.type_ == OAConfig::hbBasic || mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
		{
			// Sets the allocation number in the header
			set_allocation_num(Object, false);

			set_allocated_flag(Object, false);
		}

		// Frees by putting on free list
		put_on_freelist(Object);

		// Updates stats
		mStats.Deallocations_++;
		mStats.ObjectsInUse_--;
	}
}

// Calls the callback fn for each block still in use
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
	unsigned inUseCount = 0; // number of blocks in use found

	GenericObject* ProxyPageList = PageList_; // Proxy pointer for walking the list

	while (ProxyPageList) // Walks through the page list
	{
		unsigned char* pageSpot = reinterpret_cast<unsigned char*>(ProxyPageList); // Brings to the start of the page memory
		pageSpot += sizeof(GenericObject*); // Moves past page list ptr

		for (unsigned int i = 0; i < mConfig.ObjectsPerPage_; i++) // For each object on the page
		{
			pageSpot += mConfig.HBlockInfo_.size_; // Moves past header

			pageSpot += mConfig.PadBytes_; // Moves past left pad bytes

			if (!is_on_freelist(pageSpot))
			{
				// Calls validate callback
				fn(pageSpot, mStats.ObjectSize_);

				inUseCount++;
			}

			pageSpot += mStats.ObjectSize_; // Moves to end of object

			pageSpot += mConfig.PadBytes_; // Moves past right pad bytes
		}

		ProxyPageList = ProxyPageList->Next;
	}

	return inUseCount; // Returns number of invalid objects found
}

// Calls the callback fn for each block that is potentially corrupted
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
	if (mConfig.DebugOn_ == false || mConfig.PadBytes_ == 0) // If not debugging, or nothing to validate
	{
		return 0;
	}
	else
	{
		unsigned invalidCount = 0; // number of invalid objects found

		GenericObject* ProxyPageList = PageList_; // Proxy pointer for walking the list

		while (ProxyPageList) // Walks through the page list
		{
			unsigned char* pageSpot = reinterpret_cast<unsigned char*>(ProxyPageList); // Brings to the start of the page memory
			pageSpot += sizeof(GenericObject*); // Moves past page list ptr

			for (unsigned int i = 0; i < mConfig.ObjectsPerPage_; i++) // For each object on the page
			{
				bool valid = true;

				pageSpot += mConfig.HBlockInfo_.size_; // Moves past header

				// Checks left pad bytes
				for (unsigned int i = 0; i < mConfig.PadBytes_; i++)
				{
					if (*pageSpot != PAD_PATTERN) // If not expected pattern
					{
						valid = false; // Padding was overwritten, object is invalid
					}

					pageSpot++;
				}				

				pageSpot += mStats.ObjectSize_; // Moves to end of object

				// Checks right pad bytes
				for (unsigned int i = 0; i < mConfig.PadBytes_; i++)
				{
					if (*pageSpot != PAD_PATTERN) // If not expected pattern
					{
						valid = false; // Padding was overwritten, object is invalid
					}

					pageSpot++;
				}

				if (valid == false)
				{
					// Calls validate callback
					fn(pageSpot, mStats.ObjectSize_);

					invalidCount++;
				}
			}

			ProxyPageList = ProxyPageList->Next;
		}

		return invalidCount; // Returns number of invalid objects found
		
	}
	
}

// Testing/Debugging/Statistic methods //////////////////////////////////////////////////////////////////////////////

void ObjectAllocator::SetDebugState(bool State) // true=enable, false=disable
{
	mConfig.DebugOn_ = State;
}

const void* ObjectAllocator::GetFreeList(void) const // returns a pointer to the internal free list
{
	return FreeList_;
}

// returns a pointer to the internal page list
const void* ObjectAllocator::GetPageList(void) const
{
	return PageList_;
}

// returns the configuration parameters
OAConfig ObjectAllocator::GetConfig(void) const
{
	return mConfig;
}

// returns the statistics for the allocator
OAStats ObjectAllocator::GetStats(void) const
{
	return mStats;
}

// allocates another page of objects
void ObjectAllocator::allocate_new_page(void)
{
	char* newPage = nullptr; // For allocating the new page into

	try // Tries allocating new page
	{
		newPage = new char[mStats.PageSize_];
	}
	catch (std::bad_alloc&) // Error message thrown on allocation fail
	{
		throw OAException(OAException::E_NO_MEMORY, "allocate_new_page: No system memory available");
	}

	if (mConfig.DebugOn_)
	{
		// Sets memory to proper debug value
		memset(newPage, UNALLOCATED_PATTERN, mStats.PageSize_);
	}


	// Updates page stats
	mStats.PagesInUse_++;

	// Puts page on page list
	GenericObject* page = PageList_; // Initializes to list head
	if (PageList_) // If a page exists
	{
		GenericObject * newPageObj = reinterpret_cast<GenericObject*>(newPage); // Casts to generic object
		
		// Pushes front
		newPageObj->Next = PageList_;
		PageList_ = newPageObj;

		page = PageList_; // Sets page pointer for splitting into objects
	}
	else
	{
		PageList_ = reinterpret_cast<GenericObject*>(newPage); // Places page as only page in list
		PageList_->Next = NULL; // Inits new page's next pointer to NULL
		page = PageList_; // Sets page pointer for splitting into objects
	}

	// Splits page into free objects, and adds them to the free list
	char * pageSpot = reinterpret_cast<char *>(page); // Brings to the start of the page memory
	pageSpot += sizeof(GenericObject*); // Moves past page list ptr

	// Sets up each object (header, padding, etc.)
	for (unsigned int i = 0; i < mConfig.ObjectsPerPage_; i++)
	{
		// Header
		init_object_header(pageSpot);

		pageSpot += mConfig.HBlockInfo_.size_; // Moves past header

		// Left pad bytes
		if (mConfig.DebugOn_)
		{
			// Sets left pad bytes to debug value
			memset(pageSpot, PAD_PATTERN, mConfig.PadBytes_);
		}

		pageSpot += mConfig.PadBytes_; // Moves to beginning of object

		// Data portion
		GenericObject* obj = reinterpret_cast<GenericObject*>(pageSpot); // Casts section into an object

		put_on_freelist(obj);

		pageSpot += mStats.ObjectSize_; // Moves to end of object
		
		// Right pad bytes
		if (mConfig.DebugOn_)
		{
			// Sets right pad bytes to debug value
			memset(pageSpot, PAD_PATTERN, mConfig.PadBytes_);
		}

		pageSpot += mConfig.PadBytes_; // Moves to end of object
	}
}

// puts Object onto the free list
void ObjectAllocator::put_on_freelist(void* object)
{
	if (mConfig.DebugOn_)
	{
		if (validate_object_boundary(object) == false)
		{
			throw OAException(OAException::E_BAD_BOUNDARY, "validate_object: Object not on a boundary");
		}
	}

	GenericObject* obj = reinterpret_cast<GenericObject*>(object);

	if (FreeList_) // Pushes front if existing obj
	{
		obj->Next = FreeList_;
		FreeList_ = obj;
	}
	else // Becomes first object in list, if no others
	{
		obj->Next = NULL;
		FreeList_ = obj;
	}

	mStats.FreeObjects_++;
}

GenericObject* ObjectAllocator::grab_free_object()
{
	GenericObject* returnObj = FreeList_;
	if (!FreeList_) // If no free objects
	{
		return NULL;
	}
	else
	{
		if (FreeList_->Next) // If more free objects after this
		{
			FreeList_ = FreeList_->Next; // Advances list

			mStats.FreeObjects_--;

			return returnObj;
		}
		else  // No more free objects after this
		{
			FreeList_ = NULL;

			mStats.FreeObjects_--; // TODO: Also take off free list

			return returnObj;
		}
	}

}

// Returns if the given object is on the free list
bool ObjectAllocator::is_on_freelist(void * object) const
{
	if (mConfig.HBlockInfo_.type_)
	{
		switch (mConfig.HBlockInfo_.type_)
		{
		case OAConfig::hbBasic:
		{
			// TODO:: movge rest to unsigned if needed
			// Moves to flags byte
			unsigned char* objSpot = reinterpret_cast<unsigned char*>(object);
			objSpot -= (mConfig.PadBytes_ + 1);
			
			return !(*objSpot & 1); // On freelist if allocated byte is false
		}
		break;
		case OAConfig::hbExtended:
		{
			// Moves to flags byte
			char* objSpot = reinterpret_cast<char*>(object);
			objSpot -= mConfig.PadBytes_ + 1;

			return !(*objSpot & 1); // On freelist if allocated byte is false
		}
		break;
		case OAConfig::hbExternal:
		{
			// Gets header ptr
			char* objSpot = reinterpret_cast<char*>(object);
			objSpot -= sizeof(void*) + mConfig.PadBytes_; // Moves to beginning of ptr in header
			MemBlockInfo** objHeaderPtr = reinterpret_cast<MemBlockInfo**>(objSpot);

			if ((*objHeaderPtr) == NULL) // if ptr has been reset
			{
				return true; // Header was freed, so on freelist;
			}

			return !(*objHeaderPtr)->in_use; // Is on freelist if not in use
		}
			break;
		case OAConfig::hbNone:
			break;
		}

		return false;
	}
	else // No header, search freelist directly
	{
		GenericObject* obj = reinterpret_cast<GenericObject*>(object); // Object to 

		GenericObject* ProxyFreeList = FreeList_; // Substitute pointer for walking free list

		while (ProxyFreeList)
		{
			if (ProxyFreeList == obj)
			{
				return true;
			}

			ProxyFreeList = ProxyFreeList->Next;
		}

		return false;
	}
}

void ObjectAllocator::init_object_header(void* object)
{
	switch (mConfig.HBlockInfo_.type_)
	{
	case OAConfig::hbBasic:
	{
		// Inits allocation counter to 0
		char * objSpot= reinterpret_cast<char *>(object);
		memset(objSpot, 0, mConfig.HBlockInfo_.size_);
	}
		break;
	case OAConfig::hbExtended:
	{
		// Inits entire header to 0
		char* objSpot = reinterpret_cast<char*>(object);
		memset(objSpot, 0, mConfig.HBlockInfo_.size_);
	}
		break;
	case OAConfig::hbExternal:
	{
		// Inits entire header to 0
		char* objSpot = reinterpret_cast<char*>(object);
		memset(objSpot, 0, mConfig.HBlockInfo_.size_);
	}
		break;
	case OAConfig::hbNone:
		break;
	}
}

void ObjectAllocator::set_allocation_num(void* object, bool isAllocated)
{
	switch (mConfig.HBlockInfo_.type_)
	{
	case OAConfig::hbBasic:
	{
		// Moves to beginning of header
		char * objSpot = reinterpret_cast<char *>(object);
		objSpot -= (mConfig.HBlockInfo_.size_ + mConfig.PadBytes_);

		// Sets allocation number
		unsigned * objCounter = reinterpret_cast<unsigned*>(objSpot); // Casts to counter type
		if (isAllocated) // If being assigned an allocation number
		{
			*objCounter = mStats.Allocations_; // Assigns allocation number
		}
		else
		{
			*objCounter = 0; // Resets alloc number to 0
		}
	}
	break;
	case OAConfig::hbExtended:
	{
		// Moves allocation number
		char* objSpot = reinterpret_cast<char*>(object);
		objSpot -= (OAConfig::BASIC_HEADER_SIZE + mConfig.PadBytes_); 

		// Sets allocation number
		unsigned* objCounter = reinterpret_cast<unsigned*>(objSpot); // Casts to counter type
		if (isAllocated) // If being assigned an allocation number
		{
			*objCounter = mStats.Allocations_; // Assigns allocation number
		}
		else
		{
			*objCounter = 0; // Resets alloc number to 0
		}
	}
	break;
	case OAConfig::hbExternal:
	{
		// Gets header ptr
		char* objSpot = reinterpret_cast<char*>(object);
		objSpot -= (sizeof(void*) + mConfig.PadBytes_); // Moves to beginning of ptr in header
		MemBlockInfo** objHeaderPtr = reinterpret_cast<MemBlockInfo**>(objSpot);

		// Sets allocation num
		(*objHeaderPtr)->alloc_num = mStats.Allocations_;
	}
		break;
	case OAConfig::hbNone:
		break;
	}
}

void ObjectAllocator::set_allocated_flag(void* object, bool isAllocated)
{
	switch (mConfig.HBlockInfo_.type_)
	{
	case OAConfig::hbBasic:
	{
		// Moves to flag byte
		char* objSpot = reinterpret_cast<char*>(object);
		objSpot -= (1 + mConfig.PadBytes_);


		// Sets alloceted flag
		if (isAllocated)
		{
			*objSpot |= 1;
		}
		else
		{
			*objSpot &= 0;
		}
	}
	break;
	case OAConfig::hbExtended:
	{
		// Moves to flag byte
		char* objSpot = reinterpret_cast<char*>(object);
		objSpot -= (1 + mConfig.PadBytes_);


		// Sets alloceted flag
		if (isAllocated)
		{
			*objSpot |= 1;
		}
		else
		{
			*objSpot &= 0;
		}
	}
	break;
	case OAConfig::hbExternal:
	{
		// Gets header ptr
		char* objSpot = reinterpret_cast<char*>(object);
		objSpot -= (sizeof(void*) + mConfig.PadBytes_); // Moves to beginning of ptr in header
		MemBlockInfo** objHeaderPtr = reinterpret_cast<MemBlockInfo**>(objSpot);

		// Sets allocated flag
		(*objHeaderPtr)->in_use = isAllocated;
	}
		break;
	case OAConfig::hbNone:
		break;
	}
}

// Increases the object allocation count for the given object's header
// (Expects object ptr to be at the beginning of the data portion)
void ObjectAllocator::increase_use_count(void* object)
{
	// Moves to beginning of header
	char* objSpot = reinterpret_cast<char*>(object);
	objSpot -= (mConfig.HBlockInfo_.size_ - mConfig.HBlockInfo_.additional_ + mConfig.PadBytes_);
 
	// Increments
	unsigned int objCount = *objSpot;

	objCount++;
	memset(objSpot, objCount, 1); // TODO: figure out how to add to 2 bits, probably with xor
}

// Allocates external header, sets label, and gives to object
// (Expects object ptr to be at the beginning of the data portion)
void ObjectAllocator::allocate_and_set_external_header(void* object, const char* label)
{
	// Moves to header
	char* objSpot = reinterpret_cast<char*>(object);
	objSpot -= (OAConfig::EXTERNAL_HEADER_SIZE + mConfig.PadBytes_); // Moves to beginning of ptr in header
	MemBlockInfo** objHeaderPtr = reinterpret_cast<MemBlockInfo**>(objSpot); // Casts to recieve ptr
	*objHeaderPtr = new MemBlockInfo;

	// Inits header variables
	(*objHeaderPtr)->in_use = true;
	if (label)
	{
		(*objHeaderPtr)->label = new char[strlen(label) + 1];
		strcpy((*objHeaderPtr)->label, label);
	}
	else
	{
		(*objHeaderPtr)->label = NULL;
	}
	(*objHeaderPtr)->alloc_num = mStats.Allocations_;
}


//  Deallocates entire external header of object
// (Expects object ptr to be at the beginning of the data portion)
void ObjectAllocator::deallocate_external_header(void* object)
{
	// Gets header ptr
	char* objSpot = reinterpret_cast<char*>(object);
	objSpot -= (OAConfig::EXTERNAL_HEADER_SIZE + mConfig.PadBytes_); // Moves to beginning of ptr in header
	MemBlockInfo** objHeaderPtr = reinterpret_cast<MemBlockInfo**>(objSpot); // Casts to access header

	if (*objHeaderPtr)
	{
		// Frees header
		if ((*objHeaderPtr)->label)
		{
			delete [] (*objHeaderPtr)->label; // First frees dynamically allocated label
		}
		delete (*objHeaderPtr);
	}


	memset(objHeaderPtr, 0, OAConfig::EXTERNAL_HEADER_SIZE); // Resets to null
}

bool ObjectAllocator::validate_object_padding(void* object)
{
	unsigned char* objSpot = reinterpret_cast<unsigned char*>(object);
	objSpot -= mConfig.PadBytes_;

	// Left pad bytes
	for (unsigned int i = 0; i < mConfig.PadBytes_; i++)
	{
		if (*objSpot != PAD_PATTERN) // If not expected pattern
		{
			return false; // Padding was overwritten, object is invalid
		}

		objSpot++;
	}

	objSpot += mStats.ObjectSize_; // Moves to end of object

	// Right pad bytes
	for (unsigned int i = 0; i < mConfig.PadBytes_; i++)
	{
		if (*objSpot != PAD_PATTERN) // If not expected pattern
		{
			return false; // Padding was overwritten, object is invalid
		}

		objSpot++;
	}

	return true; // No corruption found
}

// Checks to see if the object is on a valid boundary, returns whether it is or not
bool ObjectAllocator::validate_object_boundary(void * object)
{
	// Finds which page the object is in, and perform check
	GenericObject* ProxyPageList = PageList_; // Proxy ptr for walking through page list
	while (ProxyPageList)
	{
		unsigned char * pageBegin = reinterpret_cast<unsigned char *>(ProxyPageList); // Beginning of page in memory
		unsigned char * pageEnd = pageBegin + mStats.PageSize_;						  // End of page in memory
		unsigned char * objBegin = reinterpret_cast<unsigned char *>(object);		  // Beginning of object in memory
		if (objBegin < pageEnd && objBegin > pageBegin) // If within the page
		{
			int innersize = static_cast<int>(mStats.ObjectSize_) + (mConfig.PadBytes_ * 2) + static_cast<int>(mConfig.HBlockInfo_.size_) + mConfig.LeftAlignSize_;

			if ((objBegin - pageBegin - sizeof(void*) - mConfig.PadBytes_ - static_cast<int>(mConfig.HBlockInfo_.size_) - mConfig.LeftAlignSize_) % innersize == 0) // If in the proper page boundary
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		ProxyPageList = ProxyPageList->Next;
	}

	return false; // Object not inside any page. Where it come from? Returns false
}
