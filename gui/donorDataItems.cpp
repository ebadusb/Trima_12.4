/************************> FOCUSSED GUI DOMAIN <****************************
*
*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved
*
*     Author:      Dan Slauson
*
*     Class name:  DonorDataItems
*
*     File name:   donorDataItems.cpp
*
*     Contents:    Implementation of class methods
*
*     Description: See header file for details
*
*************************< FOCUSSED GUI DOMAIN >****************************
*
*	$Header$
*
****************************************************************************/
#include <vxworks.h>


#include "guiglobs.hpp"
#include "donorDataItems.hpp"
#include "trima_variable_database.h"

DonorDataItems::DonorDataItems(const CDonor& donInfo)
   : dataItemTbv         ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemTbv")),
     dataItemHeightFeet  ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemHeightFeet")),
     dataItemHeightInches((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemHeightInches")),
     dataItemHeightCm    ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemHeightCm")),
     dataItemWeightLb    ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemWeightLb")),
     dataItemWeightKg    ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemWeightKg")),
     dataItemRbcHct      ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemRbcHct")),
     dataItemRbcHgb      ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemRbcHgb")),
     dataItemPltPrecount ((TrimaDataItemInteger*)TrimaVariableDatabase::getDataItem("dataItemPltPrecount")),
     dataItemDob         ((TrimaDataItemText*)TrimaVariableDatabase::getDataItem("dataItemDob")),
     dataItemName        ((TrimaDataItemText*)TrimaVariableDatabase::getDataItem("dataItemName"))
{
   set(dataItemTbv,            guiglobs::donor_tbv() + 0.5 /* round up */,                   "TBV");
   set(dataItemHeightFeet,     donInfo.HeightInInches() / 12,                              "height (ft)");
   set(dataItemHeightInches,   donInfo.HeightInInches() % 12,                              "height (in)");
   set(dataItemHeightCm,       donInfo.HeightInCm(),                                       "height (cm)");
   set(dataItemWeightLb,       donInfo.WeightInLb(),                                       "weight (lb)");
   set(dataItemWeightKg,       donInfo.WeightInKg(),                                       "height (kg)");
   set(dataItemRbcHct,         donInfo.Hct(),                                              "Hct");
   set(dataItemRbcHgb,         donInfo.Hgb(),                                              "Hgb");
   set(dataItemPltPrecount,    donInfo.PreCnt(),                                           "precount");
   set(dataItemDob,            donInfo.DOB(),                                              "DOB");
   set(dataItemName,           donInfo.Name(),                                             "name");
}

bool DonorDataItems::set (TrimaDataItemInteger* item, int value, const char* lbl)
{
   if (!item)
   {
      DataLog (log_level_gui_error) << "Unable to locate item " << lbl << endmsg;
      return false;
   }

   item->setValue(value);
   return true;
}

bool DonorDataItems::set (TrimaDataItemText* item, const char* value, const char* lbl)
{
   if (!item)
   {
      DataLog (log_level_gui_error) << "Unable to locate item " << lbl << endmsg;
      return false;
   }

   item->setValue(value);
   return true;
}

/* FORMAT HASH 462b9c4e017be578312ccde1b2a5cd9b */
