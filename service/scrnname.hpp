/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_Pointer_Table

*     File name:    scrnname.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will convert a logical screen name into a pointer to the
         correspoinding screen instantiation.  On construction, this class builds
         the table of "static" screen pointers that reference the screen instances
         declared herein.  Dynamic screens (child and stateless) are dynamically
         allocated when their pointer is requested and must be "deleted" by the
         recipient of the dynamic screen pointer.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/service/rcs/scrnname.hpp 1.3 2003/06/10 21:15:44Z rm70006 Exp dslausb $
$Log: scrnname.hpp $
Revision 1.3  2003/06/10 21:15:44Z  rm70006
First cut at porting service mode.
Revision 1.2  2002/06/05 15:12:36Z  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.1  2000/03/15 17:57:35  SB07663
Initial revision
Revision 1.1  2000/03/15 17:43:14  SB07663
Initial revision
Revision 1.1  1997/05/14 21:39:16  Barry_Fulkerson
Initial revision
Revision 1.1  1997/05/14 21:36:47  Barry_Fulkerson
Initial revision
Revision 1.2  1997/05/13 21:43:46  Barry_Fulkerson
- changed file name
Revision 1.1  1997/05/13 18:59:38  Barry_Fulkerson
Initial revision
Revision 1.11  1997/03/11 16:13:29  Barry_Fulkerson
- removed all references to the old cfg_repo & cfg_serv screens. They are
  do not exist.
Revision 1.10  1997/03/11 04:38:40  Barry_Fulkerson
- removed references to cfg_serv and cfg_repo.  Files no long exist.
Revision 1.9  1997/03/06 22:10:58  tw09453
Allocate all the static screens in the heap instead of on the stack --
for debugging purposes and to unburden the stack space.
Revision 1.8  1997/02/26 02:33:18  Barry_Fulkerson
- changed reference from the cfg_adju file to the cfg_priority file.  Renamed
 cfg_adju.cpp/hpp to cfg_priority to better represent what it is actually doing
 also in preperation for changing the PROCEDURE PRIORITY screen to include
 more than 5 procedures.
Revision 1.7  1997/01/10 22:56:09  tw09453
remove the bloodsmp screen from the world
Revision 1.6  1996/10/07 11:24:03  MC03015
Addition of PREDICT Screen
Revision 1.5  1996/09/28 22:27:57  Barry_Fulkerson
GUI Configuration additions for 2.3
Revision 1.4  1996/07/08 16:44:14  tw09453
Update file header


*/


// Single include define guardian
#ifndef SCRNNAME_HPP
#define SCRNNAME_HPP



// System #INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// CLASS INCLUDES
#include "gui.h"
#include "bwindow.hpp"

// Configuration Includes


class Screen_Pointer_Table
{
private:
   ///////////////////////////////////// Screen Instantiations //////////////////////////

   // the structure definition of the screen pointer table, currently only one element
   // this table defines ONLY the STATICALLY C++ allocated screens, typically the
   // apheresis dialog screens. the child screens are defined below.
   struct SCREEN_PTR_TAB
   {
      Base_Window* aph_screen_ptr;
   };


   // the declaration of the screen pointer table - direct initialization of the table would
   //  be nice... but in C++ not possibly... therefore count MAX_SCREENS carefully!
   #define MAX_SCREENS 2


   SCREEN_PTR_TAB screen_ptr_table[MAX_SCREENS];

public:

   // SCREEN_TABLE_ITER
   // Table iterator typedef -- public, so users can declare iterators
   typedef long SCREEN_TABLE_ITER;


   // CONSTRUCTOR
   Screen_Pointer_Table();


   // DESTRUCTOR
   ~Screen_Pointer_Table();


   // GET_STATIC_SCREEN_POINTER
   // This method returns a pointer to the screen instance corresponding to
   // the GUI_SCREEN_ID passed as a parameter.
   Base_Window* get_static_screen_ptr (GUI_SCREEN_ID screen_id);


   // GET_FIRST_STATIC_SCREEN_TABLE_POINTER
   // This method returns a pointer to the first screen instance in the
   // screen table. It then primes the interator parameter for subsequent
   // get_next_screen_table_pointer calls.
   Base_Window* get_first_static_screen_table_pointer (SCREEN_TABLE_ITER& screen_iter);


   // GET_NEXT_STATIC_SCREEN_TABLE_POINTER
   // This method returns a pointer to the next screen instance in the table
   // as referenced by the iterator. If there are no more entries in the table, this
   // method will return a NULL pointer.
   Base_Window* get_next_static_screen_table_pointer (SCREEN_TABLE_ITER& screen_iter);


   // GET_DYNAMIC_SCREEN_POINTER
   // This method allocates a child screen and returns the pointer. Only valid
   // child screens may be allocated in this manner. It is the responsibility of the
   // caller to C++ "delete" the screen object returned.
   Base_Window* get_dynamic_screen_pointer (GUI_SCREEN_ID screen_id);


}; // END of Screen_Pointer_Table Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 8b2aeb11d21d7925051ed41e29749723 */
