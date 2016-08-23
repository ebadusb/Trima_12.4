/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2002 Gambro BCT, Inc. All rights reserved

*     Author:      Regis McGarry

*     Description: Operator local conifg changes before committing to Config CDS

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/operator_config.h 1.5 2009/07/23 19:28:17Z dslausb Exp dslausb $

$Log: operator_config.h $
Revision 1.5  2009/07/23 19:28:17Z  dslausb
IT 9214 - Add drop-in fonts
Revision 1.4  2006/06/05 14:18:36Z  rm70006
IT 7510.  Remove CassetteOptions struct from config and move MSS fields to Procedure.
Revision 1.3  2006/05/23 18:58:51Z  dslausb
Added additional information to PLT listbox
Revision 1.2  2006/03/03 17:29:33Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.1  2002/08/05 22:41:43Z  rm70006
Initial revision

*/

#ifndef __OPERATOR_CONFIG_H__
#define __OPERATOR_CONFIG_H__

#include <vxWorks.h> // Needed for CPU compiler directives
#include "configdef.h"

class Operator_Config
{
public:

   LangUnitCfg            lang;
   BasicMachineCfg        machine;
   ProcedureConfig        procedure;
   PredictionConfig       predict;
   ProductTemplates       templates;
   ProductDefinitionArray definition;

public:
   Operator_Config();

   void refresh ();
   int  getLanguageIndex ();


   void write ();
};

#endif

/* FORMAT HASH 5253586c77a3d836b51d63a6a56c9323 */
