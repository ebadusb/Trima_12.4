/*****************************************************************************\
**
** SpoofAssignment.h
**
** Base class for spoof tasks.
**
\*****************************************************************************/

#ifndef SPOOFASSIGNMENT_H
#define SPOOFASSIGNMENT_H

/* Includes ******************************************************************/

#include "spooferapi.hpp"

namespace SPOOFNAME {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Classes *******************************************************************/

class SpoofAssignment {
public:

	 /**
	  * 
	  * SpoofAssignment
	  * 
	  * Constructor
	  * 
	  * @param api The Spoof API for executing the spoof.
	  * 
	  */
	 SpoofAssignment(SpooferAPI &api) : API(api) {}

	 /** 
	  * 
	  * ~SpoofAssignment
	  * 
	  * Destructor
	  * 
	  */
	 virtual ~SpoofAssignment(void) {}

	 /**
	  * 
	  * Initialize
	  * 
	  * Initializes the task. This function is called before executing the spoof.  
	  * 
	  */
	 virtual void Initialize(void) {}

	 /**
	  * 
	  * Execute
	  * 
	  * Execute the spoof.  
	  * 
	  */
	 virtual void Execute(void) {}

	 /**
	  * 
	  * Finalize
	  * 
	  * Finalizes the task. This function is called after executing the spoof.  
	  * 
	  */
	 virtual void Finalize(void) {}

protected:
	 SpooferAPI &API;
};

}; // namespace SPOOFNAME

#endif SPOOFASSIGNMENT_H
