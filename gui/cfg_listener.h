/************************> FOCUSSED GUI DOMAIN <****************************
*
* Copyright(c) 2003 Gambro BCT, Inc.  All rights reserved.
*
* History:
* $Log$
*
*************************< FOCUSSED GUI DOMAIN >****************************/

// Single include define guardian
#ifndef CFG_LISTENER_HPP
#define CFG_LISTENER_HPP


#include "trimamessages.h"

//
//
//
class Config_Listener
{
public:
   Config_Listener();
   virtual ~Config_Listener();

   void Initialize ();

private:
   ConfigChangedMsg _configChangedMsg;

private:
   void HandleConfigChangedMsg ();

};

#endif

/* FORMAT HASH ee6220746861317142433bffa925bb45 */
