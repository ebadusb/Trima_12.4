/*****************************************************************************\
**
** SharedPtr.h
**
** Provides a shared pointer for managing native pointers.
**
\*****************************************************************************/

#pragma once

/* Class *********************************************************************/

namespace WIZARD {

template <typename _TYPE>
ref struct SharedPtr { 
public:
	typedef _TYPE Type;

	/**
	 *
	 * SharedPtr
	 *
	 * Constructor.
	 *
	 */
    SharedPtr(void) : mTypeP(NULL)
    {
    }

	/**
	 *
	 * SharedPtr
	 *
	 * Constructor. 
	 *
	 */
	SharedPtr(Type* ptr) : mTypeP(ptr)
    {
    }

	/**
	 *
	 * SharedPtr
	 *
	 * Copy constructor. Takes ownership of given pointer.
	 *
	 */
	SharedPtr(SharedPtr<Type> %ptr) : mTypeP(ptr.mTypeP)
    {
    }

	/**
	 *
	 * ~SharedPtr
	 *
	 * Destructor.
	 *
	 */
    ~SharedPtr(void)
    {
		mTypeP = NULL;
    }

	/**
	 *
	 * operator*
	 *
	 * Gets the reference.
	 *
	 */
 	 Type& operator*()
	 { 
		 return *mTypeP; 
	 }

	/**
	 *
	 * operator->
	 *
	 * Gets the pointer.
	 *
	 */
    Type* operator->() {
        return mTypeP;
    }

	/**
	 *
	 * Get
	 *
	 * Gets the pointer.
	 *
	 */
    Type* Get(void) {
        return mTypeP;
    }

	/**
	 *
	 * Release
	 *
	 * Releases the pointer.
	 *
	 */
    Type* Release() {
        Type* released = mTypeP;
        mTypeP = NULL;
        return released;
    }

	/**
	 *
	 * Reset
	 *
	 * Resets the pointer.
	 *
	 */
    void Reset(void) {
		this->Reset(NULL);
    }

	/**
	 *
	 * Reset
	 *
	 * Resets the pointer.
	 *
	 */
    void Reset(Type* ptr) {
        mTypeP = ptr;
    }

private:
    Type* mTypeP;
};

} // namespace WIZARD