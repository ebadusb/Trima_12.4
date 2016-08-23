#ifndef __PREDICT_CONFIRM_COMP_PLS_BOX_HPP__
#define __PREDICT_CONFIRM_COMP_PLS_BOX_HPP__

/** include files **/
#include "confirmBoxBase.hpp"
#include "textDataField.hpp"

class Screen_PREDICT_COMP_PLS_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text
            _txtConfirmMsg,
            _txtDataLbl;

   TextDataField
      _dataCompPlsToVol,
      _dataCompPlsFromVol;

public:

   // CONSTRUCTOR
   Screen_PREDICT_COMP_PLS_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_PREDICT_COMP_PLS_CONFIRM_BOX();

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

/* FORMAT HASH 19d937850765c8d1030d3830175aaedf */
