/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PREDICT_CONFIRM_BOX

*     File name:   predictConfirmBox.hpp

*     Contents:    See prototype header descriptions below...

*     Description: This class represents a confirm box for the PAS connection
        screen.  Because the operator is dealing with several different fluids
        and spikes, we need to do something special to make them confirm they've
        connected the correct fluid to the correct spike.

        This class is derived from the confirmBoxBase class.  It adds extra
        text to that screen, and the background graphic has some specialized
        stuff.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/predictConfirmBox.hpp 1.2 2008/06/04 20:07:24Z dslausb Exp dslausb $
$Log: predictConfirmBox.hpp $
Revision 1.2  2008/06/04 20:07:24Z  dslausb
IT 8556 - More edits for Japan flow rates
Revision 1.1  2008/06/04 17:33:12Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef __PREDICT_CONFIRM_BOX_HPP__
#define __PREDICT_CONFIRM_BOX_HPP__

/** include files **/
#include "confirmBoxBase.hpp"
#include "textDataField.hpp"

class Screen_PREDICT_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text
            _txtConfirmMsg,
            _txtDataLbl;

   TextDataField
      _dataPltVol,
      _dataPltYield,
      _dataRbcVol,
      _dataPlasVol,
      _dataAcRequired,
      _dataTime,
      _dataTbvProcessed,
      _dataPostPlt;

public:

   // CONSTRUCTOR
   Screen_PREDICT_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_PREDICT_CONFIRM_BOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

}; // END of Screen_PREDICT_CONFIRM_BOX Class Declaration

// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of predictConfirmBox.hpp file

/* FORMAT HASH cd7d9ea57e4c30f2cd63520010b3763e */
