/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_Pointer_Table

*     File name:   scrnstat.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/scrnname.cpp 1.55 2008/07/14 20:29:30Z dslausb Exp dslausb $
$Log: scrnname.cpp $
Revision 1.57  2009/04/17 16:19:29Z  dslausb
IT 9171 - Disconnect screen changes
Revision 1.56  2009/01/05 22:51:25Z  dslausb
IT 8990 - Move RBC initial split screen to after air evac, but before disposables test.
Revision 1.55  2008/07/14 20:29:30Z  dslausb
More stringId changes requested by localization
Revision 1.54  2008/07/10 20:30:39Z  dslausb
Hopefully last bit of stringId standardization stuff
Revision 1.53  2008/07/09 23:53:31Z  dslausb
More string ID standardization as requested by Marita
Revision 1.52  2008/06/18 18:36:12Z  dslausb
IT 8568 - Work on the language selection dropdown list
Revision 1.51  2008/06/11 18:11:39Z  dslausb
IT 8686 - New "pas vol remaining" functionality on the PAS empty screen
Revision 1.50  2008/06/05 22:41:31Z  dslausb
IT 8673 - Solution connection confirmation boxes
Revision 1.49  2008/06/04 17:18:33Z  dslausb
IT 8556 - Flow adjustment on predict screen and such.
Revision 1.48  2008/05/12 22:05:42Z  dslausb
IT8446 - Add donor info confirmation screen before donor connect
Revision 1.47  2008/04/24 22:16:35Z  dslausb
IT 8588 - Add confirm box to PAS connection screen
Revision 1.46  2008/03/24 21:13:26Z  dslausb
IT 8374 - Change the offline addition screen
Revision 1.44  2008/02/21 23:47:26Z  dslausb
IT 8456 - Build listbox for drop-in languages
Revision 1.43  2008/02/19 17:17:31Z  dslausb
Add reboot confirm dialog
Revision 1.42  2007/11/29 23:47:03Z  dslausb
IT 7372 - Moved up filter load screen to be directly after the syscassette screen.
Revision 1.41  2007/10/18 21:58:55Z  dslausb
Fixed include stuff
Revision 1.40  2007/10/18 21:40:23Z  dslausb
Fixed file include error
Revision 1.39  2007/10/18 21:33:52Z  dslausb
IT 7210 - Changes to make sure graphics indicate correct RAS bag connection, etc.
Revision 1.38  2007/08/10 15:27:22Z  dslausb
Screen changed for DRBC Automated Split
Revision 1.37  2007/05/23 20:50:30Z  dslausb
IT 8015 - Fixed run summary screen allocation
Revision 1.36  2007/04/05 23:00:09Z  dslausb
Split up DRBC split alarm into its three respective versions.
Revision 1.35  2007/03/14 19:17:40Z  dslausb
Revision 1.34  2007/02/27 22:55:37Z  dslausb
IT 7878 - AAS Disconnect is now done in a single blaze of three-panel glory.
Revision 1.33  2006/12/07 20:56:09Z  dslausb
- separated "Clamp PAS line" stuff from sysclamp (it's now pasInitSplit.)
- added the clamp crossover screen.
- added functions to guiglobs to determine which of these should be shown.
Revision 1.32  2006/12/06 18:19:38Z  dslausb
Added buttons to Automated Processes config, and added ras connect screen.
Revision 1.31  2006/12/04 20:56:07Z  dslausb
Split the MSS Begin screens up into separate classes
Revision 1.30  2006/09/11 19:24:24Z  dslausb
Revision 1.29  2006/08/15 21:41:27Z  dslausb
IT 7598 - Added new alarm for mss incomplete
Revision 1.28  2006/07/28 23:29:24Z  dslausb
Redesigned cassette entry
Revision 1.27  2006/06/13 17:42:47Z  dslausb
IT 7503 - Added procedure information to PLT, Plas, and RBC buttons in the config procedure priority screen.
Revision 1.26  2006/05/23 18:58:57Z  dslausb
Added additional information to PLT listbox
Revision 1.25  2006/05/05 21:46:13Z  dslausb
IT 7212 - Separated up flag and labeling run summary screens into their respective products
Revision 1.24  2006/04/21 22:41:43Z  dslausb
Removed VerifyCassette screen
Revision 1.23  2006/04/10 23:07:15Z  dslausb
IT 7272 - Fixed many issues with set identification, added keypad set entry page before dispload
Revision 1.22  2006/03/03 20:59:14Z  dslausb
Revision 1.21  2006/03/03 17:30:07Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.20  2003/04/04 17:59:34Z  rm10919
Datalog changes. IT 5818
Revision 1.19  2002/08/15 22:17:21Z  sb07663
compiling version for Vx
Revision 1.18  2001/05/03 13:26:10  rm70006
Added barcode code from V4.2.  IT 5083.
Revision 1.17  2000/10/04 16:43:41  sb07663
Config.dat modifications for new volume removal limits
Revision 1.16  2000/02/28 22:20:42  SB07663
IT4612: new configuration guaranteed yield screen
Revision 1.15  1999/06/14 23:56:14  TW09453
Add the Product warning (verifprd) and Label product (labelprd)
screens to the screen collection class.
Revision 1.14  1999/05/28 22:21:28  TW09453
Add references to the set identification screen
Revision 1.13  1999/05/07 16:18:41  SB07663
Added new "replacement fluid" configuration screen
Revision 1.12  1999/05/05 22:11:41  TW09453
Add the reference to the Replace Fluid screen.
Revision 1.11  1998/05/20 21:54:40  MC03015
addition of donordl screen
Revision 1.10  1997/05/12 17:51:06  tw09453
Add "PREDICT" and "TROUBLE" as dynamic screens for
alert processing.
Revision 1.9  1997/03/11 16:13:27  Barry_Fulkerson
- removed all references to the old cfg_repo & cfg_serv screens. They are
  do not exist.
Revision 1.8  1997/03/06 22:10:52  tw09453
Allocate all the static screens in the heap instead of on the stack --
for debugging purposes and to unburden the stack space.
Revision 1.7  1997/02/26 02:33:17  Barry_Fulkerson
- changed reference from the cfg_adju file to the cfg_priority file.  Renamed
 cfg_adju.cpp/hpp to cfg_priority to better represent what it is actually doing
 also in preperation for changing the PROCEDURE PRIORITY screen to include
 more than 5 procedures.
Revision 1.6  1997/01/10 22:56:03  tw09453
remove the bloodsmp screen from the world
Revision 1.5  1996/10/07 11:24:01  MC03015
Addition of PREDICT Screen
Revision 1.4  1996/09/28 22:27:52  Barry_Fulkerson
GUI Configuration additions for 2.3
Revision 1.3  1996/07/08 16:44:10  tw09453
Update file header


   */


#include <vxworks.h>

// BASE CLASS INCLUDES
#include "scrnname.hpp"

#include "sysdonor.hpp"
#include "sysdisp.hpp"
#include "sysclamp.hpp"
#include "systest.hpp"
#include "donconn.hpp"
#include "disconn.hpp"
#include "runsumm1.hpp"
#include "runsum_plt.hpp"
#include "runsum_plasma.hpp"
#include "runsum_rbc.hpp"
#include "sysacatt.hpp"
#include "powerup.hpp"
#include "donvital.hpp"
#include "doninfo.hpp"
#include "runproc.hpp"
#include "trouble.hpp"
#include "predict_screen.hpp"
#include "cfg_main.hpp"
#include "cfg_mach.hpp"
#include "cfg_lang.hpp"
#include "cfg_plas.hpp"
#include "cfg_plat.hpp"
#include "cfg_rbc.hpp"
#include "cfg_proc.hpp"
#include "cfg_dono.hpp"
#include "cfg_priority.hpp"
#include "confirmDonInfo.hpp"
#include "cfg_repl.hpp"
#include "cfg_mss.hpp"
#include "cfg_volume.hpp"
#include "barcode_screen.hpp"
#include "CFR11_screen.hpp"
#include "aasClampChannel.hpp"
#include "aasChannelTest.hpp"
#include "aasConnectPas.hpp"
#include "aasConnectRas.hpp"
#include "aasPrime.hpp"
#include "pasSplit.hpp"
#include "aasAddition.hpp"
#include "mssEnd.hpp"
#include "runsumm2.hpp"
#include "abortcnf.hpp"
#include "alarm.hpp"
#include "keypad.hpp"
#include "listbox.hpp"
#include "plt_listbox.hpp"
#include "plas_listbox.hpp"
#include "rbc_listbox.hpp"
#include "setid.hpp"
#include "repfluid.hpp"
#include "syscassette.hpp"
#include "cassetteDeleteConfirmBox.hpp"
#include "aasRunSummary.hpp"
#include "clampCrossover.hpp"
#include "pasInitialSplit.hpp"
#include "drbcSplit.hpp"
#include "aasTransitionDummy.hpp"
#include "rasConnectConfirmBox.hpp"
#include "pasConnectConfirmBox.hpp"
#include "ptfMount.hpp"
#include "rebootConfirmBox.hpp"
#include "langListbox.hpp"
#include "powerdown.hpp"
#include "predictConfirmBox.hpp"
#include "salineConnectConfirmBox.hpp"
#include "acConnectConfirmBox.hpp"
#include "pasEmpty.hpp"
#include "rbcSplitInit.hpp"
#include "sealAndDetach.hpp"
#include "jpClampBloodDiversion.hpp"
#include "cfg_plt_amap.hpp"
#include "predictCompPlsConfirmBox.hpp"
#include "cfg_softOpt.hpp"
#include "ScreenKeyboard.h"
#include "jpClampPlasmaAirBag.hpp"
#include "pasBarcodeConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_Pointer_Table::Screen_Pointer_Table()
{
   // Dynamic heap allocation (one time per GUI execution).
   screen_ptr_table[0].aph_screen_ptr  = new Screen_SYSDONOR;
   screen_ptr_table[1].aph_screen_ptr  = new Screen_SYSDISP;
   screen_ptr_table[2].aph_screen_ptr  = new Screen_PTFMOUNT;
   screen_ptr_table[3].aph_screen_ptr  = new Screen_SYSCLAMP;
   screen_ptr_table[4].aph_screen_ptr  = new Screen_PAS_INIT_SPLIT;
   screen_ptr_table[5].aph_screen_ptr  = new Screen_CLAMP_CROSSOVER;
   screen_ptr_table[6].aph_screen_ptr  = new Screen_SYSCASSETTE;
   screen_ptr_table[7].aph_screen_ptr  = new Screen_SYSTEST;
   screen_ptr_table[8].aph_screen_ptr  = new Screen_DONCONN;
   screen_ptr_table[9].aph_screen_ptr  = new Screen_DISCONN;
   screen_ptr_table[10].aph_screen_ptr = new Screen_RUNSUMM1;
   screen_ptr_table[11].aph_screen_ptr = new Screen_RUNSUM_PLT;
   screen_ptr_table[12].aph_screen_ptr = new Screen_RUNSUM_PLASMA;
   screen_ptr_table[13].aph_screen_ptr = new Screen_RUNSUM_RBC;

   // The two screens for RBC run summary automated split use the same class,
   // constructed with a different ID
   screen_ptr_table[14].aph_screen_ptr = new Screen_RUNSUM_RBC(GUI_SCREEN_RUNSUM_RBC_PROD1);
   screen_ptr_table[15].aph_screen_ptr = new Screen_RUNSUM_RBC(GUI_SCREEN_RUNSUM_RBC_PROD2);

   screen_ptr_table[16].aph_screen_ptr = new Screen_AAS_RUN_SUMMARY;
   screen_ptr_table[17].aph_screen_ptr = new Screen_RUNSUMM2;
   screen_ptr_table[18].aph_screen_ptr = new Screen_SYSACATT;
   screen_ptr_table[19].aph_screen_ptr = new Screen_POWERUP;
   screen_ptr_table[20].aph_screen_ptr = new Screen_DONVITAL;
   screen_ptr_table[21].aph_screen_ptr = new Screen_DONINFO;
   screen_ptr_table[22].aph_screen_ptr = new Screen_RUNPROC;
   screen_ptr_table[23].aph_screen_ptr = new Screen_TROUBLE;
   screen_ptr_table[24].aph_screen_ptr = new Screen_PREDICT;
   screen_ptr_table[25].aph_screen_ptr = new Screen_CONFIG;
   screen_ptr_table[26].aph_screen_ptr = new Screen_CONFIG_MACHINE;
   screen_ptr_table[27].aph_screen_ptr = new Screen_CONFIG_LANGUAGE;
   screen_ptr_table[28].aph_screen_ptr = new Screen_CONFIG_PLASMA;
   screen_ptr_table[29].aph_screen_ptr = new Screen_CONFIG_PLATELET;
   screen_ptr_table[30].aph_screen_ptr = new Screen_CONFIG_RBC;
   screen_ptr_table[31].aph_screen_ptr = new Screen_CONFIG_PROCEDURE;
   screen_ptr_table[32].aph_screen_ptr = new Screen_CONFIG_DONOR;
   screen_ptr_table[33].aph_screen_ptr = new Screen_CONFIG_PRIORITY;
   screen_ptr_table[34].aph_screen_ptr = new Screen_CONFIRM_DON_INFO;
   screen_ptr_table[35].aph_screen_ptr = new Screen_CONFIG_REPL;
   screen_ptr_table[36].aph_screen_ptr = new Screen_CONFIG_VOLUME;
   screen_ptr_table[37].aph_screen_ptr = new Screen_CONFIG_MSS;
   screen_ptr_table[38].aph_screen_ptr = new Screen_AASCLAMPCHANNEL;
   screen_ptr_table[39].aph_screen_ptr = new Screen_AASCHANNELTEST;
   screen_ptr_table[40].aph_screen_ptr = new Screen_AASCONNECTPAS;
   screen_ptr_table[41].aph_screen_ptr = new Screen_AASCONNECTRAS;
   screen_ptr_table[42].aph_screen_ptr = new Screen_AASPRIME;
   screen_ptr_table[43].aph_screen_ptr = new Screen_PAS_SPLIT;
   screen_ptr_table[44].aph_screen_ptr = new Screen_AASADDITION;
   screen_ptr_table[45].aph_screen_ptr = new Screen_MSS_END;
   // Note that aasTransitionDummy is not a real graphical screen,
   // it's just a tool to allow the screen commutator to make the
   // right transition when the system moves to METERED_STORAGE state.
   screen_ptr_table[46].aph_screen_ptr = new aasTransitionDummy;
   screen_ptr_table[47].aph_screen_ptr = new Screen_POWERDOWN;
   screen_ptr_table[48].aph_screen_ptr = new Screen_RBCSPLITINIT;
   screen_ptr_table[49].aph_screen_ptr = new Screen_SEAL_AND_DETACH;
   screen_ptr_table[50].aph_screen_ptr = new Screen_JP_CLAMP_BLOOD_DIVERSION;
   screen_ptr_table[51].aph_screen_ptr = new Screen_PLT_AMAP;
   screen_ptr_table[52].aph_screen_ptr = new Screen_PREDICT_COMP_PLS_CONFIRM_BOX;
   screen_ptr_table[53].aph_screen_ptr = new Screen_CONFIG_SOFTOPT;
   screen_ptr_table[54].aph_screen_ptr = new Screen_JP_CLAMP_PLASMA_AIR_BAG;

   // Don't forget to increase the array allocation and MAX_SCREENS manifest in the header file)
}   // END of Screen_Pointer_Table CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_Pointer_Table::~Screen_Pointer_Table()
{
   // Loop through all the fixed allocation screens and make sure their resources
   // are released.

   for (int dealloc_loop = 0; dealloc_loop < MAX_SCREENS; ++dealloc_loop)
   {
      // Deallocate the resources and free the screen memory from the heap.
      if (screen_ptr_table[dealloc_loop].aph_screen_ptr != NULL)
      {
         screen_ptr_table[dealloc_loop].aph_screen_ptr -> deallocate_resources ();
         delete screen_ptr_table[dealloc_loop].aph_screen_ptr;
      }

   }

}   // END of Screen_Pointer_Table DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// GET_STATIC_SCREEN_POINTER

Base_Window* Screen_Pointer_Table::get_static_screen_ptr (GUI_SCREEN_ID screen_id)
{
   SCREEN_TABLE_ITER screen_iter;

   // walk through the screen pointer table asking each screen for its screen ID
   // and comparing it to the desired screen ID. If a match occurs, return the
   // screen pointer.
   for (screen_iter = 0; screen_iter < MAX_SCREENS; ++screen_iter)
   {
      if (((screen_ptr_table[screen_iter].aph_screen_ptr) -> get_screen_id()) == screen_id)
      {
         return(screen_ptr_table[screen_iter].aph_screen_ptr);
      }
   }

   // If the screen can't be found, software fault
   _FATAL_ERROR (__FILE__, __LINE__, "Attempted to convert invalid screen ID to screen pointer");

   // This never-to-be-reached return is to appease the officious compiler about no return value
   return(NULL);

}   // END of Screen_Pointer_Table GET_STATIC_SCREEN_POINTER



////////////////////////////////////////////////////////////////////////////////
// GET_FIRST_STATIC_SCREEN_TABLE_POINTER

Base_Window* Screen_Pointer_Table::get_first_static_screen_table_pointer (SCREEN_TABLE_ITER& screen_iter)
{

   // point to the first entry in the table
   screen_iter = 0;

   // return that entry to the caller
   return(screen_ptr_table[screen_iter].aph_screen_ptr);


}   // END of Screen_Pointer_Table GET_FIRST_STATIC_SCREEN_TABLE_POINTER




////////////////////////////////////////////////////////////////////////////////
// GET_NEXT_STATIC_SCREEN_TABLE_POINTER

Base_Window* Screen_Pointer_Table::get_next_static_screen_table_pointer (SCREEN_TABLE_ITER& screen_iter)
{

   // make sure we're not past the end of the table or using a negative iterator index
   if (((screen_iter + 1) < MAX_SCREENS) && (screen_iter >= 0))
   {
      // bump the interator to the next valid entry
      ++screen_iter;

      // still in valid-table-land, return the corresponding table pointer
      return(screen_ptr_table[screen_iter].aph_screen_ptr);
   }
   else
   {
      // past the end of the table or the iterator has been set to a negative
      // number.
      return(NULL);
   }

}   // END of Screen_Pointer_Table GET_NEXT_STATIC_SCREEN_TABLE_POINTER


////////////////////////////////////////////////////////////////////////////////
// GET_DYNAMIC_SCREEN_POINTER

Base_Window* Screen_Pointer_Table::get_dynamic_screen_pointer (GUI_SCREEN_ID screen_id)
{
   switch (screen_id)
   {
      case GUI_SCREEN_ABORTCNF :
         return(new Screen_ABORTCNF);

      case GUI_SCREEN_ALARM :
         return(new Screen_ALARM);

      case GUI_SCREEN_KEYPAD :
         return(new Screen_KEYPAD);

      case GUI_SCREEN_LISTBOX :
         return(new Screen_LISTBOX);

      case GUI_SCREEN_PLT_LISTBOX :
         return (new Screen_PLT_LISTBOX);

      case GUI_SCREEN_PLAS_LISTBOX :
         return (new Screen_PLAS_LISTBOX);

      case GUI_SCREEN_RBC_LISTBOX :
         return (new Screen_RBC_LISTBOX);

      case GUI_SCREEN_SET_LISTBOX :
         return (new Screen_SET_LISTBOX);

      case GUI_SCREEN_LANG_LISTBOX :
         return (new LanguageListbox);

      case GUI_SCREEN_CONFIRM_BOX :
         return (new Screen_CASSETTE_DELETE_CONFIRM_BOX);

      case GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX :
         return (new Screen_RAS_CONNECT_CONFIRM_BOX);

      case GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX :
         return (new Screen_PAS_CONNECT_CONFIRM_BOX);

      case GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX :
         return (new Screen_PAS_BARCODE_CONFIRM_BOX);

      case GUI_SCREEN_REBOOT_CONFIRM_BOX :
         return (new Screen_REBOOT_CONFIRM_BOX);

      // This instance is made during an alert/alarm as a stateless screen.
      case GUI_SCREEN_TROUBLE :
         return(new Screen_TROUBLE);

      // This instance is made during an alert/alarm as a stateless screen.
      case GUI_SCREEN_PREDICT :
         return(new Screen_PREDICT);

      // This instance is made during an alert/alarm as a stateless screen.
      case GUI_SCREEN_REPFLUID :
         return(new Screen_REPFLUID);

      // This instance is made during an alert/alarm as a stateless screen.
      case GUI_SCREEN_SETID :
         return(new Screen_SETID);

      case GUI_SCREEN_CFR11 :
         return(new Screen_CFR11);

      // This instance is made during an alert/alarm as a stateless screen.
      case GUI_SCREEN_BARCODE :
         return(new Screen_BARCODE);

      // This instance is made for a DRBC split notification (during collection or RAS addition).
      case GUI_SCREEN_DRBC_SPLIT :
         return(new Screen_DRBC_SPLIT);

      case GUI_SCREEN_PREDICT_CONFIRM_BOX :
         return (new Screen_PREDICT_CONFIRM_BOX);

      case GUI_SCREEN_PREDICT_COMP_PLS_CONFIRM_BOX :
         return (new Screen_PREDICT_COMP_PLS_CONFIRM_BOX);

      case GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX :
         return (new Screen_SALINE_CONNECT_CONFIRM_BOX);

      case GUI_SCREEN_AC_CONNECT_CONFIRM_BOX :
         return (new Screen_AC_CONNECT_CONFIRM_BOX);

      case GUI_SCREEN_PAS_EMPTY :
         return (new Screen_PAS_EMPTY);

      case GUI_SCREEN_KEYBOARD :
         return(ScreenKeyboard::Instance());
      // case other screens e.g. help screens:
      //   return (new );

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Attempted to C++ allocate an invalid dynamic screen");

         break;

   }   // End of switch (screen_id)...

   // This never-to-be-reached return is to appease the officious compiler about no return value
   return(NULL);

}   // END of Screen_Pointer_Table GET_DYNAMIC_SCREEN_POINTER

/* FORMAT HASH a7bb87e91d443584d113a72bf3abe421 */
