/*****************************************************************************\
**
** CustomSpoofCollection.h
**
** Provides the implementaion for loading custom spoofs.
**
\*****************************************************************************/

#ifndef CUSTOMSPOOFS_H
#define CUSTOMSPOOFS_H

/* Includes ******************************************************************/

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Classes *******************************************************************/

class CustomSpoofCollection {
public:

	 typedef SpoofBase * (*CreateCustomSpoof)(SpooferAPI &api);
	 typedef std::list<CreateCustomSpoof> CustomSpoofCreateFunctions;
	 typedef CustomSpoofCreateFunctions::const_iterator Iterator;

	 /**
	  * 
	  * GetInstance
	  * 
	  * Gets the custom spoof collection instance.
	  * 
	  * @return The custom spoof collection.
	  * 
	  */
	 static const CustomSpoofCollection& GetInstance(void);

 	 /**
	  * 
	  * Begin
	  * 
	  * Gets an iterator to the beginning of the handler list.
	  * 
	  * @return An iterator to the beginning of the handler list.
	  * 
	  */
	 Iterator Begin(void) const;

 	 /**
	  * 
	  * End
	  * 
	  * Gets an iterator to the end of the handler list.
	  * 
	  * @return An iterator to the end of the handler list.
	  * 
	  */
	 Iterator End(void) const;

private:

	 /**
	  * 
	  * CustomSpoofCollection
	  * 
	  * Constructor. Loads all the custom spoofs.
	  * 
	  */
	 CustomSpoofCollection(void);

    /**
	  * 
	  * ~CustomSpoofCollection
	  * 
	  * Destructor.
	  * 
	  */
	 ~CustomSpoofCollection(void);

    /**
	  * 
	  * CustomSpoofCollection
	  * 
	  * Copy Constructor. Undefined
	  * 
	  */
	 CustomSpoofCollection(const CustomSpoofCollection &collection);

	 CustomSpoofCreateFunctions mCustomSpoofCreateFunctions;
};

#endif CUSTOMSPOOFS_H
