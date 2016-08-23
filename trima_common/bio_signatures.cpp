/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      bio_signatures.cpp
 *             biometric signatures
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file provides utility functions for the processing of biometric
 *             signatures.
 *
 * HISTORY:
 * $Log: bio_signatures.cpp $
 * Revision 1.3  2007/11/16 00:14:21Z  dslausb
 * Added VxWorks.exe includes in source files where missing
 * Revision 1.2  2004/02/10 23:16:46Z  rm70006
 * IT 6847.  Add stream decoder for better logging.
 * Revision 1.1  2004/02/06 23:09:35Z  rm70006
 * Initial revision
 *
 ******************************************************************************/

#include <vxworks.h>
#include "bio_signatures.h"



DataLog_Stream & operator << (DataLog_Stream& os, const AuthorizationStatusType& status)
{
   switch (status)
   {
      case BIO_AUTH_NONE :
         os << "BIO_AUTH_NONE";
         break;

      case BIO_AUTH_SUCCESS :
         os << "BIO_AUTH_SUCCESS";
         break;

      case BIO_AUTH_FAILED :
         os << "BIO_AUTH_FAILED";
         break;

      case BIO_AUTH_UNKNOWN :
         os << "BIO_AUTH_UNKNOWN";
         break;
   }

   os << "(" << (int)status << ")";

   return os;
}



//
// Print routine for datalog stream
//
DataLog_Stream& operator << (DataLog_Stream& os, const CFR11_Log_Type& item)
{
   os << "(SN " << item.sequenceNumber << "; Dev " << item.device << "; " << item.trigger << "; " << item.status << "; " << item.badge << ")";

   if (item.trigger == TRIGGER_DONOR_VITALS_A_CHANGE)
   {
      os << "[" << item.data.donor_vitals_a.height << "; " << item.data.donor_vitals_a.weight << "; "
         << item.data.donor_vitals_a.gender << "]";
   }
   else if (item.trigger == TRIGGER_DONOR_VITALS_B_CHANGE)
   {
      os << "[" << item.data.donor_vitals_b.bloodType << "; " << item.data.donor_vitals_b.hematocrit << "; "
         << item.data.donor_vitals_b.plateletCount << "]";
   }
   else
   {
      os << "[" << item.data.value << "]";
   }

   return os;
}



//
// Print routine for datalog stream
//
DataLog_Stream& operator << (DataLog_Stream& os, const ComplianceTriggerType& item)
{
   switch (item)
   {
      case TRIGGER_NONE :
         os << "TRIGGER_NONE";
         break;

      case TRIGGER_DONOR_VITALS_A_CHANGE :
         os << "TRIGGER_DONOR_VITALS_A";
         break;

      case TRIGGER_DONOR_VITALS_B_CHANGE :
         os << "TRIGGER_DONOR_VITALS_B";
         break;

      case TRIGGER_PROCEDURE_SELECTED :
         os << "TRIGGER_PROC_SELECTED";
         break;

      case TRIGGER_ADJUSTMENT :
         os << "TRIGGER_ADJUSTMENT";
         break;

      case TRIGGER_ALARM :
         os << "TRIGGER_ALARM";
         break;

      case TRIGGER_SPILLOVER :
         os << "TRIGGER_SPILLOVER";
         break;

      case TRIGGER_AIR_IN_PLASMA_LINE :
         os << "TRIGGER_AIR_IN_PLASMA_LINE";
         break;
   }

   os << "(" << (int)item << ")";

   return os;
}



//
// Print routine for datalog stream
//
DataLog_Stream& operator << (DataLog_Stream& os, const BioSignatureResponse& item)
{
   switch (item)
   {
      case BIO_SIGNATURE_INVALID :
         os << "BIO_SIG_INVALID";
         break;

      case BIO_SIGNATURE_VALID :
         os << "BIO_SIG_VALID";
         break;

      case BIO_SIGNATURE_VALID_ADD :
         os << "BIO_SIGN_VALID_ADD";
         break;
   }

   os << "(" << (int)item << ")";

   return os;
}

/* FORMAT HASH 6571e3031b269862db6dd2d0d052abac */
