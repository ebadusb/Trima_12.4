/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Description: Operator local config changes before committing to Config CDS

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/operator_config.cpp 1.4 2006/06/05 14:18:36Z rm70006 Exp dslausb $
$Log: operator_config.cpp $

****************************************************************************/

#include <vxWorks.h>
#include "operator_config.h"
#include "guiglobs.hpp"

Operator_Config::Operator_Config()
{
   refresh();
}

void Operator_Config::refresh ()
{
   // Copy the latest saved config to the "working operator" copy.
   // If the operator chooses to save, then the working copy is copied to the Config CDS
   // and committed, otherwise, no action is taken and all changes to the working copy
   // are lost on config screen exit.
   lang       = guiglobs::cdsData.config.LangUnit.Get();
   machine    = guiglobs::cdsData.config.Machine.Get();
   procedure  = guiglobs::cdsData.config.Procedure.Get();
   predict    = guiglobs::cdsData.config.Predict.Get();
   templates  = guiglobs::cdsData.config.Templates.Get();
   definition = guiglobs::cdsData.config.Definition.Get();
}

int Operator_Config::getLanguageIndex ()
{
   return lang.key_lang;
}


void Operator_Config::write ()
{
   Config_CDS writeinstance(ROLE_RW);

   writeinstance.LangUnit.Set(lang);
   writeinstance.Machine.Set(machine);
   writeinstance.Procedure.Set(procedure);
   writeinstance.Predict.Set(predict);
   writeinstance.Templates.Set(templates);
   writeinstance.Definition.Set(definition);

   writeinstance.Write();
}

/* FORMAT HASH 7b8966f9e59bcfedc7ecc10d35a59172 */
