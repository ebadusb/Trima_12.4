/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  GuiProcedures_CDS

*     File name:   guiProcedures_cds.h

*     Contents:    Class definitions

*     Description: Common data structure for procedures sent between predict and proc.

*************************< FOCUSSED GUI DOMAIN >*****************************/

#ifndef __GUI_PROCEDURES_CDS_H__
#define __GUI_PROCEDURES_CDS_H__

#include "datastore.h"
#include "predict_msgs.h"

class GuiProcedures_CDS
   : public SingleWriteDataStore
{
public:

   BaseElement<ProcedureInformationStruct> ProcedureInfo;

   GuiProcedures_CDS (Role role);
   virtual ~GuiProcedures_CDS();

   ProductOptionStruct getProcedure (PROCEDURE_INDEX index);

   bool kitIsCompatibleWithCurrentProcedure (const CatalogNumber* kit);
   bool currentProcedureIsValid ();
};

#endif // #ifndef __GUI_PROCEDURES_CDS_H__

/* FORMAT HASH d2e3207ab1d14b35e13f33b2ca2cd138 */
