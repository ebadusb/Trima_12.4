// FILENAME: myspoofs.hpp
// $Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/spoof/rcs/myspoofs.hpp 6.102 2009/06/22 22:21:50Z jsylusb Exp dslausb $
// PURPOSE: defines my spoofs 
// CHANGELOG:
//   $Log: myspoofs.hpp $
//   Revision 6.102  2009/06/22 22:21:50Z  jsylusb
//   Added new spoof for testing RequestEndRunStatsMsg
//   Revision 6.101  2009/06/17 16:20:47Z  jsylusb
//   Added a new spoof for testing reservoir status.
//   Revision 6.100  2009/02/20 15:46:10Z  kgreusb
//   added CPumpAccum for testing pump code changes
//   Revision 6.99  2009/02/03 19:05:47Z  kgreusb
//   Adding spoof for testing pump ITs 6514, 7720, 8917 etc.
//   Revision 6.98  2009/01/10 00:32:33Z  kgreusb
//   Adding SCentifugeError parameterized spoof for STS fault testing
//   Revision 6.97  2008/12/01 18:45:58Z  kgreusb
//   Added LOW_PROCVOL_YIELD spoof to test IT-6933
//   Revision 6.96  2008/11/13 18:08:48Z  jsylusb
//   Added implementation for loading custom spoofs dynamically. The custom spoof collection populates itself by searching a designated folder and loading the spoofs into memory. 
//   Revision 6.95  2008/11/05 12:57:51  kgreusb
//   Added spoofs for testing hemolysis and STS.
//   Revision 6.94  2008/08/15 22:15:23Z  kgreusb
//   Adding spoofs for testing Next Generation spoofer and STS faults
//   Revision 6.93  2008/06/12 19:09:05Z  kgreusb
//   removing near duplicate spoofs and adding parameterized spoofs
//   Revision 6.92  2008/05/13 14:57:01Z  kgreusb
//   Added Spoof_SafetyTestDoneMsg
//   Revision 6.91  2008/04/30 21:17:28Z  KGreUSB
//   Adding spoofs developed in 5.13 for saturation flag testing e.g., IT 8416
//   Revision 6.90  2007/10/30 21:33:34Z  kgreusb
//   Corrected acInfusion spoof, added acTrialMult spoof, and added substates to acMultiplier to enable AC infusion testing with PTF RBC.
//   Revision 6.89  2007/05/24 22:29:02Z  kgreusb
//   Adding generic Spoof_CPS
//   Revision 6.88  2007/02/28 20:22:44Z  kgreusb
//   Migrating 8.12.26 from 5.2 to 5.R 
//   Revision 6.87  2007/02/27 22:44:43Z  kgreusb
//   Revision 6.86  2007/02/23 22:55:22Z  kgreusb
//   Adding Spoof_Safety_APS for IT 7828
//   Revision 6.85  2006/11/09 18:05:53Z  kgreusb
//   Adding run_generic spoofs for ITs sucha as 7220 and 7334
//   Revision 6.84  2006/10/06 15:58:15Z  kgreusb
//   Added spoofs for testing donor disconnect ITs 7178 and 7179
//   Adding spoofs for donor disconnect testing
//   Adding spoof for 7.1.20 MSS 
//   Revision 6.83  2004/05/07 21:35:50Z  td07711
//   IT6966 - add spoof BIG_AC_PRIME
//   Revision 6.82  2004/04/12 23:19:17Z  td07711
//   IT6922 - add LOW_YIELD spoof
//   Revision 6.81  2004/03/17 18:14:10Z  td07711
//   IT6909 - added spoof 64V_750MSEC_GLITCH and 64V_1250MSEC_GLITCH
//   Revision 6.80  2004/01/22 20:22:22Z  td07711
//   IT6794 - add 7.12.5
//   Revision 6.79  2004/01/20 00:41:53Z  td07711
//   IT6794 - added 7.12.1
//   Revision 6.78  2004/01/06 21:32:24Z  td07711
//   IT3338 - added 4.6.1 and 4.6.2
//   Revision 6.77  2003/12/30 22:08:09Z  td07711
//   IT6641 - add ems autotest spoofs
//   Revision 6.76  2003/12/26 20:44:39Z  td07711
//   IT6728 - added spoof 8.7.4
//   Revision 6.75  2003/12/26 19:59:23Z  td07711
//   IT6682 - added spoof UNKNOWN_CASSETTE_POSITION
//   Revision 6.74  2003/12/16 22:48:21Z  td07711
//   IT6026 - add PLASMA_VALVE_OPEN spoof
//   Revision 6.73  2003/12/11 18:04:13Z  td07711
//   IT6679 - add 4.46.10
//   IT6681 - add 4.36.11
//   Revision 6.72  2003/12/05 18:56:06Z  td07711
//   IT6637 - added 7.5.4
//   Revision 6.71  2003/12/03 18:38:41Z  td07711
//   IT6633 - added spoof 4.36.8
//   Revision 6.70  2003/12/01 23:20:45Z  td07711
//   IT6633 - added CHAMBER_SATURATION_c4b
//   Revision 6.69  2003/11/21 19:55:28Z  td07711
//   IT6585 - add 4.9.7.15
//   Revision 6.68  2003/11/21 18:16:28Z  td07711
//   IT6574 - added 8.3.9 - 8.3.16
//   Revision 6.67  2003/11/18 00:18:45Z  td07711
//   IT6604 - added spoof 4.9.7.14
//   Revision 6.66  2003/11/11 18:47:58Z  td07711
//   IT6091 - added 7.11.19 and 7.11.20
//   Revision 6.65  2003/11/10 21:27:28Z  td07711
//   IT6487 - added spoof 8.7.13i
//   Revision 6.64  2003/10/29 20:17:42Z  td07711
//   IT6535 - added 7.9.1, 7.9.2, 7.9.3
//   Revision 6.63  2003/10/24 17:57:59Z  td07711
//   IT6091 - added spoofs 7.11.11 - 7.11.18
//   Revision 6.62  2003/10/23 18:26:22Z  td07711
//   IT3338 - add generic fastdata spoofs
//   Revision 6.61  2003/10/23 15:56:47Z  td07711
//   IT6514 - add spoof_TOGGLE_PUMPS_OFF
//   Revision 6.60  2003/10/16 17:48:05Z  td07711
//   IT5838 - added CHAMBER_SATURATION spoof
//   Revision 6.59  2003/10/15 21:16:15Z  td07711
//   IT6344 - add spoofs 8.7.13h, 8.7.13h
//   Revision 6.58  2003/10/15 20:35:40Z  td07711
//   IT6485 - add spoof 8.3.13, removed 24V_ON, 24V_OFF, 64V_ON, 64V_OFF
//   Revision 6.57  2003/10/08 17:49:17Z  td07711
//   IT6485 - added spoofs 24V_ON, 24V_OFF, 64V_ON, 64V_OFF
//   Revision 6.56  2003/10/06 17:25:27Z  td07711
//   IT6344 - add spoofs 8.7.13e, 8.7.13f
//   Revision 6.55  2003/09/26 15:57:00Z  td07711
//   IT6413 - add spoof 8.12.20
//   Revision 6.54  2003/09/09 18:03:44Z  td07711
//   IT5303 - removed 7.12.2
//   Revision 6.53  2003/09/05 21:00:16Z  td07711
//   IT6394 - add 7.11.2a,b
//   Revision 6.52  2003/09/02 20:25:26Z  td07711
//   IT6254 - added spoofs 8.9.8, 8.9.10a,b,c, and 8.10.1
//   Revision 6.51  2003/08/25 19:09:56Z  td07711
//   IT6381 - added spoof 4.21.21
//   Revision 6.50  2003/08/19 22:19:48Z  td07711
//   IT6370 - added RBC_CAL_FAILURE spoof
//   Revision 6.49  2003/08/18 18:32:02Z  td07711
//   IT6027 - added generic APS spoofs for autocal testing.
//   Revision 6.48  2003/08/14 19:54:54Z  td07711
//   IT6351 - added 8.10.10
//   Revision 6.47  2003/08/14 19:00:36Z  td07711
//   IT6355 - add spoofs 7.3.22a,b,c
//   Revision 6.46  2003/08/14 18:22:05Z  td07711
//   IT6364 - added spoofs 7.5.12,13
//   Revision 6.45  2003/08/13 18:27:04Z  td07711
//   IT6350 - added spoofs 8.10.5a,b,c
//   Revision 6.44  2003/08/12 21:28:53Z  td07711
//   IT6348 - added spoofs 8.9.1,2,4,5,6,7
//   Revision 6.43  2003/08/12 20:48:22Z  td07711
//   IT6349 - added spoof 8.9.3
//   Revision 6.42  2003/08/12 19:00:46Z  td07711
//   IT6346 - added spoof 4.23.5
//   Revision 6.41  2003/08/12 18:17:15Z  td07711
//   IT6341 - added spoof 4.21.14
//   Revision 6.40  2003/08/11 23:09:14Z  td07711
//   IT6337 - added 7.6.2
//   Revision 6.39  2003/08/11 19:58:54Z  td07711
//   IT6343 - added 4.22.3.1 and 4.22.3.2
//   Revision 6.38  2003/08/08 22:02:29Z  td07711
//   IT6254 - added spoofs 8.4.x
//   Revision 6.37  2003/08/08 19:01:35Z  td07711
//   IT6254 - add 8.5.5, 6, 7a, 7b
//   Revision 6.36  2003/08/07 22:23:52Z  td07711
//   IT6254 - add spoof 8.5.4, 8.8.1,2,3,4
//   Revision 6.35  2003/08/06 22:22:34Z  td07711
//   IT6254 - added spoof 8.7.5,6,12,13
//   Revision 6.34  2003/08/06 21:27:52Z  td07711
//   IT6290 - added spoofs 7.17.1,2,3,7 and 7.23.6
//   Revision 6.33  2003/08/05 22:28:46Z  td07711
//   IT6290 - add spoofs 7.7.2, 7.16.4, 7.16.6, 7.16.8, 7.16.10, 7.16.11, 7.16.13
//   Revision 6.32  2003/07/28 19:07:31Z  td07711
//   IT6263 - add spoof SPILLOVER_BLACK for spillover on black sets
//   Revision 6.31  2003/07/24 21:09:57Z  td07711
//   IT6254 - add spoofs 8.5.1 and 8.6.1
//   Revision 6.30  2003/07/24 20:32:50Z  td07711
//   IT6254 - add spoofs 8.3.2 - 8.3.8
//   Revision 6.29  2003/07/24 20:03:03Z  td07711
//   IT6252 - add spoof 4.9.7.12
//   Revision 6.28  2003/07/18 21:30:39Z  td07711
//   IT6027 - add spoof 4.29.13
//   Revision 6.27  2003/06/27 22:31:45Z  td07711
//   IT5836 - added spoofs 7.5.9, 7.5.11
//   Revision 6.26  2003/06/20 21:43:21Z  td07711
//   added 8.9.11, 8.9.13, 8.9.14, 8.9.16, 7.3.23, 7.3.24, 7.3.34, 7.3.36
//   Revision 6.25  2003/06/18 20:19:46Z  td07711
//   added Spoof_INVALID_SET_HIGH
//   removed Spoof_8_1_5
//   changed Spoof_INVALID_SET to Spoof_INVALID_SET_LOW
//   Revision 6.24  2003/05/29 18:43:07Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 6.23  2003/05/19 21:55:31Z  td07711
//   added spoofs
//   Revision 6.22  2003/05/15 17:22:28Z  td07711
//   added Spoof_FAST_RINSEBACK
//   Revision 6.21  2003/05/14 23:24:08Z  td07711
//   add spooofs
//   Revision 6.20  2003/05/14 22:29:10Z  td07711
//   add 4.10.9
//   Revision 6.19  2003/05/14 20:53:08Z  td07711
//   renamed 8.1.4 to 8.1.5
//   added spoof SPILLOVER and NO_SPILLOVER
//   Revision 6.18  2003/05/14 18:45:23Z  td07711
//   added 4.9.7.13
//   Revision 6.17  2003/05/13 19:01:21Z  td07711
//   add spoofs
//   Revision 6.16  2003/04/24 22:17:15Z  td07711
//   added spoofs
//   Revision 6.15  2003/04/22 18:48:20Z  td07711
//   add spoofs
//   Revision 6.14  2003/04/21 23:20:11Z  td07711
//   add spoofs 
//   Revision 6.13  2003/04/16 19:57:30Z  td07711
//   add spoofs
//   Revision 6.12  2003/04/14 23:27:58Z  td07711
//   add spoofs 7.19.1, 8.1.4, 8.1.30, 8.1.33, 8.1.34
//   Revision 6.11  2003/04/04 00:00:35Z  td07711
//   added spoofs
//   Revision 6.10  2003/04/02 00:11:39Z  td07711
//   add spoofs
//   Revision 6.9  2003/04/01 01:17:38Z  td07711
//   add spoofs
//   Revision 6.8  2003/03/18 02:09:36Z  td07711
//   add spoofs
//   Revision 6.7  2003/03/15 00:01:06Z  td07711
//   add some spoofs
//   Revision 6.6  2003/02/18 00:35:01Z  td07711
//   add some spoofs
//   Revision 6.5  2003/01/25 00:43:13Z  td07711
//   add spoofs
//   Revision 6.4  2003/01/23 23:43:41Z  td07711
//   add spoofs
//   Revision 6.3  2003/01/18 01:23:36Z  td07711
//   spoof mods
//   Revision 6.2  2002/10/30 22:56:48Z  td07711
//   add some functions for use by spoof
//   Revision 6.1  2002/10/24 21:15:47  td07711
//   add some spoofs
//   Revision 1.1  2002/10/23 21:53:04  td07711
//   Initial revision


#ifndef MySpoofs_HPP // prevents multiple inclusion
#define MySpoofs_HPP


#include <map>    // STL map container
#include <string>

#include "spoofs.hpp"
#include "fastdata.h"


class MySpoofs : public Spoofs
{
    // RESPONSIBILITIES:
    //   1. defines and manages my spoofs

    public:

        MySpoofs( SpooferAPI* pApi ); // creates all spoofs supported

        void init(); // adds all supported spoofs

	typedef enum
	{
	    SUPPLY_64V,
	    SUPPLY_24V,
	    SUPPLY_12V,
	    SUPPLY_MINUS12V,
	} ps_type_t;

        //
        // custom behaviors that individual spoofs can call
        //
	    static void enableDisableLoop( int enableMsec, int disableMsec );
        static void acPrime(); // simulates ac prime 
        static void acInfusion( float infusionRate ); // assumes donor TBV=3000 Hct=44
        static void inletMultiplier( float multiplier ); // runs inlet slow, fast, off
        static void acMultiplier( float multiplier ); // runs ac flow, fast, off
        static void returnMultiplier( float multiplier ); // runs return flow, fast, off
        static void plateletMultiplier( float multiplier ); // runs platelet flow, fast, off
        static void plasmaMultiplier( float multiplier ); // runs plasma flow, fast, off
        static int psAdc( ps_type_t type, float voltage ); // returns adc value for ps voltage

    private:

    // split up initialization to avoid stack overflow
	void addSection4(); // adds section 4 spoofs
	void addSection7a(); // adds section 7a spoofs
    void addSection7b(); // adds section 7b spoofs
    void addSection7c(); // adds section 7c spoofs
    void addSection7d();
	void addSection8a(); // adds section 8a spoofs
    void addSection8b(); // adds section 8b spoofs
    void addSection8c(); // adds section 8c spoofs
    void addParameterized(); // adds spoofs that accept parameters 
    void addSpecialCases(); //adds spoofs simulating special cases
    void addIntMessages(); // adds spoofs of messages
	void addGenerica(); // adds generic spoofs
    void addGenericb();
	void addCustomSpoofs(); // Load spoofs from folder
    void addITSpoofs(); // adds spoofs for ITs

    // void addAutoSplit(); // adds spoofs used for testing Autosplit RBCs

        MySpoofs(); // catch unauthorized use
        MySpoofs(const MySpoofs&); // catch unauthorized use 
        MySpoofs& operator=(const MySpoofs rhs); // catch unauthorized use
};



//
// declare all my spoofs, CLASS macro defined in spoofs.hpp
// CLASS_WITHPARAMS also defined in spoofs.hpp to be used for parameterized spoof
// expecting user supplied parameters at run-time.
//

//
// spoofs with test ID's
//
CLASS( Spoof_PauseAllPumps );
CLASS( Spoof_4_6_1 );
CLASS( Spoof_4_6_2 );
CLASS_WITHPARAMS( Spoof_SCassettePosition );
CLASS( Spoof_4_9_7_13 );
CLASS( Spoof_4_9_7_14 );
CLASS_WITHPARAMS( Spoof_CCassettePosition );
CLASS( Spoof_4_10_9 );
CLASS( Spoof_4_20_1 );
CLASS( Spoof_4_20_2 );
CLASS( Spoof_4_21_14 );
CLASS( Spoof_4_21_21 );
CLASS_WITHPARAMS( Spoof_CDoorLock );
CLASS( Spoof_4_29_13 );
CLASS( Spoof_4_36_8 );
CLASS( Spoof_4_36_10 );
CLASS( Spoof_4_36_11 );
CLASS_WITHPARAMS( Spoof_4_39_gen );
CLASS( Spoof_7_1_1 );
CLASS( Spoof_7_1_4 );
CLASS( Spoof_7_1_5 );
CLASS( Spoof_7_1_6a );
CLASS( Spoof_7_1_6b );
CLASS( Spoof_7_1_9 );
CLASS( Spoof_7_1_10 );
CLASS( Spoof_7_1_11 );
CLASS_WITHPARAMS( Spoof_COInletRPM );
CLASS_WITHPARAMS( Spoof_COReturnRPM );
CLASS( Spoof_7_1_13 );
CLASS( Spoof_7_1_14 );
CLASS_WITHPARAMS( Spoof_COReturn_AcRPM );
CLASS( Spoof_7_1_20 );
CLASS_WITHPARAMS( Spoof_SReservoir );
CLASS_WITHPARAMS( Spoof_CReservoir );
CLASS( Spoof_7_2_2 );
CLASS( Spoof_7_2_5a );
CLASS( Spoof_7_2_5b );
CLASS( Spoof_7_3_2 );
CLASS( Spoof_7_3_5 );
CLASS( Spoof_7_3_7 );
CLASS_WITHPARAMS( Spoof_COAcRPM );
CLASS_WITHPARAMS( Spoof_7_3_8 );
CLASS( Spoof_7_3_10 );
CLASS( Spoof_7_3_13 );
CLASS( Spoof_7_3_15 );
CLASS( Spoof_7_3_16 );
CLASS( Spoof_7_3_19 );
CLASS( Spoof_7_3_20 );
CLASS( Spoof_7_3_21 );
CLASS( Spoof_7_3_32 );
CLASS( Spoof_7_3_37 );
CLASS_WITHPARAMS( Spoof_7_3_38 );
CLASS_WITHPARAMS( Spoof_ContTotVol );
CLASS_WITHPARAMS( Spoof_ContDeltVol );
CLASS( Spoof_7_5_3 );
CLASS( Spoof_7_5_4 );
CLASS( Spoof_7_5_5 );
CLASS( Spoof_7_5_6 );
CLASS( Spoof_7_5_7 );
CLASS( Spoof_7_5_9 );
CLASS( Spoof_7_5_11 );
CLASS_WITHPARAMS( Spoof_SDoorLock );
CLASS( Spoof_7_5_12 );
CLASS( Spoof_7_6_1a );
CLASS( Spoof_7_6_1b );
CLASS( Spoof_7_6_1c );
CLASS( Spoof_7_6_1d );
CLASS( Spoof_7_6_2 );
CLASS( Spoof_7_7_1 );
CLASS( Spoof_7_7_2 );
CLASS( Spoof_7_9_2 );
CLASS( Spoof_7_9_3 );
CLASS( Spoof_7_10_2 );
CLASS( Spoof_7_10_4 );
CLASS( Spoof_7_11_2a );
CLASS( Spoof_7_11_2b );
CLASS( Spoof_7_11_11 );
CLASS( Spoof_7_11_12 );
CLASS( Spoof_7_11_13 );
CLASS( Spoof_7_11_14 );
CLASS( Spoof_7_11_15 );
CLASS( Spoof_7_11_16 );
CLASS( Spoof_7_11_17 );
CLASS( Spoof_7_11_18 );
CLASS( Spoof_7_11_19 );
CLASS( Spoof_7_11_20 );
CLASS( Spoof_7_12_1 );
CLASS( Spoof_7_12_5 );
CLASS( Spoof_7_14_2 );
CLASS( Spoof_7_14_3 );
CLASS( Spoof_7_14_4 );
CLASS( Spoof_7_14_5 );
CLASS( Spoof_7_14_7 );
CLASS( Spoof_7_14_8 );
CLASS( Spoof_7_14_9 );
CLASS( Spoof_7_14_10 );
CLASS( Spoof_7_14_11 );
CLASS( Spoof_7_14_13 );
CLASS( Spoof_7_16_2 );
CLASS( Spoof_7_16_4 );
CLASS( Spoof_7_16_6 );
CLASS( Spoof_7_16_8 );
CLASS( Spoof_7_16_10 );
CLASS( Spoof_7_16_11 );
CLASS( Spoof_7_16_12 );
CLASS( Spoof_7_16_13 );
CLASS( Spoof_7_17_1 );
CLASS( Spoof_7_17_3a );
CLASS( Spoof_7_17_3b );
CLASS_WITHPARAMS( Spoof_SValve );
CLASS( Spoof_7_17_4 );
CLASS( Spoof_7_17_5 );
CLASS( Spoof_7_17_6 );
CLASS( Spoof_7_17_7a );
CLASS( Spoof_7_17_7b );
CLASS( Spoof_7_18_1 );
CLASS( Spoof_7_18_2 );
CLASS( Spoof_7_19_1 );
CLASS( Spoof_7_19_2 );
CLASS( Spoof_7_23_6 );
CLASS_WITHPARAMS( Spoof_LeakDetector );
CLASS( Spoof_8_3_1 );
CLASS( Spoof_8_3_2 );
CLASS( Spoof_8_3_3 );
CLASS( Spoof_8_3_4 );
CLASS( Spoof_8_3_5 );
CLASS( Spoof_8_3_6 );
CLASS( Spoof_8_3_7 );
CLASS( Spoof_8_3_8 );
CLASS( Spoof_8_3_9 );
CLASS( Spoof_8_3_10 );
CLASS( Spoof_8_3_11 );
CLASS( Spoof_8_3_12 );
CLASS( Spoof_8_3_13 );
CLASS( Spoof_8_3_14 );
CLASS( Spoof_8_3_15 );
CLASS( Spoof_8_3_16 );
CLASS_WITHPARAMS( Spoof_DonorVitals );
CLASS( Spoof_8_4_3 );
CLASS( Spoof_8_4_4 );
CLASS( Spoof_8_4_5 );
CLASS( Spoof_8_4_6 );
CLASS( Spoof_8_4_7 );
CLASS( Spoof_8_4_8 );
CLASS( Spoof_8_4_9 );
CLASS( Spoof_8_4_9_1 );
CLASS( Spoof_8_4_9_2 );
CLASS( Spoof_8_4_9_3 );
CLASS( Spoof_8_4_10 );
CLASS( Spoof_8_4_11 );
CLASS( Spoof_8_5_1 );
CLASS( Spoof_8_5_5 );
CLASS( Spoof_8_5_6 );
CLASS( Spoof_8_5_7a );
CLASS( Spoof_8_5_7b );
CLASS( Spoof_8_6_11 );
CLASS( Spoof_8_7_4 );
CLASS( Spoof_8_7_8 );
CLASS( Spoof_8_7_12 );
CLASS( Spoof_8_7_13a );
CLASS( Spoof_8_7_13b );
CLASS( Spoof_8_7_13c );
CLASS( Spoof_8_7_13d );
CLASS( Spoof_8_7_13e );
CLASS( Spoof_8_7_13g );
CLASS( Spoof_8_8_1 );
CLASS_WITHPARAMS( Spoof_CValve );
CLASS( Spoof_8_9_1 );
CLASS( Spoof_8_9_2 );
CLASS( Spoof_8_9_3 );
CLASS( Spoof_8_9_4 );
CLASS( Spoof_8_9_5 );
CLASS( Spoof_8_9_6 );
CLASS( Spoof_8_9_7 );
CLASS( Spoof_8_9_11 );
CLASS( Spoof_8_9_12 );
CLASS( Spoof_8_9_13 );
CLASS( Spoof_8_9_14 );
CLASS( Spoof_8_9_15 );
CLASS( Spoof_8_10_5a );
CLASS( Spoof_8_10_5b );
CLASS( Spoof_8_10_5c );
CLASS( Spoof_8_10_13 );
CLASS( Spoof_8_12_20 );

CLASS( Spoof_IT10221);


CLASS_WITHPARAMS( Spoof_8_12_26 );


//
// utility spoofs
//
CLASS( Spoof_3L_TBV ); 
CLASS( Spoof_enable ); // enables spoofing with timeout
CLASS( Spoof_BLACK_SET ); // black stamp set ID 
CLASS( Spoof_WHITE_SET ); // white stamp set ID 
CLASS( Spoof_INVALID_SET_LOW ); 
CLASS( Spoof_INVALID_SET_HIGH ); 
CLASS( Spoof_NO_SPILLOVER );
CLASS( Spoof_SPILLOVER );
CLASS( Spoof_SPILLOVER_BLACK );
CLASS( Spoof_12V_LOW );
CLASS( Spoof_FAST_RINSEBACK );
CLASS( Spoof_FATAL_ERROR_ON_SAFETY );
CLASS( Spoof_FATAL_ERROR_ON_CONTROL );
CLASS_WITHPARAMS( Spoof_CAccessPressure );
CLASS_WITHPARAMS( Spoof_COValve );
CLASS_WITHPARAMS( Spoof_COPwrSplyEnabled );
CLASS( Spoof_RBC_CAL_FAILURE );
CLASS( Spoof_CrAv_a );
CLASS( Spoof_CrAv_b );
CLASS( Spoof_CHAMBER_SATURATION );
CLASS( Spoof_CHAMBER_SATURATION_c4b );
CLASS( Spoof_TOGGLE_PUMPS_OFF );
CLASS( Spoof_FASTDATA_ZEROED );
CLASS( Spoof_FASTDATA_DIV10 );
CLASS( Spoof_FASTDATA_DIV100 );
CLASS( Spoof_64V_750MSEC_GLITCH );
CLASS( Spoof_64V_1250MSEC_GLITCH );
CLASS( Spoof_LOW_YIELD );
CLASS( Spoof_LOW_PROCVOL_YIELD );  // for IT 6933
CLASS( Spoof_BIG_AC_PRIME );
CLASS( Spoof_SAFETY_DISCONNECT_OK );
CLASS( Spoof_SAFETY_DISCONNECT_FAIL );
CLASS( Spoof_FORCE_EPCA );
CLASS( Spoof_Skip_Midrun );

CLASS( Spoof_PLO_Short_Midrun );
CLASS_WITHPARAMS( Spoof_PLO_EPCA );
CLASS( Spoof_PLO_Poison_Midrun );
CLASS_WITHPARAMS(Spoof_PLO_EPCA_1);
CLASS_WITHPARAMS(Spoof_PLO_EPCA_26);

CLASS_WITHPARAMS( Spoof_RED_GREEN );
CLASS ( Spoof_APS_1 );
CLASS_WITHPARAMS( Spoof_Safety_APS );
CLASS_WITHPARAMS( Spoof_CPS );
CLASS ( Spoof_CPS_1 );
CLASS_WITHPARAMS( Spoof_CPS_mmHg );
CLASS_WITHPARAMS( Spoof_APS_mmHg );
CLASS_WITHPARAMS( Valve_Controlled_Wrong );
CLASS_WITHPARAMS( Valve_Observed_Wrong );
/*  for autoSplit
CLASS_WITHPARAMS( RBC_C4R );
CLASS_WITHPARAMS( RBC_R4C );
CLASS_WITHPARAMS( RBC_O4R );
CLASS_WITHPARAMS( PLT_O4R );
*/
CLASS_WITHPARAMS( Spoof_MultiplyAc );
// adding to test changes to chamber saturation (IT 8149, 8415)
CLASS (Spoof_RGMIDRUN_TRIGGER);
CLASS (Spoof_RGPURGE_TRIGGER);
CLASS (Spoof_RGEVENT_TRIGGER);
CLASS (Spoof_c4b_TRIGGER);
CLASS (Spoof_c3EVENT_TRIGGER);
CLASS (Spoof_RequestRunsumStats);

//adding to test version 5.13 MidRun Baseline calculation
CLASS_WITHPARAMS( Spoof_Baselines );
//adding to test IT 5802 logging
CLASS_WITHPARAMS( Spoof_SafetyTestDoneMsg );
//adding to test Next Generation 
CLASS_WITHPARAMS( Spoof_TwoBaselines ); // substate awareness
CLASS_WITHPARAMS( Spoof_PumpErrors );
CLASS_WITHPARAMS( Spoof_CCentrifugeRPM );
CLASS_WITHPARAMS( Spoof_SCentrifugeRPM );

// Next Generation spoofs
CLASS_WITHPARAMS( SpoofAirToDonor);
CLASS_WITHPARAMS( Spoof_HWFault );
CLASS_WITHPARAMS( Spoof_CCentrifugeError );
CLASS_WITHPARAMS( Spoof_SCentrifugeError );

//adding to test hemolyis algorithm IT-7059
CLASS_WITHPARAMS( Spoof_HEMOLYSIS );
CLASS_WITHPARAMS( Spoof_HEMOLYSIS_SLOW );

//adding to test new pump code for Its 6514, 7720 and 8917
CLASS_WITHPARAMS( Spoof_SPumpRevs );
CLASS_WITHPARAMS( Spoof_CPumpRPM );
CLASS_WITHPARAMS( Spoof_ControlACPumpAccum );
CLASS_WITHPARAMS( Spoof_ControlInletPumpAccum );
CLASS_WITHPARAMS( Spoof_ControlPlasmaPumpAccum );
CLASS_WITHPARAMS( Spoof_ControlPlateletPumpAccum );
CLASS_WITHPARAMS( Spoof_ControlReturnPumpAccum );
CLASS_WITHPARAMS( Spoof_SafetyACPumpAccum );
CLASS_WITHPARAMS( Spoof_SafetyInletPumpAccum );
CLASS_WITHPARAMS( Spoof_SafetyPlasmaPumpAccum );
CLASS_WITHPARAMS( Spoof_SafetyPlateletPumpAccum );
CLASS_WITHPARAMS( Spoof_SafetyReturnPumpAccum );
CLASS_WITHPARAMS( Spoof_BothACPumpAccum );
CLASS_WITHPARAMS( Spoof_BothInletPumpAccum );
CLASS_WITHPARAMS( Spoof_BothPlasmaPumpAccum );
CLASS_WITHPARAMS( Spoof_BothPlateletPumpAccum );
CLASS_WITHPARAMS( Spoof_BothReturnPumpAccum );

//CLASS_WITHPARAMS( Spoof_COPlasma_PCAMIDRUN ); //no longer useful

CLASS_WITHPARAMS( Spoof_ReservoirStatus );
CLASS_WITHPARAMS( Spoof_ReservoirHct );
CLASS_WITHPARAMS( Spoof_PLO_PCA );
CLASS( Spoof_SealSafeOvertemp );
CLASS( Spoof_CassetteID );
CLASS_WITHPARAMS( Spoof_CPumpMult );
CLASS_WITHPARAMS( Spoof_ProcReturnRPM );
CLASS( Spoof_S_ATD_1_0001 );
CLASS( Spoof_S_ATD_1_0001h );
CLASS( Spoof_C_DAE );
CLASS( Spoof_C_RB_1_007a );
CLASS_WITHPARAMS( Spoof_EnableDisable );
CLASS_WITHPARAMS( PFR_Index );
CLASS_WITHPARAMS( Spoof_CPumpCmdMultLoop );
CLASS( STMBucket );
CLASS_WITHPARAMS( Spoof_CPumpRevs );
CLASS( Spoof_AvgACRatioTooHigh );
CLASS( Spoof_AvgACRatioTooLow );
CLASS_WITHPARAMS( Spoof_COrdersPumpCmdLoop );
CLASS( Spoof_InstACRatioTooHigh );
CLASS( Spoof_InstACRatioTooLow );
CLASS_WITHPARAMS( Spoof_RG_reflect );
CLASS( Spoof_IT12551_1 );
CLASS( Spoof_IT12551_2 );
CLASS_WITHPARAMS(SubstateAlert);
#endif // MySpoofs_HPP
