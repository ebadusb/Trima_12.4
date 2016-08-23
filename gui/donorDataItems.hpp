/************************> FOCUSSED GUI DOMAIN <****************************
*
*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved
*
*     Author:      Dan Slauson
*
*     Class name:  DonorDataItems
*
*     File name:   donorDataItems.hpp
*
*     Contents:    Implementation of class methods
*
*     Description: This class encapsulates the TrimaDataItems that will
*		be used by the GUI for string substitutions for donor data.
*
*************************< FOCUSSED GUI DOMAIN >****************************
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/donorDataItems.hpp 1.15 2009/07/30 20:14:07Z dslausb Exp dslausb $
*	$Log: donorDataItems.hpp $
*
****************************************************************************/

#ifndef __TRIMA_DATA_ITEMS_HPP__
#define __TRIMA_DATA_ITEMS_HPP__

#include "trima_data_item.h"
#include "donor.h"

class DonorDataItems
{
private:
   TrimaDataItemInteger* dataItemTbv;
   TrimaDataItemInteger* dataItemHeightFeet;
   TrimaDataItemInteger* dataItemHeightInches;
   TrimaDataItemInteger* dataItemHeightCm;
   TrimaDataItemInteger* dataItemWeightLb;
   TrimaDataItemInteger* dataItemWeightKg;
   TrimaDataItemInteger* dataItemRbcHct;
   TrimaDataItemInteger* dataItemRbcHgb;
   TrimaDataItemInteger* dataItemPltPrecount;
   TrimaDataItemText*    dataItemDob;
   TrimaDataItemText*    dataItemName;

   static bool set (TrimaDataItemInteger* item, int value, const char* lbl);
   static bool set (TrimaDataItemText* item, const char* value, const char* lbl);

public:

   DonorDataItems(const CDonor& donInfo);
   void update (const CDonor& donInfo);

};

#endif // #ifndef __TRIMA_DATA_ITEMS_HPP__

/* FORMAT HASH 26a25a40e202455ae8f18393a71e6f77 */
