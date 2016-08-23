/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS2.DOC
 * Select OMT:       I:\router
 * Test:             I:\ieee1498\STP2.DOC
 *
 * PUBLIC FUNCTIONS:
 * main() - main entry point
 */

// #includes

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "messagesystem.h"
#include "message.h"
#include "fastdata.h"
#include "logger.hpp"


vector<char *> parse(char*);
void et_fastdatacontrolled (char *);

char ecString[255];
char esString[255];

// SPECIFICATION:   message class, used to get pump and centrifuge data
//
// ERROR HANDLING:   none.

class fastlongReceive
{
   public:
      fastlongReceive(DATA_SOURCE src, char *filename)  // constructor
      {
         _mysrc = src;
         _fptr = fopen( filename, "W");
		 fastdatalongmsg.init(Callback<fastlongReceive>(this, &fastlongReceive::notify), MessageBase::SNDRCV_GLOBAL);
		 fastdatalongmsg.set(src);
      };
      ~fastlongReceive()                                 // destructor
      {
         fclose( _fptr);
      };
      void notify()                                      // notify function
      {

         FASTDATALONG   longData;
         longData.source = fastdatalongmsg.get();
         if (longData.source == _mysrc)
         {
            for(int i=0;i<FASTDATALENGTHFORLONGS;i++)
            {
               if ( (i % 10) == 0)
                  fprintf(_fptr,"\n");
               fprintf(_fptr, "%ld, ", longData.raw[i]);
            }
         }
      };
   private:
      FILE* _fptr;
      DATA_SOURCE _mysrc;
	  Message<DATA_SOURCE> fastdatalongmsg;
};


// SPECIFICATION:    message class, used to get aps data
//
// ERROR HANDLING:   none.

class fastapsReceive
{
   public:
      fastapsReceive(DATA_SOURCE src, char *filename)  // constructor
      {
         _mysrc = src;
         _fptr = fopen( filename, "W");
		 fastdatamsg.init(Callback<fastapsReceive>(this, &fastapsReceive::notify), MessageBase::SNDRCV_GLOBAL);
		 fastdatamsg.set(src);
      };
      ~fastapsReceive()                                 // destructor
      {
         fclose( _fptr);
      };
      void notify()                                      // notify function
      {
         FASTDATA   apsData;
         apsData.source = fastdatamsg.get();
         if (apsData.source == _mysrc)
         {
            for(int i=0;i<FASTDATALENGTH;i++)
            {
               if ( (i % 10) == 0)
                  fprintf(_fptr,"\n");
               fprintf(_fptr, "%d, ", apsData.raw[i]);
            }
         }
      };
   private:
      FILE* _fptr;
      DATA_SOURCE _mysrc;
	  Message<DATA_SOURCE> fastdatamsg;
};


// SPECIFICATION:    message class, used to get us data
//
// ERROR HANDLING:   none.

class fastusReceive
{
   public:
      fastusReceive(DATA_SOURCE src, char *filename)  // constructor
      {
         _mysrc = src;
         _fptr = fopen( filename, "W");
		  fastdatamsg.init(Callback<fastusReceive>(this, &fastusReceive::notify), MessageBase::SNDRCV_GLOBAL);
		  fastdatamsg.set(src);
      };
      ~fastusReceive()                                 // destructor
      {
         fclose( _fptr);
      };
      void notify()                                      // notify function
      {
         FASTDATA   usData;
         usData.source = fastdatamsg.get();
         if (usData.source == _mysrc)
         {
            for(int i=0;i<FASTDATALENGTH;i++)
            {
               if ( (i % 10) == 0)
                  fprintf(_fptr,"\n");
               fprintf(_fptr, "%d, ", usData.raw[i]);
            }
         }
      };
   private:
      FILE* _fptr;
      DATA_SOURCE _mysrc;
	  Message<DATA_SOURCE> fastdatamsg;
};



class fastdataControl
{
   public:
      fastdataControl(DATA_SOURCE src)         // constructor 
      {
         _myaps = NULL;
         _myus = NULL;
         _mylong = NULL;

         switch (src)
         {
         case CONTROL_ACCESS_PRESSURE: // 0
            _myaps = new fastapsReceive(src, "/d/APS");
            break;
         case CONTROL_UPPER_US_AIR:    // 1
            _myus = new fastusReceive(src, "/d/ControlUSAir");
            break;
         case CONTROL_UPPER_US_FLUID:  // 2
            _myus = new fastusReceive(src, "/d/ControlUSFluid");
            break;
         case CONTROL_LOWER_US_AIR:    // 3
            _myus = new fastusReceive(src, "/d/ControlLSAir");
            break;
         case CONTROL_LOWER_US_FLUID:  // 4
            _myus = new fastusReceive(src, "/d/ControlLSFluid");
            break;
         case SAFETY_LOWER_US_AIR:     // 5
            _myus = new fastusReceive(src, "/d/SafetyLSAir");
            break;
         case SAFETY_LOWER_US_FLUID:   // 6
            _myus = new fastusReceive(src, "/d/SafetyLSFluid");
            break;
         case CENT_RAW_ENCODER:                // 7
            _mylong = new fastlongReceive(src, "/d/CentRawEncoder");
            break;
         case CENT_AVG_ENCODER:                // 8
            _mylong = new fastlongReceive(src, "/d/CentAvgEncoder");
            break;
         case CENT_INTERNAL_ORDER:             // 9
            _mylong = new fastlongReceive(src, "/d/CentIntOrder");
            break;
         case CENT_DELTA_RPM:                   // 10
            _mylong = new fastlongReceive(src, "/d/CentDeltaRPM");
            break;
         case CENT_INTEGRAL:                    // 11
            _mylong = new fastlongReceive(src, "/d/CentIntegral");
            break;
         case CENT_CONTROL:                    // 12
            _mylong = new fastlongReceive(src, "/d/CentControl");
            break;
         case CENT_ERRORS:                      // 13
            _mylong = new fastlongReceive(src, "/d/CentErrors");
            break;
         case AC_PUMP_ENCODER:         // 14
            _mylong = new fastlongReceive(src, "/d/ACEncoder");
            break;
         case INLET_PUMP_ENCODER:      // 15
            _mylong = new fastlongReceive(src, "/d/InletEncoder");
            break;
         case PLASMA_PUMP_ENCODER:     // 16
            _mylong = new fastlongReceive(src, "/d/PlasmaEncoder");
            break;
         case PLATELET_PUMP_ENCODER:   // 17
            _mylong = new fastlongReceive(src, "/d/PlateletEncoder");
            break;
         case RETURN_PUMP_ENCODER:     // 18
            _mylong = new fastlongReceive(src, "/d/ReturnEncoder");
            break;
         case AC_PUMP_ORDER:           // 19
            _mylong = new fastlongReceive(src, "/d/ACOrder");
            break;
         case INLET_PUMP_ORDER:        // 20
            _mylong = new fastlongReceive(src, "/d/InletOrder");
            break;
         case PLASMA_PUMP_ORDER:       // 21
            _mylong = new fastlongReceive(src, "/d/PlasmaOrder");
            break;
         case PLATELET_PUMP_ORDER:     // 22
            _mylong = new fastlongReceive(src, "/d/PlateletOrder");
            break;
         case RETURN_PUMP_ORDER:       // 23
            _mylong = new fastlongReceive(src, "/d/ReturnOrder");
            break;
         case AC_PUMP_INTEGRATION_ERROR:  // 24
            _mylong = new fastlongReceive(src, "/d/ACIntErr");
           break;
         case INLET_PUMP_INTEGRATION_ERROR:  // 25
            _mylong = new fastlongReceive(src, "/d/InletIntErr");
            break;
         case PLASMA_PUMP_INTEGRATION_ERROR: // 26
            _mylong = new fastlongReceive(src, "/d/PlasmaIntErr");
            break;
         case PLATELET_PUMP_INTEGRATION_ERROR: // 27
            _mylong = new fastlongReceive(src, "/d/PlateletIntErr");
            break;
         case RETURN_PUMP_INTEGRATION_ERROR:    // 28
            _mylong = new fastlongReceive(src, "/d/ReturnIntErr");
            break;
         case AC_PUMP_VOLUME_ERROR:             // 29
            _mylong = new fastlongReceive(src, "/d/ACVolErr");
            break;
         case INLET_PUMP_VOLUME_ERROR:          // 30
            _mylong = new fastlongReceive(src, "/d/InletVolErr");
            break;
         case PLASMA_PUMP_VOLUME_ERROR:         // 31
            _mylong = new fastlongReceive(src, "/d/PlasmaVolErr");
            break;
         case PLATELET_PUMP_VOLUME_ERROR:       // 32
            _mylong = new fastlongReceive(src, "/d/PlateletVolErr");
            break;
         case RETURN_PUMP_VOLUME_ERROR:         // 33
            _mylong = new fastlongReceive(src, "/d/ReturnVolErr");
            break;
         case AC_PUMP_DELTA_SPEED:              // 34
            _mylong = new fastlongReceive(src, "/d/ACDeltaSpd");
            break;
         case INLET_PUMP_DELTA_SPEED:           // 35
            _mylong = new fastlongReceive(src, "/d/InletDeltaSpd");
            break;
         case PLASMA_PUMP_DELTA_SPEED:          // 36
            _mylong = new fastlongReceive(src, "/d/PlasmaDeltaSpd");
            break;
         case PLATELET_PUMP_DELTA_SPEED:        // 37
            _mylong = new fastlongReceive(src, "/d/PlateletDeltaSpd");
            break;
         case RETURN_PUMP_DELTA_SPEED:          // 38
            _mylong = new fastlongReceive(src, "/d/ReturnDeltaSpd");
            break;
         case AC_PUMP_ERRORS:                   // 39
            _mylong = new fastlongReceive(src, "/d/ACErrors");
            break;
         case INLET_PUMP_ERRORS:                // 40
            _mylong = new fastlongReceive(src, "/d/InletErrors");
            break;
         case PLASMA_PUMP_ERRORS:               // 41
            _mylong = new fastlongReceive(src, "/d/PlasmaErrors");
            break;
         case PLATELET_PUMP_ERRORS:             // 42
            _mylong = new fastlongReceive(src, "/d/PlateletErrors");
            break;
         case RETURN_PUMP_ERRORS:               // 43
            _mylong = new fastlongReceive(src, "/d/ReturnErrors");
            break;
         case AC_PUMP_HALL_TIME:                // 44
            _mylong = new fastlongReceive(src, "/d/ACHallTime");
            break;
         case INLET_PUMP_HALL_TIME:             // 45
            _mylong = new fastlongReceive(src, "/d/InletHallTime");
            break;
         case PLASMA_PUMP_HALL_TIME:            // 46
            _mylong = new fastlongReceive(src, "/d/PlasmaHallTime");
            break;
         case PLATELET_PUMP_HALL_TIME:          // 47
            _mylong = new fastlongReceive(src, "/d/PlateletHallTime");
            break;
         case RETURN_PUMP_HALL_TIME:            // 48
            _mylong = new fastlongReceive(src, "/d/ReturnHallTime");
            break;
         case AC_PUMP_SPEED_ORDER:                // 49
            _mylong = new fastlongReceive(src, "/d/ACSpdOrder");
            break;
         case INLET_PUMP_SPEED_ORDER:             // 50
            _mylong = new fastlongReceive(src, "/d/InletSpdOrder");
            break;
         case PLASMA_PUMP_SPEED_ORDER:            // 51
            _mylong = new fastlongReceive(src, "/d/PlasmaSpdOrder");
            break;
         case PLATELET_PUMP_SPEED_ORDER:          // 52
            _mylong = new fastlongReceive(src, "/d/PlateletSpdOrder");
            break;
         case RETURN_PUMP_SPEED_ORDER:            // 53
            _mylong = new fastlongReceive(src, "/d/ReturnSpdOrder");
            break;
         default:
            _LOG_ERROR( __FILE__, __LINE__, TRACE_CTL_DRV, src, "Unrecognized fastDataControl.");
            break;
         }
      };
      ~fastdataControl()
      {
         if (_mylong != NULL)
            delete _mylong;
         if (_myaps != NULL)
            delete _myaps;
         if (_myus != NULL)
            delete _myus;
         _mylong = NULL;
         _myaps = NULL;
         _myus = NULL;
      };
   private:
      fastlongReceive*  _mylong;
      fastapsReceive*   _myaps;
      fastusReceive*    _myus;
};





// SPECIFICATION:    main entry point
//                   parameter 1 = local queue name
//                   2 = remote node number
//                   3 = remote queue name
//
// ERROR HANDLING:   none.

void fastdatacontrolled_main(char *args)
{

   MessageSystem ms;
   ms.initBlocking();

   int source;

   vector<char *> arguments = parse(args);
   vector<fastdataControl *> FDCVec;

   if (arguments.size() > 3)
   {
      int end = arguments.size();
      for (int i=3; i<end; i++)
      {
         source  = atoi(arguments[i]);
         FDCVec.push_back(new fastdataControl((DATA_SOURCE)source));
      }
   }
   if(FDCVec.size() > 0)
   {
	   vector<fastdataControl *>::const_iterator iter;
	   for(iter = FDCVec.begin(); iter != FDCVec.end(); iter++)
		   delete iter;
   }
   ms.dispatchMessages();

}

vector<char *> parse(char* args)
{
	char* token = NULL;
	vector<char *> arguments;

	token = strtok(args, " ");
	arguments.push_back(token);
    while(token != NULL)
	{
		token = strtok(NULL, " ");
		if (token != NULL)
			arguments.push_back(token);

    } 
	return arguments;
}

