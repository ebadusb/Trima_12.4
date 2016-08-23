/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   State_Collection

*     File name:    statecol.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This is derived off of the table collection template and contains
         multiple instances of a "state" record type. It is used as a repository
         of these states for lookup and will return the "action ID" associated
         with this state.
*************************< TRIMA GUI DOMAIN >****************************

$Header$
$Log$

*/



// Single include define guardian
#ifndef STATECOL_HPP
#define STATECOL_HPP


#include "tablecol.hpp" // include the base class declaration
#include "guistate.hpp" // include the state class


class State_Collection
   : public Table_Collection<GUI_State>
{
private:



protected:



public:


   // CONSTRUCTOR
   State_Collection();



   // DESTRUCTOR
   virtual ~State_Collection();



   // SEARCH_FOR_STATE_MATCH
   // This method takes a reference to a gui_State type object and attempts to
   //  find a "state" in the collection that matches all of its state
   //  attributes. If it finds such a state, it will return its "action ID" which
   //  will be returned as an INT. If it doesn't find a matching state, this method
   //  will return a -1.
   int search_for_state_match (void* current_state) ;

};  // END of State_Collection Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 9fc68672d29138014ad0eb8a0cc5afdd */
