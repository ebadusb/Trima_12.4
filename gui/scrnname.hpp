/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_Pointer_Table

*     File name:    scrnname.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will convert a logical screen name into a pointer to the
         corresponding screen instantiation.  On construction, this class builds
         the table of "static" screen pointers that reference the screen instances
         declared herein.  Dynamic screens (child and stateless) are dynamically
         allocated when their pointer is requested and must be "deleted" by the
         recipient of the dynamic screen pointer.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/scrnname.hpp 1.36 2008/03/24 21:13:29Z dslausb Exp dslausb $
$Log: scrnname.hpp $
Revision 1.38  2009/04/17 16:19:34Z  dslausb
IT 9171 - Disconnect screen changes
Revision 1.37  2009/01/05 22:51:29Z  dslausb
IT 8990 - Move RBC initial split screen to after air evac, but before disposables test.
Revision 1.36  2008/03/24 21:13:29Z  dslausb
IT 8374 - Change the offline addition screen
Revision 1.34  2007/11/29 23:47:09Z  dslausb
IT 7372 - Moved up filter load screen to be directly after the syscassette screen.
Revision 1.33  2007/08/10 15:27:28Z  dslausb
Screen changed for DRBC Automated Split
Revision 1.32  2007/05/23 20:50:35Z  dslausb
IT 8015 - Fixed run summary screen allocation
Revision 1.31  2007/02/27 22:55:40Z  dslausb
IT 7878 - AAS Disconnect is now done in a single blaze of three-panel glory.
Revision 1.30  2006/12/07 20:56:13Z  dslausb
- separated "Clamp PAS line" stuff from sysclamp (it's now pasInitSplit.)
- added the clamp crossover screen.
- added functions to guiglobs to determine which of these should be shown.
Revision 1.29  2006/12/06 18:19:41Z  dslausb
Added buttons to Automated Processes config, and added ras connect screen.
Revision 1.28  2006/12/04 20:56:11Z  dslausb
Split the MSS Begin screens up into separate classes
Revision 1.27  2006/09/11 19:25:12Z  dslausb
Revision 1.26  2006/05/05 21:46:16Z  dslausb
IT 7212 - Separated up flag and labeling run summary screens into their respective products
Revision 1.25  2006/04/21 22:41:46Z  dslausb
Removed VerifyCassette screen
Revision 1.24  2006/04/10 23:07:18Z  dslausb
IT 7272 - Fixed many issues with set identification, added keypad set entry page before dispload
Revision 1.23  2006/03/03 20:59:17Z  dslausb
Revision 1.22  2006/03/03 17:30:10Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.21  2003/01/06 18:31:21Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.20  2002/08/15 22:17:21  sb07663
compiling version for Vx
Revision 1.19  2002/05/07 14:00:04  sb07663
First cut at VxWorks compatibility
Revision 1.18  2000/10/04 16:43:43  sb07663
Config.dat modifications for new volume removal limits
Revision 1.17  2000/02/28 22:20:44  SB07663
IT4612: new configuration guaranteed yield screen
Revision 1.16  1999/06/14 23:56:33  TW09453
Add the Product warning (verifprd) and Label product (labelprd)
screens to the screen collection class.
Revision 1.15  1999/05/28 22:21:38  TW09453
Add references to the set identification screen
Revision 1.14  1999/05/07 16:18:44  SB07663
Added new "replacement fluid" configuration screen
Revision 1.13  1999/05/05 22:12:35  TW09453
Add the reference to the Replace Fluid screen's hpp file
Revision 1.12  1998/05/20 21:54:22  MC03015
addition of donordl screen
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

#include "bwindow.hpp"


class Screen_Pointer_Table
{


private:


   // the structure definition of the screen pointer table, currently only one element
   // this table defines ONLY the STATICALLY C++ allocated screens, typically the
   // apheresis dialog screens. the child screens are defined below.
   struct SCREEN_PTR_TAB
   {
      Base_Window* aph_screen_ptr ;
   };


   // the declaration of the screen pointer table - direct initialization of the table would
   //  be nice... but in C++ not possibly... therefore count MAX_SCREENS carefully!

 #define MAX_SCREENS 55


   SCREEN_PTR_TAB screen_ptr_table[MAX_SCREENS];




protected:




public:

   // SCREEN_TABLE_ITER
   // Table iterator typedef -- public, so users can declare iterators
   typedef long SCREEN_TABLE_ITER ;


   // CONSTRUCTOR
   Screen_Pointer_Table();


   // DESTRUCTOR
   virtual ~Screen_Pointer_Table();


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


};    // END of Screen_Pointer_Table Class Declaration


// End of single include define guardian
  #endif

/* FORMAT HASH 3a320bc585e3f755619c3b52e712d18b */
