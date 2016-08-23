/**
 *  Copyright(c) 2013 Gambro BCT, Inc. All rights reserved
 *
 *  Author:      Mat Ramey
 *
 *  Class name:  Screen_PAS_BARCODE_CONFIRM_BOX
 *
 *  File name:   pasBarcodeConfirmBox.hpp
 *
 *  Description: Prompt/Confirmation for the PAS Bag barcode scan feature.
 *
 *  Because the operator is dealing with several different fluids and
 *  spikes, we need to do something special to make them confirm they've
 *  connected the correct fluid to the correct spike.
 *
 *  This class is derived from the ConfirmBoxBase class. It adds text to
 *  that screen, handles the receipt of a barcode scan, and makes sure the
 *  scanned PAS bag NDC barcode is valid.
 *
 */

// Single include define guardian
#ifndef __PAS_BARCODE_CONFIRM_BOX_HPP__
#define __PAS_BARCODE_CONFIRM_BOX_HPP__

/** include files **/
#include "confirmBoxBase.hpp"
#include "scan_structure.h"

class Screen_PAS_BARCODE_CONFIRM_BOX
   : public ConfirmBoxBase
{
public:
   // CONSTRUCTOR
   Screen_PAS_BARCODE_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_PAS_BARCODE_CONFIRM_BOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   static const char* allocParamForBadBarcode () { return "PAS BARCODE INVALID BOX"; }

protected:
   virtual void handleCancelBtn ();
   void         sendCancelMsg (bool isBadBarcode);
   void         handlePasBagBarcodeMsg ();
   bool         isValidPasBagBarcode (const char* barcode);

private:
   Message<PAS_BAG_BARCODE_MSG> _pasBagBarcodeMsg;
   Display_Text                 _txtBody;
   bool _barcodeIsValid;
protected:
#if (CPU==SIMNT)
   virtual void handleTraverseMessage ();
   SimntTraverseMsg* _screenTraverseMessage;
#endif // #if (CPU==SIMNT)

};

#endif /* __PAS_BARCODE_CONFIRM_BOX_HPP__ */

/* FORMAT HASH 449fdce35c898c3919842cd5ce68f692 */
