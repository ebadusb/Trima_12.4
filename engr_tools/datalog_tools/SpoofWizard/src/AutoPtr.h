/*****************************************************************************\
**
** AutoPtr.h
**
** Provides an auto pointer for managing native pointers.
**
\*****************************************************************************/

#pragma once

/* Class *********************************************************************/

namespace WIZARD {

template <typename _TYPE>
ref struct AutoPtr { 
public:
	typedef _TYPE Type;

	/**
	 *
	 * AutoPtr
	 *
	 * Constructor.
	 *
	 */
    AutoPtr(void) : mTypeP(NULL)
    {
    }

	/**
	 *
	 * AutoPtr
	 *
	 * Constructor. Takes ownership of given pointer.
	 *
	 */
	AutoPtr(Type* ptr) : mTypeP(ptr)
    {
    }

	/**
	 *
	 * AutoPtr
	 *
	 * Copy constructor. Takes ownership of given pointer.
	 *
	 */
    AutoPtr(AutoPtr<Type> %ptr) : mTypeP(ptr.Release())
    {
    }

	/**
	 *
	 * ~AutoPtr
	 *
	 * Destructor. Deletes the pointer.
	 *
	 */
    ~AutoPtr(void)
    {
        if (mTypeP != NULL) {
            delete mTypeP;
            mTypeP = NULL;
        }
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
		 return *.mTypeP; 
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
        if (mTypeP != NULL) {
            delete mTypeP;
        }
        mTypeP = ptr;
    }

private:
    Type* mTypeP;
};

} // namespace WIZARD