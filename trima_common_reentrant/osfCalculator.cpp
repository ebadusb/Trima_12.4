////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2005 Gambro BCT, Inc.  All rights reserverd
//
// Author:       Jerry Heintz
//
// Class Name:   OSFCalculator
//
// Description:
//
//
// $Log $
//
////////////////////////////////////////////////////////////////////////////

#include <vxworks.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usrLib.h>


#include "procrun_cds.h"
#include "filenames.h"
#include "OSFCalculator.h"

// #include "procrun_cds.h"  // contains all the reasons to kick out the run


// #define OSF_FILE "osfData.txt"
/////////////////////////////////////////////////////////////////////
// Just a little helper class to parse the text file   //////////////
class Tokenizer
{
public:
   Tokenizer(const string& xs, const string& xdelim)
      : s(xs), delim(xdelim)
   {
      pos = s.find_first_not_of(delim);
   }

   ~Tokenizer() {}

   bool has_more_tokens ()
   {
      return pos!=string::npos;
   }

   string next_token ()
   {
      string::size_type end_pos = s.find_first_of(delim, pos);
      string            token   = s.substr(pos, end_pos - pos);
      pos = s.find_first_not_of(delim, end_pos);

      return token;
   }


private:
   string            s, delim;
   string::size_type pos;
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

// move to cobe config ????
const int   maxVals      = 50; // 50 numbers will be saved
const int   minVals      = 10; // min numbers necessary to do a calc... otherwise use default
const float defaultVal   = 0.77f;
const float maintAlertCV = 0.2f;




OSFCalculator::OSFCalculator ()
   : numberValuesRetained(maxVals),
     minValuesForCalc(minVals),
     doNotPersist(false),
     haveAlert(false)
{
   int n = restoreOSFData();
   DataLog(log_level_trima_library_info) << n << " Values restored to OSF vector" << endmsg;
   // cout << n << " Values restored to OSF vector" << endl;
}

OSFCalculator::~OSFCalculator ()
{
   if (doNotPersist)
   {

      DataLog(log_level_trima_library_info) << "OSF_Restart ordered; No Values saved" << endmsg;
      // cout << "OSF_Restart ordered; No Values saved" << endl;
   }
   else
   {

      int n = persistOSFData();
      DataLog(log_level_trima_library_info) << n << " Values saved from OSF vector" << endmsg;
      // cout << n << " Values saved from OSF vector" << endl;
   }
}


float OSFCalculator::osf ()
{

   DataLog(log_level_trima_library_info) << "getting your OSF value" << endmsg;

   float osfValue = defaultVal;

   // if we have collected less than the minimum data...
   // then return the default value;
   if (osfData.size() < minValuesForCalc)
   {

      DataLog(log_level_trima_library_info) << "using Default OSF value" << endmsg;
      // cout << "using Default OSF value" << endl;
      // osfValue = CobeConfig::data().OSFd;
   }
   else  // we have enough data to do the actual calulation
   {
      osfValue = calculateOSF();
   }

   return osfValue;

}



bool OSFCalculator::addrun (const float val, bool forceAdd)
{
   /*
   ProcRun_CDS p( ROLE_RO );

  // check out the conditons to add or not
  if ( !forceAdd )
  {




     bool runOK = true;


     if (p.OQCAlert.Get())         ;

     if (p.APSAlert.Get())         ;

     if (p.AnyAlarm.Get())          ;

     if (p.APSPauses.Get())         ;

     if (p.AnyFlags.Get())          ;

     if (p.InletFlowChanges.Get())     ;

     if (p.DataDonorTimeMins.Get())    ;



  }


   */

   DataLog(log_level_trima_library_info) << " adding value : " << val << endmsg;

   // add this run to the vector

   OSF_data dataPoint;

   dataPoint.val = val;

   if (osfData.size() < numberValuesRetained )
   {
      osfData.push_back(dataPoint);
   }
   else
   {

      osfData.erase(osfData.begin());   // dump the oldest
      osfData.push_back(dataPoint);

   }
   float currentSigma = sigma() ;


   osfData[osfData.size() - 1].cv = currentSigma;

   if (currentSigma  > maintAlertCV)
   {
      // cout << "We should be raising and RBC Maintenance Alert!!!" << endl;
      DataLog(log_level_trima_library_info) << "We should be raising and RBC Maintenance Alert!!!" << endmsg;
      haveAlert = true;
   }
   // should this trigger a restart??
   restart(currentSigma);

   return true;

}



float OSFCalculator::calculateOSF ()
{

   // DataLog( log_level_trima_library_info ) << " Calculating OSF   " << endmsg;
   // cout  << " Calculating OSF   " << endl;
   float sum = 0.0;

   for (int i = 0; i < osfData.size() ; i++)
      sum = sum + osfData[i].val;

   return ( sum / osfData.size());
}

int OSFCalculator::persistOSFData ()
{
   attrib(OSF_FILE, "-R");

   DataLog(log_level_trima_library_info) << " Saving OSF data " << endmsg;
   // cout <<  " Saving OSF data " << endl;

   // nothing to do...
   if (osfData.size() < 1)
   {
      return 0;
   }

   ostrstream bufHeader;
   bufHeader << "#############################\n"
             << "#  DO NOT EDIT THIS FILE!!  #\n"
             << "#  OSF data for online QC.  #\n"
             << "#     Ryc  ,  std_dev       #\n"
             << "#############################\n" << '\0';

   // make a buffer to write
   ostrstream buf;
   int        i   = 0;

   int        num = osfData.size();
   if (num > numberValuesRetained)
   {
      num = numberValuesRetained;
   }

   for (i = 0; i < num; i++)
   {
      buf << osfData[i].val << "," << osfData[i].cv << "\n";
   }
   buf << '\0';  // dont forget the null terminator!

   // open the file
   ofstream ofs(OSF_FILE, ios::out);
   if (ofs.fail())
   {
      DataLog(log_level_trima_library_info) << "Can't open file " << OSF_FILE << endmsg;
      // cout <<  "Can't open file " << OSF_FILE << endl;
      return 0;
   }


   // write the buffer
   ofs << bufHeader.str() << buf.str() << '\0';
   // cout << "Writing to file ->" << buf.str() << endl;

   // close it
   ofs.close();

   attrib(OSF_FILE, "+R");

   // return how many we added for the curious
   return i;
}



int OSFCalculator::restoreOSFData ()
{
   string line;

   DataLog(log_level_trima_library_info) << " Restoring OSF data " << endmsg;

   // open file to read
   ifstream ifs(OSF_FILE);
   if (ifs.fail())
   {
      DataLog(log_level_trima_library_info) << "Can't read file... " << OSF_FILE << " (Ok for the initail run) " << endmsg;
      // cout <<  "Can't read file... " << OSF_FILE << " (Ok for the initail run) " << endl;
      return 0;
   }


   int    cnt = 0;
   string token;
   while (!(ifs.eof()) && (cnt < numberValuesRetained) )
   {


      // read the data line and close the file
      getline(ifs, line);
      // cout << " read a line ->" << line << "<---" <<  endl;

      // cout << " line length ->" << line.length()  <<  endl;
      if ( (line.length() < 3))
      {
         continue;
      }

      // ignore header and comments
      if ( ispunct(line[0]) )
      {
         continue;
      }


      // tokenize and reconstrct the vector.
      Tokenizer tok(line, ",");
      // cout << " Tokenizing line "  << endl;

      OSF_data dataPoint;

      token = tok.next_token();
      // cout << " first val token ->" <<  token << "<---" << endl;

      dataPoint.val = atof(token.c_str() );

      token         = tok.next_token();
      // cout << " second val token ->" <<  token << "<---" << endl;

      dataPoint.cv = atof(token.c_str() );

      osfData.push_back(dataPoint);

      cnt++;
      // cout << " cnt of vals -> " <<  cnt << endl;

   }

   ifs.close();

   // return the number of data points read for the curious
   return cnt;
}


float OSFCalculator::sigma ()
{

   DataLog(log_level_trima_library_info) << "Calc Sigma" << endmsg;
   if (osfData.size() < 2)
      return 0.0;

   int   OSFPoints = osfData.size();
   float sum       = 0.0;
   float sumSq     = 0.0;
   float sigma     = 0.0f;


   for (int i = 0; i < OSFPoints; i++)
   {
      sum   = sum   + osfData[i].val ;
      sumSq = sumSq + osfData[i].val * osfData[i].val;
   }

   if (sumSq <= (sum * sum) / osfData.size())
   {
      sigma =  0.0f;
   }
   else
   {
      sigma = sqrt( (sumSq - (sum * sum) / osfData.size() ) / (osfData.size() - 1)  );
   }

   return sigma;
}


void OSFCalculator::dumpValues () const
{
   DataLog(log_level_trima_library_info) << "Value Dump to follow" << endmsg;
   DataLog(log_level_trima_library_info) << "   " << endmsg;
   // cout << "Dump Online QC data-> " << endl;
   for ( int i = 0; i < osfData.size(); i++)
   {
      DataLog(log_level_trima_library_info) << osfData[i].val << "," << osfData[i].cv << endmsg;
      // cout << osfData[i].val << "," << osfData[i].cv << endl;
   }


}



void OSFCalculator::restart (const float sigma)
{

   int sigmaErrorCntr = 0;

   // only possible for minValuesForCalc or more values
   if (osfData.size() < minValuesForCalc)
   {
      // cout << "less than min values No restart!" << endl;
      return;
   }

   float A, B, C =  0.0f;
   float CV2 = ( 2.0f  * sigma );

   // get three consecutive vals to track
   for (int i = 2; i < osfData.size(); i++)
   {
      A = fabs(osfData[i - 2].val - 1.0f);
      B = fabs(osfData[i - 1].val - 1.0f);
      C = fabs(osfData[i].val - 1.0f);

      // if we're  more than 2sigma out for 2 out'a 3 tries
      if ( A > CV2 )
      {
         sigmaErrorCntr++;
      }

      if ( B > CV2 )
      {
         sigmaErrorCntr++;
      }

      if ( C > CV2 )
      {
         sigmaErrorCntr++;
      }


      if  (sigmaErrorCntr > 1)
      {
         DataLog(log_level_trima_library_info) << "Two out of three values beyond two sigma!.... Purging values" << endmsg;
         attrib(OSF_FILE, "-R");
         // cout << "Two out of three values beyond two sigma!.... Purging values!" << endl;
         unlink(OSF_FILE);
         doNotPersist = true;
         return;    // youve done your job
      }
      sigmaErrorCntr = 0;  // re-set for next consec 3
   }

}

/* FORMAT HASH 3c0a3f23e847b579c7ed6e34622d9329 */
