/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  LanguageListbox

*     File name:   langListbox.hpp

*     Contents:    Header file for dynamic listbox class

*     Description: This is the listbox that's used for language selection. It's
    kind of a special case, in that different fonts are needed per box.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef LANGUAGE_LISTBOX_HPP__
#define LANGUAGE_LISTBOX_HPP__

#include "dynamicListbox.hpp"

class LanguageListbox
   : public DynamicListbox
{
protected:

   virtual void updateListboxStrings ();

public:

   // CONSTRUCTOR
   LanguageListbox();

   // DESTRUCTOR
   virtual ~LanguageListbox();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

private:
   DynamicListboxVector _languageNameStrings;


};

// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of langListbox.hpp file

/* FORMAT HASH 0a5e301c4e817ffedf3ca48cf27386ff */
