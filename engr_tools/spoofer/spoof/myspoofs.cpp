// FILENAME: myspoofs.cpp
// $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/spoof/rcs/myspoofs.cpp 6.194 2009/07/14 22:02:03Z dslausb Exp $
// PURPOSE: implements MySpoofs
//  - defines what spoofs are supported and implements each spoofs run function
// CHANGELOG:
//   $Log: myspoofs.cpp $
//   Revision 6.194  2009/07/14 22:02:03Z  dslausb
//   Add power supply enabled
//   Revision 6.193  2009/06/22 22:21:39Z  jsylusb
//   Added new spoof for testing RequestEndRunStatsMsg
//   Revision 6.192  2009/06/17 16:20:38Z  jsylusb
//   Added a new spoof for testing reservoir status.
//   Revision 6.191  2009/02/20 15:46:50Z  kgreusb
//   Added CPumpAccum for testing pump code changes
//   Revision 6.190  2009/02/03 19:06:57Z  kgreusb
//   Adding spoofs for testing pump ITs such as 6514, 7720, and 8917.
//   Revision 6.189  2009/01/10 00:48:35Z  kgreusb
//   Added SCentrifugeError for STS fault testing
//   Revision 6.188  2008/12/22 17:24:54Z  kgreusb
//   Change to PumpErrors spoof for IT 8937
//   Revision 6.187  2008/12/01 18:46:35Z  kgreusb
//   Added LOW_PROCVOL_YIELD spoof to test IT-6933
//   Revision 6.186  2008/11/13 18:08:34Z  jsylusb
//   Added implementation for loading custom spoofs dynamically. The custom spoof collection populates itself by searching a designated folder and loading the spoofs into memory. 
//   Revision 6.185  2008/11/05 12:58:30  kgreusb
//   Added spoofs for testing hemolysis and STS faults.
//   Revision 6.184  2008/08/15 22:16:05Z  kgreusb
//   Adding spoofs to test Next Generation spoofer and STS
//   Revision 6.183  2008/06/12 19:09:25Z  kgreusb
//   removing near duplicate spoofs and adding parameterized spoofs
//   Revision 6.182  2008/05/13 14:57:19Z  kgreusb
//   Added Spoof_SafetyTestDoneMsg
//   Revision 6.181  2008/04/30 21:17:10Z  KGreUSB
//   Adding spoofs developed in 5.13 for saturation flag testing e.g., IT 8416
//   Revision 6.180  2007/10/30 21:35:20Z  kgreusb
//   Fixed acInfusion, added acTrialMult spoofs and added substates to  acMultiplier to enable AC infusion testing for PTF RBC collections.
//   Revision 6.179  2007/05/24 22:30:08Z  kgreusb
//   Added generic Spoof_CPS for F 7.37.xx tests
//   Revision 6.178  2007/02/28 20:23:30Z  kgreusb
//   Revision 6.177  2007/02/23 22:55:50Z  kgreusb
//   Added Spoof_Safety_APS for IT 7828
//   Revision 6.176  2006/11/09 18:06:35Z  kgreusb
//   Adding paramterized run_generic spoofs for ITs such as 7220 and 7334
//   Revision 6.175  2006/10/06 15:58:53Z  kgreusb
//   Added spoofs for tsting donor disconnect ITs 7178 and 7179
//   Adding spoofs, DonorDisconnect, SAFETY_DISCONNECT_OK, and SAFETY_DISCONNECT_FAIL for ITs 7179, 7692
//   Revision 6.174  2005/05/31 16:22:00Z  rgx2112
//   IT7154: Modified spoof AT11.6.a
//   Revision 6.173  2004/05/07 21:35:40Z  td07711
//   IT6966 - add spoof BIG_AC_PRIME
//   Revision 6.172  2004/04/15 18:48:41Z  td07711
//   IT6922 - change LOW_YIELD spoof to spoof SelectedPlateletYield instead of PlateletYieldTarget
//   Revision 6.171  2004/04/12 23:19:01Z  td07711
//   IT6922 - add LOW_YIELD spoof
//   Revision 6.170  2004/03/17 18:13:35Z  td07711
//   IT6909 - added spoof 64V_750MSEC_GLITCH and 64V_1250MSEC_GLITCH
//   Revision 6.169  2004/02/25 17:14:06Z  td07711
//   IT6886 - fixed 8.7.13i
//   Revision 6.168  2004/01/22 20:22:13Z  td07711
//   IT6794 - add 7.12.5
//   Revision 6.167  2004/01/22 18:57:58Z  td07711
//   IT6803 - fix 4.36.8
//   Revision 6.166  2004/01/20 00:41:42Z  td07711
//   IT6794 - added 7.12.1
//   Revision 6.165  2004/01/16 22:38:18Z  td07711
//   IT6641 - update for AT11.1.c,d,e
//   Revision 6.164  2004/01/09 21:20:12Z  td07711
//   IT6350 - fix runtime comment on 8.10.5c
//   Revision 6.163  2004/01/09 21:16:46Z  td07711
//   IT6350 - fix 8.10.5.c to run longer
//   Revision 6.162  2004/01/09 19:54:17Z  td07711
//   IT6776 - update spoof AT11.1.b
//   Revision 6.161  2004/01/08 17:00:28Z  td07711
//   IT6641 - added "spoof started" msg for operator
//   Revision 6.160  2004/01/06 21:32:16Z  td07711
//   IT3338 - added 4.6.1 and 4.6.2
//   Revision 6.159  2004/01/02 20:38:41Z  td07711
//   IT6710 - change 8.9.4 to run return pump at 2 RPM
//   Revision 6.158  2004/01/02 20:22:31Z  td07711
//   IT6091 - added 7.11.19 and 7.11.20 to spoof table
//   Revision 6.157  2004/01/02 20:04:27Z  td07711
//   IT6641 - add ems autotest spoofs
//   Revision 6.156  2003/12/30 22:56:13Z  td07711
//   IT6641 - filled in more EMS autotest spoofs
//   Revision 6.155  2003/12/30 22:08:01Z  td07711
//   IT6641 - add ems autotest spoofs
//   Revision 6.154  2003/12/29 22:52:59Z  td07711
//   IT6681 - fix 4.36.11
//   Revision 6.153  2003/12/26 20:44:29Z  td07711
//   IT6728 - added spoof 8.7.4
//   Revision 6.152  2003/12/26 19:59:00Z  td07711
//   IT6682 - added spoof UNKNOWN_CASSETTE_POSITION
//   Revision 6.151  2003/12/26 19:10:39Z  td07711
//   IT6714 - fix 4.36.8
//   Revision 6.150  2003/12/19 23:23:09Z  td07711
//   IT6714 - fixed 4.36.8
//   Revision 6.149  2003/12/18 23:01:51Z  td07711
//   IT6487 - fixed 8.7.13i
//   Revision 6.148  2003/12/18 22:41:15Z  td07711
//   IT6679 - added 4.36.10 to the spoof table.
//   Revision 6.147  2003/12/17 18:10:05Z  td07711
//   IT6681 - really add spoof 4.36.11
//   Revision 6.146  2003/12/16 22:48:13Z  td07711
//   IT6026 - add PLASMA_VALVE_OPEN spoof
//   Revision 6.145  2003/12/16 20:43:08Z  td07711
//   IT6637 - fix help text in 7.5.4
//   Revision 6.144  2003/12/12 19:01:14Z  td07711
//   IT6688 - update 7.17.4 for 5.1
//   Revision 6.143  2003/12/11 18:04:24Z  td07711
//   IT6679 - add 4.46.10
//   IT6681 - add 4.36.11
//   Revision 6.142  2003/12/10 18:06:50Z  td07711
//   IT6676 - fix for 4.36.8
//   Revision 6.141  2003/12/09 21:36:15Z  td07711
//   IT6670 - changed 7.5.7 back to 5 seconds
//   Revision 6.140  2003/12/09 21:15:52Z  td07711
//   IT6670 - extended 7.5.7 and 7.5.8 to 10 seconds.
//   Revision 6.139  2003/12/05 18:56:58Z  td07711
//   IT6658 - modified 7.5.3, 7.5.5, 7.5.6, 7.5.9, 7.5.12
//   Revision 6.138  2003/12/03 18:38:21Z  td07711
//   IT6633 - added spoof 4.36.8
//   Revision 6.137  2003/12/02 23:40:15Z  td07711
//   IT6653 - fix 8.9.14
//   Revision 6.136  2003/12/02 23:06:47Z  td07711
//   IT6604 - fix for 4.9.7.14
//   Revision 6.135  2003/12/01 23:20:36Z  td07711
//   IT6633 - added CHAMBER_SATURATION_c4b
//   Revision 6.134  2003/11/26 23:24:43Z  td07711
//   IT6574 - fix 8.3.9
//   Revision 6.133  2003/11/26 22:35:33Z  td07711
//   IT6637 - fix for 7.5.3
//   Revision 6.132  2003/11/26 22:20:07Z  td07711
//   IT6637 - fix 7.5.5, 7.5.6, 7.5.9, 7.5.12 for new behavior
//   Revision 6.131  2003/11/21 19:55:18Z  td07711
//   IT6585 - add 4.9.7.15
//   Revision 6.130  2003/11/21 18:16:18Z  td07711
//   IT6574 - added 8.3.9 - 8.3.16
//   Revision 6.129  2003/11/19 23:29:50Z  td07711
//   IT6485 - changed Spoof 8.10.13
//   Revision 6.128  2003/11/18 00:18:36Z  td07711
//   IT6604 - added spoof 4.9.7.14
//   Revision 6.127  2003/11/11 18:48:10Z  td07711
//   IT6091 - added 7.11.19 and 7.11.20
//   Revision 6.126  2003/11/10 21:27:40Z  td07711
//   IT6487 - added spoof 8.7.13i
//   Revision 6.125  2003/10/29 20:17:33Z  td07711
//   IT6535 - added 7.9.1, 7.9.2, 7.9.3
//   Revision 6.124  2003/10/28 19:43:54Z  td07711
//   IT6350 - changed 8.10.5a, b, c to spoof check that out of range green does not alarm and out of range red does alarm.
//   Revision 6.123  2003/10/24 17:58:06Z  td07711
//   IT6091 - added spoofs 7.11.11 - 7.11.18
//   Revision 6.122  2003/10/23 18:26:32Z  td07711
//   IT3338 - add generic fastdata spoofs
//   Revision 6.121  2003/10/23 15:57:03Z  td07711
//   IT6514 - add spoof_TOGGLE_PUMPS_OFF
//   Revision 6.120  2003/10/17 17:36:08Z  td07711
//   IT6426 - fixed 8.4.3 to send  WEIGHT_UNITS_UNKNOWN (-1)
//   IT6428 - fixed 8.4.4 to send invalid HEIGHT_UNITS
//   Revision 6.119  2003/10/17 15:58:49Z  td07711
//   IT7527 - 7.16.13 fixed (wasn't completely added)
//   Revision 6.118  2003/10/16 20:31:34Z  td07711
//   IT6524 - fix 8.9.13 and 8.9.14
//   Revision 6.117  2003/10/16 17:48:18Z  td07711
//   IT5838 - added CHAMBER_SATURATION spoof
//   Revision 6.116  2003/10/15 21:17:11Z  td07711
//   IT6344 - add spoofs 8.7.13g, 8.7.13h
//   Revision 6.115  2003/10/15 20:36:40Z  td07711
//   IT6485 - add spoof 8.3.13, removed 24V_ON, 24V_OFF, 64V_ON, 64V_OFF
//   Revision 6.114  2003/10/08 17:49:29Z  td07711
//   IT6485 - added spoofs 24V_ON, 24V_OFF, 64V_ON, 64V_OFF
//   Revision 6.113  2003/10/07 18:19:21Z  td07711
//   IT5443 - fix typo in 4.10.9
//   Revision 6.112  2003/10/06 17:25:52Z  td07711
//   IT6344 - fix spoofs 8.7.13a-8.7.13f
//   Revision 6.111  2003/10/03 20:43:21Z  td07711
//   IT5443 - help text mod to 4.10.9 to start spoof before RBC collect
//   Revision 6.110  2003/10/02 23:00:59Z  td07711
//   IT6413 - fix 8.12.20 wrong substate for EPCA
//   Revision 6.109  2003/09/29 20:38:18Z  td07711
//   IT5984 - rewrote 7.1.5 to avoid ac alarm
//   Revision 6.108  2003/09/26 15:57:11Z  td07711
//   IT6413 - add spoof 8.12.20
//   Revision 6.107  2003/09/24 21:16:47Z  td07711
//   IT6347 - fixed 8.5.5, 6, 7 help text for 12L donor.
//   Revision 6.106  2003/09/22 22:20:19Z  td07711
//   IT6347 - spoof predict flows and valve position for 8.5.5, 8.5.6, 8.5.7a, 8.5.7b
//   Revision 6.105  2003/09/09 18:03:36Z  td07711
//   IT5303 - removed 7.12.2
//   Revision 6.104  2003/09/05 21:46:20Z  td07711
//   IT6078 - fix 7.3.32 to return ~35ml
//   Revision 6.103  2003/09/05 21:00:07Z  td07711
//   IT6394 - add 7.11.2a,b
//   Revision 6.102  2003/09/04 17:12:14Z  td07711
//   IT6340 - extending 8.7.12 to 10 minutes
//   Revision 6.101  2003/09/04 16:07:18Z  td07711
//   IT6082 - fix 7.19.2
//   Revision 6.100  2003/09/03 18:27:58Z  td07711
//   IT6347 - update text on spoofs 8.5.5, 8.5.6, 8.5.7a,b
//   Revision 6.99  2003/09/03 18:10:45Z  td07711
//   IT6347 - fix 8.5.5, 8.5.6, 8.5.7a,b
//   Revision 6.98  2003/09/02 20:25:37Z  td07711
//   IT6254 - added spoofs 8.9.8, 8.9.10a,b,c, and 8.10.1
//   Revision 6.97  2003/08/29 15:39:50Z  td07711
//   IT6381 - modified 4.21.21
//   Revision 6.96  2003/08/28 20:47:50Z  td07711
//   IT6355 - 7.3.22b changed overinfusion from 6% to 5%
//   Revision 6.95  2003/08/28 20:17:40Z  td07711
//   IT6263 - extending SPILLOVER_BLACK to 105 seconds
//   Revision 6.94  2003/08/26 22:13:09Z  td07711
//   IT6082 - added other collection states to 7.19.2
//   Revision 6.93  2003/08/26 22:00:14Z  td07711
//   IT6347 - fixed 8.5.5 to also overcollect platelets in PCA, PIR, EPCA
//   Revision 6.92  2003/08/26 20:30:04Z  td07711
//   IT6355 - change 7.3.22b for 6% overinfusion instead of 7%.
//   Revision 6.91  2003/08/26 18:15:30Z  td07711
//   IT6263 - fixed spoof SPILLOVER_BLACK to run 65 seconds.
//   Also enabled spoof 7.16.11.
//   Revision 6.90  2003/08/25 19:10:07Z  td07711
//   IT6381 - added spoof 4.21.21
//   Revision 6.89  2003/08/22 19:00:39Z  td07711
//   IT6082 - modified 7.19.2 for 5.0 version of plt depletion, will need to change once IT6099 resolves.
//   Revision 6.88  2003/08/22 18:56:03Z  td07711
//   IT6082 - fixed 7.19.2
//   Revision 6.87  2003/08/22 18:18:45Z  td07711
//   IT5754 - 8.4.13 modified for higher Pct limit
//   Revision 6.86  2003/08/20 16:49:49Z  td07711
//   IT6355 - faster infusion for 7.3.22b,c
//   Revision 6.85  2003/08/19 22:19:59Z  td07711
//   IT6370 - added RBC_CAL_FAILURE spoof
//   Revision 6.84  2003/08/19 20:56:27Z  td07711
//   IT6364 - fixed 7.5.13
//   Revision 6.83  2003/08/18 19:35:42Z  td07711
//   IT6355 - fixed 7.3.22a to run ac a little faster.
//   Revision 6.82  2003/08/18 18:32:13Z  td07711
//   IT6027 - added generic APS spoofs for autocal testing.
//   Revision 6.81  2003/08/15 21:17:41Z  td07711
//   IT6347 - fix 8.5.5,6,7
//   Revision 6.80  2003/08/14 19:55:07Z  td07711
//   IT6351 - added 8.10.10
//   Revision 6.79  2003/08/14 19:00:49Z  td07711
//   IT6355 - add spoofs 7.3.22a,b,c
//   Revision 6.78  2003/08/14 18:22:15Z  td07711
//   IT6364 - added spoofs 7.5.12,13
//   Revision 6.77  2003/08/14 17:51:35Z  td07711
//   IT6361 - fixed spoofs 8.9.2,3,4,5,6,7
//   Revision 6.76  2003/08/14 17:19:31Z  td07711
//   IT6341 - fixed spoof 4.21.14
//   Revision 6.75  2003/08/13 18:27:44Z  td07711
//   IT6350 - added spoofs 8.10.5a,b,c
//   Revision 6.74  2003/08/12 21:29:10Z  td07711
//   IT6348 - added spoofs 8.9.1,2,4,5,6,7
//   Revision 6.73  2003/08/12 20:48:38Z  td07711
//   IT6349 - added spoof 8.9.3
//   Revision 6.72  2003/08/12 20:25:05Z  td07711
//   IT6339 - fixed 8.7.6
//   Revision 6.71  2003/08/12 19:01:06Z  td07711
//   IT6346 - added spoof 4.23.5
//   Revision 6.70  2003/08/12 18:17:39Z  td07711
//   IT6341 - added spoof 4.21.14
//   Revision 6.69  2003/08/11 23:09:25Z  td07711
//   IT6337 - added 7.6.2
//   Revision 6.68  2003/08/11 21:36:19Z  td07711
//   IT6199 - had to add() 7.1.9 for it to show up.
//   Revision 6.67  2003/08/11 19:59:12Z  td07711
//   IT6343 - added 4.22.3.1 and 4.22.3.2
//   Revision 6.66  2003/08/08 22:02:55Z  td07711
//   IT6254 - added spoofs 8.4.x
//   Revision 6.65  2003/08/08 19:01:47Z  td07711
//   IT6254 - add 8.5.5, 6, 7a, 7b
//   Revision 6.64  2003/08/07 22:24:04Z  td07711
//   IT6254 - add spoof 8.5.4, 8.8.1,2,3,4
//   Revision 6.63  2003/08/06 22:22:45Z  td07711
//   IT6254 - added spoof 8.7.5,6,12,13
//   Revision 6.62  2003/08/06 21:28:07Z  td07711
//   IT6290 - added spoofs 7.17.1,2,3,7 and 7.23.6
//   Revision 6.61  2003/08/05 22:28:56Z  td07711
//   IT6290 - add spoofs 7.7.2, 7.16.4, 7.16.6, 7.16.8, 7.16.10, 7.16.11, 7.16.13
//   Revision 6.60  2003/07/30 17:06:14Z  td07711
//   IT6200 - fix spoof 8.5.1
//   Revision 6.59  2003/07/28 19:07:51Z  td07711
//   IT6263 - add spoof SPILLOVER_BLACK for spillover on black sets
//   Revision 6.58  2003/07/24 21:10:08Z  td07711
//   IT6254 - add spoofs 8.5.1 and 8.6.1
//   Revision 6.57  2003/07/24 20:33:01Z  td07711
//   IT6254 - add spoofs 8.3.2 - 8.3.8
//   Revision 6.56  2003/07/24 20:03:17Z  td07711
//   IT6252 - add spoof 4.9.7.12
//   Revision 6.55  2003/07/22 18:00:53Z  td07711
//   IT6235 - run AC faster to trip 1ml limit during rinseback
//   Revision 6.54  2003/07/18 21:31:06Z  td07711
//   IT6027 - add spoof 4.29.13
//   Revision 6.53  2003/07/15 17:40:38Z  td07711
//   IT6195 - for FAST_RINSEBACK, have to spoof CHwOrder message.
//   Revision 6.52  2003/07/03 16:40:02Z  td07711
//   IT6160 - 7.3.21 - spoof AC at 5x for 2.2 seconds, changed to accomodate AC being off for first 3ml of draw.
//   Revision 6.51  2003/06/27 22:31:59Z  td07711
//   IT5836 - added spoofs 7.5.9, 7.5.11
//   Revision 6.50  2003/06/27 16:41:27Z  td07711
//   IT6164 - 7.5.7 - fixed to spoof control door status locked but not closed.
//   Revision 6.49  2003/06/26 19:03:26Z  td07711
//   IT6166 - 7.10.4 using "RequestSafetyPower" instead of "RequestSafetyPowerMsg"
//   Revision 6.48  2003/06/26 17:44:15Z  td07711
//   IT6165 - fix 7.5.2 to spoof only control side door status and to spoof it closed but not locked.
//   Revision 6.47  2003/06/25 19:54:48Z  td07711
//   7.3.21 fixed to reliably generate short draw cycles - IT6160
//   Revision 6.46  2003/06/20 21:43:36Z  td07711
//   added 8.9.11, 8.9.13, 8.9.14, 8.9.16, 7.3.23, 7.3.24, 7.3.34, 7.3.36
//   Revision 6.45  2003/06/18 20:20:00Z  td07711
//   added Spoof_INVALID_SET_HIGH
//   removed Spoof_8_1_5
//   changed Spoof_INVALID_SET to Spoof_INVALID_SET_LOW
//   Revision 6.44  2003/06/12 18:42:00Z  td07711
//   whoops, another fix for Spoof_4.10.9
//   Revision 6.43  2003/06/12 18:00:36Z  td07711
//   fixed Spoof_4.10.9 - IT5443 
//   Revision 6.42  2003/06/11 23:09:37Z  td07711
//   fixed  Spoof_7.18.1 per IT6129
//   Revision 6.41  2003/06/11 20:54:29Z  td07711
//   update  Spoof_7.19.2 - extended default time to 10000 per IT6082
//   Revision 6.40  2003/05/29 18:42:59Z  td07711
//   support FATAL_ERROR spoofs
//   Revision 6.39  2003/05/27 21:55:07Z  td07711
//   fix name of SPOOF_WHITE_SET and SPOOF_BLACK_SET
//   Revision 6.38  2003/05/20 22:31:46Z  td07711
//   7.23.1 should run 5min to insure tripping the expected alarm
//   Revision 6.37  2003/05/19 21:57:05Z  td07711
//   added 4.20.1, 4.22.1.1, 4.22.1.2, 4.22.2.1, 4.22.2.2, 7.19.2,
//   8.6.4, 8.6.11, 8.7.8
//   Revision 6.36  2003/05/15 17:23:24Z  td07711
//   added Spoof_FAST_RINSEBACK
//   Revision 6.35  2003/05/14 23:24:41Z  td07711
//   add generic spoofs for 12V_LOW, AC_RUNAWAY, AC_OFF
//   Revision 6.34  2003/05/14 22:29:31Z  td07711
//   add 4.10.9
//   Revision 6.33  2003/05/14 21:03:00Z  td07711
//   renamed 8.1.4 to 8.1.5
//   added SPILLOVER and NO_SPILLOVER spoofs
//   Revision 6.32  2003/05/14 18:46:08Z  td07711
//   added 4.9.7.13
//   Revision 6.31  2003/05/14 17:56:55Z  td07711
//   fix 7.18.2 - had to spoof stopSwitch as well as event type.
//   Revision 6.30  2003/05/13 18:46:42Z  td07711
//   added WHITE and BLACK set ID spoofs
//   Revision 6.29  2003/05/12 23:16:52Z  td07711
//   use Qrp to denote return flow for consistency with predict_cds.h
//   Revision 6.28  2003/04/29 21:22:47Z  td07711
//   7.6.1a change from 10 to 15sec
//   Revision 6.27  2003/04/24 22:18:14Z  td07711
//   add spoofs 7.14.7, 7.14.8, 7.14.9, 7.14.10, 7.4.11, 7.14.13, 7.16.12, 7.17.4, 7.17.6
//   Revision 6.26  2003/04/22 18:48:58Z  td07711
//   add spoofs 7.5.8, 7.10.2, 7.10.4, 7.14,4, 7.14.5
//   Revision 6.25  2003/04/21 23:21:14Z  td07711
//   add spoofs 7.3.21, 7.3.25, 7.3.37, 7.5.2, 7.5.7
//   Revision 6.24  2003/04/16 19:58:02Z  td07711
//   add 8.3.1, 8.4.5, 8.4.10, 8.6.7, 8.9.15
//   Revision 6.23  2003/04/14 23:28:31Z  td07711
//   add spoofs 7.19.1, 8.1.4, 8.1.30, 8.1.33, 8.1.34
//   Revision 6.22  2003/04/07 21:09:29Z  td07711
//   fixed spoof 7.6.1d 
//   Revision 6.21  2003/04/04 00:00:48Z  td07711
//   added spoofs
//   Revision 6.20  2003/04/02 00:11:50Z  td07711
//   add spoofs
//   Revision 6.19  2003/04/01 01:17:49Z  td07711
//   add spoofs
//   Revision 6.18  2003/03/28 23:06:37Z  td07711
//   fixed 7.3.2 and 7.3.5
//   Revision 6.17  2003/03/28 02:37:25Z  td07711
//   in 7.6.1b, change spoof from multiplier to HW_ENABLE
//   Revision 6.16  2003/03/18 02:09:26Z  td07711
//   add spoofs
//   Revision 6.15  2003/03/15 00:02:10Z  td07711
//   add some spoofs
//   Revision 6.14  2003/03/05 23:15:51Z  td07711
//   updated 7.3.19 to ensure 0.4ml AC to donor
//   Revision 6.13  2003/02/18 00:34:50Z  td07711
//   add some spoofs
//   Revision 6.12  2003/01/27 21:03:54Z  td07711
//   bugfix - syntax error broke build
//   Revision 6.11  2003/01/25 00:43:05Z  td07711
//   add spoofs
//   Revision 6.10  2003/01/23 23:43:34Z  td07711
//   add spoofs
//   Revision 6.9  2003/01/21 00:20:29Z  td07711
//   add help()
//   Revision 6.8  2003/01/18 01:23:18Z  td07711
//   spoof mods
//   Revision 6.7  2003/01/17 02:26:55Z  td07711
//   fix typos
//   Revision 6.6  2003/01/17 01:54:57Z  td07711
//   spoof mods
//   Revision 6.5  2002/11/05 17:56:51Z  td07711
//   remove Spoofs* from SpoofBase
//   Revision 6.4  2002/10/31 22:42:20  td07711
//   settle on new function name timedOut()
//   Revision 6.3  2002/10/30 22:57:40  td07711
//   define spoof info stuff next to spoof code
//   Revision 6.2  2002/10/24 23:03:11  td07711
//   removed sleep() member function
//   Revision 6.1  2002/10/24 21:16:17  td07711
//   add some spoofs
//   Revision 1.1  2002/10/23 21:53:04  td07711
//   Initial revision


#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdio.h>
// trima includes
#include "states.h"
#include "safproc.h" // ENABLE_24V_POWER
#include "predict_cds.h" // support Next Generation Spoofer
#include "../../../proc/interface/procdata.h"
#include "../../../safety/s_pfr.hpp"

// spoofer includes
#include "spooferlog.hpp"
#include "myspoofs.hpp"
#include "spooferapi.hpp"
#include "CustomSpoofCollection.cpp"
#include "Spoof_PIRFlags.h"

MySpoofs :: MySpoofs( SpooferAPI* pApi )
: Spoofs( pApi )
{
}

void MySpoofs :: init()
{
    //
    // split up init to avoid compiler stack overflow 
    //
    addSection4();
    addSection7a();
    addSection7b();
    addSection7c();
    addSection7d();
    addSection8a();
    addSection8b();
    addSection8c();
    addParameterized();
    addSpecialCases();
    addIntMessages();
    addGenerica();
    addGenericb();
	addCustomSpoofs();
    addITSpoofs();
    // addAutoSplit(); //for now skip these
}

void MySpoofs :: addSection4()
{
    // PURPOSE: adds section 4 spoofs

    add( new Spoof_PauseAllPumps );
    add( new Spoof_4_6_1 );
    add( new Spoof_4_6_2 );
    add( new Spoof_4_9_7_13 );
    add( new Spoof_4_9_7_14 );
    add( new Spoof_4_10_9 );
    add( new Spoof_4_20_1 );
    add( new Spoof_4_20_2 );
    add( new Spoof_4_21_14 );
    add( new Spoof_4_21_21 );
    add( new Spoof_4_29_13 );
    add( new Spoof_4_36_8 );
    add( new Spoof_4_36_10 );
    add( new Spoof_4_36_11 );
	add( new Spoof_4_39_gen );
}

void MySpoofs :: addSection7a()
{
    // PURPOSE: adds section 7a spoofs

    add( new Spoof_7_1_1 );
	add( new Spoof_7_1_4 );
    add( new Spoof_7_1_5 );
	add( new Spoof_7_1_6a );
	add( new Spoof_7_1_6b );
    add( new Spoof_7_1_9 );
    add( new Spoof_7_1_10 );
	add( new Spoof_7_1_11 );
	add( new Spoof_7_1_13 );
    add( new Spoof_7_1_14 );
	add( new Spoof_7_1_20 );
    add( new Spoof_7_2_2 );
    add( new Spoof_7_2_5b );
    add( new Spoof_7_3_2 );
    add( new Spoof_7_3_5 );
	add( new Spoof_7_3_7 );
    add( new Spoof_7_3_8 );
    add( new Spoof_7_3_10 );
	add( new Spoof_7_3_13 );
	add( new Spoof_7_3_15 );
    add( new Spoof_7_3_16 );
    add( new Spoof_7_3_19 );
    add( new Spoof_7_3_20 );
    add( new Spoof_7_3_21 );
    add( new Spoof_7_3_32 );
    add( new Spoof_7_3_37 );
	add( new Spoof_7_3_38 );
	add( new Spoof_ContTotVol );
	add( new Spoof_ContDeltVol );
}

void MySpoofs :: addSection7b()
{
    // PURPOSE: adds section 7b spoofs
    add( new Spoof_7_5_3 );
    add( new Spoof_7_5_4 );
    add( new Spoof_7_5_5 );
    add( new Spoof_7_5_6 );
    add( new Spoof_7_5_7 );
    add( new Spoof_7_5_9 );
	add( new Spoof_7_5_11 );
    add( new Spoof_7_5_12 );
    add( new Spoof_7_6_1a );
    add( new Spoof_7_6_1b );
    add( new Spoof_7_6_1c );
    add( new Spoof_7_6_1d );
    add( new Spoof_7_6_2 );
    add( new Spoof_7_7_1 );
    add( new Spoof_7_7_2 );
    add( new Spoof_7_9_2 );
    add( new Spoof_7_9_3 );
    add( new Spoof_7_10_2 );
    add( new Spoof_7_10_4 );
}
    
void MySpoofs :: addSection7c()
{
    // Purpose adds section 7c spoofs
    add( new Spoof_7_11_2a );
    add( new Spoof_7_11_2b );
    add( new Spoof_7_11_11 );
    add( new Spoof_7_11_12 );
    add( new Spoof_7_11_13 );
    add( new Spoof_7_11_14 );
    add( new Spoof_7_11_15 );
    add( new Spoof_7_11_16 );
    add( new Spoof_7_11_17 );
    add( new Spoof_7_11_18 );
    add( new Spoof_7_11_19 );
    add( new Spoof_7_11_20 );
    add( new Spoof_7_12_1 );
    add( new Spoof_7_12_5 );
}

void MySpoofs :: addSection7d()
{
    add( new Spoof_7_14_2 );
    add( new Spoof_7_14_3 );
    add( new Spoof_7_14_4 );
    add( new Spoof_7_14_5 );
    add( new Spoof_7_14_7 );
    add( new Spoof_7_14_8 );
    add( new Spoof_7_14_9 );
    add( new Spoof_7_14_10 );
    add( new Spoof_7_14_11 );
    add( new Spoof_7_14_13 );
    add( new Spoof_7_16_2 );
    add( new Spoof_7_16_4 );
    add( new Spoof_7_16_6 );
    add( new Spoof_7_16_8 );
    add( new Spoof_7_16_10 );
	add( new Spoof_7_16_11 );
    add( new Spoof_7_16_12 );
	add( new Spoof_7_16_13 );
    add( new Spoof_7_17_1 );
	add( new Spoof_7_17_3a );
	add( new Spoof_7_17_3b );
    add( new Spoof_7_17_4 );
	add( new Spoof_7_17_5 );
	add( new Spoof_7_17_6 );
	add( new Spoof_7_17_7a );
	add( new Spoof_7_17_7b );
    add( new Spoof_7_18_1 );
	add( new Spoof_7_18_2 );
	add( new Spoof_7_19_1 );
    add( new Spoof_7_19_2 );
    add( new Spoof_7_23_6 );
}


void MySpoofs :: addSection8a()
{
    // PURPOSE: adds section 8a spoofs

    add( new Spoof_8_3_1 );
    add( new Spoof_8_3_2 );
    add( new Spoof_8_3_3 );
    add( new Spoof_8_3_4 );
    add( new Spoof_8_3_5 );
    add( new Spoof_8_3_6 );
    add( new Spoof_8_3_7 );
    add( new Spoof_8_3_8 );
    add( new Spoof_8_3_9 );
    add( new Spoof_8_3_10 );
    add( new Spoof_8_3_11 );
    add( new Spoof_8_3_12 );
    add( new Spoof_8_3_13 );
    add( new Spoof_8_3_14 );
    add( new Spoof_8_3_15 );
    add( new Spoof_8_3_16 );
	add( new Spoof_8_4_3 );
	add( new Spoof_8_4_4 );
	add( new Spoof_8_4_5 );
	add( new Spoof_8_4_6 );
	add( new Spoof_8_4_7 );
	add( new Spoof_8_4_8 );
    add( new Spoof_8_4_9 );
	add( new Spoof_8_4_9_1 );
	add( new Spoof_8_4_9_2 );
	add( new Spoof_8_4_9_3 );
	add( new Spoof_8_4_10 );
	add( new Spoof_8_4_11 );
}

void MySpoofs :: addSection8b()
{
    // PUPOSE: adds section 8b spoofs
    add( new Spoof_8_5_1 );
    add( new Spoof_8_5_5 );
    add( new Spoof_8_5_6 );
    add( new Spoof_8_5_7a );
    add( new Spoof_8_5_7b );
    add( new Spoof_8_6_11 );
    add( new Spoof_8_7_4 );
    add( new Spoof_8_7_8 );
    add( new Spoof_8_7_12 );
    add( new Spoof_8_7_13a );
    add( new Spoof_8_7_13b );
    add( new Spoof_8_7_13c );
    add( new Spoof_8_7_13d );
    add( new Spoof_8_7_13e );
    add( new Spoof_8_7_13g );
}

void MySpoofs :: addSection8c()
{
    add( new Spoof_8_8_1 );
    add( new Spoof_8_9_1 );
    add( new Spoof_8_9_2 );
    add( new Spoof_8_9_3 );
    add( new Spoof_8_9_4 );
    add( new Spoof_8_9_5 );
    add( new Spoof_8_9_6 );
    add( new Spoof_8_9_7 );
	add( new Spoof_8_9_11 );
    add( new Spoof_8_9_12 );
    add( new Spoof_8_9_13 );
    add( new Spoof_8_9_14 );
    add( new Spoof_8_9_15 );
    add( new Spoof_8_10_5a );
    add( new Spoof_8_10_5b );
    add( new Spoof_8_10_5c );
    add( new Spoof_8_10_13 );
    add( new Spoof_8_12_20 );
    add( new Spoof_8_12_26 );
}


void MySpoofs :: addParameterized()
{
    // adds the spoofs I have combined into a parameterized version
    add( new Spoof_COReturnRPM );
	add( new Spoof_COInletRPM );
    add( new Spoof_COAcRPM );
    add( new Spoof_COReturn_AcRPM );
    add( new Spoof_SCassettePosition );
    add( new Spoof_CCassettePosition );
    add( new Spoof_SDoorLock );
    add( new Spoof_CDoorLock );
    add( new Spoof_SReservoir );
    add( new Spoof_CReservoir );
    add( new Spoof_SValve );
    add( new Spoof_CValve );
    add( new Spoof_COValve );
    add( new Spoof_LeakDetector );
    add( new Spoof_DonorVitals );
    add( new Spoof_CAccessPressure );
	add( new Spoof_ReservoirStatus );
	add( new Spoof_ReservoirHct );
 }


void MySpoofs :: addSpecialCases()
{ 
    // PURPOSE: adds spoofs of special cases such as those needed to simulate
    // chamber saturation events
    add( new Spoof_CHAMBER_SATURATION );
    add( new Spoof_CHAMBER_SATURATION_c4b );
    add( new Spoof_CrAv_a);
    add( new Spoof_CrAv_b);
    add( new Spoof_64V_750MSEC_GLITCH );
    add( new Spoof_64V_1250MSEC_GLITCH );
    add( new Spoof_LOW_YIELD );
    add( new Spoof_LOW_PROCVOL_YIELD );
    add( new Spoof_BIG_AC_PRIME );
    add( new Spoof_RGMIDRUN_TRIGGER );
    add( new Spoof_RGPURGE_TRIGGER );
    add( new Spoof_RGEVENT_TRIGGER );
    add( new Spoof_c4b_TRIGGER );
    add( new Spoof_c3EVENT_TRIGGER );
    add( new Spoof_Baselines );
    add( new Spoof_TwoBaselines );
	add( new Spoof_APS_1 );
    add( new Spoof_Safety_APS );
    add( new Spoof_RBC_CAL_FAILURE );
    add( new Spoof_CPS );
	add( new Spoof_CPS_1 );
	add( new Spoof_CPS_mmHg );
	add( new Spoof_APS_mmHg );
	add( new Spoof_PIRFlagsCase1 );
	add( new Spoof_PIRFlagsCase2 );
	add( new Spoof_PIRFlagsCase3 );
	add( new Spoof_PIRFlagsCase4 );
   	add( new Spoof_PIRFlagsCase5 );
   	add( new Spoof_PIRFlagsCase6 );
}

void MySpoofs::addCustomSpoofs()
{
	 CustomSpoofCollection::Iterator iter = CustomSpoofCollection::GetInstance().Begin();
	 CustomSpoofCollection::Iterator end = CustomSpoofCollection::GetInstance().End();

	 for (; iter != end; ++iter) {
		  add((*iter)(this->api()));
	 }
}

void MySpoofs :: addIntMessages()
{
	add( new Spoof_SAFETY_DISCONNECT_OK );
	add( new Spoof_SAFETY_DISCONNECT_FAIL );
    add( new Spoof_SafetyTestDoneMsg );
	add( new Spoof_RequestRunsumStats );
}


void MySpoofs :: addGenerica()
{
    // PURPOSE: adds generic spoofs
    
    add( new Spoof_3L_TBV );
    add( new Spoof_enable );
    add( new Spoof_BLACK_SET );
    add( new Spoof_WHITE_SET );
    add( new Spoof_INVALID_SET_LOW );
    add( new Spoof_INVALID_SET_HIGH );
    add( new Spoof_NO_SPILLOVER );
    add( new Spoof_SPILLOVER );
    add( new Spoof_SPILLOVER_BLACK );
    add( new Spoof_HEMOLYSIS );
    add( new Spoof_HEMOLYSIS_SLOW );
}

void MySpoofs :: addGenericb()
{
    add( new Spoof_12V_LOW );
    add( new Spoof_FAST_RINSEBACK );
    add( new Spoof_FATAL_ERROR_ON_SAFETY );
    add( new Spoof_FATAL_ERROR_ON_CONTROL );
    add( new Spoof_TOGGLE_PUMPS_OFF );
    add( new Spoof_FASTDATA_ZEROED );
    add( new Spoof_FASTDATA_DIV10 );
    add( new Spoof_FASTDATA_DIV100 );
    add( new Spoof_RED_GREEN );
    add( new Valve_Controlled_Wrong );
    add( new Valve_Observed_Wrong );
    add( new Spoof_MultiplyAc );
    add( new Spoof_PumpErrors );
    add( new SpoofAirToDonor );
    add( new Spoof_HWFault );
    add( new Spoof_CCentrifugeRPM );
    add( new Spoof_SCentrifugeRPM );
    add( new Spoof_CCentrifugeError );
    add( new Spoof_SCentrifugeError );
    add( new Spoof_SPumpRevs );
    add( new Spoof_CPumpRPM );
	add( new Spoof_CPumpMult );
    add( new Spoof_ControlACPumpAccum );
	add( new Spoof_ControlInletPumpAccum );
	add( new Spoof_ControlPlasmaPumpAccum );
	add( new Spoof_ControlPlateletPumpAccum );
	add( new Spoof_ControlReturnPumpAccum );
	add( new Spoof_SafetyACPumpAccum );
	add( new Spoof_SafetyInletPumpAccum );
	add( new Spoof_SafetyPlasmaPumpAccum );
	add( new Spoof_SafetyPlateletPumpAccum );
	add( new Spoof_SafetyReturnPumpAccum );
	add( new Spoof_BothACPumpAccum );
	add( new Spoof_BothInletPumpAccum );
	add( new Spoof_BothPlasmaPumpAccum );
	add( new Spoof_BothPlateletPumpAccum );
	add( new Spoof_BothReturnPumpAccum );
	add( new Spoof_SealSafeOvertemp );
    // add( new Spoof_COPlasma_PCAMIDRUN ); // no longer useful

	/* MSC - Port from Trima 5.1 to add Plasma Line Occlusion 
	 *	Testing to Trima 6.0.3 */
    add( new Spoof_FORCE_EPCA );
    add( new Spoof_Skip_Midrun );

    add( new Spoof_PLO_Short_Midrun );
    add( new Spoof_PLO_EPCA );
    add( new Spoof_PLO_Poison_Midrun );
	add( new Spoof_PLO_PCA );
	add( new Spoof_PLO_EPCA_1 );
	add( new Spoof_PLO_EPCA_26 );
	add( new Spoof_CassetteID );
	add( new Spoof_ProcReturnRPM );
	add( new Spoof_S_ATD_1_0001 );
	add( new Spoof_S_ATD_1_0001h );
	add( new Spoof_C_DAE );
	add( new Spoof_C_RB_1_007a );
	add( new Spoof_EnableDisable );
	add( new PFR_Index );
	add( new Spoof_CPumpCmdMultLoop );
	add( new STMBucket );
	add( new Spoof_CPumpRevs );
	add( new Spoof_AvgACRatioTooHigh );
	add( new Spoof_AvgACRatioTooLow );
	add( new Spoof_COrdersPumpCmdLoop );
	add( new Spoof_InstACRatioTooHigh );
	add( new Spoof_InstACRatioTooLow );
	add( new Spoof_RG_reflect );
	add( new Spoof_IT12551_1 );
	add( new Spoof_IT12551_2 );
    add( new SubstateAlert);
}


void MySpoofs :: addITSpoofs()
{
    add( new Spoof_IT10221 );
}


/* 
void MySpoofs :: addAutoSplit()
{
    // PURPOSE: adds spoofs specific to autosplit testing in case this 
    // get added back in
    add( new RBC_C4R );
    add( new RBC_R4C );
    add( new RBC_O4R );
    add( new PLT_O4R );
}
*/

//
// custom behaviors callable by individual spoofs
//
void MySpoofs :: enableDisableLoop( int enableMsec, int disableMsec )
{
    // PURPOSE: loops until timeout, disabling the spoof every to avoid
    // control side shutdown for expected vs actual error

    while( api().timedOut() == false )
    {
        api().spoofEnable();
        api().sleepMsec( enableMsec ); 
        api().spoofDisable();
        api().sleepMsec( disableMsec );
    }
}


int MySpoofs :: psAdc( ps_type_t psType, float voltage )
{
    // PURPOSE:  returns adc value representing voltage of given supply
    //
    // the scaling equations for the following voltages were (at Steve Scovills
    // recomendation) copied from pump/src/abmain.cpp. The equation he uses is
    // v = (a*a_d)/b + c      which yields the voltage * 100
    // solving to convert desired voltages to a/d counts we get:
    // a_d = b * (100 V - c)/a
    // he does all his calculations in integer, we will use floats
    //
    //       signal                a      b       c
    //  sixtyFourVoltSupply      10000   1883       0
    //  twentyFourVoltSupply     10000   9091       0
    //  twelveVoltSupply         10000  16367       0
    //  minusTwelvePS            10000  16231   -2555

    float a = 10000;
    float b;
    float c;

    switch( psType )
    {
        case SUPPLY_64V:
	    b = 1883;
	    c = 0;
	    break;

        case SUPPLY_24V:
	    b = 9091;
	    c = 0;
	    break;

        case SUPPLY_12V:
	    b = 16367;
	    c = 0;
	    break;

        case SUPPLY_MINUS12V:
        default:
	    b = 16231;
	    c = -2555;
	    break;
    }

    return( (int)( b * ( 100 * voltage - c ) / a ) );
}


void MySpoofs :: acPrime()
{
    LOG_INFO("simulating AC prime for 4 seconds to turn on air to donor monitoring");

    api().spoofData( "CHwOrders", "acRPM", "50" );
    api().spoofData( "CHwOrders", "inletRPM", "50" );
    api().spoofActivate();
    
    api().sleepMsec( 3500 );
    
    api().unspoofData( "CHwOrders", "acRPM" );
    api().unspoofData( "CHwOrders", "inletRPM" );
    api().spoofActivate();

    api().sleepMsec( 1000 ); // sleep 1 sec to avoid erroneous "commanded vs actual" shutdown
}

void MySpoofs :: inletMultiplier( float multiplier )
{
    LOG_INFO( "running inlet at %1.2f x commanded speed", multiplier );

    api().spoofData( "Predict_SubstateInfo", "QinD_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinD_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_12", multiplier, 0 ); // new substates with PTF
    api().spoofData( "Predict_SubstateInfo", "QinR_13", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QinR_14", multiplier, 0 );
    //api().spoofData( "Predict_SubstateInfo", "QinR_15", multiplier, 0 );
}


void MySpoofs :: plateletMultiplier( float multiplier )
{
    LOG_INFO( "running platelet at %1.2f x commanded speed", multiplier );

    api().spoofData( "Predict_SubstateInfo", "QcD_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcD_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_12", multiplier, 0 ); // new substates with PTF
    api().spoofData( "Predict_SubstateInfo", "QcR_13", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QcR_14", multiplier, 0 );
    //api().spoofData( "Predict_SubstateInfo", "QcR_15", multiplier, 0 );
}


void MySpoofs :: plasmaMultiplier( float multiplier )
{
    LOG_INFO( "running plasma at %1.2f x commanded speed", multiplier );

    api().spoofData( "Predict_SubstateInfo", "QpD_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpD_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_12", multiplier, 0 ); // new substates with PTF
    api().spoofData( "Predict_SubstateInfo", "QpR_13", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "QpR_14", multiplier, 0 );
    //api().spoofData( "Predict_SubstateInfo", "QpR_15", multiplier, 0 );
}

void MySpoofs :: acMultiplier( float multiplier )
{
    LOG_INFO( "running ac at %1.2f x commanded speed", multiplier );

    api().spoofData( "Predict_SubstateInfo", "Qac_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_11", multiplier, 0 );  // adding to support PTF RBC substates 
    api().spoofData( "Predict_SubstateInfo", "Qac_12", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_13", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qac_14", multiplier, 0 );
    //api().spoofData( "Predict_SubstateInfo", "Qac_15", multiplier, 0 );
    LOG_INFO( "have defined Predict_SubstateInfo Qac for all 15 possible substates");
}


void MySpoofs :: returnMultiplier( float multiplier )
{
    LOG_INFO( "running return at %1.2f x commanded speed", multiplier );

    api().spoofData( "Predict_SubstateInfo", "Qrp_1", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_2", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_3", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_4", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_5", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_6", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_7", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_8", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_9", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_10", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_11", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_12", multiplier, 0 ); // new substates with PTF
    api().spoofData( "Predict_SubstateInfo", "Qrp_13", multiplier, 0 );
    api().spoofData( "Predict_SubstateInfo", "Qrp_14", multiplier, 0 );
    //api().spoofData( "Predict_SubstateInfo", "Qrp_15", multiplier, 0 );

}


void MySpoofs :: acInfusion( float infusionRate )
{
    LOG_INFO( "running flows for ac infusion = %1.2f", infusionRate );
    LOG_INFO( "- presumes donor TBV=3000 and Hct=44" );

    // IT 8260 changing spoof to be consistent with tests using it - donor TBV =3000
    // instead of 3127; Hct=44, Qac=4.1 gives infusion rate of 1.015
    const float baselineFlow = 5.7;
    const float baselineRate = 1.18;
    float acFlow = infusionRate/baselineRate * baselineFlow;

    api().spoofData( "Predict_SubstateInfo", "Qac_1",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_1", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_1",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_1",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_1",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_1",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_1",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_1",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_2",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_2", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_2",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_2",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_2",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_2",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_2",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_2",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_3",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_3", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_3",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_3",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_3",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_3",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_3",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_3",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_4",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_4", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_4",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_4",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_4",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_4",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_4",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_4",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_5",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_5", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_5",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_5",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_5",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_5",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_5",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_5",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_6",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_6", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_6",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_6",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_6",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_6",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_6",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_6",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_7",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_7", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_7",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_7",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_7",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_7",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_7",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_7",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_8",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_8", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_8",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_8",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_8",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_8",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_8",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_8",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_9",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_9", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_9",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_9",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_9",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_9",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_9",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_9",   "177.0" );

    api().spoofData( "Predict_SubstateInfo", "Qac_10",    0, acFlow );
    api().spoofData( "Predict_SubstateInfo", "QinD_10", "57.0" );
    api().spoofData( "Predict_SubstateInfo", "QpD_10",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcD_10",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "QinR_10",   "30.7" );
    api().spoofData( "Predict_SubstateInfo", "QpR_10",    "0.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_10",    "2.1" );
    api().spoofData( "Predict_SubstateInfo", "Qrp_10",   "177.0" );
}

//
// Each spoof's run function is defined below
//


void Spoof_PauseAllPumps :: init()
{
    _name = "PauseAllPumps";
    _comment = "Safety protection against > 10min pause with no AC in needle";
    _timeout = 610;
}
void Spoof_PauseAllPumps :: help()
{
    LOG_INFO( "- run during draw cycle" );
    LOG_INFO( "- spoof pauses all pumps for 10min 10sec" );
    LOG_INFO( "- verify procedure cannot be continued" );
    LOG_INFO( "- verify pumps don't move until disconnect test" );
}
void Spoof_PauseAllPumps :: run()
{
    MySpoofs::acMultiplier( 0 );
    MySpoofs::plateletMultiplier( 0 );
    MySpoofs::plasmaMultiplier( 0 );
    MySpoofs::inletMultiplier( 0 );
    MySpoofs::returnMultiplier( 0 );
    api().spoofActivate();
}


void Spoof_4_6_1 :: init()
{
    _name = "4.6.1";
    _comment = "Generate Pressure Sensor Error";
    _timeout = 5;
}
void Spoof_4_6_1 :: help()
{
    LOG_INFO( "start during a draw" );
}
void Spoof_4_6_1 :: run()
{
    api().spoofData( "FastData", "raw", "0" );
    api().spoofActivate();
}


void Spoof_4_6_2 :: init()
{
    _name = "4.6.2";
    _comment = "Generate Pressure Sensor Error";
    _timeout = 5;
}
void Spoof_4_6_2 :: help()
{
    LOG_INFO( "start during a draw" );
}
void Spoof_4_6_2 :: run()
{
    api().spoofData( "FastData", "raw", 0.05, 2000 );
    api().spoofActivate();
}


void Spoof_SCassettePosition :: init()
{
    _name = "SCassettePosition";
    _comment = "safety disallows PFR if system reboots with cassette in UP position";
    _timeout = 60*3;
}
void Spoof_SCassettePosition :: help()
{
    LOG_INFO( "- Include -var1 position -var2 position on command line to spoof" );
    LOG_INFO( "- Safety's cassette position state to position" ); 
    LOG_INFO( "- possible values are HW_CASSETTE_UP, HW_CASSETTE_DOWN, and HW_CASSETTE_UNKNOWN" );
    LOG_INFO( "- With no var1 parameter spoof spoofs cassette UP to verify that Safety disallows PFR" );
    LOG_INFO( "- if system reboots with cassette in UP position - instructions are as follows for that case:" );
    LOG_INFO( "- PFR during a return cycle" );
    LOG_INFO( "- start spoof as soon as GUI transitions off splash screen" );
    LOG_INFO( "- attempt to continue the run from the PFR recovery screen" );
    LOG_INFO( "- verify that safety alarms" );
}
void Spoof_SCassettePosition :: run()
{
    api().spoofData( "SHwStates", "cassettePosition", "HW_CASSETTE_UP" );
    api().spoofActivate();
    LOG_INFO( "continue from the PFR now" );
}


void Spoof_SCassettePosition :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"HW_CASSETTE_UNKNOWN") == 0) || (strcmp(Var1,"HW_CASSETTE_UP") == 0) ||
    (strcmp(Var1, "HW_CASSETTE_DOWN") == 0)) 
    {
        LOG_INFO( "Spoofing safety cassettePosition to %s", Var1 );
        api().spoofData( "SHwStates", "cassettePosition", Var1 );
        api().spoofActivate();
    }
    else
    {
        LOG_INFO( "Invalid value for var1 so not spoofing cassette position" );
    }
}


void Spoof_4_9_7_13 :: init()
{
    _name = "4.9.7.13";
    _comment = "safety disallows PFR if system reboots with cassette in unknown position";
    _timeout = 60*3;
}
void Spoof_4_9_7_13 :: help()
{
    LOG_INFO( "- PFR during a return cycle" );
    LOG_INFO( "- start spoof as soon as GUI transitions off splash screen" );
    LOG_INFO( "- attempt to continue the run from the PFR recovery screen" );
    LOG_INFO( "- verify that safety alarms" );
}
void Spoof_4_9_7_13 :: run()
{
    api().spoofData( "SHwStates", "cassettePosition", "HW_CASSETTE_UNKNOWN" );
    api().spoofData( "SHwStates", "reservoir", "SHW_RESERVOIR_EMPTY" );
    api().spoofActivate();
    LOG_INFO( "continue from the PFR now" );
}


void Spoof_4_9_7_14 :: init()
{
    _name = "4.9.7.14";
    _comment = "safety disallows PFR if system reboots with cassette in invalid position";
    _timeout = 60*3;
}
void Spoof_4_9_7_14 :: help()
{
    LOG_INFO( "- PFR during a return cycle" );
    LOG_INFO( "- start spoof as soon as GUI transitions off splash screen" );
    LOG_INFO( "- attempt to continue the run from the PFR recovery screen" );
    LOG_INFO( "- verify that safety alarms" );
}
void Spoof_4_9_7_14 :: run()
{
    api().spoofData( "SHwStates", "cassettePosition", 0, -1 );
    api().spoofData( "SHwStates", "reservoir", "SHW_RESERVOIR_EMPTY" );
    api().spoofActivate();
    LOG_INFO( "continue from the PFR now" );
}


void Spoof_CCassettePosition :: init()
{
    _name = "CCassettePosition";
    _comment = "control disallows PFR if system reboots with cassette in invalid position";
    _timeout = 60*5;
}
void Spoof_CCassettePosition :: help()
{
    LOG_INFO( "- Include -var1 position on command line to" );
    LOG_INFO( "- spoof Control CassettePosition to value given for var1" );
    LOG_INFO( "- with no parameters, spoof defaults to invalid (unknown) position to verify that" );
    LOG_INFO( "- Control will disallow PFR if system reboots with cassette in invalid position" );
    LOG_INFO( "_ instruction for that case are as follows:" );
    LOG_INFO( "- PFR during a return cycle" );
    LOG_INFO( "- start spoof on splash screen with cmd line option: -servers 1" );
    LOG_INFO( "- verify that only endrun option is given from PFR screen" );
}
void Spoof_CCassettePosition :: run()
{
    api().spoofData( "CHwStates", "cassettePosition", "HW_CASSETTE_UNKNOWN" );
    api().spoofActivate();
    LOG_INFO( "continue from the PFR now" );
}


void Spoof_CCassettePosition :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"HW_CASSETTE_UNKNOWN") == 0) || (strcmp(Var1,"HW_CASSETTE_UP") == 0) ||
        (strcmp(Var1, "HW_CASSETTE_DOWN") == 0)) 
    {
        LOG_INFO( "Spoofing Control cassettePosition to %s", Var1 );
        api().spoofData( "CHwStates", "cassettePosition", Var1 );
        api().spoofActivate();
    }
    else
    {
        LOG_INFO("Invalid value for Var1 so not spoofing" );
    }
}


void Spoof_4_10_9 :: init()
{
    _name = "4.10.9";
    _comment = "Control transitions to rinseback early if hypovolemia limit is reached";
    _timeout = 90*60;
}
void Spoof_4_10_9 :: help()
{
    LOG_INFO( "- config for donor volume limit of 15%" );
    LOG_INFO( "- setup collection for platelet, AMAP plasma, and RBC product" );
    LOG_INFO( "- start spoof before start of RBC collect, RBC target on GUI should jump 50ml" );
    LOG_INFO( "- verify procedure transitions to rinseback prior to new RBC target" );
}
void Spoof_4_10_9 :: run()
{
    api().spoofData( "ProcRunTargets_RbcVolumeTarget", "data", 1, 50 );
    api().spoofData( "Predict_SubstateInfo", "Vtarget_11", 1, 50 );
    api().spoofActivate();
}


void Spoof_4_20_1 :: init()
{
    _name = "4.20.1";
    _comment = "Control verifies safety can switch 64V on and off";
    _timeout = 30;
}
void Spoof_4_20_1 :: help()
{
    LOG_INFO( "- run spoof just prior to set load" );
    LOG_INFO( "- press load cassette" );
    LOG_INFO( "- verify safety alarm" );
}
void Spoof_4_20_1 :: run()
{
    LOG_INFO( "spoofing 64V on to force power test failure" ); 
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" );
    api().spoofActivate();
}


void Spoof_4_20_2 :: init()
{
    _name = "4.20.2";
    _comment = "Control verifies safety can switch 24V on and off";
    _timeout = 30;
}
void Spoof_4_20_2 :: help()
{
    LOG_INFO( "- sync Tornado shell and prepare spoof command line when splash screen appears" );
    LOG_INFO( "- run spoof when lights go out or 2 button screen first appears" );
    LOG_INFO( "- NOTE: available window to start spoof is only 3-4 seconds" );
    LOG_INFO( "- spoof must be run before valve or door lock testing is done" );
    LOG_INFO( "- verify non-continuable alarm" );
}
void Spoof_4_20_2 :: run()
{
    LOG_INFO( "spoofing 24V on to force power test failure" ); 
    api().spoofData( "SHwOrders", "pumpPower", "HW_ENABLE" );
    api().spoofActivate();
}


void Spoof_4_21_14 :: init()
{
    _name = "4.21.14";
    _comment = "Control protects against disconnect test failure";
    _timeout = 60;  // was 30 a bit quick for test
}
void Spoof_4_21_14 :: help()
{
    LOG_INFO( "- start spoof at donor disconnect prompt" );
    LOG_INFO( "- start and confirm donor disconnect when prompted" );
    LOG_INFO( "- verify donor disconnect test fails" );
}
void Spoof_4_21_14 :: run()
{
    LOG_INFO( "start and confirm donor disconnect test now" );
    api().spoofData( "CHwStates", "accessPressure", 1, 60 ); // presumes about 20mmHg
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 2000, 2000 ); // toggle spoof on/off every 2 seconds to force failure
}


void Spoof_4_21_21 :: init()
{
    _name = "4.21.21";
    _comment = "Safety protects against hypovolemia in disconnect test";
    _timeout = 60;
}
void Spoof_4_21_21 :: help()
{
    LOG_INFO( "- perform an AMAP collection on a 3L donor" );
    LOG_INFO( "- at transition to rinseback, pause and go to donor disconnect" );
    LOG_INFO( "- run disconnect test with donor clamp open causing it to fail" );
    LOG_INFO( "- start spoof at disconnect failure alarm" );
    LOG_INFO( "- verify safety Hypovelemia alarm" );
}
void Spoof_4_21_21 :: run()
{
    LOG_INFO( "pulling another 84ml from the donor in next 60 seconds" );
    api().spoofData( "CHwOrders", "inletRPM", "150" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids control side expected vs actual shutdown
}


void Spoof_CDoorLock :: init()
{
    _name = "CDoorLock";
    _comment = "Spoof Control DoorLock State default is HW_DOOR_CLOSED_STATE";
    _timeout = 60;
}
void Spoof_CDoorLock :: help()
{
    LOG_INFO( "- Include -var1 xxxx -var2 xxxx to spoof control door lock state" );
    LOG_INFO( "- possible values for var1 are HW_DOOR_OPEN, HW_DOOR_CLOSED_STATE," );
    LOG_INFO( "- HW_DOOR_LOCKED_STATE, and HW_DOOR_CLOSED_AND_LOCKED" );
    LOG_INFO( "- See test procedure for specifics of timing and values" );
    LOG_INFO( "- Without var1 and var2 spoofs door lock to HW_DOOR_CLOSED_STATE" );
}
void Spoof_CDoorLock :: run()
{
    api().spoofData( "CHwStates", "doorLocks", "HW_DOOR_CLOSED_STATE" );
    api().spoofActivate();
    LOG_INFO( "start next step" );
}


void Spoof_CDoorLock :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1, "HW_DOOR_OPEN") == 0) || (strcmp(Var1, "HW_DOOR_CLOSED_STATE") == 0) ||
         (strcmp(Var1, "HW_DOOR_LOCKED_STATE") == 0) || (strcmp(Var1,"HW_DOOR_CLOSED_AND_LOCKED") == 0))
    {
        LOG_INFO( "Spoofing Control DoorLock to %s", Var1 );
        api().spoofData( "CHwStates", "doorLocks", Var1 );
        api().spoofActivate();
        LOG_INFO( "start next step" );
    }
    else
    {
        LOG_INFO( "value for var1 is incorrect so not spoofing" );

    }
}


void Spoof_SDoorLock :: init()
{
    _name = "SDoorLock";
    _comment = "Spoof Safety DoorLock State default is HW_DOOR_CLOSED_STATE";
    _timeout = 60;
}
void Spoof_SDoorLock :: help()
{
    LOG_INFO( "- Include -var1 xxxx -var2 xxxx to spoof safety door lock state" );
    LOG_INFO( "- possible values for var1 are HW_DOOR_OPEN, HW_DOOR_CLOSED_STATE," );
    LOG_INFO( "- HW_DOOR_LOCKED_STATE, and HW_DOOR_CLOSED_AND_LOCKED" );
    LOG_INFO( "- See test procedure for specifics of timing and values" );
    LOG_INFO( "- Without var1 and var2 spoofs door lock to HW_DOOR_OPEN" );
    LOG_INFO( "- to verify that Control won't start centrifuge if Safety sees door open" );
    LOG_INFO( "- for that case start spoof after AC prime" );
    LOG_INFO( "- attempt to start draw" );
    LOG_INFO( "- verify that centrifuge does not start and system alarms" );

}
void Spoof_SDoorLock :: run()
{
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_OPEN" );
    api().spoofActivate();
    LOG_INFO( "start next step" );
}


void Spoof_SDoorLock :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1, "HW_DOOR_OPEN") == 0) || (strcmp(Var1, "HW_DOOR_CLOSED_STATE") == 0) ||
         (strcmp(Var1, "HW_DOOR_LOCKED_STATE") == 0) || (strcmp(Var1,"HW_DOOR_CLOSED_AND_LOCKED") == 0))
    {
        LOG_INFO( "Spoofing Safety DoorLock state to %s", Var1 );
        api().spoofData( "SHwStates", "doorLocks", Var1 );
        api().spoofActivate();
        LOG_INFO( "start next step" );
    }
    else
    {
        LOG_INFO( "value for var1 is incorrect so not spoofing" );

    }
}


void Spoof_4_29_13 :: init()
{
    _name = "4.29.13";
    _comment = "Auto APS should fail if any sample is more than 10mmHg from average";
    _timeout = 20;
}
void Spoof_4_29_13 :: help()
{
    LOG_INFO( "- start spoof just prior to set load" );
    LOG_INFO( "- when prompted, start cassette load" );
    LOG_INFO( "- verify cassette is raised and that control alarms with non-continuable alarm" ); 
}
void Spoof_4_29_13 :: run()
{

    api().spoofData( "CHwStates", "accessPressure", 1, 100 );
    api().spoofActivate();

    LOG_INFO( "Start cassette load now" );
    MySpoofs::enableDisableLoop( 500, 500 ); 
}


void Spoof_4_36_8 :: init()
{
    _name = "4.36.8";
    _comment = "Simulates chamber saturation with platelet concentration < 1000";
    _timeout = 27*60;
}
void Spoof_4_36_8 :: help()
{
    LOG_INFO( "- enable only sws3a and sws3b in rbc.dat" );
    LOG_INFO( "- select platelet only procedure with runtime of at least 40 minutes" );
    LOG_INFO( "- start spoof at beginning for 4th draw cycle" );
}
void Spoof_4_36_8 :: run()
{
    LOG_INFO( "Setting up baseline for next 20 minutes" );
    //
    // spoof red to 1350 for Cr < 1.0 assuming max REDpl of 1500
    //
    api().spoofData( "CHwStates", "red", "1350" );
    api().spoofActivate();
    for ( int i = 1; i < 1200; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "1350" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "1355" );
	api().sleepMsec( 6*1000 );
    }
    LOG_INFO( "Baseline setup complete" );


    LOG_INFO( "Now spoofing the trigger conditions for next 6.5 minutes" );
    //
    // spoof green to toggle ratio between 1.0 and 0.6 every 6 seconds for 5.5 minutes
    //
    for( int i = 1; i <= 330; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "1350" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "2250" );
	api().sleepMsec( 6*1000 );
    }
    //
    // spoof green to toggle ratio between 1.1 and 1.4 every 6 seconds for 1 minute
    //
    for( int i = 1; i <= 60; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "1227" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "964" );
	api().sleepMsec( 6*1000 );
    }
}


void Spoof_4_36_10 :: init()
{
    _name = "4.36.10";
    _comment = "Verify chamber saturation detection in presence of short pauses";
    _timeout = 35*60+10;
}
void Spoof_4_36_10 :: help()
{
    LOG_INFO( "- enable only the sws3a trigger in rbc.dat" );
    LOG_INFO( "- select platelet only procedure with runtime of at least 60 minutes" );
    LOG_INFO( "- start spoof at beginning of 4th draw cycle" );
    LOG_INFO( "- every 2 minutes of procedure time, pause for about 10 seconds" );
    LOG_INFO( "- verify spillover detection prior to end of spoof" );
}
void Spoof_4_36_10 :: run()
{
    LOG_INFO( "Setting up baseline for next 25 minutes" );
    //
    // spoof red to 650 for Cr > 1.0
    //
    api().spoofData( "CHwStates", "red", "650" );
    api().spoofActivate();
    api().sleepMsec( 25*60*1000 );
    LOG_INFO( "Baseline setup complete" );


    LOG_INFO( "Now spoofing the trigger conditions for next 10 minutes" );
    //
    // spoof green to toggle ratio between 1.0 and 0.6 every 6 seconds (650 and 1083)  for 8 minutes
    //
    for( int i = 1; i <= 480; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "650" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "1083" );
	api().sleepMsec( 6*1000 );
    }
    //
    // spoof green to toggle ratio between 1.1 and 1.4 every 6 seconds ( 591 and 464) for 2 minute
    //
    for( int i = 1; i <= 120; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "591" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "464" );
	api().sleepMsec( 6*1000 );
    }
}


void Spoof_4_36_11 :: init()
{
    _name = "4.36.11";
    _comment = "Verify chamber saturation detection in presence of centrifuge slowdowns";
    _timeout = 54*60+10;
}
void Spoof_4_36_11 :: help()
{
    LOG_INFO( "- enable only the sws3b trigger in rbc.dat" );
    LOG_INFO( "- select platelet only procedure with runtime of at least 60 minutes" );
    LOG_INFO( "- Part I:  start spoof at beginning of 4th draw cycle" );
    LOG_INFO( "- Part I:  every 2 minutes of procedure time, pause for a little over 1 minute" );
    LOG_INFO( "- Part I:  verify no spillover detection prior to end of spoof" );
    LOG_INFO( "- Part II:  start spoof at beginning of 4th draw cycle or following part I" );
    LOG_INFO( "- Part II:  every 4 minutes of procedure time, pause for a little over 1 minute" );
    LOG_INFO( "- Part II:  verify spillover detection prior to end of spoof" );
}
void Spoof_4_36_11 :: run()
{
    LOG_INFO( "Setting up baseline for next 40 minutes" );
    //
    // spoof red to 650 for Cr > 1.0
    //
    api().spoofData( "CHwStates", "red", "650" );
    api().spoofActivate();
    api().sleepMsec( 40*60*1000 );
    LOG_INFO( "Baseline setup complete" );


    LOG_INFO( "Now spoofing the trigger conditions for next 14 minutes" );
    //
    // spoof green to toggle ratio between 1.0 and 0.6 every 6 seconds (650 and 1083)  for 11 minutes
    //
    for( int i = 1; i <= 660; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "650" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "1083" );
	api().sleepMsec( 6*1000 );
    }
    //
    // spoof green to toggle ratio between 1.1 and 1.4 every 6 seconds ( 591 and 464) for 3 minute
    //
    for( int i = 1; i <= 180; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "591" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "464" );
	api().sleepMsec( 6*1000 );
    }
}

void Spoof_4_39_gen :: init()
{
    _name = "4.39.gen";
    _comment = "Spoof for IT 5634 - spoofs red and green reflectance regardless of conc";
    _timeout = 40*60+10;
}

void Spoof_4_39_gen :: help()
{
    LOG_INFO( "- Start when \"Start Run\" is pressed." );
    LOG_INFO( "- Original parameterless spoof does the following:" );
    LOG_INFO( "    - Spoof red and green to 1100 for 10 minutes." );
    LOG_INFO( "    - Spoof red and green to 921 for 30 minutes." );
    LOG_INFO( "- Include -var1 xxxx -var2 yyyy to spoof red and green" );
    LOG_INFO( "  to xxxx for 10 minutes and then red and green to yyyy" );
    LOG_INFO( "- Values suggested in IT 7220 for test procedures:" );
    LOG_INFO( "    4.39.1a: var1 1100 var2 831" );
    LOG_INFO( "    4.39.1b: var1 1100 var2 652" );
    LOG_INFO( "    4.39.2a: var1 1100 var2 705" );
    LOG_INFO( "    4.39.2b: var1 1100 var2 442" );
    LOG_INFO( "    4.39.3a: var1 1100 var2 597" );
    LOG_INFO( "    4.39.3b: var1 1100 var2 262" );
    LOG_INFO( "    4.39.4a: var1 1100 var2 550" );
    LOG_INFO( "    4.39.4b: var1 1100 var2 183" );
    LOG_INFO( "    4.39.5a: var1 1100 var2 442" );
    LOG_INFO( "    4.39.5b: var1 1100 var2 3" );
    LOG_INFO( "    4.39.6a: var1 1100 var2 284" );
    LOG_INFO( "    4.39.6b: var1 1100 var2 -261" ); // negative RBCs not possible
    LOG_INFO( "    4.39.7a: var1 1100 var2 573" );
    LOG_INFO( "    4.39.7b: var1 1100 var2 222" );

}

void Spoof_4_39_gen :: run()
{
    LOG_INFO( "Spoofing red and green to 1100." );
    // red is spoofed to spoof the concentration
    // green is spoofed the same to avoid spillovers.
	api().spoofData( "CHwStates", "green", "1100" );	
    api().spoofData( "CHwStates", "red", "1100" );
	api().spoofActivate();
	api().sleepMsec( 10*60*1000 );
    LOG_INFO( "Spoofing red and green to 921." );
	api().spoofData( "CHwStates", "green", "921" );
    api().spoofData( "CHwStates", "red", "921" );
}


void Spoof_4_39_gen :: run_generic (const char* Var1, const char* Var2)
{
    // Var1 is used for red & green values for first 10 minutes
    // then Var2 is used for the red & green value
    // red is spoofed to spoof the concentration
    // green is spoofed the same to avoid spillovers.
    LOG_INFO( "Spoofing red and green to %s", Var1 );
    api().spoofData( "CHwStates", "green", Var1 );
    api().spoofData( "CHwStates", "red", Var1 );
    api().spoofActivate();
	api().sleepMsec( 10*60*1000 );
    LOG_INFO( "Now spoofing red and green to %s", Var2 );
	api().spoofData( "CHwStates", "green", Var2 );
	api().spoofData( "CHwStates", "red", Var2 );
}
void Spoof_7_1_1 :: init()
{
    _name = "7.1.1";
    _comment = "Control Alert for loss of occlusion during disposable test";
    _timeout = 7;
}
void Spoof_7_1_1 :: help()
{
    LOG_INFO( "- run during leakage check of disposable test" );
    LOG_INFO( "- spoof drops APS 51mmHg" );
    LOG_INFO( "- verify control alerts with Pressure Test Failure" ); 
}
void Spoof_7_1_1 :: run()
{
    api().spoofData( "HalStatus_aps", "data", 1, -51 );
    api().spoofActivate();
}



void Spoof_7_1_4 :: init()
{
    _name = "7.1.4";
    _comment = "Safety air to donor protection during prime";
    _timeout = 3;
}
void Spoof_7_1_4 :: help()
{
    LOG_INFO( "- run after donor connect, prior to 1st fluid" );
    LOG_INFO( "- verify safety alarms with Air To Donor" );
    LOG_INFO( "- verify return pump moves less than 2 revolutions" );
}
void Spoof_7_1_4 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofActivate();
}


void Spoof_7_1_5 :: init()
{
    _name = "7.1.5";
    _comment = "Control air to donor protection after 1st fluid";
    _timeout = 60;
}
void Spoof_7_1_5 :: help()
{
    LOG_INFO( "- start spoof during return cycle" );
    LOG_INFO( "- verify pumps stop at low level sensor" );
    LOG_INFO( "- verify no air gets into return filter or line for duration of spoof" );
    LOG_INFO( "- Note: will likely result in Draw Too Long alarm on subsequent draw" );
}
void Spoof_7_1_5 :: run()
{
    api().spoofActivate();

    for( int i = 1; i <= 12; i++ )
    {
	api().spoofData( "CHwOrders", "acRPM", "0" );
	api().sleepMsec( 500 );
    api().spoofData( "CHwOrders", "returnRPM", "160" );
	api().sleepMsec( 2500 );
	api().unspoofData( "CHwOrders", "returnRPM" );
	api().sleepMsec( 500 );
	api().unspoofData( "CHwOrders", "acRPM" );
	api().sleepMsec( 1500 );
    }
}


void Spoof_7_1_6a :: init()
{
    _name = "7.1.6a";
    _comment = "Safety return overspeed does not alarm at 160RPM";
    _timeout = 3;
}
void Spoof_7_1_6a :: help()
{
    LOG_INFO( "- run during return cycle" );
    LOG_INFO( "- verify spoof completes with no alarms" );
}
void Spoof_7_1_6a :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofActivate();
}


void Spoof_7_1_6b :: init()
{
    _name = "7.1.6b";
    _comment = "Safety hemolysis protection - max return speed of 165RPM";
    _timeout = 3;
}
void Spoof_7_1_6b :: help()
{
    LOG_INFO( "- run during return cycle");
    LOG_INFO( "- verify safety alarms for return pump overspeed" );
}
void Spoof_7_1_6b :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "170" );
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofActivate();
}


void Spoof_7_1_9 :: init()
{
    _name = "7.1.9";
    _comment = "Control air to donor protection at end of rinseback";
    _timeout = 30;
}
void Spoof_7_1_9 :: help()
{
    LOG_INFO( "- run at end of rinseback" );
    LOG_INFO( "- verify pumps stop at low level sensor" );
    LOG_INFO( "- verify no air gets into return filter or line" );
    LOG_INFO( "- verify no alarms" );
}
void Spoof_7_1_9 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "50" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown
}


void Spoof_7_1_10 :: init()
{
    _name = "7.1.10";
    _comment = "Safety air to donor protection at 1 RPM";
    _timeout = 20; // allow enough time for spoof to execute
}
void Spoof_7_1_10 :: help()
{ 
    LOG_INFO( "- run after AC prime" );
    LOG_INFO( "- verify that safety alarms for air to donor" );
}
void Spoof_7_1_10 :: run()
{

    LOG_INFO( "7.1.10 spoofing pump speeds" );
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofData( "CHwOrders", "inletRPM", "10" ); // ensures safety not using needle flow
    api().spoofData( "CHwOrders", "returnRPM", "1" );
    api().spoofActivate();
	MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown
}


void Spoof_7_1_11 :: init()
{
    _name = "7.1.11";
    _comment = "Safety air to donor protecton after PFR - 249 RPM";
    _timeout = 3;
}
void Spoof_7_1_11 :: help()
{
    LOG_INFO( "- do PFR at connect donor prompt" );
    LOG_INFO( "- continue to connect donor prompt again and run spoof" );
    LOG_INFO( "- verify safety alarms for air to donor" );
    LOG_INFO( "- verify return pump does not turn more than 2 revs" );
}
void Spoof_7_1_11 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "249" );
    api().spoofActivate();
}


void Spoof_COInletRPM :: init()
{
    _name = "COInletRPM";
    _comment = "Safety monitoring of Control Orders to Inlet pump";
    _timeout = 3;
}
void Spoof_COInletRPM :: help()
{
    LOG_INFO( "- run per test directions" );
    LOG_INFO( "- include -var1 InletRPM -timeout duration(seconds) on command line" );
    LOG_INFO( "-without the parameters, inlet pump is ordered to 15rpm for 3 seconds" );
}
void Spoof_COInletRPM :: run()
{
    api().spoofData( "CHwOrders", "inletRPM", "15" );
    api().spoofActivate();
}
void Spoof_COInletRPM :: run_generic(const char* Var1, const char* Var2)
{
    int RPM = atoi(Var1);
    LOG_INFO( " spoofing Inlet pump to %d RPM", RPM );
    api().spoofData( "CHwOrders", "inletRPM", Var1 );
    api().spoofActivate();
}


void Spoof_COReturnRPM :: init()
{
    _name = "COReturnRPM";
    _comment = "Safety air to donor protecton by monitoring return pump RPM";
    _timeout = 3;
}
void Spoof_COReturnRPM :: help()
{
    LOG_INFO( "- Use -var1 RPMvalue and -timeout duration(seconds)" );
    LOG_INFO( "- in command line according to test directions");
}
void Spoof_COReturnRPM :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
}
void Spoof_COReturnRPM :: run_generic(const char* Var1, const char* Var2)
{
    int RPM = atoi(Var1);
    LOG_INFO( " spoofing return pump to %d RPM", RPM );
    api().spoofData( "CHwOrders", "returnRPM", Var1 );
    api().spoofActivate();
}


void Spoof_7_1_13 :: init()
{
    _name = "7.1.13";
    _comment = "Safety air to donor protection at 160 RPM";
    _timeout = 8; // allow 5 seconds for ac prime
}
void Spoof_7_1_13 :: help()
{
    LOG_INFO( "- run after cassette load before donor AC prime" );
    LOG_INFO( "- the spoof simulates AC prime flows to enable safety air to donor protection" );
    LOG_INFO( "- verify that safety alarms for air to donor" );
}
void Spoof_7_1_13 :: run()
{
    MySpoofs::acPrime(); // turns on safety air to donor monitoring

    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
}


void Spoof_7_1_14 :: init()
{
    _name = "7.1.14";
    _comment = "Control air to donor protection after PFR - 160 RPM";
    _timeout = 60;
}
void Spoof_7_1_14 :: help()
{    
    LOG_INFO( "- PFR during a return cycle" );
    LOG_INFO( "- continue back into the procedure and run the spoof" );
    LOG_INFO( "- verify pumps stop at low level sensor" );
    LOG_INFO( "- verify no air gets into return filter or line" );
    LOG_INFO( "- verify no alarms" );
}
void Spoof_7_1_14 :: run()
{
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown
}


void Spoof_COReturn_AcRPM :: init()
{
    _name = "COReturn_AcRPM";
    _comment = "Safety monitoring of return and AC pumps";
    _timeout = 3;
}
void Spoof_COReturn_AcRPM :: help()
{
    LOG_INFO( "- run spoof according to test directions" );
    LOG_INFO( "- include -var1 returnRPM -var2 acRPM -timeout duration (second) " );
    LOG_INFO( "- in the command line to match to test" );
    LOG_INFO( "- with no parameters the spoof command the return pump to 160" );
    LOG_INFO( "- and AC pump to 0 for 3 seconds" );
}
void Spoof_COReturn_AcRPM :: run()
{
    LOG_INFO( "spoofing return pump to 160 and AC pump to 0 RPM" );
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
}


void Spoof_COReturn_AcRPM :: run_generic(const char* Var1, const char* Var2)
{
    int returnRPM = atoi(Var1);
    int acRPM = atoi(Var2);
    LOG_INFO( " spoofing return pump to %d RPM and AC pump to %d", returnRPM, acRPM );
    api().spoofData( "CHwOrders", "returnRPM", Var1 );
    api().spoofData( "CHwOrders", "acRPM", Var2 );
    api().spoofActivate();
}


void Spoof_7_1_20 :: init()
{
    _name = "7.1.20";
    _comment = "Control air to donor protection during Additive Solution Addition";
    _timeout = 10; // 10 seconds should be enough to alarm with out causing extreme excess pressure
}
void Spoof_7_1_20 :: help()
{
    LOG_INFO( "- run during addition of Plt or RBC Additive solution" );
    LOG_INFO( "- verify pumps stop at low level sensor" );
    LOG_INFO( "- verify no air gets into return filter or line" );
    LOG_INFO( "- verify that if an alarm is received it is a pressure alarm and not air to donor" );
}
void Spoof_7_1_20 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "50" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown
}

void Spoof_SReservoir :: init()
{
    _name = "SReservoir";
    _comment = "Safety low level sensor monitoring";
    _timeout = 180;
}
void Spoof_SReservoir :: help()
{
    LOG_INFO( "- Include -var1 xxxx on command line where xxxx is") ;
    LOG_INFO( "- SHW_RESERVOIR_LOW, or SHW_RESERVOIR_EMPTY");
    LOG_INFO( "- Use -timeout duration to set the duration in seconds to spoof the reservoir state" );
    LOG_INFO( "- for safety fluid too soon protection testing" );
    LOG_INFO( "- run spoof anytime before return line prime for 5 seconds" );
    LOG_INFO( "- verify that safety alarms for System Test Failure #32784" );
}
void Spoof_SReservoir :: run()
{
    api().spoofData( "SHwStates", "reservoir", "SHW_RESERVOIR_LOW" );
    api().spoofActivate();
}


void Spoof_SReservoir :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"SHW_RESERVOIR_LOW") == 0) || (strcmp(Var1, "SHW_RESERVOIR_EMPTY") == 0)) 
    {
        LOG_INFO( "Spoofing Safety reservoir state to %s", Var1);
        api().spoofData("SHwStates", "reservoir", Var1);
        api().spoofActivate();
    }
    else
    {
        LOG_INFO( "Invalid value for reservoir state so not spoofing");
    }
}


void Spoof_CReservoir :: init()
{
    _name = "CReservoir";
    _comment = "Control protection against long or fast returns";
    _timeout = 120;
}
void Spoof_CReservoir :: help()
{
    LOG_INFO( "- Include -var1 xxxx on command line where xxxx is") ;
    LOG_INFO( "- CHW_RESERVOIR_ERROR, CHW_RESERVOIR_EMPTY, CHW_RESERVOIR_MIDDLE, or CHW_RESERVOIR_HIGH" );
    LOG_INFO( "- Use -timeout duration to set the duration in seconds to spoof the reservoir state" );
    LOG_INFO( "- per test directions default is 120 saeconds" );
}
void Spoof_CReservoir :: run()
{  
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_MIDDLE" );
    api().spoofActivate();
}
void Spoof_CReservoir :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"CHW_RESERVOIR_ERROR") == 0) || (strcmp(Var1,"CHW_RESERVOIR_EMPTY") == 0) ||
        (strcmp(Var1,"CHW_RESERVOIR_MIDDLE") == 0) || (strcmp(Var1,"CHW_RESERVOIR_HIGH") == 0)) 
    {
        LOG_INFO( "Spoofing Control reservoir state to %s", Var1);
        api().spoofData("CHwStates", "reservoir", Var1);
        api().spoofActivate();
    }
    else
    {
        LOG_INFO( "Invalid value for reservoir state so not spoofing var1 must be one of the following:");
        LOG_INFO( "CHW_RESERVOIR_ERROR, CHW_RESERVOIR_EMPTY, CHW_RESERVOIR_MIDDLE, or CHW_RESERVOIR_HIGH" );

    }
}

void Spoof_7_2_5a :: init()
{
    _name = "7.2.5a";
    _comment = "Control protection against long draw cycle";
    _timeout = 180;
}
void Spoof_7_2_5a :: help()
{
    LOG_INFO( "- run spoof during a draw (not 1st draw)" );
    LOG_INFO( "- it will run for 3 minutes to allow time for following" );
    LOG_INFO( "- verify control alarms with Draw Cycle Too Long" );
    LOG_INFO( "- expect approx 10ml of fluid will be in the vent bag" );
    LOG_INFO( "- hit continue to continue the draw" );
    LOG_INFO( "- verify control alarms with A1 System Failure" );
    LOG_INFO( "- expect approx 77ml of fluid will be in the vent bag" );
}
void Spoof_7_2_5a :: run()
{  
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_MIDDLE" );
    api().spoofActivate();
}

void Spoof_7_2_2 :: init()
{

    _name = "7.2.2";
    _comment = "Safety fluid too soon protection";
    _timeout = 5;
}

void Spoof_7_2_2 :: help()
{
    LOG_INFO( "- run spoof anytime before return line prime" );
    LOG_INFO( "- verify that safety alarms for System Test Failure #32784" );
}

void Spoof_7_2_2 :: run()
{
    api().spoofData( "SHwStates", "reservoir", "SHW_RESERVOIR_LOW" );
    api().spoofActivate();
}



void Spoof_7_2_5b :: init()
{
    _name = "7.2.5b";
    _comment = "Safety protection against long draw cycle";
    _timeout = 30;
}
void Spoof_7_2_5b :: help()
{
    LOG_INFO( "- run spoof with the A1 alarm present from 7.2.5a" );
    LOG_INFO( "- it will intermittently continue the draw for 30 seconds" );
    LOG_INFO( "- verify safety alarms Detected Draw Cycle Too Big" );
    LOG_INFO( "- expect safety to alarm after another 8ml to the reservoir" );
}
void Spoof_7_2_5b :: run()
{  
    api().spoofData( "CHwOrders", "inletRPM", "60" );
    api().spoofData( "CHwOrders", "acRPM", "6" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown
}


void Spoof_7_3_2 :: init()
{
    _name = "7.3.2";
    _comment = "Safety ac to donor protection - Inlet Pump off during Draw";
    _timeout = 10;
}
void Spoof_7_3_2 :: help()
{
    LOG_INFO( "- inlet pump off during draw results in AC pumping direct to the donor" );
    LOG_INFO( "- run spoof during a draw cycle to deliver .6ml AC to donor" );
    LOG_INFO( "- verify safety alarms for AC Ratio Too Low (trips at .4ml)" );
}
void Spoof_7_3_2 :: run()
{
    api().spoofData( "CHwOrders", "inletRPM", "0" );
    api().spoofData( "CHwOrders", "acRPM", "7" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown
}

void Spoof_7_3_5 :: init()
{
    _name = "7.3.5";
    _comment = "Safety ac to donor protecton - AC pump on during return";
    _timeout = 3;
}
void Spoof_7_3_5 :: help()
{
    LOG_INFO( "- run during a return cycle to deliver .6ml AC to donor" );
    LOG_INFO( "- verify safety alarms for AC Ratio Too Low (trips at .4ml)" );
}
void Spoof_7_3_5 :: run()
{
    api().spoofData( "CHwOrders", "acRPM", "15" );
    api().spoofActivate();
}


void Spoof_COAcRPM :: init()
{
    _name = "COAcRPM";
    _comment = "Safety monitoring of Control Orders to AC pump";
    _timeout = 3;
}
void Spoof_COAcRPM :: help()
{
    LOG_INFO( "- run per test directions" );
    LOG_INFO( "- include -var1 acRPM -timeout duration(seconds) on command line" );
    LOG_INFO( "-without the parameters spoof ac pump ordered to 15 for 3 seconds" );
}
void Spoof_COAcRPM :: run()
{
    api().spoofData( "CHwOrders", "acRPM", "15" );
    api().spoofActivate();
}
void Spoof_COAcRPM :: run_generic(const char* Var1, const char* Var2)
{
    int RPM = atoi(Var1);
    LOG_INFO( " spoofing AC pump to %d RPM", RPM );
    api().spoofData( "CHwOrders", "acRPM", Var1 );
    api().spoofActivate();
}


void Spoof_7_3_7 :: init()
{
    _name = "7.3.7";
    _comment = "Safety ac to donor protection - ac running faster than inlet during Draw";
    _timeout = 3;
}
void Spoof_7_3_7 :: help()
{
    LOG_INFO( "- run during a draw" );
    LOG_INFO( "- runs AC at 200RPM, exceeding exceed max inlet of 142ml/min" );
    LOG_INFO( "- verify safety alarms for AC Ratio Too Low" );
}
void Spoof_7_3_7 :: run()
{
    api().spoofData( "CHwOrders", "acRPM", "200" );
    api().spoofActivate();
}


void Spoof_7_3_8 :: init()
{
    _name = "7.3.8";
    _comment = "Safety AC infusion protection - 3 cycle";
    _timeout = 300;
}
void Spoof_7_3_8 :: help()
{
    LOG_INFO( "- with no parameters produces infusion rate of 1.33 for TBV=3127 Hct=44" );
    LOG_INFO( "- use form 7.3.8 -var1 x.xx -var2 x.xx to produce an infusion rate of about x.xx");
    LOG_INFO( "- start spoof during return after product collection begins" );
    LOG_INFO( "- verify safety alarms for ac infusion at start of 3rd subsequent return" ); 
}
void Spoof_7_3_8 :: run()
{
    MySpoofs::acInfusion( 1.33 );
    api().spoofActivate();
}
void Spoof_7_3_8 :: run_generic(const char* Var1, const char* Var2)
{
    float acRate = atof(Var1);
    LOG_INFO( " spoofing acInfusion to %f", acRate );
    MySpoofs::acInfusion(acRate);
    api().spoofActivate();
}


void Spoof_7_3_10 :: init()
{
    _name = "7.3.10";
    _comment = "Safety protection against AC Ratio Too Low from slow inlet";
    _timeout = 90;
}
void Spoof_7_3_10 :: help()
{
    LOG_INFO( "- config for AC ratio of 6.0, safety enforces minimum ratio of 5.0" );
    LOG_INFO( "- config AC infusion setting of 1" );
    LOG_INFO( "- run anytime between 1st draw and rinseback" );
    LOG_INFO( "- verify safety alarms for AC Ratio Too Low" );
}
void Spoof_7_3_10 :: run()
{
    MySpoofs::inletMultiplier( .78 );
    api().spoofActivate();
}


void Spoof_7_3_13 :: init()
{
    _name = "7.3.13";
    _comment = "Control protects against ac to donor from returning collected plasma";
    _timeout = 60;
}
void Spoof_7_3_13 :: help()
{
    LOG_INFO( "- run during return cycle to spoof fluid at high sensor" );
    LOG_INFO( "- add 15ml fluid to reservoir to force long return cycle" );
    LOG_INFO( "- verify control alarms with Level Sensor Error" );
    LOG_INFO( "- verify that continue/rinseback options are not offered" );
}
void Spoof_7_3_13 :: run()
{
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_HIGH" );
    api().spoofActivate();
}


void Spoof_7_3_15 :: init()
{
    _name = "7.3.15";
    _comment = "Safety ac infusion protection - single cycle, RBC collect on black set with replacement fluid";
    _timeout = 60*5;
}
void Spoof_7_3_15 :: help()
{
    LOG_INFO( "- config small donor, AC ratio set to 10, AC infusion setting to 6" );
    LOG_INFO( "- select RBC product on black set with replacement fluid" );
    LOG_INFO( "- start spoof during return during RBC collection" );
    LOG_INFO( "- verify safety alarms Overdelivery Of AC at start of next return" ); 
}
void Spoof_7_3_15 :: run()
{  
    MySpoofs::acMultiplier( 1.7/1.2 ); // spoofs infusion of 1.7
    api().spoofActivate();
}

void Spoof_7_3_16 :: init()
{
    _name = "7.3.16";
    _comment = "Safety ac infusion protection - two cycle, plasma collection on white set";
    _timeout = 60*5;
}
void Spoof_7_3_16 :: help()
{
    LOG_INFO( "- config TBV=3100 Hct=44, Ratio=10, Infusion=6" );
    LOG_INFO( "- select a plasma collection with runtime < 60min" );
    LOG_INFO( "- start spoof during return after plasma collection begins" );
    LOG_INFO( "- verify safety alarms Overdelivery Of AC at start of 2nd subsequent return" ); 
}
void Spoof_7_3_16 :: run()
{   
    MySpoofs::acInfusion( 1.46 ) ;
    api().spoofActivate();
}


void Spoof_7_3_19 :: init()
{
    _name = "7.3.19";
    _comment = "Safety ac to donor protection - inlet failure in 1st draw";
    _timeout = 120;
}
void Spoof_7_3_19 :: help()
{
    LOG_INFO( "- run during 1st draw cycle" );
    LOG_INFO( "- safety should alarm when AC to donor > 0.4ml" );
    LOG_INFO( "- verify safety alarms for AC Ratio Too Low" );
}
void Spoof_7_3_19 :: run()
{
    api().spoofData( "CHwOrders", "inletRPM", 0.5, 0 );
    api().spoofActivate();
}


void Spoof_7_3_20 :: init()
{
    _name = "7.3.20";
    _comment = "Safety protects against 300ml of cummulative inlet with no AC";
    _timeout = 210 * 3;
}
void Spoof_7_3_20 :: help()
{
    LOG_INFO( "- run following 1st draw cycle" );
    LOG_INFO( "- verify safety alarms for AC Ratio Too Low" );
}
void Spoof_7_3_20 :: run()
{
    MySpoofs::acMultiplier( 0 );
    api().spoofActivate();
    LOG_INFO( "- spoofing spillover AC off for 3.5 minutes, expecting no alarms" );
    api().sleepMsec( 210000 );

    LOG_INFO( "- AC back on for 3.5 minutes, expecting no alarms" );
    api().spoofDisable();
    api().sleepMsec( 210000 );

    LOG_INFO( "- spoofing AC off for 3.5 minutes, expecting alarm" );
    api().spoofEnable();
}


void Spoof_7_3_21 :: init()
{
    _name = "7.3.21";
    _comment = "Safety ac ratio too low protection during multiple short draws - 2ml cummulative AC max";
    _timeout = 5;
}
void Spoof_7_3_21 :: help()
{
    LOG_INFO( "- start spoof right at beginning of draw" );
    LOG_INFO( "- spoof will run AC pump fast and will end the draw early" );
    LOG_INFO( "- rerun spoof up to 6 times and verify safety alarms for AC Ratio Too Low" );
}
void Spoof_7_3_21 :: run()
{
    api().spoofData( "CHwOrders", "acRPM", 5, 0 ); // run at 5x
    api().spoofActivate();
    api().sleepMsec( 2200 );
    api().spoofDisable();

    api().unspoofData( "CHwOrders", "acRPM" );
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_HIGH" );
    api().spoofData( "CHwStates", "event", "CHW_RESERVOIR_EVENT" );
    api().setSpoofedMessageLimit( 1 ); // generate one res event to end draw cycle
    api().spoofEnable();
}


void Spoof_7_3_32 :: init()
{
    _name = "7.3.32";
    _comment = "Safety ac to donor protection - 20ml bolus in 1st draw (potentially from multiple AKO)";
    _timeout = 30;
}
void Spoof_7_3_32 :: help()
{
    LOG_INFO( "- pause during 1st draw with reservoir over half full" );
    LOG_INFO( "- run spoof - will return 6ml 6 times" );
    LOG_INFO( "- verify safety alarms for AC Infusion" );
}
void Spoof_7_3_32 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", 0, 120/2.7 );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3000, 2000 ); // avoids expected vs actual shutdown
}


void Spoof_7_3_37 :: init()
{
    _name = "7.3.37";
    _comment = "Safety calculates AC infusion safely with high plasma collect flow";
    _timeout = 10 * 60;
}
void Spoof_7_3_37 :: help()
{
    LOG_INFO( "- start spoof while collecting large plasma product" );
    LOG_INFO( "- verify reasonable AC infusion log by safety" );
}
void Spoof_7_3_37 :: run()
{
    MySpoofs::plasmaMultiplier( 1.5 );
    api().spoofActivate();
    api().sleepMsec( 3*60*1000 );

    MySpoofs::plasmaMultiplier( 2.0 );
    api().spoofActivate();
    api().sleepMsec( 3*60*1000 );

    MySpoofs::plasmaMultiplier( 2.5 );
    api().spoofActivate();
}


void Spoof_7_3_38 :: init()
{
	_name = "7.3.38";
	_comment = "Spoofs the commanded speed in control for the inlet pump by a scalar factor.";
	_comment = "Uses a enable/disable loop (3.5s enable, 1.5s disable) so as to avoid an expected vs actual alarm.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_7_3_38 :: help()
{
	LOG_INFO("If no parameters are provided, will run the inlet pump at the default");
	LOG_INFO("scalar factor of 1.5 (new pump speed will be 1.5 times the originally commanded speed).");
	LOG_INFO("Provide a custom scalar factor with -var1 #.## (new_speed = (var1)X(original_speed))");
	LOG_INFO("Use -timeout option to override the default 30 second timeout (enable/disable loop will run with duty cycle 3.5s/1.5s until -timeout time has passed).");
}
void Spoof_7_3_38 :: run()
{
    MySpoofs::inletMultiplier( 1.5 );
    api().spoofActivate();
	
	MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}
void Spoof_7_3_38 :: run_generic(const char* Var1, const char* Var2)
{
   float factor = atof(Var1);
   //LOG_INFO( "running inlet pump at %f x commanded speed", Var1 );
   MySpoofs::inletMultiplier( factor );
   api().spoofActivate();

   MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}


void Spoof_ContTotVol :: init()
{
	_name = "ContTotVol";
	_comment = "Spoofs the total volume accumulated (in mL) for a given container (inlet, return, ac, RBC, plasma or platelet) to a given amount.";
	_comment = "Default mode is to spoof the inlet contianer volume to 0 ml.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_ContTotVol :: help()
{
	LOG_INFO("If no parameters are provided, will spoof the total inlet container volume to 0 mL");
	LOG_INFO("To parameterize: use -var1 to specify a container (inlet, return, ac, RBC, plasma or platelet), use -var2 to specify desired soofed volume (in mL)");
	LOG_INFO("Note: Plasma, platelet and RBC container include AC volume; RBC container includes plasma volume.");
	LOG_INFO("Use -timeout option to override the default 30 second timeout.");
}
void Spoof_ContTotVol :: run()
{
	this->run_generic("inlet", "0");
}
void Spoof_ContTotVol :: run_generic(const char* Var1, const char* Var2)
{
	ProcVolumes_CDS cds(ROLE_RO);
	if (Var1 != NULL && Var2 != NULL) {

		if ((strcmp(Var1,"return") == 0) || (strcmp(Var1, "plasma") == 0) ||
		(strcmp(Var1,"platelet")== 0) || (strcmp(Var1, "inlet") == 0) ||
		(strcmp(Var1, "ac") == 0) || (strcmp(Var1,"rbc") == 0) ) 
		{
			if ((atoi(Var2) >= 0) || (atof(Var2) >= 0))
			{
				LOG_INFO( "Spoofing the %s container volume to %s mL.", Var1, Var2 );
			}
			else
			{
				LOG_ERROR( "var2 must be a number" );
				return;
			}
		}
		else
		{
			LOG_ERROR( "var1 must be a container name (inlet, return, ac, rbc, plasma or platelet");
			return;
		}
	}
	else
	{
		LOG_ERROR( "Both -var1 and -var2 parameters must be used");
		LOG_ERROR( "run with -help for usage information");
	}


    if (strcmp(Var1,"ac") == 0)
    {
		api().spoofData( "ProcVolumes_VacTotal", "data", Var2 ); 
    }
    else if (strcmp(Var1,"inlet") == 0)
    {
		api().spoofData( "ProcVolumes_VinTotal", "data", Var2 ); 
    }
    else if (strcmp(Var1,"plasma") == 0)
    {
		api().spoofData( "ProcVolumes_PlasmaVolume", "data", Var2 ); 
    }
    else if (strcmp(Var1,"platelet") == 0)
    {
		api().spoofData( "ProcVolumes_PlateletVolume", "data", Var2 ); 
    }
    else if (strcmp(Var1,"return") == 0)
    {
		api().spoofData( "ProcVolumes_Vreturn", "data", Var2 ); 
    }
	else if (strcmp(Var1,"RBC") == 0)
    {
		api().spoofData( "ProcVolumes_RBCVolume", "data", Var2 ); 
    }

    api().spoofActivate();

}


void Spoof_ContDeltVol :: init()
{
	_name = "ContDeltVol";
	_comment = "Spoofs the change in volume for the current cycle for the given container (inlet, return, ac) to a given amount.";
	_comment = "Default is to spoof the inlet contianer volume to 0 ml.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_ContDeltVol :: help()
{
	LOG_INFO("If no parameters are provided, will spoof the inlet container cycle volume to 0 mL");
	LOG_INFO("To parameterize: use -var1 to specify a container (inlet, return, ac), use -var2 to specify desired soofed volume (in mL)");
	LOG_INFO("Use -timeout option to override the default 30 second timeout.");
}
void Spoof_ContDeltVol :: run()
{
	this->run_generic("inlet", "0");
}
void Spoof_ContDeltVol :: run_generic(const char* Var1, const char* Var2)
{
	ProcVolumes_CDS cds(ROLE_RO);
	if (Var1 != NULL && Var2 != NULL) {

		if ((strcmp(Var1,"return") == 0) || (strcmp(Var1, "inlet") == 0) ||
		(strcmp(Var1, "ac") == 0)) 
		{
			if ((atoi(Var2) >= 0) || (atof(Var2) >= 0))
			{
				LOG_INFO( "Spoofing the %s container volume to %s mL.", Var1, Var2 );
			}
			else
			{
				LOG_ERROR( "var2 must be a number" );
				return;
			}
		}
		else
		{
			LOG_ERROR( "var1 must be a container name (inlet, return, ac");
			return;
		}
	}
	else
	{
		LOG_ERROR( "Both -var1 and -var2 parameters must be used");
		LOG_ERROR( "run with -help for usage information");
	}


    if (strcmp(Var1,"ac") == 0)
    {
		api().spoofData( "ProcVolumes_Vac", "data", Var2 ); 
    }
    else if (strcmp(Var1,"inlet") == 0)
    {
		api().spoofData( "ProcVolumes_Vin", "data", Var2 ); 
    }
    else if (strcmp(Var1,"return") == 0)
    {
		api().spoofData( "ProcVolumes_Vreturn", "data", Var2 ); 
    }

    api().spoofActivate();

}


void Spoof_7_5_3 :: init()
{
    _name = "7.5.3";
    _comment = "Safety protects against centrifuge at 70RPM with door locked and not closed";
    _timeout = 130;
}
void Spoof_7_5_3 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms" );
}
void Spoof_7_5_3 :: run()
{
    api().spoofData( "CHwOrders", "centrifugeRPM", "70" ); 
    api().spoofActivate();

    LOG_INFO( "- allowing 2 minutes for centrifuge to ramp down to 70RPM" );
    api().sleepMsec( 120000 );

    LOG_INFO( "- spoofing safety door sensor locked but not closed" );
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_LOCKED_STATE" ); 
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" ); 
    api().spoofActivate();
}


void Spoof_7_5_4 :: init()
{
    _name = "7.5.4";
    _comment = "Safety side centrifuge monitoring based on safety side speed and door sensors";
    _timeout = 30;
}
void Spoof_7_5_4 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen with basin door closed" );
    LOG_INFO( "- verify no safety alarm in first 20 seconds" );
    LOG_INFO( "- verify safety alarms before spoof ends" );
}
void Spoof_7_5_4 :: run()
{
    LOG_INFO( "- spoofing safety side door open" );
    LOG_INFO( "- spoofing safety side centrifuge speed to 60RPM for 20 seconds" );
    LOG_INFO( "- verify no safety alarm" );
    api().spoofData( "CHwStates", "sixtyFourVoltSwitched",
		     0, MySpoofs::psAdc( MySpoofs::SUPPLY_64V, 64 ) );
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_OPEN" ); 
    api().spoofData( "SHwStates", "centrifugeRPM", "60" );
    api().spoofActivate();
    api().sleepMsec( 20000 );
    
    LOG_INFO( "- spoofing safety side centrifuge speed to 61RPM" );
    LOG_INFO( "- verify safety alarms" );
    api().spoofData( "SHwStates", "centrifugeRPM", "61" );
    api().spoofActivate();
}


void Spoof_7_5_5 :: init()
{
    _name = "7.5.5";
    _comment = "Safety protects against centrifuge running with door open";
    _timeout = 5;
}
void Spoof_7_5_5 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms" );
}
void Spoof_7_5_5 :: run()
{
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_OPEN" ); 
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" ); 
    api().spoofActivate();
}


void Spoof_7_5_6 :: init()
{
    _name = "7.5.6";
    _comment = "Safety protects against centrifuge running with door closed but not locked";
    _timeout = 5;
}
void Spoof_7_5_6 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms" );
}
void Spoof_7_5_6 :: run()
{
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_CLOSED_STATE" ); 
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" ); 
    api().spoofActivate();
}


void Spoof_7_5_7 :: init()
{
    _name = "7.5.7";
    _comment = "Control protects against centrifuge running door locked but not closed";
    _timeout = 5;
}
void Spoof_7_5_7 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify control alarms Door Not Closed and that centrifuge stops" );
}
void Spoof_7_5_7 :: run()
{
    api().spoofData( "CHwStates", "doorLocks", "HW_DOOR_LOCKED_STATE" ); 
    api().spoofActivate();
}


void Spoof_7_5_9 :: init()
{
    _name = "7.5.9";
    _comment = "Safety protects against centrifuge running at 200RPM with door open and unlocked";
    _timeout = 130;
}
void Spoof_7_5_9 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms and that centrifuge stops" );
}
void Spoof_7_5_9 :: run()
{
    LOG_INFO( "- slowing centrifuge to 200RPM" );
    api().spoofData( "CHwOrders", "centrifugeRPM", "200" );
    api().spoofActivate();

    api().sleepMsec( 120000 );
    LOG_INFO( "- spoofing safety door status open" );
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" ); 
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_OPEN" );
}


void Spoof_7_5_11 :: init()
{
    _name = "7.5.11";
    _comment = "Control will not start centrifuge if safety sees door open";
    _timeout = 300;
}
void Spoof_7_5_11 :: help()
{
    LOG_INFO( "- start spoof after AC prime" );
    LOG_INFO( "- attempt to start draw" );
    LOG_INFO( "- verify that centrifuge does not start and system alarms" );
}
void Spoof_7_5_11 :: run()
{
    LOG_INFO( "- spoofing safety door status to open" );
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_OPEN" );
    api().spoofActivate();
}


void Spoof_7_5_12 :: init()
{
    _name = "7.5.12";
    _comment = "Safety protects against centrifuge running door locked but not closed";
    _timeout = 5;
}
void Spoof_7_5_12 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms" );
}
void Spoof_7_5_12 :: run()
{
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_LOCKED_STATE" ); 
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" ); 
    api().spoofActivate();
}


void Spoof_7_6_1a :: init()
{
    _name = "7.6.1a";
    _comment = "System safe states upon loss of safety status messages";
    _timeout = 15;
}
void Spoof_7_6_1a :: help()
{
    LOG_INFO( "- run spoof at any time" );
    LOG_INFO( "- verify system safe states" );
}
void Spoof_7_6_1a :: run()
{
    api().spoofData( "SHwStates", "valveLedTest", 1, 0 ); 
    api().msgMultiplier( 0 ); 
    api().spoofActivate();
}


void Spoof_7_6_1b :: init()
{
    _name = "7.6.1b";
    _comment = "System safe states upon loss of safety order messages";
    _timeout = 10;
}
void Spoof_7_6_1b :: help()
{
    LOG_INFO( "- run spoof at any time" );
    LOG_INFO( "- verify system safe states" );
}
void Spoof_7_6_1b :: run()
{
    api().spoofData( "SHwOrders", "centrifugePower", "HW_ENABLE" ); 
    api().msgMultiplier( 0 ); 
    api().spoofActivate();
}


void Spoof_7_6_1c :: init()
{
    _name = "7.6.1c";
    _comment = "System safe states upon loss of control status messages";
    _timeout = 10;
}
void Spoof_7_6_1c :: help()
{
    LOG_INFO( "- run spoof at any time" );
    LOG_INFO( "- verify system safe states" );
}
void Spoof_7_6_1c :: run()
{
    api().spoofData( "CHwStates", "accessPressure", 1, 0 ); 
    api().msgMultiplier( 0 ); 
    api().spoofActivate();
}


void Spoof_7_6_1d :: init()
{
    _name = "7.6.1d";
    _comment = "System safe states upon loss of control order messages";
    _timeout = 10;
}
void Spoof_7_6_1d :: help()
{
    LOG_INFO( "- run spoof at any time" );
    LOG_INFO( "- verify system safe states" );
}
void Spoof_7_6_1d :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", 1, 0 ); 
    api().msgMultiplier( 0 ); 
    api().spoofActivate();
}


void Spoof_7_6_2 :: init()
{
    _name = "7.6.2";
    _comment = "System protected against safety side communication overload";
    _timeout = 30;
}
void Spoof_7_6_2 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify system safe states" );
}
void Spoof_7_6_2 :: run()
{
    api().spoofData( "SHwStates", "centrifugeRPM", 1, 0 ); 
    api().spoofActivate();

    LOG_INFO( "spoofing safety status message load x10" );
    api().msgMultiplier( 10 );
    api().sleepMsec( 10000 );

    LOG_INFO( "spoofing safety status message load x30" );
    api().msgMultiplier( 30 );
    api().sleepMsec( 10000 );

    LOG_INFO( "spoofing safety status message load x50" );
    api().msgMultiplier( 50 );
}


void Spoof_7_7_1 :: init()
{
    _name = "7.7.1";
    _comment = "Safety protects against centrifuge exceeding 3300RPM";
    _timeout = 10;
}
void Spoof_7_7_1 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms with Centrifuge over-speed error" );
}
void Spoof_7_7_1 :: run()
{
    api().spoofData( "SHwStates", "centrifugeRPM", "3300" ); 
    api().spoofActivate();
}


void Spoof_7_7_2 :: init()
{
    _name = "7.7.2";
    _comment = "Safety protects against centrifuge error";
    _timeout = 20;
}
void Spoof_7_7_2 :: help()
{
    LOG_INFO( "- run spoof during a procedure with centrifuge running" );
    LOG_INFO( "- verify safety alarms with Centrifuge Error" );
}
void Spoof_7_7_2 :: run()
{
    // HW_CENT_HARDWARE_ERROR = 1,
    api().spoofData( "SHwStates", "centrifugeError", "1" ); 
    api().spoofActivate();
}


void Spoof_7_9_2 :: init()
{
    _name = "7.9.2";
    _comment = "Safety protects against TBV mismatch (Proc too low)";
    _timeout = 2*60;
}
void Spoof_7_9_2 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor info" );
    LOG_INFO( "- verify safety alarms Invalid TBV" );
}
void Spoof_7_9_2 :: run()
{
    api().spoofData( "DVitalsFromProc", "ProcDonorTBV", 1, -51 );
    api().spoofActivate();
}


void Spoof_7_9_3 :: init()
{
    _name = "7.9.3";
    _comment = "Safety TBV is displayed by GUI";
    _timeout = 2*60;
}
void Spoof_7_9_3 :: help()
{
    LOG_INFO( "- enter donor gender, height and weight, and note the TBV displayed" );
    LOG_INFO( "- start the spoof" );
    LOG_INFO( "- reenter same weight again" );
    LOG_INFO( "- verify that TBV displayed has increased 10-20ml" );
}
void Spoof_7_9_3 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorWeight", 1, 1 );
    api().spoofActivate();
}


void Spoof_7_10_2 :: init()
{
    _name = "7.10.2";
    _comment = "Safety protects against cassette unloading before donor disconnect";
    _timeout = 3;
}
void Spoof_7_10_2 :: help()
{
    LOG_INFO( "- run spoof after ac prime and before donor disconnect test passed" );
    LOG_INFO( "- verify safety alarms with Cassette Raise Attempt Error" );
}
void Spoof_7_10_2 :: run()
{
    api().spoofData( "CHwOrders", "cassettePosition", "CHW_CASSETTE_UP" ); 
    api().spoofActivate();
}


void Spoof_7_10_4 :: init()
{
    _name = "7.10.4";
    _comment = "Safety protects against control starting donor disconnect test too rapidly";
    _timeout = 5;
}
void Spoof_7_10_4 :: help()
{
    LOG_INFO( "- confirm donor disconnect and run spoof while donor disconnect test is running" );
    LOG_INFO( "- verify safety alarm and safe state" );
    LOG_INFO( "- verify rundata shows SW_FAULT: extraneous Power Request during Donor Disconnect Test" ); 
}
void Spoof_7_10_4 :: run()
{
    api().messageSend( "RequestSafetyPowerMsg", ENABLE_24V_POWER );
    api().sleepMsec( 1000 );
    api().messageSend( "RequestSafetyPowerMsg", ENABLE_24V_POWER );
}


void Spoof_7_11_2a :: init()
{
    _name = "7.11.2a";
    _comment = "Safety system lockup - saf_exec task";
    _timeout = 5;
}
void Spoof_7_11_2a :: help()
{
    LOG_INFO( "- start the spoof anytime after donor connect" );
    LOG_INFO( "- verify pumps stop and system reboots" );
    LOG_INFO( "- verify after reboot that only donor disconnect option is offered" );
}
void Spoof_7_11_2a :: run()
{
    LOG_INFO( "suspending task saf_exec" );
    api().suspend( "SAFETY", "saf_exec" );
}


void Spoof_7_11_2b :: init()
{
    _name = "7.11.2b";
    _comment = "Safety system lockup - safe_drv task";
    _timeout = 5;
}
void Spoof_7_11_2b :: help()
{
    LOG_INFO( "- start the spoof anytime after donor connect" );
    LOG_INFO( "- verify pumps stop and system reboots" );
    LOG_INFO( "- verify after reboot that only donor disconnect option is offered" );
}
void Spoof_7_11_2b :: run()
{
    LOG_INFO( "suspending task safe_drv" );
    api().suspend( "SAFETY", "safe_drv" );
}


void Spoof_7_11_11 :: init()
{
    _name = "7.11.11";
    _comment = "tests nullptr read protection on safety computer";
    _timeout = 10;
}
void Spoof_7_11_11 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_11 :: run()
{
    api().nullptrRead( "SAFETY" );
    api().fatalError( "SAFETY" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_12 :: init()
{
    _name = "7.11.12";
    _comment = "tests nullptr read protection on control computer";
    _timeout = 10;
}
void Spoof_7_11_12 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_12 :: run()
{
    api().nullptrRead( "CONTROL" );
    api().fatalError( "CONTROL" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_13 :: init()
{
    _name = "7.11.13";
    _comment = "tests text write protection on safety computer";
    _timeout = 10;
}
void Spoof_7_11_13 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_13 :: run()
{
    api().textWrite( "SAFETY" );
    api().fatalError( "SAFETY" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_14 :: init()
{
    _name = "7.11.14";
    _comment = "tests text write protection on control computer";
    _timeout = 10;
}
void Spoof_7_11_14 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_14 :: run()
{
    api().textWrite( "CONTROL" );
    api().fatalError( "CONTROL" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_15 :: init()
{
    _name = "7.11.15";
    _comment = "tests stack overflow protection on safety computer";
    _timeout = 10;
}
void Spoof_7_11_15 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_15 :: run()
{
    api().stackOverflow( "SAFETY" );
    api().fatalError( "SAFETY" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_16 :: init()
{
    _name = "7.11.16";
    _comment = "tests stack overflow protection on control computer";
    _timeout = 10;
}
void Spoof_7_11_16 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_16 :: run()
{
    api().stackOverflow( "CONTROL" );
    api().fatalError( "CONTROL" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_17 :: init()
{
    _name = "7.11.17";
    _comment = "tests data overflow protection on safety computer";
    _timeout = 10;
}
void Spoof_7_11_17 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_17 :: run()
{
    api().dataOverflow( "SAFETY" );
    api().fatalError( "SAFETY" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_18 :: init()
{
    _name = "7.11.18";
    _comment = "tests data overflow protection on control computer";
    _timeout = 10;
}
void Spoof_7_11_18 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_18 :: run()
{
    api().dataOverflow( "CONTROL" );
    api().fatalError( "CONTROL" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_19 :: init()
{
    _name = "7.11.19";
    _comment = "tests bss overflow protection on safety computer";
    _timeout = 10;
}
void Spoof_7_11_19 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_19 :: run()
{
    api().bssOverflow( "SAFETY" );
    api().fatalError( "SAFETY" ); // should get ack timeout instead of fatal error
}


void Spoof_7_11_20 :: init()
{
    _name = "7.11.20";
    _comment = "tests bss overflow protection on control computer";
    _timeout = 10;
}
void Spoof_7_11_20 :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify receipt of ack timeout" );
    LOG_INFO( "- verify system does not reboot" );
    LOG_INFO( "- verify rundata for expected pagefault logging" );
}
void Spoof_7_11_20 :: run()
{
    api().bssOverflow( "CONTROL" );
    api().fatalError( "CONTROL" ); // should get ack timeout instead of fatal error
}


void Spoof_7_12_1 :: init()
{
    _name = "7.12.1";
    _comment = "Control protects against hypovolemia - fast plasma and platelet pumps";
    _timeout = 7200;
}
void Spoof_7_12_1 :: help()
{
    LOG_INFO( "- start spoof at donor connect during an AMAP run with 15% volume limit" );
    LOG_INFO( "- verify safety does not alarm" );
    LOG_INFO( "- verify run ends with rinseback" );
    LOG_INFO( "- verify run summary procedure time was several minutes less than initial prediction" );
}
void Spoof_7_12_1 :: run()
{
    MySpoofs :: plasmaMultiplier( 1.2 );
    MySpoofs :: plateletMultiplier( 1.2 );
    api().spoofActivate();
}


void Spoof_7_12_5 :: init()
{
    _name = "7.12.5";
    _comment = "Control protects against hypovolemia - fast inlet pump during rbc collect";
    _timeout = 7200;
}
void Spoof_7_12_5 :: help()
{
    LOG_INFO( "- config AC infusion = 1, and enter 3L donor" );
    LOG_INFO( "- start spoof at donor connect during an AMAP run, 15% volume limit, and RBC product" );
    LOG_INFO( "- verify safety does not alarm" );
    LOG_INFO( "- verify run ends with rinseback" );
    LOG_INFO( "- verify run summary procedure time was several minutes less than initial prediction" );
}
void Spoof_7_12_5 :: run()
{
    MySpoofs :: inletMultiplier( 1.3 );
    api().spoofActivate();
}


void Spoof_7_14_2 :: init()
{
    _name = "7.14.2";
    _comment = "Safety protects against DONOR_CONNECTED to POST_RUN transition";
    _timeout = 10;
}
void Spoof_7_14_2 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_2 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );
    
    LOG_INFO( "sending a POST_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", POST_RUN );
}


void Spoof_7_14_3 :: init()
{
    _name = "7.14.3";
    _comment = "Safety protects against BLOOD_PRIME to POST_RUN transition";
    _timeout = 10;
}
void Spoof_7_14_3 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_3 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );
    
    LOG_INFO( "sending a BLOOD_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_PRIME );
    
    LOG_INFO( "sending a POST_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", POST_RUN );
}


void Spoof_7_14_4 :: init()
{
    _name = "7.14.4";
    _comment = "Safety protects against BLOOD_RUN to POST_RUN transition";
    _timeout = 10;
}
void Spoof_7_14_4 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_4 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );
    
    LOG_INFO( "sending a BLOOD_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_PRIME );
    
    LOG_INFO( "sending a BLOOD_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RUN );
    
    LOG_INFO( "sending a POST_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", POST_RUN );
}


void Spoof_7_14_5 :: init()
{
    _name = "7.14.5";
    _comment = "Safety protects against BLOOD_RINSEBACK to POST_RUN transition";
    _timeout = 10;
}
void Spoof_7_14_5 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_5 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );
    
    LOG_INFO( "sending a BLOOD_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_PRIME );
    
    LOG_INFO( "sending a BLOOD_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RUN );
    
    LOG_INFO( "sending a BLOOD_RINSEBACK state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RINSEBACK );
    
    LOG_INFO( "sending a POST_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", POST_RUN );
}


void Spoof_7_14_7 :: init()
{
    _name = "7.14.7";
    _comment = "Safety protects against transition to BLOOD_RUN without door sensor gate";
    _timeout = 10;
}
void Spoof_7_14_7 :: help()
{
    LOG_INFO( "- open door and run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_7 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );
    
    LOG_INFO( "sending a BLOOD_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_PRIME );
    
    LOG_INFO( "sending a BLOOD_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RUN );
}


void Spoof_7_14_8 :: init()
{
    _name = "7.14.8";
    _comment = "Safety protects against transition from AC_PRIME to BLOOD_RUN";
    _timeout = 10;
}
void Spoof_7_14_8 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_8 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a BLOOD_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RUN );
}


void Spoof_7_14_9 :: init()
{
    _name = "7.14.9";
    _comment = "Safety protects against transition from DONOR_CONNECTED to BLOOD_RUN";
    _timeout = 10;
}
void Spoof_7_14_9 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_9 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );

    LOG_INFO( "sending a BLOOD_RUN state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RUN );
}


void Spoof_7_14_10 :: init()
{
    _name = "7.14.10";
    _comment = "Safety protects against transition from DONOR_CONNECTED to BLOOD_RINSEBACK";
    _timeout = 10;
}
void Spoof_7_14_10 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_10 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );

    LOG_INFO( "sending a BLOOD_RINSEBACK state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RINSEBACK );
}


void Spoof_7_14_11 :: init()
{
    _name = "7.14.11";
    _comment = "Safety protects against transition from BLOOD_PRIME to BLOOD_RINSEBACK";
    _timeout = 10;
}
void Spoof_7_14_11 :: help()
{
    LOG_INFO( "- run spoof at AC connect prompt" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_11 :: run()
{
    LOG_INFO( "sending a AC_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_CONNECTED );
    
    LOG_INFO( "sending a AC_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", AC_PRIME );

    LOG_INFO( "sending a DONOR_CONNECTED state transition message" );
    api().messageSend( "SystemStateChangeMsg", DONOR_CONNECTED );

    LOG_INFO( "sending a BLOOD_PRIME state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_PRIME );

    LOG_INFO( "sending a BLOOD_RINSEBACK state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RINSEBACK );
}


void Spoof_7_14_13 :: init()
{
    _name = "7.14.13";
    _comment = "Safety protects against transition to BLOOD_RINSEBACK before 1st draw completes";
    _timeout = 10;
}
void Spoof_7_14_13 :: help()
{
    LOG_INFO( "- run spoof during 1st draw" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_14_13 :: run()
{
    LOG_INFO( "sending a BLOOD_RINSEBACK state transition message" );
    api().messageSend( "SystemStateChangeMsg", BLOOD_RINSEBACK );
}


void Spoof_7_16_2 :: init()
{
    _name = "7.16.2";
    _comment = "Safety protects against Inlet pump sensor error";
    _timeout = 10;
}
void Spoof_7_16_2 :: help()
{
    LOG_INFO( "- spoofs inlet pump encoder with a 7 rev positive offset" ); 
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- verify safety alarms with Pump Error" );
}
void Spoof_7_16_2 :: run()
{
    float encodersPerRev = 20608;
    api().spoofData( "CHwStates", "inletPumpAccum", 1, 1.01*7.0*encodersPerRev );
    api().spoofActivate();
}


void Spoof_7_16_4 :: init()
{
    _name = "7.16.4";
    _comment = "Safety protects against Return pump sensor error";
    _timeout = 10;
}
void Spoof_7_16_4 :: help()
{
    LOG_INFO( "- spoofs return pump encoder with a 7 rev negative offset" ); 
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- verify safety alarms with Pump Error" );
}
void Spoof_7_16_4 :: run()
{
    float encodersPerRev = 20608;
    api().spoofData( "CHwStates", "returnPumpAccum", 1, -1.01*7.0*encodersPerRev );
    api().spoofActivate();
}


void Spoof_7_16_6 :: init()
{
    _name = "7.16.6";
    _comment = "Safety protects against AC pump sensor error";
    _timeout = 10;
}
void Spoof_7_16_6 :: help()
{
    LOG_INFO( "- spoofs ac pump encoder with a 4 rev negative offset" ); 
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- verify safety alarms with Pump Error" );
}
void Spoof_7_16_6 :: run()
{
    float encodersPerRev = 20608;
    api().spoofData( "CHwStates", "acPumpAccum", 1, -1.01*4.0*encodersPerRev );
    api().spoofActivate();
}


void Spoof_7_16_8 :: init()
{
    _name = "7.16.8";
    _comment = "Safety protects against plasma pump sensor error";
    _timeout = 10;
}
void Spoof_7_16_8 :: help()
{
    LOG_INFO( "- spoofs plasma pump encoder with a 4 rev negative offset" ); 
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- verify safety alarms with Pump Error" );
}
void Spoof_7_16_8 :: run()
{
    float encodersPerRev = 20608;
    api().spoofData( "CHwStates", "plasmaPumpAccum", 1, -1.01*4.0*encodersPerRev );
    api().spoofActivate();
}


void Spoof_7_16_10 :: init()
{
    _name = "7.16.10";
    _comment = "Safety protects against platelet pump sensor error";
    _timeout = 10;
}
void Spoof_7_16_10 :: help()
{
    LOG_INFO( "- spoofs platelet pump encoder with a 4 rev positive offset" ); 
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- verify safety alarms with Pump Error" );
}
void Spoof_7_16_10 :: run()
{
    float encodersPerRev = 20608;
    api().spoofData( "CHwStates", "plateletPumpAccum", 1, 1.01*4.0*encodersPerRev );
    api().spoofActivate();
}


void Spoof_7_16_11 :: init()
{
    _name = "7.16.11";
    _comment = "Safety protects against return pump > 250ml/min in rinseback (ac to donor protection)";
    _timeout = 3;
}
void Spoof_7_16_11 :: help()
{
    LOG_INFO( "- start spoof at any time during rinseback with at least a half full reservoir" );
    LOG_INFO( "- verify safety alarms with AC to Donor" );
}
void Spoof_7_16_11 :: run()
{
    // 250ml/min, with stroke volume of 2.737, = 91.3RPM
    api().spoofData( "CHwOrders", "returnRPM", "97" );
    api().spoofActivate();
}


void Spoof_7_16_12 :: init()
{
    _name = "7.16.12";
    _comment = "Safety protects against return pump overspeed > 165RPM (hemolysis protection)";
    _timeout = 3;
}
void Spoof_7_16_12 :: help()
{
    LOG_INFO( "- start spoof during a return cycle" );
    LOG_INFO( "- verify safety alarms with Pump Error" );
}
void Spoof_7_16_12 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "170" );
    api().spoofActivate();
}


void Spoof_7_16_13 :: init()
{
    _name = "7.16.13";
    _comment = "Safety does not alarm with return pump < 165RPM in return cycle";
    _timeout = 3;
}
void Spoof_7_16_13 :: help()
{
    LOG_INFO( "- start spoof at start of a return cycle" );
    LOG_INFO( "- verify safety does not alarm" );
}
void Spoof_7_16_13 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
}


void Spoof_7_17_1 :: init()
{
    _name = "7.17.1";
    _comment = "Safety alarms on safety blink test failure";
    _timeout = 10;
}
void Spoof_7_17_1 :: help()
{
    LOG_INFO( "- start spoof anytime after ac prime" );
    LOG_INFO( "- verify safety alarms #32783" );
}
void Spoof_7_17_1 :: run()
{
    api().spoofData( "SHwStates", "valveLedTest", "0" );
    api().spoofActivate();
}


void Spoof_7_17_3a :: init()
{
    _name = "7.17.3a";
    _comment = "Safety protects against rbc valve in unknown position during collection";
    _timeout = 25;
}
void Spoof_7_17_3a :: help()
{
    LOG_INFO( "- start spoof during collect" );
    LOG_INFO( "- verify safety alarms with Valve Error after 20 seconds" );
}
void Spoof_7_17_3a :: run()
{
    api().spoofData( "SHwStates", "rbcValve", "HW_VALVE_UNKNOWN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}


void Spoof_7_17_3b :: init()
{
    _name = "7.17.3b";
    _comment = "Safety protects against rbc valve in open position during collection";
    _timeout = 25;
}
void Spoof_7_17_3b :: help()
{
    LOG_INFO( "- start spoof during collect" );
    LOG_INFO( "- verify safety alarms with Valve Error after 20 seconds" );
}
void Spoof_7_17_3b :: run()
{
    api().spoofData( "SHwStates", "rbcValve", "HW_VALVE_OPEN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}


void Spoof_SValve :: init()
{
    _name = "SValve";
    _comment = "Safety protects valve in incorrect position";
    _timeout = 25;
}
void Spoof_SValve :: help()
{
    LOG_INFO( "- start spoof according to test directions" );
    LOG_INFO( "- include -var1 valve -var2 position in your command line" );
    LOG_INFO( "- where valve is rbcValve, plasmaValve, or plateletValve");
    LOG_INFO( "- and where position is HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN," );
    LOG_INFO( "- or HW_VALVE_UNKNOWN" );
    LOG_INFO( "- without paramters spoofs rbcValve state to unknown");
}
void Spoof_SValve :: run()
{
    api().spoofData( "SHwStates", "rbcValve", "HW_VALVE_UNKNOWN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}

void Spoof_SValve :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"rbcValve") == 0) || (strcmp(Var1, "plasmaValve") == 0) ||
        (strcmp(Var1,"plateletValve")== 0)) 
    {

        if ((strcmp(Var2,"HW_VALVE_COLLECT") == 0) || (strcmp(Var2,"HW_VALVE_OPEN") == 0) ||
            (strcmp(Var2, "HW_VALVE_RETURN") == 0) || (strcmp(Var2,"HW_VALVE_UNKNOWN") == 0)) 
        {
            api().spoofData( "SHwStates", Var1, Var2 );
            api().spoofActivate();

            LOG_INFO( "- should not alarm for 20 seconds" );
            api().sleepMsec( 20000 );
            LOG_INFO( "- should now alarm (if system is going to)" );

        }
        else
        {
            LOG_INFO("Invalid var2 position value so not spoofing" );
        }
    }
    else
    {
        LOG_INFO("Invalid var1 valve so not spoofing");
    }
}


void Spoof_7_17_4 :: init()
{
    _name = "7.17.4";
    _comment = "Safety protects against plasma valve open during rinseback";
    _timeout = 70;
}
void Spoof_7_17_4 :: help()
{
    LOG_INFO( "- setup ac ratio of 6, infusion of 6" );
    LOG_INFO( "- select 3L donor, 40 Hct" );
    LOG_INFO( "- pause in 1st minute of rinseback and run spoof" );
    LOG_INFO( "- continue when spoof ends" );
    LOG_INFO( "- verify safety alarms with Overdelivery of AC" );
}
void Spoof_7_17_4 :: run()
{
    api().spoofData( "SHwStates", "plasmaValve", "HW_VALVE_OPEN" );
    api().spoofActivate();

    api().sleepMsec( 30000 );
    api().spoofDisable();
    api().sleepMsec( 10000 );
    api().spoofEnable();
}


void Spoof_7_17_5 :: init()
{
    _name = "7.17.5";
    _comment = "Safety protects against plasma valve open during plasma collect";
    _timeout = 25;
}
void Spoof_7_17_5 :: help()
{
    LOG_INFO( "- start spoof during plasma collect" );
    LOG_INFO( "- verify safety alarms with Valve Error after 20 seconds" );
}
void Spoof_7_17_5 :: run()
{
    api().spoofData( "SHwStates", "plasmaValve", "HW_VALVE_OPEN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}


void Spoof_7_17_6 :: init()
{
    _name = "7.17.6";
    _comment = "Safety protects against platelet valve open during rinseback";
    _timeout = 25;
}
void Spoof_7_17_6 :: help()
{
    LOG_INFO( "- start spoof during rinseback" );
    LOG_INFO( "- verify safety alarms with Valve Error after 20 seconds" );
}
void Spoof_7_17_6 :: run()
{
    api().spoofData( "SHwStates", "plateletValve", "HW_VALVE_OPEN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}


void Spoof_7_17_7a :: init()
{
    _name = "7.17.7a";
    _comment = "Safety protects against platelet valve in unknown position during collection";
    _timeout = 25;
}
void Spoof_7_17_7a :: help()
{
    LOG_INFO( "- start spoof during collection" );
    LOG_INFO( "- verify safety alarms with Valve Error after 20 seconds" );
}
void Spoof_7_17_7a :: run()
{
    api().spoofData( "SHwStates", "plateletValve", "HW_VALVE_UNKNOWN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}


void Spoof_7_17_7b :: init()
{
    _name = "7.17.7b";
    _comment = "Safety protects against platelet valve in open position during collection";
    _timeout = 25;
}
void Spoof_7_17_7b :: help()
{
    LOG_INFO( "- start spoof during collection" );
    LOG_INFO( "- verify safety alarms with Valve Error after 20 seconds" );
}
void Spoof_7_17_7b :: run()
{
    api().spoofData( "SHwStates", "plateletValve", "HW_VALVE_OPEN" );
    api().spoofActivate();

    LOG_INFO( "- should not alarm for 20 seconds" );
    api().sleepMsec( 20000 );
    LOG_INFO( "- should now alarm" );
}


void Spoof_7_18_1 :: init()
{
    _name = "7.18.1";
    _comment = "Safety protects against control not responding to PAUSE";
    _timeout = 30;
}
void Spoof_7_18_1 :: help()
{
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- press PAUSE button" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_18_1 :: run()
{
    api().spoofData( "CHwStates", "event", "CHW_NORMAL_UPDATE" );
    api().spoofData( "CHwStates", "pauseSwitch", "HW_SWITCH_DISABLED" );
    api().spoofActivate();
    LOG_INFO( "press PAUSE button now" );
}


void Spoof_7_18_2 :: init()
{
    _name = "7.18.2";
    _comment = "Safety protects against control not responding to STOP";
    _timeout = 30;
}
void Spoof_7_18_2 :: help()
{
    LOG_INFO( "- start spoof at any time during a run" );
    LOG_INFO( "- press STOP button" );
    LOG_INFO( "- verify safety alarms with Software Failure" );
}
void Spoof_7_18_2 :: run()
{
    api().spoofData( "CHwStates", "event", "CHW_NORMAL_UPDATE" );
    api().spoofData( "CHwStates", "stopSwitch", "HW_SWITCH_DISABLED" );
    api().spoofActivate();
    LOG_INFO( "press STOP button now" );
}


void Spoof_7_19_1 :: init()
{
    _name = "7.19.1";
    _comment = "Safety protects against RBC overdepletion";
    _timeout = 30*60;
}
void Spoof_7_19_1 :: help()
{
    LOG_INFO( "- Safety limits whole blood processed while RBC valve is not in return" );
    LOG_INFO( "- to not let donor Hct fall below 0.28" );
    LOG_INFO( "- makes worst case assumption that Hct of incoming blood remains constant" );
    LOG_INFO( "- assumes set residual of 26ml RBC and 20ml of whole blood lost to sample bag" );
    LOG_INFO( "- setup and start spoof per test procedure" );
    LOG_INFO( "- verify safety alarms with Blood Volume Processed Too High" );
}
void Spoof_7_19_1 :: run()
{
    api().spoofData( "SHwStates", "rbcValve", "HW_VALVE_COLLECT" );
    api().spoofActivate();
}


void Spoof_7_19_2 :: init()
{
    _name = "7.19.2";
    _comment = "Safety protects against Platelet overdepletion";
    _timeout = 30000;
}
void Spoof_7_19_2 :: help()
{
    LOG_INFO( "- Safety limits whole blood processed while in draw portion of platelet collect to 2.2*TBV" );
    LOG_INFO( "- to not let donor postcount fall below 20,000" );
    LOG_INFO( "- setup a platelet only collection, 4.0E11 in 200ml on small donor, TBV=3L precount=400" );
    LOG_INFO( "- config for high ratio and high infusion" );
    LOG_INFO( "- start spoof during platelet collection" );
    LOG_INFO( "- verify safety alarms with Blood Volume Processed Too High" );
}
void Spoof_7_19_2 :: run()
{
    // Midrun substate
    // really slow collect flow to avoid hypovolemia
    api().spoofData( "Predict_SubstateInfo", "QcD_6", "1.0" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_6", "1.0" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_6", "0.0" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_6", "0.0" ); 
    api().spoofData( "Predict_SubstateInfo", "Vtarget_6", "8000" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_6", "VBP" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_6", "NOCOLLECT" ); 

    api().spoofActivate();
}

void Spoof_7_23_6 :: init()
{
    _name = "7.23.6";
    _comment = "Safety protects against ac infusion limit when replacement fluid is not delivered";
    _timeout = 60*15;
}
void Spoof_7_23_6 :: help()
{
    LOG_INFO( "- set ac infusion = 6, Ratio = 6, RBC ac ratio = 8.14, replacement = 120" );
    LOG_INFO( "- select small donor, AMAP or large RBC product" );
    LOG_INFO( "- load DRBC (black stamp) set" );
    LOG_INFO( "- start spoof during plasma or rbc collection" );
    LOG_INFO( "- verify rundata for elevated ac infusion rate" );
    LOG_INFO( "- verify AC infusion alarm if AC infusion exceeds 1.3 for 3 cycles" );
}
void Spoof_7_23_6 :: run()
{
    MySpoofs :: plateletMultiplier( 0 );
    api().spoofActivate();
}


void Spoof_CCentrifugeRPM :: init()
{
    _name = "CCentrifugeRPM";
    _comment = "Spoofs Control centrifugeRPM status ";
    _timeout = 120;
}
void Spoof_CCentrifugeRPM :: help()
{
    LOG_INFO( "- Include -var1 xxxx in command line " );
    LOG_INFO( "- where xxxx is an integer value range 0-3200" );
    LOG_INFO( "- with no parameters spoofs RPM=0" );
    LOG_INFO( "- default timeout is 120 seconds" );
}
void Spoof_CCentrifugeRPM :: run()
{
    LOG_INFO( "since no parameter used, spoofing centifugeRPM=0 to Control" );
    api().spoofData( "CHwStates", "centrifugeRPM", "0" );
    api().spoofActivate();
}
void Spoof_CCentrifugeRPM :: run_generic(const char* Var1, const char* Var2)
{
    int RPM = atoi(Var1);
    if (RPM >= 0 && RPM <=3200)
    {
        LOG_INFO( "spoofing control centrifugeRPM status to %s", Var1 );
        api().spoofData( "CHwStates", "centrifugeRPM", Var1 );
        api().spoofActivate();
    }
}


void Spoof_SCentrifugeRPM :: init()
{
    _name = "SCentrifugeRPM";
    _comment = "Spoofs Safety centrifugeRPM status";
    _timeout = 120;
}
void Spoof_SCentrifugeRPM :: help()
{
    LOG_INFO( "- Include -var1 xxxx in command line " );
    LOG_INFO( "- where xxxx is an integer value range 0-3200" );
    LOG_INFO( "- with no parameters spoofs RPM=0" );
    LOG_INFO( "- default timeout is 120 seconds" );
}
void Spoof_SCentrifugeRPM :: run()
{
    LOG_INFO( "since no parameter used, spoofing centifugeRPM=0 to Safety" );
    api().spoofData( "SHwStates", "centrifugeRPM", "0" );
    api().spoofActivate();
}
void Spoof_SCentrifugeRPM :: run_generic(const char* Var1, const char* Var2)
{
    int RPM = atoi(Var1);
    if (RPM >= 0 && RPM <=3200)
    {
        LOG_INFO( "spoofing safety centrifugeRPM status to %s", Var1 );
        api().spoofData( "SHwStates", "centrifugeRPM", Var1 );
        api().spoofActivate();
    }
}


void Spoof_8_3_1 :: init()
{
    _name = "8.3.1";
    _comment = "Control protects against 64V too high (exceeding 84V)";
    _timeout = 2;
}
void Spoof_8_3_1 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 64V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_1 :: run()
{
    api().spoofData( "CHwStates", "sixtyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_64V, 84.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_2 :: init()
{
    _name = "8.3.2";
    _comment = "Control protects against 64V too low (less than 44V)";
    _timeout = 2;
}
void Spoof_8_3_2 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 64V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_2 :: run()
{
    api().spoofData( "CHwStates", "sixtyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_64V, 43.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_3 :: init()
{
    _name = "8.3.3";
    _comment = "Control protects against 24V too high (exceeding 28.8V)";
    _timeout = 2;
}
void Spoof_8_3_3 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 24V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_3 :: run()
{
    api().spoofData( "CHwStates", "twentyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 28.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_4 :: init()
{
    _name = "8.3.4";
    _comment = "Control protects against 24V too low (less than 19.2V)";
    _timeout = 2;
}
void Spoof_8_3_4 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 24V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_4 :: run()
{
    api().spoofData( "CHwStates", "twentyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 19.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_5 :: init()
{
    _name = "8.3.5";
    _comment = "Control protects against 12V too high (exceeding 13V)";
    _timeout = 2;
}
void Spoof_8_3_5 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 12V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_5 :: run()
{
    api().spoofData( "CHwStates", "twelveVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_12V, 13.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_6 :: init()
{
    _name = "8.3.6";
    _comment = "Control protects against 12V too low (less than 11V)";
    _timeout = 2;
}
void Spoof_8_3_6 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 12V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_6 :: run()
{
    api().spoofData( "CHwStates", "twelveVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_12V, 10.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_7 :: init()
{
    _name = "8.3.7";
    _comment = "Control protects against -12V too high (exceeding -13V)";
    _timeout = 2;
}
void Spoof_8_3_7 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with -12V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_7 :: run()
{
    api().spoofData( "CHwStates", "minusTwelvePS", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_MINUS12V, -13.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_8 :: init()
{
    _name = "8.3.8";
    _comment = "Control protects against -12V too low (above -11V)";
    _timeout = 2;
}
void Spoof_8_3_8 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with -12V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_8 :: run()
{
    api().spoofData( "CHwStates", "minusTwelvePS", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_MINUS12V, -10.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_9 :: init()
{
    _name = "8.3.9";
    _comment = "Control protects against 64V too high (above 84V)";
    _timeout = 120;
}
void Spoof_8_3_9 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 64V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_9 :: run()
{
    LOG_INFO( "spoofing 64V status to 84.1V" );
    api().spoofData( "CHwStates", "sixtyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_64V, 84.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_10 :: init()
{
    _name = "8.3.10";
    _comment = "Control protects against 64V too low (below 44V)";
    _timeout = 120;
}
void Spoof_8_3_10 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 64V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_10 :: run()
{
    LOG_INFO( "spoofing 64V status to 43.9V" );
    api().spoofData( "CHwStates", "sixtyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_64V, 43.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_11 :: init()
{
    _name = "8.3.11";
    _comment = "Control protects against 24V too high (above 28.8V)";
    _timeout = 120;
}
void Spoof_8_3_11 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 24V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_11 :: run()
{
    LOG_INFO( "spoofing 24V status to 28.9V" );
    api().spoofData( "CHwStates", "twentyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 28.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_12 :: init()
{
    _name = "8.3.12";
    _comment = "Control protects against 24V too low (below 19.2V)";
    _timeout = 120;
}
void Spoof_8_3_12 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 24V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_12 :: run()
{
    LOG_INFO( "spoofing 24V status to 19.1V" );
    api().spoofData( "CHwStates", "twentyFourVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 19.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_13 :: init()
{
    _name = "8.3.13";
    _comment = "Control protects against 12V too high (above 13V)";
    _timeout = 120;
}
void Spoof_8_3_13 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 12V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_13 :: run()
{
    LOG_INFO( "spoofing 12V status to 13.1V" );
    api().spoofData( "CHwStates", "twelveVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_12V, 13.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_14 :: init()
{
    _name = "8.3.14";
    _comment = "Control protects against 12V too low (below 11V)";
    _timeout = 120;
}
void Spoof_8_3_14 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with 12V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_14 :: run()
{
    LOG_INFO( "spoofing 12V status to 10.9V" );
    api().spoofData( "CHwStates", "twelveVoltSupply", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_12V, 10.9 ) );
    api().spoofActivate();
}


void Spoof_8_3_15 :: init()
{
    _name = "8.3.15";
    _comment = "Control protects against -12V too high (below -13V)";
    _timeout = 120;
}
void Spoof_8_3_15 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with -12V Supply Too High" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_15 :: run()
{
    LOG_INFO( "spoofing -12V status to -13.1V" );
    api().spoofData( "CHwStates", "minusTwelvePS", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_MINUS12V, -13.1 ) );
    api().spoofActivate();
}


void Spoof_8_3_16 :: init()
{
    _name = "8.3.16";
    _comment = "Control protects against -12V too low (above -11V)";
    _timeout = 120;
}
void Spoof_8_3_16 :: help()
{
    LOG_INFO( "- run spoof during a procedure" );
    LOG_INFO( "- verify control alarm with -12V Supply Too Low" );
    LOG_INFO( "- verify centrifuge stops" );
    LOG_INFO( "- verify continue/rinseback not offered" );
    LOG_INFO( "- verify that when endrun is pressed, that alarm re-asserts" );
    LOG_INFO( "- once spoof ends, verify endrun proceeds to donor disconnect test" );
    LOG_INFO( "- verify can disconnect donor and unload set" );
}
void Spoof_8_3_16 :: run()
{
    LOG_INFO( "spoofing -12V status to -10.9V" );
    api().spoofData( "CHwStates", "minusTwelvePS", 0, MySpoofs::psAdc( MySpoofs::SUPPLY_MINUS12V, -10.9 ) );
    api().spoofActivate();
}


void Spoof_DonorVitals :: init()
{
    _name = "DonorVitals";
    _comment = "Safety uses donor vitals to provide protection";
    _timeout = 2*60;
}
void Spoof_DonorVitals :: help()
{
    LOG_INFO( "- Include -var1 vitalname -var 2 xxxx in the command line" );
    LOG_INFO( "- where vitalname is DonorSex, DonorWeight, DonorHeight, ProcDonorTBV, DonorHCT" );
    LOG_INFO( "- PltPrecount, DonorWeightUnits, or DonorHeightUnits and xxxx is the value to spoof" );
    LOG_INFO( "- without parameters spoof invalid donor gender as follows:" );
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor gender" );
    LOG_INFO( "- verify safety alarms Invalid Donor Gender" );
}
void Spoof_DonorVitals :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorSex", "0" );
    api().spoofActivate();
}
void Spoof_DonorVitals :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"DonorSex") == 0) || (strcmp(Var1, "DonorWeightUnits") == 0) ||
        (strcmp(Var1,"DonorHeightUnits")== 0) || (strcmp(Var1,"DonorWeight") == 0) ||
        (strcmp(Var1,"DonorHeight")== 0) || (strcmp(Var1,"ProcDonorTBV") == 0) ||
        (strcmp(Var1,"DonorHct") == 0) || (strcmp(Var1,"PltPrecount")==0)) 
    {
        LOG_INFO( "Spoofing %s to %s", Var1, Var2 );
        api().spoofData( "DVitalsFromProc", Var1, Var2 );
        api().spoofActivate();
    }
    else
    {
        LOG_INFO( "Invalid vital name for var1 so not spoofing" );
        LOG_INFO( "Var1 must be  DonorSex, DonorWeight, DonorHeight, ProcDonorTBV, DonorHCT" );
        LOG_INFO( " PltPrecount, DonorWeightUnits, or DonorHeightUnits" );
    }
}


void Spoof_8_4_3 :: init()
{
    _name = "8.4.3";
    _comment = "Safety protects against invalid donor weight units";
    _timeout = 2*60;
}
void Spoof_8_4_3 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor weight" );
    LOG_INFO( "- verify safety alarms" );
}
void Spoof_8_4_3 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorWeightUnits", "-1" );
    api().spoofActivate();
}


void Spoof_8_4_4 :: init()
{
    _name = "8.4.4";
    _comment = "Safety protects against invalid donor height units";
    _timeout = 2*60;
}
void Spoof_8_4_4 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor height" );
    LOG_INFO( "- verify safety alarm" );
}
void Spoof_8_4_4 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorHeightUnits", "2" );
    api().spoofActivate();
}


void Spoof_8_4_5 :: init()
{
    _name = "8.4.5";
    _comment = "Safety protects against donor weight too low (less than 50lbs)";
    _timeout = 2*60;
}
void Spoof_8_4_5 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor weight" );
    LOG_INFO( "- verify safety alarms Donor Weight Too Low" );
}
void Spoof_8_4_5 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorWeight", "49" );
    api().spoofActivate();
}


void Spoof_8_4_6 :: init()
{
    _name = "8.4.6";
    _comment = "Safety protects against donor weight too high (greater than 500lbs)";
    _timeout = 2*60;
}
void Spoof_8_4_6 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor weight" );
    LOG_INFO( "- verify safety alarms Donor Weight Too High" );
}
void Spoof_8_4_6 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorWeight", "501" );
    api().spoofActivate();
}


void Spoof_8_4_7 :: init()
{
    _name = "8.4.7";
    _comment = "Safety protects against donor height too low (less than 48in.)";
    _timeout = 2*60;
}
void Spoof_8_4_7 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor height" );
    LOG_INFO( "- verify safety alarms Donor Height Too Low" );
}
void Spoof_8_4_7 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorHeight", "47" );
    api().spoofActivate();
}


void Spoof_8_4_8 :: init()
{
    _name = "8.4.8";
    _comment = "Safety protects against donor height too high (greater than 8ft)";
    _timeout = 2*60;
}
void Spoof_8_4_8 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor height" );
    LOG_INFO( "- verify safety alarms Donor Height Too High" );
}
void Spoof_8_4_8 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorHeight", "97" );
    api().spoofActivate();
}


void Spoof_8_4_9 :: init()
{
    _name = "8.4.9";
    _comment = "Safety protects against TBV mismatch (Proc too high)";
    _timeout = 2*60;
}
void Spoof_8_4_9 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor info" );
    LOG_INFO( "- verify safety alarms Invalid TBV" );
}
void Spoof_8_4_9 :: run()
{
    api().spoofData( "DVitalsFromProc", "ProcDonorTBV", 1, 51 );
    api().spoofActivate();
}


void Spoof_8_4_9_1 :: init()
{
    _name = "8.4.9.1";
    _comment = "Safety protects against TBV mismatch (Proc too high)";
    _timeout = 2*60;
}
void Spoof_8_4_9_1 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor info" );
    LOG_INFO( "- verify safety alarms Invalid TBV" );
}
void Spoof_8_4_9_1 :: run()
{
    api().spoofData( "DVitalsFromProc", "ProcDonorTBV", 1, 24 );
    api().spoofActivate();
}

void Spoof_8_4_9_2 :: init()
{
    _name = "8.4.9.2";
    _comment = "Safety protects against TBV mismatch (Proc too high)";
    _timeout = 2*60;
}
void Spoof_8_4_9_2 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor info" );
    LOG_INFO( "- verify safety alarms Invalid TBV" );
}
void Spoof_8_4_9_2 :: run()
{
    api().spoofData( "DVitalsFromProc", "ProcDonorTBV", 1, 26 );
    api().spoofActivate();
}

void Spoof_8_4_9_3 :: init()
{
    _name = "8.4.9.3";
    _comment = "Safety protects against TBV mismatch (Proc too low)";
    _timeout = 2*60;
}
void Spoof_8_4_9_3 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- enter donor info" );
    LOG_INFO( "- verify safety alarms Invalid TBV" );
}
void Spoof_8_4_9_3 :: run()
{
    api().spoofData( "DVitalsFromProc", "ProcDonorTBV", 1, -26 );
    api().spoofActivate();
}


void Spoof_8_4_10 :: init()
{
    _name = "8.4.10";
    _comment = "Safety protects against donor Hct too low ( < 30.0 )";
    _timeout = 2*60;
}
void Spoof_8_4_10 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- attempt to enter donor Hct" );
    LOG_INFO( "- verify safety alarms Invalid Donor HCT" );
}
void Spoof_8_4_10 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorHct", "29" );
    api().spoofActivate();
}


void Spoof_8_4_11 :: init()
{
    _name = "8.4.11";
    _comment = "Safety protects against donor Hct too high ( > 55 )";
    _timeout = 2*60;
}
void Spoof_8_4_11 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- attempt to enter donor Hct" );
    LOG_INFO( "- verify safety alarms Invalid Donor HCT" );
}
void Spoof_8_4_11 :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorHct", "56" );
    api().spoofActivate();
}


void Spoof_8_5_1 :: init()
{
    _name = "8.5.1";
    _comment = "Safety protects against using a wet set (fluid at low sensor before blood prime)";
    _timeout = 5;
}
void Spoof_8_5_1 :: help()
{
    LOG_INFO( "- run spoof prior to blood prime" );
    LOG_INFO( "- verify safety Fluid Too Soon alarm" );
    LOG_INFO( "- verify procedure cannot be continued" );
}
void Spoof_8_5_1 :: run()
{
    api().spoofData( "SHwStates", "reservoir", "SHW_RESERVOIR_LOW" );
    api().spoofData( "SHwStates", "event", "SHW_RESERVOIR_EVENT" );
    api().setSpoofedMessageLimit( 1 );
    api().spoofActivate();
}


void Spoof_8_5_5 :: init()
{
    _name = "8.5.5";
    _comment = "Safety protects against platelet bag overfill (1560ml limit)";
    _timeout = 2*60*60;
}
void Spoof_8_5_5 :: help()
{
    LOG_INFO( "- setup large 12L donor platelet only run" );
    LOG_INFO( "- start spoof before or during platelet collect" );
    LOG_INFO( "- verify safety Hypovolemia alarm before platelet collection ends" );
}
void Spoof_8_5_5 :: run()
{
    LOG_INFO( "running platelet collection to 1600ml to platelet bag" );

    // PCA substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_5", "1600" );
    api().spoofData( "Predict_SubstateInfo", "VtargetType_5", "VCOLBAG" );
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_5", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlateletValve_5", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_5", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_5", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_5", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_5", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_5", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_5", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_5", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_5", "333.99" ); 

    // Midrun substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_6", "1600" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_6", "VCOLBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_6", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlateletValve_6", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_6", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_6", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_6", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_6", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_6", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_6", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_6", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_6", "333.99" ); 

    // PIR substates
    api().spoofData( "Predict_SubstateInfo", "Vtarget_7", "1600" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_7", "VCOLBAG" );
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_7", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlateletValve_7", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_7", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_7", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_7", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_7", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_7", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_7", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_7", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_7", "333.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Vtarget_8", "1600" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_8", "VCOLBAG" );
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_8", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlateletValve_8", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_8", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_8", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_8", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_8", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_8", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_8", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_8", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_8", "333.99" ); 

    // EPCA substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_9", "1600" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_9", "VCOLBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlateletValve_9", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_9", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_9", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_9", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_9", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_9", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_9", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_9", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_9", "333.99" ); 

    api().spoofActivate();
}


void Spoof_8_5_6 :: init()
{
    _name = "8.5.6";
    _comment = "Safety protects against plasma bag overfill (1300ml limit)";
    _timeout = 2*60*60;
}
void Spoof_8_5_6 :: help()
{
    LOG_INFO( "- setup large 12L donor AMAP plasma only run" );
    LOG_INFO( "- start spoof before or during plasma collect" );
    LOG_INFO( "- verify safety Hypovolemia alarm before plasma collection ends" );
}
void Spoof_8_5_6 :: run()
{
    LOG_INFO( "running plasma collection to 1350ml of collected plasma" );

    // PrePlateletPlasma substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_2", "1350" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_2", "VPBAG" );
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_2", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlasmaValve_2", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_2", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_2", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_2", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_2", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_2", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_2", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_2", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_2", "333.99" ); 

    // PlateletPlasma substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_4", "1350" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_4", "VPBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_4", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlasmaValve_4", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_4", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_4", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_4", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_4", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_4", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_4", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_4", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_4", "333.99" ); 

    // ExtendedPlasma substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_9", "1350" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_9", "VPBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "PlasmaValve_9", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_9", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_9", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_9", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_9", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_9", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_9", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_9", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_9", "333.99" ); 

    api().spoofActivate();
}


void Spoof_8_5_7a :: init()
{
    _name = "8.5.7a";
    _comment = "Safety protects against rbc bag overfill (650 white set)";
    _timeout = 2*60*60;
}
void Spoof_8_5_7a :: help()
{
    LOG_INFO( "- setup large 12L donor rbc only run on white set" );
    LOG_INFO( "- start spoof before or during rbc collect" );
    LOG_INFO( "- verify safety Hypovolemia alarm before rbc collection ends" );
}
void Spoof_8_5_7a :: run()
{
    LOG_INFO( "running rbc collection to 700ml of collected rbc" );

    // rbc substate - white set
    api().spoofData( "Predict_SubstateInfo", "Vtarget_11", "700" );
    api().spoofData( "Predict_SubstateInfo", "VtargetType_11", "VRBCBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_11", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "RBCValve_11", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_11", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_11", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_11", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_11", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_11", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_11", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_11", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_11", "333.99" ); 

    api().spoofActivate();
}


void Spoof_8_5_7b :: init()
{
    _name = "8.5.7b";
    _comment = "Safety protects against rbc bag overfill (1300 black set)";
    _timeout = 2*60*60;
}
void Spoof_8_5_7b :: help()
{
    LOG_INFO( "- setup large 12L donor rbc only run on black set" );
    LOG_INFO( "- start spoof before or during rbc collect" );
    LOG_INFO( "- verify safety Hypovolemia alarm before rbc collection ends" );
}
void Spoof_8_5_7b :: run()
{
    LOG_INFO( "running rbc collection to 1350ml of collected rbc" );

    // rbc substate - black set
    api().spoofData( "Predict_SubstateInfo", "Vtarget_3", "1350" );
    api().spoofData( "Predict_SubstateInfo", "VtargetType_3", "VRBCBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_3", "NOCOLLECT" );
    api().spoofData( "Predict_SubstateInfo", "RBCValve_3", "HAL_VALVE_COLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "Qac_3", "8.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcD_3", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_3", "4.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinD_3", "119.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QinR_3", "84.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_3", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_3", "20.99" ); 
    api().spoofData( "Predict_SubstateInfo", "Qrp_3", "333.99" ); 

    api().spoofActivate();
}


void Spoof_8_6_11 :: init()
{
    _name = "8.6.11";
    _comment = "Control ignores early transient fluid at low level sensor";
    _timeout = 20;
}
void Spoof_8_6_11 :: help()
{
    LOG_INFO( "- start spoof at start of return line prime" );
    LOG_INFO( "- verify no alarms" );
}
void Spoof_8_6_11 :: run()
{
    LOG_INFO( "spoofing empty reservoir for 5 seconds" );
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_EMPTY" );
    api().spoofActivate();
    api().sleepMsec( 5000 );

    LOG_INFO( "spoofing fluid for 1 second" );
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_MIDDLE" );
    api().sleepMsec( 1000 );
    
    LOG_INFO( "spoofing empty reservoir for 9 seconds" );
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_EMPTY" );
    api().sleepMsec( 9000 );
    
    LOG_INFO( "spoofing fluid for 1 second" );
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_MIDDLE" );
    api().sleepMsec( 1000 );
    
    LOG_INFO( "spoofing empty reservoir for 4 more seconds" );
    api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_EMPTY" );
}


void Spoof_8_7_4 :: init()
{
    _name = "8.7.4";
    _comment = "Control side Centrifuge Hardware Failure alarm";
    _timeout = 60;
}
void Spoof_8_7_4 :: help()
{
    LOG_INFO( "- start spoof just prior to cassette load" );
    LOG_INFO( "- start the cassette load" );
    LOG_INFO( "- while cassette is loading, rotate centrifuge slowly counter clockwise" );
    LOG_INFO( "- verify Centrifuge Hardware Failure alarm" );
    LOG_INFO( "- verify only option is end-run" );
}
void Spoof_8_7_4 :: run()
{
    api().spoofData( "CHwStates", "doorLocks", "HW_DOOR_CLOSED_AND_LOCKED" );
    api().spoofData( "SHwStates", "doorLocks", "HW_DOOR_CLOSED_AND_LOCKED" );
    api().spoofData( "CHwOrders", "centrifugeRPM", "0" );
    api().spoofActivate();
}


void Spoof_8_7_8 :: init()
{
    _name = "8.7.8";
    _comment = "Control protects against slowly running centrifuge at startup";
    _timeout = 60*3;
}
void Spoof_8_7_8 :: help()
{
    LOG_INFO( "- start spoof at Trima splash screen prior to donor info screen" );
    LOG_INFO( "- verify control alarms with Power Test Failure shortly after donor info screen appears" );
    LOG_INFO( "- verify procedure cannot be continued" );
}
void Spoof_8_7_8 :: run()
{
    api().spoofData( "CHwStates", "centrifugeRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_7_12 :: init()
{
    _name = "8.7.12";
    _comment = "Control alarms if centrifuge does not make speed";
    _timeout = 600;
}
void Spoof_8_7_12 :: help()
{
    LOG_INFO( "- start spoof when centrifuge first ramps up above 200" );
    LOG_INFO( "- verify Hardware Failure alarm by the time spoof ends" );
    LOG_INFO( "- verify procedure can be continued" );
}
void Spoof_8_7_12 :: run()
{
    api().spoofData( "CHwOrders", "centrifugeRPM", "2710" );
    api().spoofActivate();
}


void Spoof_8_7_13a :: init()
{
    _name = "8.7.13a";
    _comment = "Control Hardware Failure alarm if ac pump is not running as expected";
    _timeout = 14;
}
void Spoof_8_7_13a :: help()
{
    LOG_INFO( "- start spoof at start of a draw cycle" );
    LOG_INFO( "- verify Motor Hardware Failure alarm by the time spoof ends" );
    LOG_INFO( "- verify procedure can be continued" );
}
void Spoof_8_7_13a :: run()
{
    LOG_INFO( "spoofing ac slow" );
    api().spoofData( "CHwOrders", "acRPM", 0.90, -0.7 );
    api().spoofActivate();
}


void Spoof_8_7_13b :: init()
{
    _name = "8.7.13b";
    _comment = "Control Hardware Failure alarm if inlet pump is not running as expected";
    _timeout = 14;
}
void Spoof_8_7_13b :: help()
{
    LOG_INFO( "- start spoof at start of a draw cycle" );
    LOG_INFO( "- verify Motor Hardware Failure alarm by the time spoof ends" );
    LOG_INFO( "- verify procedure can be continued" );
}
void Spoof_8_7_13b :: run()
{
    LOG_INFO( "spoofing inlet fast" );
    api().spoofData( "CHwOrders", "inletRPM", 1.10, 0.7 );
    api().spoofActivate();
}


void Spoof_8_7_13c :: init()
{
    _name = "8.7.13c";
    _comment = "Control does not alarm if pumps stall for less than 5 seconds";
    _timeout = 3;
}
void Spoof_8_7_13c :: help()
{
    LOG_INFO( "- start during draw or return" );
    LOG_INFO( "- verify no alarm" );
}
void Spoof_8_7_13c :: run()
{
    LOG_INFO( "stalling all pumps" );
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofData( "CHwOrders", "inletRPM", "0" );
    api().spoofData( "CHwOrders", "returnRPM", "0" );
    api().spoofData( "CHwOrders", "plasmaRPM", "0" );
    api().spoofData( "CHwOrders", "plateletRPM", "0" );
    api().spoofActivate();
}


void Spoof_8_7_13d :: init()
{
    _name = "8.7.13d";
    _comment = "Control Motor Hardware Failure alarm if plasma pump not running as expected";
    _timeout = 14;
}
void Spoof_8_7_13d :: help()
{
    LOG_INFO( "- start spoof during plasma collection" );
    LOG_INFO( "- verify Motor Hardware Failure alarm" );
    LOG_INFO( "- verify procedure can be continued" );
}
void Spoof_8_7_13d :: run()
{
    LOG_INFO( "spoofing plasma very slow" );
    api().spoofData( "CHwOrders", "plasmaRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_7_13e :: init()
{
    _name = "8.7.13e";
    _comment = "Control Motor Hardware Failure alarm if platelet pump not running as expected";
    _timeout = 14;
}
void Spoof_8_7_13e :: help()
{
    LOG_INFO( "- start spoof during platelet collection" );
    LOG_INFO( "- verify Motor Hardware Failure alarm" );
    LOG_INFO( "- verify procedure can be continued" );
}
void Spoof_8_7_13e :: run()
{
    LOG_INFO( "spoofing platelet fast" );
    api().spoofData( "CHwOrders", "plateletRPM", 1.10, 0.7 );
    api().spoofActivate();
}


void Spoof_8_7_13g :: init()
{
    _name = "8.7.13g";
    _comment = "Control does not alarm if pumps slow don't run as expected for < 5 seconds";
    _timeout = 3;
}
void Spoof_8_7_13g :: help()
{
    LOG_INFO( "- start spoof while paused" );
    LOG_INFO( "- verify no alarm" );
    LOG_INFO( "- verify procedure can be continued" );
}
void Spoof_8_7_13g :: run()
{
    LOG_INFO( "spoofing all pumps slow" );
    api().spoofData( "CHwOrders", "acRPM", "1" );
    api().spoofData( "CHwOrders", "inletRPM", "6" );
    api().spoofData( "CHwOrders", "returnRPM", "1" );
    api().spoofData( "CHwOrders", "plasmaRPM", "1" );
    api().spoofData( "CHwOrders", "plateletRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_8_1 :: init()
{
    _name = "8.8.1";
    _comment = "Control alarms if valve is out of position";
    _timeout = 15;
}
void Spoof_8_8_1 :: help()
{
    LOG_INFO( "- start spoof at 2 button screen" );
    LOG_INFO( "- verify control alarms Valve Out Of Position" );
    LOG_INFO( "- verify procedure can be continued after spoof ends" );
}
void Spoof_8_8_1 :: run()
{
    api().spoofData( "CHwStates", "rbcValve", "HW_VALVE_COLLECT" );
    api().spoofData( "CHwStates", "plasmaValve", "HW_VALVE_RETURN" );
    api().spoofData( "CHwStates", "plateletValve", "HW_VALVE_UNKNOWN" );
    api().spoofActivate();
}


void Spoof_CValve :: init()
{
    _name = "CValve";
    _comment = "Control alarms if control sees valve is out of position";
    _timeout = 15;
}
void Spoof_CValve :: help()
{
    LOG_INFO( "- start spoof according to test directions" );
    LOG_INFO( "- include -var1 valve -var2 position in your command line" );
    LOG_INFO( "- where valve is rbcValve, plasmaValve, or plateletValve");
    LOG_INFO( "- and where position is HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN," );
    LOG_INFO( "- or HW_VALVE_UNKNOWN" );
    LOG_INFO( "- without parameters spoofs platelet valve state to unknown" );
}
void Spoof_CValve :: run()
{
    api().spoofData( "CHwStates", "plateletValve", "HW_VALVE_UNKNOWN" );
    api().spoofActivate();
}


void Spoof_CValve :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"rbcValve") == 0) || (strcmp(Var1, "plasmaValve") == 0) ||
        (strcmp(Var1,"plateletValve")== 0)) 
    {

        if ((strcmp(Var2,"HW_VALVE_COLLECT") == 0) || (strcmp(Var2,"HW_VALVE_OPEN") == 0) ||
            (strcmp(Var2, "HW_VALVE_RETURN") == 0) || (strcmp(Var2,"HW_VALVE_UNKNOWN") == 0)) 
        {
            LOG_INFO( "spoofing %s to %s", Var1, Var2 );
            api().spoofData( "CHwStates", Var1, Var2 );
            api().spoofActivate();
        }
        else
        {
            LOG_INFO("Invalid var2 position value so not spoofing" );
        }
    }
    else
    {
        LOG_INFO("Invalid var1 valve so not spoofing");
    }
}

void Spoof_8_9_1 :: init()
{
    _name = "8.9.1";
    _comment = "Control allows minimal pump creep";
    _timeout = 60;
}
void Spoof_8_9_1 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify system does not alarm" );
}
void Spoof_8_9_1 :: run()
{
    LOG_INFO( "spoofing AC to 0.375RPM" );
    LOG_INFO( "spoofing return to 0.1RPM" );
    LOG_INFO( "spoofing platelet to 0.375RPM" );
    LOG_INFO( "spoofing plasma to 0.375RPM" );
    LOG_INFO( "spoofing inlet to 0.375RPM" );
    api().spoofData( "CHwOrders", "acRPM", "0.375" );
    api().spoofData( "CHwOrders", "returnRPM", "0.1" );
    api().spoofData( "CHwOrders", "plateletRPM", "0.375" );
    api().spoofData( "CHwOrders", "plasmaRPM", "0.375" );
    api().spoofData( "CHwOrders", "inletRPM", "0.375" );
    api().spoofActivate();
}


void Spoof_8_9_2 :: init()
{
    _name = "8.9.2";
    _comment = "Control T1 protects against pump creep";
    _timeout = 120;
}
void Spoof_8_9_2 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify Power Test Failure alarm" );
    LOG_INFO( "- continue and verify Power Test Failure alarm re-asserts" );
    LOG_INFO( "- after spoof ends, continue and verify procedure can proceed" );
}
void Spoof_8_9_2 :: run()
{
    LOG_INFO( "spoofing all pumps to 1 RPM" );
    api().spoofData( "CHwOrders", "acRPM", "1" );
    api().spoofData( "CHwOrders", "returnRPM", "1" );
    api().spoofData( "CHwOrders", "plateletRPM", "1" );
    api().spoofData( "CHwOrders", "plasmaRPM", "1" );
    api().spoofData( "CHwOrders", "inletRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_9_3 :: init()
{
    _name = "8.9.3";
    _comment = "Control power test protects against AC pump creep";
    _timeout = 120;
}
void Spoof_8_9_3 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify Power Test Failure alarm" );
    LOG_INFO( "- continue and verify Power Test Failure alarm re-asserts" );
    LOG_INFO( "- after spoof ends, continue and verify procedure can proceed" );
}
void Spoof_8_9_3 :: run()
{
    LOG_INFO( "spoofing AC to 1RPM" );
    api().spoofData( "CHwOrders", "acRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_9_4 :: init()
{
    _name = "8.9.4";
    _comment = "Control power test protects against return pump creep";
    _timeout = 120;
}
void Spoof_8_9_4 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify Power Test Failure alarm" );
    LOG_INFO( "- continue and verify Power Test Failure alarm re-asserts" );
    LOG_INFO( "- after spoof ends, continue and verify procedure can proceed" );
}
void Spoof_8_9_4 :: run()
{
    LOG_INFO( "spoofing return to 2RPM" );
    api().spoofData( "CHwOrders", "returnRPM", "2" );
    api().spoofActivate();
}


void Spoof_8_9_5 :: init()
{
    _name = "8.9.5";
    _comment = "Control power test protects against inlet pump creep";
    _timeout = 120;
}
void Spoof_8_9_5 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify Power Test Failure alarm" );
    LOG_INFO( "- continue and verify Power Test Failure alarm re-asserts" );
    LOG_INFO( "- after spoof ends, continue and verify procedure can proceed" );
}
void Spoof_8_9_5 :: run()
{
    LOG_INFO( "spoofing inlet to 1RPM" );
    api().spoofData( "CHwOrders", "inletRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_9_6 :: init()
{
    _name = "8.9.6";
    _comment = "Control power test protects against platelet pump creep";
    _timeout = 120;
}
void Spoof_8_9_6 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify Power Test Failure alarm" );
    LOG_INFO( "- continue and verify Power Test Failure alarm re-asserts" );
    LOG_INFO( "- after spoof ends, continue and verify procedure can proceed" );
}
void Spoof_8_9_6 :: run()
{
    LOG_INFO( "spoofing platelet to 1RPM" );
    api().spoofData( "CHwOrders", "plateletRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_9_7 :: init()
{
    _name = "8.9.7";
    _comment = "Control power test protects against plasma pump creep";
    _timeout = 120;
}
void Spoof_8_9_7 :: help()
{
    LOG_INFO( "- start spoof using cmd line option:  -servers 1" );
    LOG_INFO( "- start spoof when LED's go out" );
    LOG_INFO( "- verify Power Test Failure alarm" );
    LOG_INFO( "- continue and verify Power Test Failure alarm re-asserts" );
    LOG_INFO( "- after spoof ends, continue and verify procedure can proceed" );
}
void Spoof_8_9_7 :: run()
{
    LOG_INFO( "spoofing plasma to 1RPM" );
    api().spoofData( "CHwOrders", "plasmaRPM", "1" );
    api().spoofActivate();
}


void Spoof_8_9_11 :: init()
{
    _name = "8.9.11";
    _comment = "Safety alarms if cassette is not seen in UNKNOWN position prior to AC prime";
    _timeout = 10*60;
}
void Spoof_8_9_11 :: help()
{
    LOG_INFO( "- start spoof before loading cassette" );
    LOG_INFO( "- load cassette and proceed to AC prime" );
    LOG_INFO( "- verify Safety alarms prior to AC prime" );
}
void Spoof_8_9_11 :: run()
{
    api().spoofData( "SHwStates", "cassettePosition", "HW_CASSETTE_DOWN" );
    api().spoofActivate();
}


void Spoof_8_9_12 :: init()
{
    _name = "8.9.12";
    _comment = "Safety alarms if driver reports blink test failure";
    _timeout = 5;
}
void Spoof_8_9_12 :: help()
{
    LOG_INFO( "- run spoof to generate a blink test failure" );
    LOG_INFO( "NOTE: this does nothing to verify blink test is being run or its effectiveness" );
    LOG_INFO( "- verify Safety alarms" );
}
void Spoof_8_9_12 :: run()
{
    api().spoofData( "SHwStates", "valveLedTest", "0" );
    api().spoofActivate();
}


void Spoof_8_9_13 :: init()
{
    _name = "8.9.13";
    _comment = "Safety alarms if 24V is not seen off prior to donor connect";
    _timeout = 10*60;
}
void Spoof_8_9_13 :: help()
{
    LOG_INFO( "- start spoof a little before LEDs go out" );
    LOG_INFO( "- proceed to AC prime and donor connect" );
    LOG_INFO( "- verify Safety alarms prior to donor connect" );
}
void Spoof_8_9_13 :: run()
{
    api().spoofData( "CHwStates", "twentyFourVoltSwitched", 
		     0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 24 ) );
    api().spoofActivate();
}


void Spoof_8_9_14 :: init()
{
    _name = "8.9.14";
    _comment = "Safety alarms if 64V is not seen on in its power test";
    _timeout = 30;
}
void Spoof_8_9_14 :: help()
{
    LOG_INFO( "- start spoof just before cassette load" );
    LOG_INFO( "- press load cassette and verify Safety alarms" );
}
void Spoof_8_9_14 :: run()
{
    api().spoofData( "CHwStates", "sixtyFourVoltSwitched",
		     0, MySpoofs::psAdc( MySpoofs::SUPPLY_64V, 0 ) );
    api().spoofActivate();
}


void Spoof_8_9_15 :: init()
{
    _name = "8.9.15";
    _comment = "Safety alarms at transition to blood run if centrifuge has not moved";
    _timeout = 10*60;
}
void Spoof_8_9_15 :: help()
{
    LOG_INFO( "- start spoof before blood prime" );
    LOG_INFO( "- verify Safety alarms System Test Failure #32819 upon completion of return line prime" );
}
void Spoof_8_9_15 :: run()
{
    api().spoofData( "SHwStates", "centrifugeRPM", "0" );
    api().spoofActivate();
}


void Spoof_8_10_5a :: init()
{
    _name = "8.10.5a";
    _comment = "Control detects red out of range low during platelet collection";
    _timeout = 40;
}
void Spoof_8_10_5a :: help()
{
    LOG_INFO( "- start spoof during platelet collection" );
    LOG_INFO( "- verify control alarms with RBC Detector Calibration Failure" );
}
void Spoof_8_10_5a :: run()
{
    LOG_INFO( "spoofing red=15, green=14, verify no alarm" );
    api().spoofData( "CHwStates", "red", "15" );
    api().spoofData( "CHwStates", "green", "14" );
    api().spoofActivate();
    api().sleepMsec( 30000 );

    api().spoofData( "CHwStates", "red", "14" );
    LOG_INFO( "spoofing red to 14, expect alarm in ~2 seconds" );
    api().sleepMsec( 4000 );

    LOG_INFO( "verify RBC Detector Calibration Failure alarm" );
    api().spoofData( "CHwStates", "red", "15" );
    LOG_INFO( "after spoof ends, verify run can continue" );
}


void Spoof_8_10_5b :: init()
{
    _name = "8.10.5b";
    _comment = "Control detects red out of range low during platelet collection";
    _timeout = 40;
}
void Spoof_8_10_5b :: help()
{
    LOG_INFO( "- start spoof during platelet collection" );
    LOG_INFO( "- verify control alarms with RBC Detector Calibration Failure" );
}
void Spoof_8_10_5b :: run()
{
    LOG_INFO( "spoofing red=4600, green=4601, verify no alarm" );
    api().spoofData( "CHwStates", "red", "4600" );
    api().spoofData( "CHwStates", "green", "4601" );
    api().spoofActivate();
    api().sleepMsec( 30000 );

    api().spoofData( "CHwStates", "red", "4601" );
    LOG_INFO( "spoofing red to 4601, expect alarm in ~2 seconds" );
    api().sleepMsec( 4000 );

    LOG_INFO( "verify RBC Detector Calibration Failure alarm" );
    api().spoofData( "CHwStates", "red", "4600" );
    LOG_INFO( "after spoof ends, verify run can continue" );
}


void Spoof_8_10_5c :: init()
{
    _name = "8.10.5c";
    _comment = "Control detects red out of range low during plasma collection on black set";
    _timeout = 255;
}
void Spoof_8_10_5c :: help()
{
    LOG_INFO( "- start spoof during plasma collection on black set" );
    LOG_INFO( "- verify control alarms with RBC Detector Calibration Failure" );
}
void Spoof_8_10_5c :: run()
{
    LOG_INFO( "spoofing red=5, green=4, verify no alarm" );
    api().spoofData( "CHwStates", "red", "5" );
    api().spoofData( "CHwStates", "green", "4" );
    api().spoofActivate();
    api().sleepMsec( 120000 );

    api().spoofData( "CHwStates", "red", "0" );
    LOG_INFO( "spoofing red to 0, expect alarm before spoof ends" );
    api().sleepMsec( 125000 );

    LOG_INFO( "verify RBC Detector Calibration Failure alarm" );
    api().spoofData( "CHwStates", "red", "5" );
    LOG_INFO( "after spoof ends, verify run can continue" );
}


void Spoof_8_10_13 :: init()
{
    _name = "8.10.13";
    _comment = "Control detects loss of switched 24V";
    _timeout = 30;
}
void Spoof_8_10_13 :: help()
{
    LOG_INFO( "- after cassette load, hit STOP button" );
    LOG_INFO( "- open and close basin door" );
    LOG_INFO( "- start spoof and then hit continue" );
    LOG_INFO( "- verify Motor Hardware Failure" );
    LOG_INFO( "- verify can continue after spoof ends" );
}
void Spoof_8_10_13 :: run()
{
    api().spoofData( "SHwOrders", "pumpPower", "HW_DISABLE" );
    api().spoofActivate();
}


void Spoof_8_12_20 :: init()
{
    _name = "8.12.20";
    _comment = "Platelet concentration outside of range, > 2100";
    _timeout = 5000;
}
void Spoof_8_12_20 :: help()
{
    LOG_INFO( "- select a platelet product with concentration of 2000" );
    LOG_INFO( "- start spoof before platelet collection begins" );
    LOG_INFO( "- complete the procedure" );
    LOG_INFO( "- verify run summary flag is set for Platelet concentration outside of range" );
    LOG_INFO( "- verify reason for flag is detected concentration > 2100" );
}
void Spoof_8_12_20 :: run()
{
    // PlateletPlasma substate - plasma bag volume target, reduce collect flow 10% 
    api().spoofData( "Predict_SubstateInfo", "QcD_4", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_4", 0.9, 0 ); 
    
    // PCA - vin target, cut plasma flow to platelet bag 10% 
    api().spoofData( "Predict_SubstateInfo", "QcD_5", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_5", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_5", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_5", 0.9, 0 ); 

    // Midrun substate - vin target, cut collect and chamber flow 10%
    api().spoofData( "Predict_SubstateInfo", "QcD_6", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QcR_6", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QpD_6", 0.9, 0 ); 
    api().spoofData( "Predict_SubstateInfo", "QpR_6", 0.9, 0 ); 
    
    // ExtendedPCA - bag volume target, final volume target 10% short
    api().spoofData( "Predict_SubstateInfo", "Vtarget_9", 0.9, 0 ); 

    api().spoofActivate();
}

void Spoof_8_12_26 :: init()
{
    _name = "8.12.26";
    _comment = "Spoofs spillover spike during ramp centrifuge part of prime";
    _timeout = 6 * 60; //timeout is 6 minutes
}

void Spoof_8_12_26 :: help()
{
    LOG_INFO( "- start spoof immediately before pressing StartRun button" );
    LOG_INFO( "- Var1 is red = green value for first 5 minutes" );
    LOG_INFO( "- Var2 is the green value to use for the 10 second spike during the ramp" );
}

void Spoof_8_12_26 :: run()
{
    LOG_INFO( " No parameters given so running red=green at 1000 for 5 minutes" );
    LOG_INFO( " then green at 900 for 10 seconds and then back to 1000" );
    api().spoofData( "CHwStates", "red", "1000" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofActivate();
    api().sleepMsec( 5 *60 * 1000 );
	api().spoofData( "CHwStates", "red", "1000" );
    api().spoofData( "CHwStates", "green", "900" );
    api().sleepMsec( 10 * 1000 );
	api().spoofData( "CHwStates", "red", "1000" );
    api().spoofData( "CHwStates", "green", "1000" );
}

void Spoof_8_12_26 :: run_generic( const char* Var1, const char* Var2 )
{
    LOG_INFO( "Spoofing red and green to %s", Var1 );
    LOG_INFO( "  If red/green ratio increased by 10 percent or more should get spillover flag" );
    api().spoofData( "CHwStates", "green", Var1 );
    api().spoofData( "CHwStates", "red", Var1 );
    api().spoofActivate();
	api().sleepMsec( 5*60*1000 );  //keep Red/Green ratio constant through PrimeVent
    LOG_INFO( "Now spoofing just green to %s", Var2 );
	api().spoofData( "CHwStates", "green", Var2 ); // "spike" during RampCentrifuge
	api().spoofData( "CHwStates", "red", Var1 );
	// api().spoofActivate();
    api().sleepMsec( 10*1000 );
	LOG_INFO( "Now spoofing original value of %s", Var1 );
	api().spoofData( "CHwStates", "green", Var1 );
	api().spoofData( "CHwStates", "red", Var1 ); // back to previous
}


void Spoof_3L_TBV :: init()
{
    _name = "3L_TBV";
    _comment = "Spoofs TBV to 3L for safety";
    _timeout = 60;
}
void Spoof_3L_TBV :: help()
{
    LOG_INFO( "- enter all donor info including Hct and Pct" );
    LOG_INFO( "- start spoof before final confirm" );
    LOG_INFO( "- confirm the donor info to spoof safety TBV to 3L" );
}
void Spoof_3L_TBV :: run()
{
    api().spoofData( "DVitalsFromProc", "DonorSex", "2" ); // 2 = FEMALE 
    api().spoofData( "DVitalsFromProc", "DonorWeight", "104" ); 
    api().spoofData( "DVitalsFromProc", "DonorHeight", "60" ); 
    api().spoofData( "DVitalsFromProc", "ProcDonorTBV", "3003" ); 
    api().setSpoofedMessageLimit( 1 );
    api().spoofActivate();

    LOG_INFO( "- confirm TBV now" );
}


void Spoof_enable :: init()
{
    _name = "enable";
    _comment = "enables spoofing with timeout";
    _timeout = 3;
}
void Spoof_enable :: help()
{
    LOG_INFO( "activates previously setup spoof" );
    LOG_INFO( "cleans up spoof upon timeout" );
}
void Spoof_enable :: run()
{
    api().spoofActivate();
}


void Spoof_BLACK_SET :: init()
{
    _name = "BLACK_SET";
    _comment = "Spoofs black stamp set ID";
    _timeout = 30;
}
void Spoof_BLACK_SET :: help()
{
    LOG_INFO( "- start spoof as set loads" );
}
void Spoof_BLACK_SET :: run()
{
    LOG_INFO( "spoofing black stamp set ID" ); 
    api().spoofData( "CHwStates", "red", "100" );
    api().spoofActivate();
}


void Spoof_WHITE_SET :: init()
{
    _name = "WHITE_SET";
    _comment = "Spoofs white stamp set ID";
    _timeout = 30;
}
void Spoof_WHITE_SET :: help()
{
    LOG_INFO( "- start spoof as set loads" );
}
void Spoof_WHITE_SET :: run()
{
    LOG_INFO( "spoofing white stamp set ID" ); 
    api().spoofData( "CHwStates", "red", "1000" );
    api().spoofActivate();
}


void Spoof_INVALID_SET_LOW :: init()
{
    _name = "INVALID_SET_LOW";
    _comment = "Spoofs invalid set ID";
    _timeout = 30;
}
void Spoof_INVALID_SET_LOW :: help()
{
    LOG_INFO( "- start spoof as set loads" );
}
void Spoof_INVALID_SET_LOW :: run()
{
    LOG_INFO( "spoofing red to 0" ); 
    api().spoofData( "CHwStates", "red", "0" );
    api().spoofActivate();
}


void Spoof_INVALID_SET_HIGH :: init()
{
    _name = "INVALID_SET_HIGH";
    _comment = "Spoofs invalid set ID";
    _timeout = 30;
}
void Spoof_INVALID_SET_HIGH :: help()
{
    LOG_INFO( "- start spoof as set loads" );
}
void Spoof_INVALID_SET_HIGH :: run()
{
    LOG_INFO( "spoofing red to 1900" ); 
    api().spoofData( "CHwStates", "red", "1900" );
    api().spoofActivate();
}


void Spoof_SPILLOVER :: init()
{
    _name = "SPILLOVER";
    _comment = "Spoofs red/green ratio to simulate a spillover";
    _timeout = 30;
}
void Spoof_SPILLOVER :: help()
{
    LOG_INFO( "- start spoof during run to simulate a spillover" );
}
void Spoof_SPILLOVER :: run()
{
    LOG_INFO( "spoofing red/green ratio to 1.51" ); 
    api().spoofData( "CHwStates", "red", "1510" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofActivate();
}


void Spoof_SPILLOVER_BLACK :: init()
{
    _name = "SPILLOVER_BLACK";
    _comment = "Spoofs red/green ratio to simulate a spillover on a black set";
    _timeout = 105;
}
void Spoof_SPILLOVER_BLACK :: help()
{
    LOG_INFO( "- start spoof during run to simulate a spillover" );
}
void Spoof_SPILLOVER_BLACK :: run()
{
    LOG_INFO( "spoofing red/green ratio to 2.01" ); 
    api().spoofData( "CHwStates", "red", "2010" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofActivate();
}


void Spoof_NO_SPILLOVER :: init()
{
    _name = "NO_SPILLOVER";
    _comment = "Spoofs red/green ratio to simulate a spillover";
    _timeout = 30;
}
void Spoof_NO_SPILLOVER :: help()
{
    LOG_INFO( "- start spoof during run to simulate no spillover" );
}
void Spoof_NO_SPILLOVER :: run()
{
    LOG_INFO( "spoofing red/green ratio to 1.49" ); 
    api().spoofData( "CHwStates", "red", "1490" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofActivate();
}


void Spoof_12V_LOW :: init()
{
    _name = "12V_LOW";
    _comment = "Spoofs 12V supply a/d to read 0";
    _timeout = 10;
}
void Spoof_12V_LOW :: help()
{
    LOG_INFO( "- start spoof as needed" );
}
void Spoof_12V_LOW :: run()
{
    api().spoofData( "CHwStates", "twelveVoltSupply", "0" );
    api().spoofActivate();
}


void Spoof_FAST_RINSEBACK :: init()
{
    _name = "FAST_RINSEBACK";
    _comment = "Spoofs Return pump 20% fast";
    _timeout = 60;
}
void Spoof_FAST_RINSEBACK :: help()
{
    LOG_INFO( "- start spoof during rinseback (rinseback to donor phase)" );
}
void Spoof_FAST_RINSEBACK :: run()
{
    //
    // 1.72x for duty cycle of 3.5/5 is same as 1.2x fulltime
    //
    api().spoofData( "CHwOrders", "returnRPM", 1.72, 0 );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 );
}


void Spoof_FATAL_ERROR_ON_SAFETY :: init()
{
    _name = "FATAL_ERROR_ON_SAFETY";
    _comment = "calls FATAL_ERROR on safety computer";
    _timeout = 10;
}
void Spoof_FATAL_ERROR_ON_SAFETY :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify system goes to safe state" );
    LOG_INFO( "- if hw watchdog enabled, system will reboot" );
    LOG_INFO( "- verify rundata for expected FATAL_ERROR logging" );
}
void Spoof_FATAL_ERROR_ON_SAFETY :: run()
{
    api().fatalError( "SAFETY" );
}


void Spoof_FATAL_ERROR_ON_CONTROL :: init()
{
    _name = "FATAL_ERROR_ON_CONTROL";
    _comment = "calls FATAL_ERROR on control computer";
    _timeout = 10;
}
void Spoof_FATAL_ERROR_ON_CONTROL :: help()
{
    LOG_INFO( "- start spoof anytime" );
    LOG_INFO( "- verify system goes to safe state" );
    LOG_INFO( "- if hw watchdog enabled, system will reboot" );
    LOG_INFO( "- verify rundata for expected FATAL_ERROR logging" );
}
void Spoof_FATAL_ERROR_ON_CONTROL :: run()
{
    api().fatalError( "CONTROL" );
}


void Spoof_CAccessPressure :: init()
{
    _name = "CAccessPressure";
    _comment = "Spoofs APS pressure in mmHg";
    _timeout = 20;
}
void Spoof_CAccessPressure :: help()
{
    LOG_INFO( "- include -var1 mmHg -var2 mmHg -timeout duration(seconds) in command line" );
    LOG_INFO( "- as per test directions" );
    LOG_INFO( "- without parameters spoofs to plus 80 for 20 seconds" );
}
void Spoof_CAccessPressure :: run()
{

    api().spoofData( "CHwStates", "accessPressure", 1, 240 );
    api().spoofActivate();

    LOG_INFO( "Start cassette load now" );
}
void Spoof_CAccessPressure :: run_generic(const char* Var1, const char* Var2)
{
    int APS = atoi(Var1);
    LOG_INFO( " spoofing access pressure to %d", APS );
    // value needs to be multiplied by 3 to actually spoof the correct value(s) for APS
    APS = APS * 3;
    api().spoofData( "CHwStates", "accessPressure", 1, APS );

    api().spoofActivate();
}


void Spoof_RBC_CAL_FAILURE :: init()
{
    _name = "RBC_CAL_FAILURE";
    _comment = "Spoofs RBC calibration failure";
    _timeout = 60;
}
void Spoof_RBC_CAL_FAILURE :: help()
{
    LOG_INFO( "- start spoof just prior to set load" );
    LOG_INFO( "- when prompted, start cassette load" );
}
void Spoof_RBC_CAL_FAILURE :: run()
{
    LOG_INFO( "Start cassette load now" );
    
    api().spoofData( "CHwStates", "red", "735" );
    api().spoofData( "CHwStates", "green", "700" );
    api().spoofActivate();

    for( float redValue = 735; redValue > 670; redValue -= 7 )
    {
	api().spoofData( "CHwStates", "red", 0, redValue );
	api().sleepMsec( 1500 );
    }

    api().spoofData( "CHwStates", "red", "770" );
}


void Spoof_CHAMBER_SATURATION :: init()
{
    _name = "CHAMBER_SATURATION";
    _comment = "Spoofs red/green values to simulate chamber saturation";
    _timeout = 27*60;
}
void Spoof_CHAMBER_SATURATION :: help()
{
    LOG_INFO( "- select platelet only procedure with runtime of at least 40 minutes" );
    LOG_INFO( "- start spoof at beginning for 4th draw cycle" );
}
void Spoof_CHAMBER_SATURATION :: run()
{
    LOG_INFO( "Setting up baseline of next 20 minutes" );
    //
    // spoof red to 650 for Cr > 1.0
    //
    api().spoofData( "CHwStates", "red", "650" );
    api().spoofActivate();
    api().sleepMsec( 20*60*1000 );
    LOG_INFO( "Baseline setup complete" );


    LOG_INFO( "Now spoofing the trigger conditions for next 6.5 minutes" );
    //
    // spoof green to toggle ratio between 1.0 and 0.6 every 6 seconds (650 and 1083)  for 5.5 minutes
    //
    for( int i = 1; i <= 330; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "650" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "1083" );
	api().sleepMsec( 6*1000 );
    }
    //
    // spoof green to toggle ratio between 1.1 and 1.4 every 6 seconds ( 591 and 464) for 1 minute
    //
    for( int i = 1; i <= 60; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "591" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "464" );
	api().sleepMsec( 6*1000 );
    }
}


void Spoof_CHAMBER_SATURATION_c4b :: init()
{
    _name = "CHAMBER_SATURATION_c4b";
    _comment = "Spoofs red/green values to simulate chamber saturation trigger c#4b";
    _timeout = 8*60;
}
void Spoof_CHAMBER_SATURATION_c4b :: help()
{
    LOG_INFO( "- select platelet only procedure with runtime of at least 40 minutes" );
    LOG_INFO( "- start spoof at beginning of 1st draw cycle" );
}
void Spoof_CHAMBER_SATURATION_c4b :: run()
{
    LOG_INFO( "Now spoofing the trigger condition for next 8 minutes" );
    //
    // spoof green to toggle ratio between 1.0 and 0.9 every 6 seconds (650 and 722) for 8 minutes
    //   
    api().spoofData( "CHwStates", "red", "650" );
    api().spoofActivate();
    for( int i = 1; i <= 480; i+=12 )
    {
	api().spoofData( "CHwStates", "green", "650" );
	api().sleepMsec( 6*1000 );
	api().spoofData( "CHwStates", "green", "722" );
	api().sleepMsec( 6*1000 );
    }
}

void Spoof_CrAv_a :: init()
{
    _name = "CrAv_a";
    _comment = "Spoofs CrAv to 0.7 for 70 seconds";
    _timeout = 70;
}

void Spoof_CrAv_a :: help()
{
    LOG_INFO( "- Run during platelet collection procedure excluding all PURGE_PAUSE, PCA, EPCA or PIR phases" );
    LOG_INFO( "- The Saturation Flag for c3 shouldn't get set" );
}

void Spoof_CrAv_a :: run()
{
    api().spoofData("ProcRun_CrAv", "data", "0.7");
    api().spoofActivate();
}

void Spoof_CrAv_b :: init()
{
    _name = "CrAv_b";
    _comment = "Spoofs CrAv to 0.71 for 70 seconds";
    _timeout = 70;
}

void Spoof_CrAv_b :: help()
{
    LOG_INFO( "- Run during platelet collection procedure excluding all PURGE_PAUSE, PCA, EPCA or PIR phases" );
    LOG_INFO( "- This spoof should set the 3c saturation flag" );
}

void Spoof_CrAv_b :: run()
{
    api().spoofData("ProcRun_CrAv", "data", "0.71");
    api().spoofActivate();
}


void Spoof_TOGGLE_PUMPS_OFF :: init()
{
    _name = "TOGGLE_PUMPS_OFF";
    _comment = "Toggles all pumps off every other second";
    _timeout = 20;
}
void Spoof_TOGGLE_PUMPS_OFF :: help()
{
    LOG_INFO( "- start anytime" );
    LOG_INFO( "- stops running pumps, does not start stopped pumps" );
}
void Spoof_TOGGLE_PUMPS_OFF :: run()
{
    api().spoofData( "CHwOrders", "acRPM", "0" );
    api().spoofData( "CHwOrders", "returnRPM", "0" );
    api().spoofData( "CHwOrders", "plateletRPM", "0" );
    api().spoofData( "CHwOrders", "plasmaRPM", "0" );
    api().spoofData( "CHwOrders", "inletRPM", "0" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 1000, 1000 ); // toggles off every other second
}


void Spoof_FASTDATA_ZEROED :: init()
{
    _name = "FASTDATA_ZEROED";
    _comment = "Clears fast data";
    _timeout = 60;
}
void Spoof_FASTDATA_ZEROED :: help()
{
    LOG_INFO( "- start anytime" );
}
void Spoof_FASTDATA_ZEROED :: run()
{
    api().spoofData( "FastData", "raw", "0" );
    api().spoofActivate();
}


void Spoof_FASTDATA_DIV10 :: init()
{
    _name = "FASTDATA_DIV10";
    _comment = "Divides fast data by 10";
    _timeout = 60;
}
void Spoof_FASTDATA_DIV10 :: help()
{
    LOG_INFO( "- start anytime" );
}
void Spoof_FASTDATA_DIV10 :: run()
{
    api().spoofData( "FastData", "raw", 0.1, 0 );
    api().spoofActivate();
}


void Spoof_FASTDATA_DIV100 :: init()
{
    _name = "FASTDATA_DIV100";
    _comment = "Divides fast data by 100";
    _timeout = 60;
}
void Spoof_FASTDATA_DIV100 :: help()
{
    LOG_INFO( "- start anytime" );
}
void Spoof_FASTDATA_DIV100 :: run()
{
    api().spoofData( "FastData", "raw", 0.01, 0 );
    api().spoofActivate();
}


void Spoof_COValve :: init()
{
    _name = "COValve";
    _comment = "Command valve specified to position specified";
    _timeout = 60;
}
void Spoof_COValve :: help()
{
    LOG_INFO( "- Include -var1 valveName -var2 position on command line" );
    LOG_INFO( "- to command valveName to position where valveName is rbcValve, plasmaValve" );
    LOG_INFO(" - or plateletValve  and position is HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN" );
    LOG_INFO( "- without paramters spoof commands plasma valve to open" );
}                                              
void Spoof_COValve :: run()
{
    api().spoofData( "CHwOrders", "plasmaValve", "CHW_VALVE_OPEN" );
    api().spoofActivate();
}
void Spoof_COValve :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"rbcValve") == 0) || (strcmp(Var1, "plasmaValve") == 0) ||
        (strcmp(Var1,"plateletValve")== 0)) 
    {

        if ((strcmp(Var2,"HW_VALVE_COLLECT") == 0) || (strcmp(Var2,"HW_VALVE_OPEN") == 0) ||
            (strcmp(Var2, "HW_VALVE_RETURN") == 0)) 
        {
            LOG_INFO( "Spoofing %s to %s", Var1, Var2 );
            api().spoofData( "CHwOrders", Var1, Var2 );
            api().spoofActivate();
        }
        else
        {
            LOG_INFO("Invalid var2 position value so not spoofing" );
        }
    }
    else
    {
        LOG_INFO("Invalid var1 valve so not spoofing");
    }
}







void Spoof_COPwrSplyEnabled :: init()
{
    _name = "COPwrSply";
    _comment = "Toggle power supply command";
    _timeout = 60;
}

void Spoof_COPwrSplyEnabled :: help()
{
    LOG_INFO( "- Include -var1 0 or 1 on command line" );
    LOG_INFO( "- to command power supply to disabled or enabled" );
    LOG_INFO(" - or without parameters to toggle" );
}                                              

void Spoof_COPwrSplyEnabled :: run()
{
    api().spoofData( "CHwOrders", "powerSupplyEnabled", "HW_PS_OUTPUT_STATUS_ENABLED" );
    api().spoofActivate();
}

void Spoof_COPwrSplyEnabled :: run_generic(const char* Var1, const char* Var2)
{
	char runStr[32];

    if ((strcmp(Var1,"0") == 0) ||
		(strcmp(Var1, "false") == 0) ||
		(strcmp(Var1, "FALSE") == 0) ||
		(strcmp(Var1, "off") == 0) ||
		(strcmp(Var1, "OFF") == 0) ||
		(strcmp(Var1, "disabled") == 0) ||
		(strcmp(Var1, "DISABLED") == 0))
    {
		LOG_INFO( "Disabling power supply (Var1=%s)", Var1 );
		sprintf(runStr, "%s", "HW_PS_OUTPUT_STATUS_DISABLED");
    }
    else
    {
		LOG_INFO( "Enabling power supply (Var1=%s)", Var1 );
		sprintf(runStr, "%s", "HW_PS_OUTPUT_STATUS_ENABLED");
    }

	api().spoofData( "CHwOrders", "powerSupplyEnabled", runStr );
	api().spoofActivate();
}


void Spoof_64V_750MSEC_GLITCH :: init()
{
    _name = "64V_750MSEC_GLITCH";
    _comment = "switches 64V off for 750msec to induce centrifuge RPM overshoot";
    _timeout = 5;
}
void Spoof_64V_750MSEC_GLITCH :: help()
{
    LOG_INFO( "- run spoof with centrifuge at speed" );
}
void Spoof_64V_750MSEC_GLITCH :: run()
{
    // spoof 24V switched to 0 to get control to issue power request msgs
    api().spoofData( "CHwStates", "twentyFourVoltSwitched", 
		     0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 0 ) );
    api().spoofData( "SHwOrders", "centrifugePower", "HW_DISABLE" );
    api().spoofActivate();
    api().sleepMsec( 750 );
    api().unspoofData( "SHwOrders", "centrifugePower" );
}


void Spoof_64V_1250MSEC_GLITCH :: init()
{
    _name = "64V_1250MSEC_GLITCH";
    _comment = "switches 64V off for 750msec to induce centrifuge RPM overshoot";
    _timeout = 5;
}
void Spoof_64V_1250MSEC_GLITCH :: help()
{
    LOG_INFO( "- run spoof with centrifuge at speed" );
}
void Spoof_64V_1250MSEC_GLITCH :: run()
{
    // spoof 24V switched to 0 to get control to issue power request msgs
    api().spoofData( "CHwStates", "twentyFourVoltSwitched", 
		     0, MySpoofs::psAdc( MySpoofs::SUPPLY_24V, 0 ) );
    api().spoofData( "SHwOrders", "centrifugePower", "HW_DISABLE" );
    api().spoofActivate();
    api().sleepMsec( 1250 );
    api().unspoofData( "SHwOrders", "centrifugePower" );
}


void Spoof_LOW_YIELD :: init()
{
    _name = "LOW_YIELD";
    _comment = "spoofs yield target to half";
    _timeout = 10;
}
void Spoof_LOW_YIELD :: help()
{
    LOG_INFO( "- run after platelet collection ends in extended PCA, extended plasma, or rbc collect" );
}
void Spoof_LOW_YIELD :: run()
{
    // spoof platelet yield run target to 50% of real value
    api().spoofData( "ProcRunTargets_SelectedPlateletYield", "data", 0.5, 0 );
    api().spoofActivate();
}


void Spoof_LOW_PROCVOL_YIELD :: init()
{
    // to test IT 6933
    _name = "LOW_PROCVOL_YIELD";
    _comment = "spoofs proc_volume CDS previous yield target to half";
    _timeout = 10;
}
void Spoof_LOW_PROCVOL_YIELD :: help()
{
    LOG_INFO( "- run after platelet collection ends in extended PCA, extended plasma, or rbc collect" );
}
void Spoof_LOW_PROCVOL_YIELD :: run()
{
    // spoof platelet yield run target to 50% of real value
    //api().spoofData( "ProcVolumes_PreviousPlateletYield", "data", 0.5, 0 );
    api().spoofData( "ProcVolumes_PlateletYield", "data", 0.5, 0 );
    api().spoofActivate();
}


void Spoof_BIG_AC_PRIME :: init()
{
    _name = "AC_PRIME";
    _comment = "runs ac prime for ~30ml";
    _timeout = 60;
}
void Spoof_BIG_AC_PRIME :: help()
{
    LOG_INFO( "- pause before end of AC prime as AC reaches manifold and run spoof while paused" );
}
void Spoof_BIG_AC_PRIME :: run()
{
    for( int i = 1; i <= 11; i++ )
    {
	MySpoofs::acPrime();  // runs AC 50RPM for 4 seconds, ~2.7ml
    }
}

void Spoof_SAFETY_DISCONNECT_OK :: init()
{
	_name = "SAFETY_DISCONNECT_OK";
	_comment = "Spoofs message from safety that disconnect passed";
	_timeout = 60;
}

void Spoof_SAFETY_DISCONNECT_OK :: help()
{
	LOG_INFO( "- Leave donor line unclamped" );
	LOG_INFO( "- Start spoof just before pressing/continue/confirm disconnect buttons" );
}

void Spoof_SAFETY_DISCONNECT_OK :: run()
{
    LOG_INFO( "- Spoofing a successful disconnect message from Safety" );
	api().messageSend( "SafetyDisconnectStatMsg", 1 );
	api().spoofActivate();
}

void Spoof_SAFETY_DISCONNECT_FAIL :: init()
{
	_name = "SAFETY_DISCONNECT_FAIL";
	_comment = "Spoofs message from safety that disconnect failed";
	_timeout = 60;
}

void Spoof_SAFETY_DISCONNECT_FAIL :: help()
{
	LOG_INFO( "- Don't clamp donor line at first and get the first disconnect alarm" );
    LOG_INFO( "- Clamp donor line and continue" );
	LOG_INFO( "- Start spoof while inlet pump is running for the test" );
}

void Spoof_SAFETY_DISCONNECT_FAIL :: run()
{
    LOG_INFO( "Spoofing a donor disconnect failure message from Safety" );
	api().messageSend( "SafetyDisconnectStatMsg", 0 );
    api().msgMultiplier( 3 ); 
	api().spoofActivate();
}

void Spoof_RED_GREEN :: init()
{
    _name = "RED_GREEN";
    _comment = "Spoofs the red and green values as variables var1 and var2 respectively";
    _timeout = 5 * 60; // 5 minutes 
}

void Spoof_RED_GREEN :: help()
{
    LOG_INFO( "Include -var1 xxxx -var2 yyyy to spoof red to xxxx and green to yyyy" );
    LOG_INFO( "See test procedure for specifics of timing and values" );
}

void Spoof_RED_GREEN :: run()
{
    LOG_INFO( "Spoofing Red and Green to 1000 as no values were given" );
    api().spoofData( "CHwStates", "red", "1000" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofActivate();
}

void Spoof_RED_GREEN :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO( "Spoofing red and green to %s and %s.", Var1, Var2 );
    // red is spoofed to Var1
    // green is spoofed to Var2.
    api().spoofData( "CHwStates", "red", Var1 );	
    api().spoofData( "CHwStates", "green", Var2 );
    api().spoofActivate();
}


void Spoof_APS_1 :: init()
{
    _name = "Spoof_APS_1";
    _comment = "Spoofs the Access Pressure readings";
    _timeout = 20; // 20 seconds
}


void Spoof_APS_1 :: help()
{
    LOG_INFO( "Spoofs APS reading to 301 for 20 seconds, run after loading set when the AC pump starts turning");
}

void Spoof_APS_1 :: run()
{
    LOG_INFO("Spoofing APS reading to 301 for 20 seconds");
    api().spoofData("HalStatus_aps", "data", 0, 301);
    api().spoofActivate();
}


void Spoof_Safety_APS :: init()
{
    _name = "Spoof_Safety_APS";
    _comment = "Spoofs the Access Pressure readings for safety only.";
    _timeout = 30; // 30 seconds to match the test failure time
}

void Spoof_Safety_APS :: help()
{
    LOG_INFO("Clamp donor line so control will pass disconnect test while spoofing safety." );
    LOG_INFO("Non-parameterized spoofs -100 for 2 sec and then drops to -50 and 300 msec later to -10" );
    LOG_INFO("Include -var1 -x and -var2 -y parameters to spoof APS reading from -x to -y to adjust test" );
	LOG_INFO("var1 should be around -50, timeout 30 to 40" );
}

void Spoof_Safety_APS :: run()
{
    LOG_INFO( "Spoofing APS reading for safety only as -100 and then -50" );
    api().spoofData( "HwStatesFromControl", "accessPressure", "-100" );
    api().spoofActivate();
    api().sleepMsec(2000);  // short of the 3 second stabilization time on donor disconnect test seems to work
    api().spoofData( "HwStatesFromControl", "accessPressure", "-50" );
    api().sleepMsec(300);
    api().spoofData( "HwStatesFromControl", "accessPressure", "-10");
}


void Spoof_Safety_APS :: run_generic(const char* Var1, const char* Var2)
{
    //api().enableDebug();
    LOG_INFO( "Spoofing APS reading for safety only as %s and then %s.", Var1, Var2 );
    api().spoofData( "HwStatesFromControl", "accessPressure", Var1 );
    api().spoofActivate();
    api().sleepMsec(2000); // short of the 3 second stabilization time on donor disconnect seems to work
    api().spoofData( "HwStatesFromControl", "accessPressure", Var2 );
}


void Spoof_APS_mmHg :: init()
{
    _name = "Spoof_APS_mmHg";
    _comment = "Spoofs the APS Pressure in mmHg";
    _timeout = 30; // 30 seconds
}


void Spoof_APS_mmHg :: help()
{
    LOG_INFO( "Use -var1 xxxx to spoof APS to xxxx use -var2 as multiplier, set -var2 to 0 to get direct mmHg value");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void Spoof_APS_mmHg :: run()
{
    LOG_INFO("Spoofing APS reading to 100mmHg for 30 seconds");
    api().spoofData("HalStatus_aps", "data", 0, 100);
    api().spoofActivate();
}

void Spoof_APS_mmHg :: run_generic(const char* Var1, const char* Var2)
{
   LOG_INFO( "Spoofing APS reading as %s mmHg, with a multiplier of %s.", Var1, Var2);
   api().spoofData("HalStatus_aps", "data", atof(Var2), atof(Var1));
   api().spoofActivate();
}

 

void Spoof_CPS :: init()
{
    _name = "Spoof_CPS";
    _comment = "Spoofs the Centrifuge Pressure readings mostly used by the F 4.37.03 to 05 tests.";
    _timeout = 30; // 30 seconds
}


void Spoof_CPS :: help()
{
    LOG_INFO( "Use -var1 xxxx to spoof CPS to xxxx.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void Spoof_CPS :: run()
{
    LOG_INFO("Spoofing CPS reading to 800 for 30 seconds");
    api().spoofData("CHwStates", "centrifugePressure", "800");
    api().spoofActivate();
}

void Spoof_CPS :: run_generic(const char* Var1, const char* Var2)
{
   LOG_INFO( "Spoofing CPS reading as %s.", Var1);
   api().spoofData("CHwStates", "centrifugePressure", Var1);
   api().spoofActivate();
}



void Spoof_CPS_mmHg :: init()
{
    _name = "Spoof_CPS_mmHg";
    _comment = "Spoofs the Centrifuge Pressure in mmHg";
    _timeout = 30; // 30 seconds
}


void Spoof_CPS_mmHg :: help()
{
    LOG_INFO( "Use -var1 xxxx to spoof CPS to xxxx use -var2 as multiplier, set -var2 to 0 to get direct mmHg value");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void Spoof_CPS_mmHg :: run()
{
    LOG_INFO("Spoofing CPS reading to 800mmHg for 30 seconds");
    api().spoofData("HalStatus_cps", "data", 0, 800);
    api().spoofActivate();
}

void Spoof_CPS_mmHg :: run_generic(const char* Var1, const char* Var2)
{
   LOG_INFO( "Spoofing CPS reading as %s mmHg, with a multiplier of %s.", Var1, Var2);
   api().spoofData("HalStatus_cps", "data", atof(Var2), atof(Var1));
   api().spoofActivate();
}



void Spoof_CPS_1 :: init()
{
    _name = "Spoof_CPS_1";
    _comment = "Spoofs the Centrifuge Pressure readings";
    _timeout = 241; // 241 seconds
}


void Spoof_CPS_1 :: help()
{
    LOG_INFO( "Spoofs CPS reading to 1001 for 1 second...");
    LOG_INFO( "then 316 for 240 seconds" );
}

void Spoof_CPS_1 :: run()
{
    LOG_INFO("Spoofing CPS reading to 1001 for 1 second then 316 for 240 seconds");
    api().spoofData("HalStatus_cps", "data", 0, 1001);
    api().spoofActivate();
	api().sleepMsec(1000);
	api().spoofData("HalStatus_cps", "data", 0, 316);
	api().sleepMsec(24000);
}


void Valve_Controlled_Wrong :: init()
{
    _name = "Valve_Controlled_Wrong";
    _comment = "Spoofs the valve specified by var1 to position specified by var2 command and control and safety states";
    _timeout = 60; // 60 seconds overrride as necessary for test
}


void Valve_Controlled_Wrong :: help()
{
    LOG_INFO( "This is a parameterized spoof to simulate a 'bad' control to ensure that some alarm is raised.");
    LOG_INFO( "Use spoof to command valve and set control and safety state during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1 rbcValve, plasmaValve, or plateletValve  to change/spoof that valve's position.");
    LOG_INFO( "Use -var2 HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN to specify the position for the valve.");
    LOG_INFO( "Use  -timeout tt if 60 second default timeout not desirable" );
}

void Valve_Controlled_Wrong :: run()
{
    LOG_INFO("Spoofing rbcValve to HW_VALVE_COLLECT for 60 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_COLLECT");
    api().spoofData("CHwStates", "rbcValve", "HW_VALVE_COLLECT");
    api().spoofData("SHwStates", "rbcValve", "HW_VALVE_COLLECT");
    api().spoofActivate();
}

void Valve_Controlled_Wrong :: run_generic(const char* Var1, const char* Var2)
{
   bool valid_position = false;
   LOG_INFO( "Spoofing %s to position %s.", Var1, Var2);
   if (strcmp(Var2, "HW_VALVE_COLLECT") == 0 ) 
   {
        api().spoofData("CHwOrders", Var1, "CHW_VALVE_COLLECT");
        valid_position = true;
   }
   else if (strcmp(Var2, "HW_VALVE_OPEN") == 0) 
   {
        api().spoofData("CHwOrders", Var1, "CHW_VALVE_OPEN");
        valid_position = true;
   }
   else if (strcmp(Var2, "HW_VALVE_RETURN") == 0) 
   {
        api().spoofData("CHwOrders", Var1, "CHW_VALVE_RETURN");
        valid_position = true;
   }
   else if (strcmp(Var2, "HW_VALVE_UNKNOWN") == 0) 
   {
        api().spoofData("CHwOrders", Var1, "CHW_VALVE_UNKNOWN");
        valid_position = true;
   }
   else
   {
        LOG_INFO("Var2 valve order must be one of HW_VALVE_OPEN, HW_VALVE_COLLECT, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN");
   }
   
   if (valid_position) 
   {

        api().spoofData("CHwStates", Var1, Var2);
        api().spoofData("SHwStates", Var1, Var2);
        api().spoofActivate();
   }
}

void Valve_Observed_Wrong :: init()
{
    _name = "Valve_Observed_Wrong";
    _comment = "Spoofs the valve specified by var1 to position specified by var2 command and control and safety states";
    _timeout = 60; // 60 seconds overrride as necessary for test
}


void Valve_Observed_Wrong :: help()
{
    LOG_INFO( "This is a parameterized spoof to simulate both control and safety observing the valve in wrong position to ensure that some alarm is raised.");
    LOG_INFO( "Use spoof to set control and safety state during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1 rbcValve, plasmaValve, or plateletValve  to change/spoof that valve's position.");
    LOG_INFO( "Use -var2 HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN to specify the position for the valve.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void Valve_Observed_Wrong :: run()
{
    LOG_INFO("Spoofing rbcValve to HW_VALVE_COLLECT for 30 seconds");
    api().spoofData("CHwStates", "rbcValve", "HW_VALVE_COLLECT");
    api().spoofData("SHwStates", "rbcValve", "HW_VALVE_COLLECT");
    api().spoofActivate();
}

void Valve_Observed_Wrong :: run_generic(const char* Var1, const char* Var2)
{
    api().spoofData("CHwStates", Var1, Var2);
    api().spoofData("SHwStates", Var1, Var2);
    api().spoofActivate();
}

/* for now comment out since AutoSplit feature has been removed from version 5.R=6.0
void RBC_C4R :: init()
{
    _name = "RBC_C4R";
    _comment = "Commands the RBCvalve to Collect instead of Return -  var1 specifies control and var2 safety states spoofed";
    _timeout = 30; // 30 seconds overrride as necessary for test
}


void RBC_C4R :: help()
{
    LOG_INFO( "This is a parameterized spoof to command the RBC valve to the Collect positioninstead of Return" );
    LOG_INFO( "and to spoof both control and safety states to the values input to test valve monitoring.");
    LOG_INFO( "Use spoof during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1  control state to set  either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use -var2 safety state to set either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void RBC_C4R :: run()
{
    LOG_INFO("Commanding rbcValve to HW_VALVE_COLLECT for 30 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_COLLECT");
    LOG_INFO("Spoofing Control and Safety observation to HW_VALVE_RETURN");
    api().spoofData("CHwStates", "rbcValve", "HW_VALVE_RETURN");
    api().spoofData("SHwStates", "rbcValve", "HW_VALVE_RETURN");
    api().spoofActivate();
}

void RBC_C4R :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO("Commanding rbcValve to HW_VALVE_COLLECT for 30 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_COLLECT");
    LOG_INFO("Spoofing Control to %s and Safety observation to %s", Var1, Var2);
    api().spoofData("CHwStates", "rbcValve", Var1);
    api().spoofData("SHwStates", "rbcValve", Var2);
    api().spoofActivate();
}

void RBC_R4C :: init()
{
    _name = "RBC_R4C";
    _comment = "Commands the RBCvalve to Return instead of Collect -  var1 specifies control and var2 safety states spoofed";
    _timeout = 30; // 30 seconds overrride as necessary for test
}


void RBC_R4C :: help()
{
    LOG_INFO( "This is a generic spoof to command the RBC valve to the Return position instead of Collect" );
    LOG_INFO( "and to spoof both control and safety states to the values input to test valve monitoring.");
    LOG_INFO( "Use spoof during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1  control state to set  either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use -var2 safety state to set either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void RBC_R4C :: run()
{
    LOG_INFO("Commanding rbcValve to HW_VALVE_RETURN for 30 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_RETURN");
    LOG_INFO("Spoofing Control and Safety observation to HW_VALVE_COLLECT");
    api().spoofData("CHwStates", "rbcValve", "HW_VALVE_COLLECT");
    api().spoofData("SHwStates", "rbcValve", "HW_VALVE_COLLECT");
    api().spoofActivate();
}

void RBC_R4C :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO("Commanding rbcValve to HW_VALVE_RETURN for 30 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_RETURN");
    LOG_INFO("Spoofing Control to %s and Safety observation to %s", Var1, Var2);
    api().spoofData("CHwStates", "rbcValve", Var1);
    api().spoofData("SHwStates", "rbcValve", Var2);
    api().spoofActivate();
}

void RBC_O4R :: init()
{
    _name = "RBC_O4R";
    _comment = "Commands the RBCvalve to Open instead of Return -  var1 specifies control and var2 safety states spoofed";
    _timeout = 30; // 30 seconds overrride as necessary for test
}


void RBC_O4R :: help()
{
    LOG_INFO( "This is a generic spoof to command the RBC valve to the Open position instead of Return" );
    LOG_INFO( "and to spoof both control and safety states to the values input to test valve monitoring.");
    LOG_INFO( "Use spoof during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1  control state to set  either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use -var2 safety state to set either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void RBC_O4R :: run()
{
    LOG_INFO("Commanding rbcValve to HW_VALVE_OPEN for 30 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_OPEN");
    LOG_INFO("Spoofing Control and Safety observation to HW_VALVE_RETURN");
    api().spoofData("CHwStates", "rbcValve", "HW_VALVE_RETURN");
    api().spoofData("SHwStates", "rbcValve", "HW_VALVE_RETURN");
    api().spoofActivate();
}

void RBC_O4R :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO("Commanding rbcValve to HW_VALVE_OPEN for 30 seconds");
    api().spoofData("CHwOrders", "rbcValve", "CHW_VALVE_OPEN");
    LOG_INFO("Spoofing Control to %s and Safety observation to %s", Var1, Var2);
    api().spoofData("CHwStates", "rbcValve", Var1);
    api().spoofData("SHwStates", "rbcValve", Var2);
    api().spoofActivate();
}

void PLT_O4R :: init()
{
    _name = "PLT_O4R";
    _comment = "Commands the PLTvalve to Open instead of Return -  var1 specifies control and var2 safety states spoofed";
    _timeout = 30; // 30 seconds overrride as necessary for test
}


void PLT_O4R :: help()
{
    LOG_INFO( "This is a generic spoof to command the PLT valve to the Open position instead of Return" );
    LOG_INFO( "and to spoof both control and safety states to values input to test valve monitoring.");
    LOG_INFO( "Use spoof during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1 control state to set  either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use -var2 safety state to set either HW_VALVE_COLLECT, HW_VALVE_OPEN, HW_VALVE_RETURN, or HW_VALVE_UNKNOWN.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void PLT_O4R :: run()
{
    LOG_INFO("Commanding platletValve to HW_VALVE_OPEN for 30 seconds");
    api().spoofData("CHwOrders", "plateletValve", "CHW_VALVE_OPEN");
    LOG_INFO("Spoofing Control and Safety observation to HW_VALVE_RETURN");
    api().spoofData("CHwStates", "plateletValve", "HW_VALVE_RETURN");
    api().spoofData("SHwStates", "plateletValve", "HW_VALVE_RETURN");
    api().spoofActivate();
}

void PLT_O4R :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO("Commanding plateletValve to HW_VALVE_OPEN for 30 seconds");
    api().spoofData("CHwOrders", "plateletValve", "CHW_VALVE_OPEN");
    LOG_INFO("Spoofing Control to %s and Safety observation to %s", Var1, Var2);
    api().spoofData("CHwStates", "plateletValve", Var1);
    api().spoofData("SHwStates", "plateletValve", Var2);
    api().spoofActivate();
}
*/

void Spoof_MultiplyAc :: init()
{
    _name = "MultiplyAc";
    _comment = "Runs the AC pump specified at the factor specified - usually  0 or fast as in 1.06 for a 6% increase in speed.";
    _timeout = 5 * 60; // 5 minutes overrride as necessary for test
}


void Spoof_MultiplyAc :: help()
{
    LOG_INFO( "This is a parameterized spoof to command the AC pump to run at a multiplicative factor" );
    LOG_INFO( "to run at a multiplicative factor of the speed.");
    LOG_INFO( "Use spoof during the appropriate part of a procedure.");
    LOG_INFO( "Use -var1 x.xx to specify factor by which to multiple the AC pump speed");
    LOG_INFO( "Very common use is 0.0 for no AC");
    LOG_INFO( "Use  -timeout tt if 5 minute default timeout not desirable" );
}

void Spoof_MultiplyAc :: run()
{
    LOG_INFO("running ac at 1.06 x commanded speed.");
    MySpoofs::acMultiplier( 1.06 );
    api().spoofActivate();
}

void Spoof_MultiplyAc :: run_generic(const char* Var1, const char* Var2)
{
   float factor = atof(Var1);
   LOG_INFO( "running ac at %1.2f x commanded speed", factor );
   MySpoofs::acMultiplier( factor );
   api().spoofActivate();
}

void Spoof_RGMIDRUN_TRIGGER :: init()
{
	_name = "Spoof_RGMIDRUN_TRIGGER";
	_comment = "spoofs red and green including noise for testing chamber saturation IT 8149";
	_timeout = 46*60;
}

void Spoof_RGMIDRUN_TRIGGER :: help()
{
	LOG_INFO("-use to create baseline for chamber saturation testing");
    LOG_INFO("- assumes PLT of 7.0 yield in 452 mL and 400 mL PLS procedure");
    LOG_INFO("- use Male, 77 in., 286 pound, 43 HCT, 265 PCT donor");
    LOG_INFO("- start following the channel priming at about 6 minutes in");
}

void Spoof_RGMIDRUN_TRIGGER :: run()
{
   LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
   api().spoofData( "CHwStates", "red", "1122" );
   api().spoofData( "CHwStates", "green", "1271" );
   api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including noise
    //
    for (int i = 1; i <= 300; i+=12 )  // five minutes 
    {
        api().spoofData( "CHwStates", "red", "1125" );
        api().spoofData( "CHwStates", "green", "1270" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1129" );
        api().spoofData( "CHwStates", "green", "1275" );
        api().sleepMsec( 6*1000 ); // five minutes or 11 into proc
	}
    for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1150" );
        api().spoofData( "CHwStates", "green", "1287" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1143" );
        api().spoofData( "CHwStates", "green", "1282" );
        api().sleepMsec( 6*1000 ); // 12 min in
    }
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1096" );
        api().spoofData( "CHwStates", "green", "1215" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1100" );
        api().spoofData( "CHwStates", "green", "1220" );
        api().sleepMsec( 6*1000 );   // 13 min in
    }
    LOG_INFO ("Now will lower red and green values" );
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "810" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "815" );
        api().spoofData( "CHwStates", "green", "749" );
        api().sleepMsec( 6*1000 );    // 14 min in
    }
     LOG_INFO ("Now will hold the PlateletPlasma baseline value for 18 minutes" );
     for (int i = 1; i <= 1080; i+=12 )   
     {
        api().spoofData( "CHwStates", "red", "834" );
        api().spoofData( "CHwStates", "green", "754" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "823" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
    }
     LOG_INFO ("spoofing initial baseline finished");
     api().spoofData( "CHwStates", "red", "848" );
     api().spoofData( "CHwStates", "green", "755" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "860" );
     api().spoofData( "CHwStates", "green", "767" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "900" );
     api().spoofData( "CHwStates", "green", "803" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "930" );
     api().spoofData( "CHwStates", "green", "837" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "950" );
     api().spoofData( "CHwStates", "green", "870" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "965" );
     api().spoofData( "CHwStates", "green", "886" );

     //
     // now do a second baseline so there isn't a discontinuity
     // spoof red and green with more noise to simulate Midrun platelets
     //
     for (int i = 1; i <= 720; i+=12 )  // twelve minutes
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("MidRun baseline setup complete " );

     for (int i = 1; i <= 60; i+=12) 
     {
         api().spoofData( "CHwStates", "red", "806" );
         api().spoofData( "CHwStates", "green", "700" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "763" );
         api().spoofData( "CHwStates", "green", "659" );
     }
     api().spoofData( "CHwStates", "red", "802" );
     api().spoofData( "CHwStates", "green", "684");
     api().sleepMsec( 12*1000 );
     LOG_INFO("Back to MidRun baseline" );
     for (int i = 1; i <= 360; i+=12 )  // six minutes
 {
     api().spoofData( "CHwStates", "red", "957" );
     api().spoofData( "CHwStates", "green", "871" );
     api().sleepMsec( 6*1000 );
     api().spoofData( "CHwStates", "red", "968" );
     api().spoofData( "CHwStates", "green", "884" );
     api().sleepMsec( 6*1000 ); 
 }


}

void Spoof_RGPURGE_TRIGGER :: init()
{
	_name = "Spoof_RGPURGE_TRIGGER";
	_comment = "spoofs red and green including noise through to purge then saturates";
	_timeout = 66*60;
}

void Spoof_RGPURGE_TRIGGER :: help()
{
	LOG_INFO("-use for chamber saturation testing IT-8149 and IT-8415");
    LOG_INFO("-assumes PLT of 7.0 yield in 452 mL and 200 mL PLS procedure");
    LOG_INFO("use Male, 77 in., 286 pound, 43 HCT, 265 PCT donor");
    LOG_INFO("start spoof at about 6 minutes proc time");
}

void Spoof_RGPURGE_TRIGGER :: run()
{
      LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
   api().spoofData( "CHwStates", "red", "1122" );
   api().spoofData( "CHwStates", "green", "1271" );
   api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including noise
    //
    for (int i = 1; i <= 300; i+=12 )  // five minutes 
    {
        api().spoofData( "CHwStates", "red", "1125" );
        api().spoofData( "CHwStates", "green", "1270" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1129" );
        api().spoofData( "CHwStates", "green", "1275" );
        api().sleepMsec( 6*1000 ); // five minutes or 11 into proc
	}
    for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1150" );
        api().spoofData( "CHwStates", "green", "1287" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1143" );
        api().spoofData( "CHwStates", "green", "1282" );
        api().sleepMsec( 6*1000 ); // 12 min in
    }
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1096" );
        api().spoofData( "CHwStates", "green", "1215" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1100" );
        api().spoofData( "CHwStates", "green", "1220" );
        api().sleepMsec( 6*1000 );   // 13 min in
    }
    LOG_INFO ("Now will lower red and green values" );
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "810" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "815" );
        api().spoofData( "CHwStates", "green", "749" );
        api().sleepMsec( 6*1000 );    // 14 min in
    }
     LOG_INFO ("Now will hold the PlateletPlasma baseline value for 18 minutes" );
     for (int i = 1; i <= 1080; i+=12 )   
     {
        api().spoofData( "CHwStates", "red", "834" );
        api().spoofData( "CHwStates", "green", "754" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "823" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
    } // 32 minutes in
     LOG_INFO ("spoofing initial baseline finished");
     api().spoofData( "CHwStates", "red", "848" );
     api().spoofData( "CHwStates", "green", "755" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "860" );
     api().spoofData( "CHwStates", "green", "767" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "900" );
     api().spoofData( "CHwStates", "green", "803" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "930" );
     api().spoofData( "CHwStates", "green", "837" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "950" );
     api().spoofData( "CHwStates", "green", "870" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "965" );
     api().spoofData( "CHwStates", "green", "886" ); // 33 min in

     //
     // now do a second baseline so there isn't a discontinuity
     // spoof red and green with more noise to simulate Midrun platelets
     //
     for (int i = 1; i <= 1320; i+=12 )  // twenty two minutes more to get to the purge
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Now the WBC chamber purge " ); // 55 minutes in
     for (int i = 1; i <= 60; i+= 12) 
     {
         api().spoofData( "CHwStates", "red", "967" );
         api().spoofData( "CHwStates", "green", "783" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "962" );
         api().spoofData( "CHwStates", "green", "774" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 60; i+= 12)   // 56 minute in
     {
         api().spoofData( "CHwStates", "red", "1245" );
         api().spoofData( "CHwStates", "green", "1119" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "1257" );
         api().spoofData( "CHwStates", "green", "1134" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 260; i+= 12)   // 57 minute in for 2 min
     {
         api().spoofData( "CHwStates", "red", "1261" );
         api().spoofData( "CHwStates", "green", "1140" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "1259" );
         api().spoofData( "CHwStates", "green", "1139" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Generating an event");
     for (int i = 1; i <= 60; i+= 12)   // 59 minute in spoof an event to make sure system disables after purge    
     {
         api().spoofData( "CHwStates", "red", "774" );
         api().spoofData( "CHwStates", "green", "619" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "705" );
         api().spoofData( "CHwStates", "green", "546" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 780; i+=12 )  // thirteen minutes more at the Midrun Baseline
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
}

void Spoof_RGEVENT_TRIGGER :: init()
{
	_name = "Spoof_RGEVENT_TRIGGER";
	_comment = "spoofs red and green including noise till after the purge then saturates";
	_timeout = 63*60;
}

void Spoof_RGEVENT_TRIGGER :: help()
{
	LOG_INFO("-use for chamber saturation testing IT-8149 and IT-8415");
    LOG_INFO("-assumes PLT of 7.0 yield in 452 mL and 200 mL PLS procedure");
    LOG_INFO("use Male, 77 in., 286 pound, 43 HCT, 265 PCT donor");
    LOG_INFO("start spoof at about 6 minutes proc time");
}

void Spoof_RGEVENT_TRIGGER :: run()
{
   LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
   api().spoofData( "CHwStates", "red", "1122" );
   api().spoofData( "CHwStates", "green", "1271" );
   api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including noise
    //
    for (int i = 1; i <= 300; i+=12 )  // five minutes 
    {
        api().spoofData( "CHwStates", "red", "1125" );
        api().spoofData( "CHwStates", "green", "1270" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1129" );
        api().spoofData( "CHwStates", "green", "1275" );
        api().sleepMsec( 6*1000 ); // five minutes or 11 into proc
	}
    for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1150" );
        api().spoofData( "CHwStates", "green", "1287" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1143" );
        api().spoofData( "CHwStates", "green", "1282" );
        api().sleepMsec( 6*1000 ); // 12 min in
    }
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1096" );
        api().spoofData( "CHwStates", "green", "1215" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1100" );
        api().spoofData( "CHwStates", "green", "1220" );
        api().sleepMsec( 6*1000 );   // 13 min in
    }
    LOG_INFO ("Now will lower red and green values" );
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "810" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "815" );
        api().spoofData( "CHwStates", "green", "749" );
        api().sleepMsec( 6*1000 );    // 14 min in
    }
     LOG_INFO ("Now will hold the PlateletPlasma baseline value for 18 minutes" );
     for (int i = 1; i <= 1080; i+=12 )   
     {
        api().spoofData( "CHwStates", "red", "834" );
        api().spoofData( "CHwStates", "green", "754" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "823" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
    } // 32 minutes in
     LOG_INFO ("spoofing initial baseline finished");
     api().spoofData( "CHwStates", "red", "848" );
     api().spoofData( "CHwStates", "green", "755" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "860" );
     api().spoofData( "CHwStates", "green", "767" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "900" );
     api().spoofData( "CHwStates", "green", "803" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "930" );
     api().spoofData( "CHwStates", "green", "837" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "950" );
     api().spoofData( "CHwStates", "green", "870" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "965" );
     api().spoofData( "CHwStates", "green", "886" ); // 33 min in

     //
     // now do a second baseline so there isn't a discontinuity
     // spoof red and green with more noise to simulate Midrun platelets
     //
     for (int i = 1; i <= 1320; i+=12 )  // twenty two minutes more to get to the purge
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Now the WBC chamber purge " ); // 55 minutes in
     for (int i = 1; i <= 60; i+= 12) 
     {
         api().spoofData( "CHwStates", "red", "967" );
         api().spoofData( "CHwStates", "green", "783" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "962" );
         api().spoofData( "CHwStates", "green", "774" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 360; i+= 12)   
     {
         api().spoofData( "CHwStates", "red", "1245" );
         api().spoofData( "CHwStates", "green", "1119" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "1257" );
         api().spoofData( "CHwStates", "green", "1134" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 120; i+= 12)   // go just past the scheduled purge
     {
         api().spoofData( "CHwStates", "red", "1261" );
         api().spoofData( "CHwStates", "green", "1140" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "1248" );
         api().spoofData( "CHwStates", "green", "1124" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Generating an event");
     for (int i = 1; i <= 200; i+= 12)   // an event to make sure system re-enables flagging    
     {
         api().spoofData( "CHwStates", "red", "774" );
         api().spoofData( "CHwStates", "green", "601" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "705" );
         api().spoofData( "CHwStates", "green", "516" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 300; i+=12 )  // five minutes more at the Midrun Baseline
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
}

void Spoof_c4b_TRIGGER :: init()
{
	_name = "Spoof_c4b_TRIGGER";
	_comment = "spoofs red and green including noise to generate c4b flag";
	_timeout = 6*60;
}

void Spoof_c4b_TRIGGER :: help()
{
	LOG_INFO("-use for chamber saturation testing IT-8149 and IT-8415");
    LOG_INFO("-assumes PLT of 7.0 yield in 452 mL and 200 mL PLS procedure");
    LOG_INFO("use Male, 77 in., 286 pound, 43 HCT, 265 PCT donor");
    LOG_INFO("c4b flag is generated when Vin<= 600 mL");
    LOG_INFO("start spoof at about 6 minutes proc time");
}

void Spoof_c4b_TRIGGER :: run()
{
    LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
    api().spoofData( "CHwStates", "red", "1122" );
    api().spoofData( "CHwStates", "green", "1271" );
    api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including noise
    //
    for (int i = 1; i <= 180; i+=12 )  // three minutes 
    {
        api().spoofData( "CHwStates", "red", "1125" );
        api().spoofData( "CHwStates", "green", "1270" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1129" );
        api().spoofData( "CHwStates", "green", "1275" );
        api().sleepMsec( 6*1000 ); // three minutes or 9 into proc
    }
    LOG_INFO("Generating higher R/G flicker");
    // simulate a higher average R/G flicker, i.e., a c4b event
    for (int i = 1; i <=90; i+=12) 
    {
        api().spoofData( "CHwStates", "red", "1144" );
        api().spoofData( "CHwStates", "green", "1260" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "950" );
        api().spoofData( "CHwStates", "green", "1275" );
        api().sleepMsec( 6*1000 );
    }

 }

void Spoof_c3EVENT_TRIGGER :: init()
{
	_name = "Spoof_c3EVENT_TRIGGER";
	_comment = "spoofs red and green including noise then creates c3 event prior to scheduled purge";
	_timeout = 63*60;
}

void Spoof_c3EVENT_TRIGGER :: help()
{
	LOG_INFO("-use for chamber saturation testing IT-8149 and IT-8415");
    LOG_INFO("-assumes PLT of 7.0 yield in 452 mL and 200 mL PLS procedure");
    LOG_INFO("use Male, 77 in., 286 pound, 43 HCT, 265 PCT donor");
    LOG_INFO("start spoof at about 6 minutes proc time");
}

void Spoof_c3EVENT_TRIGGER :: run()
{
   LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
   api().spoofData( "CHwStates", "red", "1122" );
   api().spoofData( "CHwStates", "green", "1271" );
   api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including noise
    //
    for (int i = 1; i <= 300; i+=12 )  // five minutes 
    {
        api().spoofData( "CHwStates", "red", "1125" );
        api().spoofData( "CHwStates", "green", "1270" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1129" );
        api().spoofData( "CHwStates", "green", "1275" );
        api().sleepMsec( 6*1000 ); // five minutes or 11 into proc
	}
    for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1150" );
        api().spoofData( "CHwStates", "green", "1287" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1143" );
        api().spoofData( "CHwStates", "green", "1282" );
        api().sleepMsec( 6*1000 ); // 12 min in
    }
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "1096" );
        api().spoofData( "CHwStates", "green", "1215" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1100" );
        api().spoofData( "CHwStates", "green", "1220" );
        api().sleepMsec( 6*1000 );   // 13 min in
    }
    LOG_INFO ("Now will lower red and green values" );
     for (int i = 1; i <= 60; i+=12 ) 
    {
        api().spoofData( "CHwStates", "red", "810" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "815" );
        api().spoofData( "CHwStates", "green", "749" );
        api().sleepMsec( 6*1000 );    // 14 min in
    }
     LOG_INFO ("Now will hold the PlateletPlasma baseline value for 18 minutes" );
     for (int i = 1; i <= 1080; i+=12 )   
     {
        api().spoofData( "CHwStates", "red", "834" );
        api().spoofData( "CHwStates", "green", "754" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "823" );
        api().spoofData( "CHwStates", "green", "743" );
        api().sleepMsec( 6*1000 );
    } // 32 minutes in
     LOG_INFO ("spoofing initial baseline finished");
     api().spoofData( "CHwStates", "red", "848" );
     api().spoofData( "CHwStates", "green", "755" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "860" );
     api().spoofData( "CHwStates", "green", "767" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "900" );
     api().spoofData( "CHwStates", "green", "803" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "930" );
     api().spoofData( "CHwStates", "green", "837" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "950" );
     api().spoofData( "CHwStates", "green", "870" );
     api().sleepMsec( 12*1000 );
     api().spoofData( "CHwStates", "red", "965" );
     api().spoofData( "CHwStates", "green", "886" ); // 33 min in

     //
     // now do a second baseline so there isn't a discontinuity
     // spoof red and green with more noise to simulate Midrun platelets
     //
     for (int i = 1; i <= 540; i+=12 )  // 9 minutes more 
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Generating an event");
     for (int i = 1; i <= 60; i+= 12)   // an event to make sure system flags    
     {
         api().spoofData( "CHwStates", "red", "774" );
         api().spoofData( "CHwStates", "green", "619" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "705" );
         api().spoofData( "CHwStates", "green", "546" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Now assuming a WBC chamber purge has been triggered " ); 
     for (int i = 1; i <= 60; i+= 12) 
     {
         api().spoofData( "CHwStates", "red", "967" );
         api().spoofData( "CHwStates", "green", "783" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "962" );
         api().spoofData( "CHwStates", "green", "774" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 120; i+= 12)   
     {
         api().spoofData( "CHwStates", "red", "1245" );
         api().spoofData( "CHwStates", "green", "1119" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "1257" );
         api().spoofData( "CHwStates", "green", "1134" );
         api().sleepMsec( 6*1000 ); 
     }
     for (int i = 1; i <= 240; i+= 12)   // go just past the purge
     {
         api().spoofData( "CHwStates", "red", "1106" );
         api().spoofData( "CHwStates", "green", "1000" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "1098" );
         api().spoofData( "CHwStates", "green", "1007" );
         api().sleepMsec( 6*1000 ); 
     }
     LOG_INFO("Back to baseline");
     for (int i = 1; i <= 780; i+=12 )  // 13 minutes more at the Midrun Baseline
     {
         api().spoofData( "CHwStates", "red", "957" );
         api().spoofData( "CHwStates", "green", "871" );
         api().sleepMsec( 6*1000 );
         api().spoofData( "CHwStates", "red", "968" );
         api().spoofData( "CHwStates", "green", "884" );
         api().sleepMsec( 6*1000 ); 
     }
}

void Spoof_RequestRunsumStats :: init()
{
    _name = "RequestRunsumStats";
    _comment = "Spoofs a RequestEndRunStatsMsg message. System should not crash!";
    _timeout = 30;
}
void Spoof_RequestRunsumStats :: help()
{
    LOG_INFO( "Spoofs a RequestEndRunStatsMsg message. System should not crash!" );
}
void Spoof_RequestRunsumStats :: run()
{
    LOG_INFO( "Spoofing RequestEndRunStatsMsg message" );
	api().messageSend( "RequestEndRunStatsMsg", 0);
    api().spoofActivate();
}

// adding to test version 5.13 MidRun Baseline calculation
void Spoof_Baselines :: init()
{
    _name = "BASELINES";
    _comment = "Spoofs the red and green values used for two baseline calcs using var1 and var2 to specify the time";
    _timeout = 30 * 60; // 30 minutes 
}

void Spoof_Baselines :: help()
{
    LOG_INFO( "Start spoof at proc time of 6 minutes" );
    LOG_INFO( "Include -var1 xxx -var2 yyy to spoof first baseline for xxx seconds" );
    LOG_INFO( " and second baseline for yyy seconds" );
    LOG_INFO( "First Baseline corresponds roughly to Red Green ratio in PlateletPlasma" );
    LOG_INFO( "Second Baseline corresponds roughly to Red Green ratio in MidRun " );
    LOG_INFO( "See test procedure for specifics of timing and values" );
    LOG_INFO( "Default timeout on spoof is 30 minutes");
}

void Spoof_Baselines :: run()
{
    LOG_INFO( "Spoofing Baseline A for 6 minutes as default" );
    api().spoofData( "CHwStates", "red", "1122" );
    api().spoofData( "CHwStates", "green", "1002" );
    api().spoofActivate();

    for (int i = 1; i <= 300; i+=12 )  // six minutes 
    {
       api().spoofData( "CHwStates", "red", "1121" );
       api().spoofData( "CHwStates", "green", "1007" );
       api().sleepMsec( 6*1000 );
       api().spoofData( "CHwStates", "red", "1119" );
       api().spoofData( "CHwStates", "green", "1005" );
       api().sleepMsec( 6*1000 ); // six minutes or 12 into proc
    }
    LOG_INFO( "Changing ratio and noise" );
    for (int i = 1; i <= 30; i+=6)
    {
        api().spoofData( "CHwStates", "red", "1110" );
        api().spoofData( "CHwStates", "green", "1005" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "1115" );
        api().spoofData( "CHwStates", "green", "1007" );
        api().sleepMsec( 3*1000 );
    }
    for (int i = 1; i <= 1440; i+=12 )  // rest of the 30 minutes
    {
        api().spoofData( "CHwStates", "red", "1110" );
        api().spoofData( "CHwStates", "green", "1002" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1102" );
        api().spoofData( "CHwStates", "green", "985" );
        api().sleepMsec( 6*1000 ); 
    }
}

void Spoof_Baselines :: run_generic(const char* Var1, const char* Var2)
{   
    int time1 = atoi(Var1);
    int time2 = atoi(Var2);
    int timeRemaining;
    if ((time1< 0) || (time2 < 0) || time1 > 6000 || time2 > 6000) 
    {
        LOG_INFO( "variable times not in range >0 <= 6000 seconds so running default spoof");
        Spoof_Baselines::run();
    }
    
    timeRemaining = _timeout - time1 - time2 - 30;

    LOG_INFO( "Spoofing BaselineA for %d seconds and BaselineB for %d.", time1, time2 );
    LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
    api().spoofData( "CHwStates", "red", "1122" );
    api().spoofData( "CHwStates", "green", "1007" );
    api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including low noise
    //
    for (int i = 1; i <= time1; i+=12 )   
    {
       api().spoofData( "CHwStates", "red", "1119" );
       api().spoofData( "CHwStates", "green", "1007" );
       api().sleepMsec( 6*1000 );
       api().spoofData( "CHwStates", "red", "1116" );
       api().spoofData( "CHwStates", "green", "1005" );
       api().sleepMsec( 6*1000 ); 
    }
    // now transition to second baseline with larger noise 
    LOG_INFO( "Changing ratio and noise" );
    for (int i = 1; i <= 24; i+=6)
    {
       api().spoofData( "CHwStates", "red", "1095" );
       api().spoofData( "CHwStates", "green", "1002" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "1110" );
       api().spoofData( "CHwStates", "green", "1005" );
       api().sleepMsec( 3*1000 );
    }
    api().spoofData("CHwStates", "red", "1094" );
    api().spoofData( "CHwStates", "green", "998" );
    api().sleepMsec( 3*1000 );
    api().spoofData( "CHwStates", "red", "1050" );
    api().spoofData( "CHwStates", "green", "940" );
    api().sleepMsec( 3*1000 );

    LOG_INFO( "Spoofing next baseline values");
    for (int i = 1; i <= time2; i+=12 )   
    {
       api().spoofData( "CHwStates", "red", "976" );
       api().spoofData( "CHwStates", "green", "876" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "950" );
       api().spoofData( "CHwStates", "green", "848" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "945" );
       api().spoofData( "CHwStates", "green", "843" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "916" );
       api().spoofData( "CHwStates", "green", "781" );
       api().sleepMsec( 3*1000 );
    }
    LOG_INFO( "Spoofing remaining time with new values for R/G");
    for (int i = 1; i <= timeRemaining; i+=12 ) // any time remaining simulate platelet conc  
    {
        api().spoofData( "CHwStates", "red", "900" );
        api().spoofData( "CHwStates", "green", "781" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "872" );
        api().spoofData( "CHwStates", "green", "740" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "910" );
        api().spoofData( "CHwStates", "green", "780" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "873" );
        api().spoofData( "CHwStates", "green", "735" );
        api().sleepMsec( 3*1000 );
    }
}

void Spoof_SafetyTestDoneMsg :: init()
{
	_name = "Spoof_SafetyTestDoneMsg";
	_comment = "Spoofs message from safety that the T1 test is done in response to power request";
	_timeout = 60;
}

void Spoof_SafetyTestDoneMsg :: help()
{
	LOG_INFO( "- Start just before system will test solenoid and door lock" );
    LOG_INFO( "- To use generic version Var1 value is one of the trima_common/safproc.h values" );
    LOG_INFO( "- for the SafteyTestdoneMsg like PWR_CONTROL_SOLENOID_IGNORED" );
    LOG_INFO( "-Var2 has no meaning" );
}

void Spoof_SafetyTestDoneMsg :: run()
{
    LOG_INFO( "- Spoofing a solenoid ignored test from Safety" );
	api().messageSend( "SafetyTestDoneMsg", PWR_CONTROL_SOLENOID_IGNORED );
	api().spoofActivate();
}

void Spoof_SafetyTestDoneMsg :: run_generic(const char* Var1, const char* Var2)
{
    int testDoneFlg;
    if (strcmp(Var1, "PWR_CONTROL_SOLENOID_IGNORED") == 0)
        testDoneFlg = 1<<5;
    else if (strcmp(Var1, "PWR_CONTROL_64V_IGNORED") == 0)
        testDoneFlg = 1<<4;
    else if (strcmp(Var1, "PWR_CONTROL_24V_IGNORED") == 0)
        testDoneFlg = 1<<3;
    else if (strcmp(Var1, "PWR_CONTROL_SOLENOID_NOMINAL") == 0)
        testDoneFlg = 1<<2;
    else if (strcmp(Var1, "PWR_CONTROL_64V_NOMINAL") == 0)
        testDoneFlg = 1<<1;
    else if (strcmp(Var1, "PWR_CONTROL_24V_NOMINAL") == 0)
        testDoneFlg = 1<<0;
    else if (strcmp(Var1, "PWR_CONTROL_64V_FAILED") == 0)
        testDoneFlg = -2;
    else if (strcmp(Var1, "PWR_CONTROL_24V_FAILED") == 0)
        testDoneFlg = -1;
    else if (strcmp(Var1, "PWR_CONTROL_BOTH_NOMINAL") == 0) 
    {
        testDoneFlg = (1<<0 | 1<<1);
    }
    else if (strcmp(Var1, "PWR_CONTROL_BOTH_FAILED") == 0) 
    {
        testDoneFlg = (-1 | -2);
    }
    else
    {
        LOG_INFO("Invalid value using PWR_CONTROL_SOLENOID_IGNORED");
        testDoneFlg = 1<<5;
    }
    LOG_INFO("Spoofing SafetyTestDoneMsg to %s ", Var1);
    api().messageSend( "SafetyTestDoneMsg", testDoneFlg);
    api().spoofActivate();
}

// variation of SpoofBaselines to test Next Generation Spoofer substate awareness
void Spoof_TwoBaselines :: init()
{
    _name = "TWO_BASELINES";
    _comment = "Spoofs the red and green values used for two baseline calcs";
    _timeout = 30 * 60; // 30 minutes 
}

void Spoof_TwoBaselines :: help()
{
    LOG_INFO( "Start spoof any time before PlateletPlasma" );
    LOG_INFO( "First Baseline corresponds roughly to Red Green ratio in PlateletPlasma" );
    LOG_INFO( "Second Baseline corresponds roughly to Red Green ratio in MidRun " );
    LOG_INFO( "Default timeout on spoof is 30 minutes");
}

void Spoof_TwoBaselines :: run()
{
    int theSS;
    int* buf = &theSS;;
    api().waitForSubstate("PLATELETPLASMA", buf, sizeof(int), 20 * 60 * 1000); 
    LOG_INFO( "Spoofing Baseline A starting at PlateletPlasma if seen within 20 minutes" );
    api().spoofData( "CHwStates", "red", "1122" );
    api().spoofData( "CHwStates", "green", "1002" );
    api().spoofActivate();

    for (int i = 1; i <= 300; i+=12 )  // six minutes 
    {
       api().spoofData( "CHwStates", "red", "1121" );
       api().spoofData( "CHwStates", "green", "1007" );
       api().sleepMsec( 6*1000 );
       api().spoofData( "CHwStates", "red", "1119" );
       api().spoofData( "CHwStates", "green", "1005" );
       api().sleepMsec( 6*1000 ); // six minutes or 12 into proc
    }
    LOG_INFO( "Changing ratio and noise" );
    for (int i = 1; i <= 30; i+=6)
    {
        api().spoofData( "CHwStates", "red", "1110" );
        api().spoofData( "CHwStates", "green", "1005" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "1115" );
        api().spoofData( "CHwStates", "green", "1007" );
        api().sleepMsec( 3*1000 );
    }
    for (int i = 1; i <= 1440; i+=12 )  // rest of the 30 minutes
    {
        api().spoofData( "CHwStates", "red", "1110" );
        api().spoofData( "CHwStates", "green", "1002" );
        api().sleepMsec( 6*1000 );
        api().spoofData( "CHwStates", "red", "1102" );
        api().spoofData( "CHwStates", "green", "985" );
        api().sleepMsec( 6*1000 ); 
    }
}

void Spoof_TwoBaselines :: run_generic(const char* Var1, const char* Var2)
{   
    int time1 = atoi(Var1);
    int time2 = atoi(Var2);
    int timeRemaining;
    if ((time1< 0) || (time2 < 0) || time1 > 6000 || time2 > 6000) 
    {
        LOG_INFO( "variable times not in range >0 <= 6000 seconds so running default spoof");
        Spoof_TwoBaselines::run();
    }
    
    timeRemaining = _timeout - time1 - time2 - 30;

    LOG_INFO( "Spoofing BaselineA for %d seconds and BaselineB for %d.", time1, time2 );
    LOG_INFO( "Setting up baseline R/G for PrePlateletPlasma & PlateletPlasma" );
    api().spoofData( "CHwStates", "red", "1122" );
    api().spoofData( "CHwStates", "green", "1007" );
    api().spoofActivate();
    //
    // spoof red and green to simulate the PrePlateletPlasma values including low noise
    //
    for (int i = 1; i <= time1; i+=12 )   
    {
       api().spoofData( "CHwStates", "red", "1119" );
       api().spoofData( "CHwStates", "green", "1007" );
       api().sleepMsec( 6*1000 );
       api().spoofData( "CHwStates", "red", "1116" );
       api().spoofData( "CHwStates", "green", "1005" );
       api().sleepMsec( 6*1000 ); 
    }
    // now transition to second baseline with larger noise 
    LOG_INFO( "Changing ratio and noise" );
    for (int i = 1; i <= 24; i+=6)
    {
       api().spoofData( "CHwStates", "red", "1095" );
       api().spoofData( "CHwStates", "green", "1002" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "1110" );
       api().spoofData( "CHwStates", "green", "1005" );
       api().sleepMsec( 3*1000 );
    }
    api().spoofData("CHwStates", "red", "1094" );
    api().spoofData( "CHwStates", "green", "998" );
    api().sleepMsec( 3*1000 );
    api().spoofData( "CHwStates", "red", "1050" );
    api().spoofData( "CHwStates", "green", "940" );
    api().sleepMsec( 3*1000 );

    LOG_INFO( "Spoofing next baseline values");
    for (int i = 1; i <= time2; i+=12 )   
    {
       api().spoofData( "CHwStates", "red", "976" );
       api().spoofData( "CHwStates", "green", "876" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "950" );
       api().spoofData( "CHwStates", "green", "848" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "945" );
       api().spoofData( "CHwStates", "green", "843" );
       api().sleepMsec( 3*1000 );
       api().spoofData( "CHwStates", "red", "916" );
       api().spoofData( "CHwStates", "green", "781" );
       api().sleepMsec( 3*1000 );
    }
    LOG_INFO( "Spoofing remaining time with new values for R/G");
    for (int i = 1; i <= timeRemaining; i+=12 ) // any time remaining simulate platelet conc  
    {
        api().spoofData( "CHwStates", "red", "900" );
        api().spoofData( "CHwStates", "green", "781" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "872" );
        api().spoofData( "CHwStates", "green", "740" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "910" );
        api().spoofData( "CHwStates", "green", "780" );
        api().sleepMsec( 3*1000 );
        api().spoofData( "CHwStates", "red", "873" );
        api().spoofData( "CHwStates", "green", "735" );
        api().sleepMsec( 3*1000 );
    }
}


void Spoof_PumpErrors :: init()
{
    _name = "PumpErrors";
    _comment = "Spoofs the pump specified by variable var1 to the pump error status specified by var2";
    _timeout = 30; // 30 seconds
}

void Spoof_PumpErrors :: help()
{
    LOG_INFO( "Include -var1 <pumpName> -var2 <CHW_PUMP_ERROR> to spoof pump status (control)" );
    LOG_INFO( " where pumpName is acPump, inletPump, returnPump, plasmaPump, or plateletPump" );
    LOG_INFO( " and var2 is one of the CHW_PUMP_ERROR enumerations or a number" );
    LOG_INFO( "Default time is 30 seconds" );
}

void Spoof_PumpErrors :: run()
{
    LOG_INFO( "Spoofing AC pump to CHW_PUMP_INTEGRAL_ERROR" );
    api().spoofData( "CHwStates", "acPump_Errors", "CHW_PUMP_INTEGRAL_ERROR" );
    api().spoofActivate();
}

void Spoof_PumpErrors :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||
     (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||
     (strcmp(Var1, "acPump") == 0) ) 
    {
        if ((strcmp(Var2,"HW_NO_FAULT") == 0) || (strcmp(Var2,"CHW_PUMP_FAIL_TO_MAINTAIN_SPEED") == 0) ||
            (strcmp(Var2,"CHW_PUMP_INTEGRAL_ERROR") == 0) || (strcmp(Var2,"CHW_PUMP_VOLUME_ERROR") == 0) ||
            (strcmp(Var2,"CHW_MOTOR_FAULT") == 0) || (atoi(Var2) >=0)  )
        {
            LOG_INFO( "Spoofing pump %s to status %s.", Var1, Var2 );
        }
        else
        {
            LOG_ERROR( "var2 must be CHW_PUMP_ERRORS enumeration or a number" );
            return;
        }
       
    }
    else
    {
        LOG_ERROR( "var1 must be a pump name");
        return;
    }

    if (strcmp(Var1,"acPump") == 0)
    {
        api().spoofData( "CHwStates", "acPump_Errors", Var2 ); 
    }
    else if (strcmp(Var1,"inletPump") == 0)
    {
        api().spoofData( "CHwStates", "inletPump_Errors", Var2 );
    }
    else if (strcmp(Var1,"plasmaPump") == 0)
    {
        api().spoofData( "CHwStates", "plasmaPump_Errors", Var2 );
    }
    else if (strcmp(Var1,"plateletPump") == 0)
    {
        api().spoofData( "CHwStates", "plateletPump_Errors", Var2 );
    }
    else if (strcmp(Var1,"returnPump") == 0)
    {
        api().spoofData( "CHwStates", "returnPump_Errors", Var2 );
    }

    api().spoofActivate();
}

void Spoof_LeakDetector :: init()
{
    _name = "LeakDetector";
    _comment = "Control detects Leak Detector ";
    _timeout = 10;
}void Spoof_LeakDetector :: help()
{
    LOG_INFO( "- Include -var1 xxxx in command line " );
    LOG_INFO( "- where xxxx is an integer valuerange 0-3500" );
    LOG_INFO( "- with no parameters spoofs 0=failing shorted" );
    LOG_INFO( "- default timeout is 10 seconds" );
}
void Spoof_LeakDetector :: run()
{
    LOG_INFO( "since no parameter used, spoofing leak detector failing shorted (0)" );
    api().spoofData( "CHwStates", "leakDetector", "0" );
    api().spoofActivate();
}
void Spoof_LeakDetector :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO( "spoofing leak detector to %s", Var1 );
    api().spoofData( "CHwStates", "leakDetector", Var1 );
    api().spoofActivate();
}


// NEXT GENERATION SPOOF 
// Need to skip substates and to simulate GUI Repsonses

void SpoofAirToDonor :: init()
{
    _name = "AirToDonor";
    _comment = "Orders return pump movement with safety seeing reservoir empty";
    _timeout = 1; // 1 second should be enough   should detect within 1/4 of a second
}

void SpoofAirToDonor :: help()
{
    LOG_INFO( "Start any time after donor is connected" );
    LOG_INFO( "Use var1 to specify the return pump RPM" );
    LOG_INFO( "Default return pump RPM is 100, default timeout is 30 seconds" );
}

void SpoofAirToDonor :: run()
{
    LOG_INFO( "In SpoofAirToDonor");
    api().spoofData( "CHwOrders", "returnRPM", "100" );
    api().spoofData("SHwStates", "reservoir", "SHW_RESERVOIR_EMPTY");
    api().spoofActivate();
}

void SpoofAirToDonor :: run_generic(const char* Var1, const char* Var2)
{
    int RPM = atoi(Var1);
    LOG_INFO( " spoofing return pump to %d RPM", RPM );
    api().spoofData( "CHwOrders", "returnRPM", Var1 );
    api().spoofData("SHwStates", "reservoir", "SHW_RESERVOIR_EMPTY");
    api().spoofActivate();
}


// the next are added for testing STS
void Spoof_CCentrifugeError :: init()
{
    _name = "CCentrifugeError";
    _comment = "spoofs control centrifuge error";
    _timeout = 20; // 20 second should be enough   
}

void Spoof_CCentrifugeError :: help()
{
    LOG_INFO( "Start any time" ); // ?? may need centifuge to be running??
    LOG_INFO( "Use var1 to specify the error as HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR, " );
    LOG_INFO( "or HW_CENT_INTEGRAL_ERROR. " );
    LOG_INFO( "Default timeout is 20 seconds, but < 10 seconds won't cause an A2 alarm." );
}

void Spoof_CCentrifugeError :: run()
{
    LOG_INFO( "In CCentrifugeError");
    api().spoofData( "CHwStates", "centrifugeError", "HW_CENT_HARDWARE_ERROR" );
    api().spoofActivate();
}

void Spoof_CCentrifugeError :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"HW_CENT_NO_FAULT") == 0) || (strcmp(Var1,"HW_CENT_HARDWARE_ERROR") == 0) ||
        (strcmp(Var1,"HW_CENT_INTEGRAL_ERROR") == 0))
    {
        LOG_INFO( " spoofing control centrifuge error to %s", Var1);
        api().spoofData("CHwStates", "centrifugeError", Var1);
        api().spoofActivate();
    }
    else
    {
        LOG_INFO( "var1 must be HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR, or HW_CENT_INTEGRAL_ERROR" );
        LOG_INFO( " so not spoofing" );
    }
}


void Spoof_SCentrifugeError :: init()
{
    _name = "SCentrifugeError";
    _comment = "spoofs safety centrifuge error";
    _timeout = 20; // 20 second should be enough   
}

void Spoof_SCentrifugeError :: help()
{
    LOG_INFO( "Start any time" ); // ?? may need centifuge to be running??
    LOG_INFO( "Use var1 to specify the error as HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR, " );
    LOG_INFO( "or HW_CENT_INTEGRAL_ERROR. " );
    LOG_INFO( "Default timeout is 20 seconds, but < 10 seconds won't cause A2 alarm" );
}

void Spoof_SCentrifugeError :: run()
{
    LOG_INFO( "In SCentrifugeError");
    api().spoofData( "SHwStates", "centrifugeError", "1" );
    api().spoofActivate();
}

void Spoof_SCentrifugeError :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"HW_CENT_NO_FAULT") == 0) || (strcmp(Var1,"HW_CENT_HARDWARE_ERROR") == 0) ||
        (strcmp(Var1,"HW_CENT_INTEGRAL_ERROR") == 0))
    {
        LOG_INFO( " spoofing safety centrifuge error to %s", Var1);
        // safety status message is handled slightly differently so convert
        if (strcmp(Var1,"HW_CENT_HARDWARE_ERROR") == 0)
        {  
            api().spoofData("SHwStates","centrifugeError", "1");
        }
        else if (strcmp(Var1,"HW_CENT_INTEGRAL_ERROR") == 0)
        {
            api().spoofData("SHwStates","centrifugeError", "2");
        }
        else if (strcmp(Var1,"HW_CENT_NO_FAULT") == 0)
        {
            api().spoofData("SHwStates","centrifugeError", "0");
        }

        //api().spoofData("SHwStates", "centrifugeError", Var1);
        api().spoofActivate();
    }
    else
    {
        LOG_INFO( "var1 must be HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR, or HW_CENT_INTEGRAL_ERROR" );
        LOG_INFO( " so not spoofing" );
    }
}


void Spoof_HWFault :: init()
{
    _name = "Spoof_HWFault";
    _comment = "Spoofs control hw state for HW_ERROR fields to test STS interface ";
    _timeout = 20; // a total guess ??
}

void Spoof_HWFault :: help()
{
    LOG_INFO( "Start any time EMS or STS is connected" );
    LOG_INFO( "Use var1 to specify the field to spoof e.g., solenoidFault" );
    LOG_INFO( "Use var2 to specify HW_FAULT or HW_NO_FAULT" );
    LOG_INFO( "Default timeout is 30 seconds" );
}

void Spoof_HWFault :: run()
{
    LOG_INFO( "In Spoof_HWFault spoofing motorFault to HW_FAULT as default");
    api().spoofData( "CHwStates", "motorFault", "HW_FAULT" );
    api().spoofActivate();
}

void Spoof_HWFault :: run_generic(const char* Var1, const char* Var2)
{
    bool spoofFault;
    LOG_INFO( "In Spoof_HWFault with parameters %s and %s", Var1, Var2 );

    if ((strcmp(Var1,Var2) == 0) || (strcmp(Var2,"HW_FAULT") == 0)) // SHOULD ALSO CHECK TO SEE IF NULL
    {
        spoofFault = TRUE; 
    }
    else
    {
        spoofFault = FALSE;
    }

    if ((strcmp(Var1,"motorFault") == 0) || (strcmp(Var1,"solenoidFault") == 0) ||
        (strcmp(Var1,"valveFault") == 0) || (strcmp(Var1,"cassetteError") == 0) ||
        (strcmp(Var1,"rbcCommError") == 0) || (strcmp(Var1,"tempError") == 0) ||
        (strcmp(Var1,"fanSense0") == 0) || (strcmp(Var1,"fanSense1") == 0) ||
        (strcmp(Var1,"fanSense2") == 0) )
    {
        if (spoofFault)
        {
            api().spoofData("CHwStates", Var1, "HW_FAULT" );    
        }
        else
        {
            api().spoofData("CHwStates", Var1, "HW_NO_FAULT" );
        }
        api().spoofActivate();
    }
    else
    {
        LOG_INFO("Invalid parameter %s", Var1);
    }
}


// the hemolysis spoofs are very specific to the hemolysis signature and algorithm it
// is used to test. If this algorithm is changed these spoofs will most likely need to
// change as well
void Spoof_HEMOLYSIS :: init()
{
    _name = "HEMOLYSIS";
    _comment = "Spoofs the hemolysis red/green ratios var1 is first draw ratio and var2 is first return ratio";
    _timeout = 6 * 60; // 6 minutes 
}

void Spoof_HEMOLYSIS :: help()
{
    LOG_INFO( "Include -var1 xxxx -var2 yyyy to spoof red green ratio in first draw to xxxx" );
    LOG_INFO( "and red green ratio to yyyy during first return" );
    LOG_INFO( "If hemolysis detection is enabled, var1 >= 50.0 and var2 >= 1.1 show cause the alarm" );
    LOG_INFO( "See test procedure for specifics of timing and values" );
}

void Spoof_HEMOLYSIS :: run()
{
    int theSS;
    int* buf = &theSS;
    int ii;

    LOG_INFO( "Spoofing R/G ratio to 50.0 during first draw and to 1.1 with green <1000 " );
    LOG_INFO( "during first return as no values were given" );
    api().spoofData( "CHwStates", "red", "1000" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofActivate();
    api().sleepMsec(60000); // sleep 1 minute

    // now spoof the signature high R/G ratio IT-7059
    api().spoofData( "CHwStates", "red", "480" );
    api().spoofData( "CHwStates", "green", "9" );
    api().sleepMsec(2400000); // 4 minutes 

    // now spoof the return part of the signature
    api().spoofData( "CHwStates", "red", "1100" );
    api().spoofData( "CHwStates", "green", "1000" );
}

void Spoof_HEMOLYSIS :: run_generic(const char* Var1, const char* Var2)
{
    float drawRatio = atof(Var1);
    float returnRatio = atof(Var2);
    int greenVal;
    int ii;

    if (drawRatio == 0 || returnRatio == 0)
    {
        LOG_INFO( "R/G Ratio cannot be zero so not spoofing" );
    }
    else if (drawRatio < 0 || returnRatio < 0)
    {
        LOG_INFO( "R/G Ratio cannot be negative so not spoofing" );
    }
    else
    {
        api().sleepMsec(60000);
        LOG_INFO( "Spoofing R/G ratio to %s in first draw and %s in first return.", Var1, Var2 );
        // to be able to input R/G ratios assuming a red of 480 during first draw 
        // and a red of 1100 during first return
        greenVal = (int)(480.0/drawRatio);
        
        LOG_INFO( "Activating the spoof to set red to 480 and green to %d", greenVal );
        api().spoofData( "CHwStates", "red", "480" ); 
        api().spoofData( "CHwStates", "green", 0, greenVal );
        api().spoofActivate();

        // wait for the return
        api().sleepMsec(240000); // wait for 4 minutes for the return.

        LOG_INFO( "setting red to 1100 and green to %d", greenVal );
        greenVal = (int)(1100.0/returnRatio);
        api().spoofData( "CHwStates", "red", "1100" );
        api().spoofData( "CHwStates", "green", 0, greenVal );
    }
}


void Spoof_HEMOLYSIS_SLOW :: init()
{
    _name = "HEMOLYSIS_SLOW";
    _comment = "Spoofs the hemolysis red/green ratios var1 is first draw ratio and var2 is first return ratio";
    _timeout = 6 * 60 + 30; // 6.5 minutes 
}

void Spoof_HEMOLYSIS_SLOW :: help()
{
    LOG_INFO( "Include -var1 xxxx -var2 yyyy to spoof red green ratio in first draw to xxxx" );
    LOG_INFO( "and red green ratio to yyyy during first return" );
    LOG_INFO( "If hemolysis detection is enabled, var1 >= 50.0 and var2 >= 1.1 show cause the alarm" );
    LOG_INFO( "Start just before BLOOD_PRIME see test procedure for specific values to use" );
}

void Spoof_HEMOLYSIS_SLOW :: run()
{
    int theSS;
    int* buf = &theSS;
    int ii;

    LOG_INFO( "Spoofing R/G ratio to 50.0 during first draw (BLOOD_PRIME) and to 1.1 with green <1000 " );
    LOG_INFO( "with centrifuge RPM at 199 during first return as no values were given" );
    /* the folloing caused a pause because the centrifuge is not at speed so reversing for a second test
    api().spoofData( "CHwStates", "red", "1000" );
    api().spoofData( "CHwStates", "green", "1000" );
    api().spoofData( "CHwStates", "centrifugeRPM", "199" );
    api().waitForSubstate("BLOOD_PRIME", buf, sizeof(int), 4 * 60 * 1000); // wait up to 4 minutes
    api().spoofActivate();
    // wait 1 minute
    api().sleepMsec(60000);

    // now spoof the signature
    api().spoofData( "CHwStates", "red", "480" );
    api().spoofData( "CHwStates", "green", "9" );
    */

    api().waitForSubstate("BLOOD_PRIME", buf, sizeof(int), 4 * 60 * 1000); // wait up to 4 minutes
    api().spoofData( "CHwStates", "centrifugeRPM", "199" );
    api().spoofData( "CHwStates", "red", "480" );
    api().spoofData( "CHwStates", "green", "9" );
    api().spoofActivate();

    api().sleepMsec(60000);
    api().unspoofData( "CHwStates", "centrifugeRPM" ); 
    api().unspoofData( "CHwStates", "red" );
    api().unspoofData( "CHwStates", "green" );


    // wait for the return
    api().sleepMsec(240000);  // sleep 4 minutes

    api().spoofData( "CHwStates", "red", "1100" );
    api().spoofData( "CHwStates", "green", "1000" );
}

void Spoof_HEMOLYSIS_SLOW :: run_generic(const char* Var1, const char* Var2)
{
    float drawRatio = atof(Var1);
    float returnRatio = atof(Var2);
    int greenVal;
    int ii;

    if (drawRatio == 0 || returnRatio == 0)
    {
        LOG_INFO( "R/G Ratio cannot be zero so not spoofing" );
    }
    else if (drawRatio < 0 || returnRatio < 0)
    {
        LOG_INFO( "R/G Ratio cannot be negative so not spoofing" );
    }
    else
    {
        /* the following caused a pause so changing spoof
        api().sleepMsec(60000);
        
        LOG_INFO( "Spoofing R/G ratio to %s in first draw and %s in first return with slow (199RPM) centifuge.", Var1, Var2 );

        // to be able to input R/G ratios assuming a red of 480 during first draw 
        // and a red of 1100 during first return
        greenVal = (int) (480.0/drawRatio);
        api().spoofData( "CHwStates", "centrifugeRPM", "199" );
        api().spoofData( "CHwStates", "green", 0, greenVal );
        api().spoofData( "CHwStates", "red", "480" ); 
        */

        LOG_INFO( "Spoofing R/G ratio to %s in first draw and %s in first return with slow (199RPM) centifuge.", Var1, Var2 );

        greenVal = (int) (480.0/drawRatio);
        api().spoofData( "CHwStates", "centrifugeRPM", "199" );
        api().spoofData( "CHwStates", "green", 0, greenVal );
        api().spoofData( "CHwStates", "red", "480" ); 
        api().spoofActivate();

        api().sleepMsec(60000);
        api().unspoofData( "CHwStates", "centrifugeRPM" );
        api().unspoofData( "CHwStates", "red" );
        api().unspoofData( "CHwStates", "green" );

        // wait for the return cycle
        api().sleepMsec(240000);  // sleep 4 minutes

        greenVal = (int)(1100.0/returnRatio);
        api().spoofData( "CHwStates", "red", "1100" );
        api().spoofData( "CHwStates", "green", 0, greenVal );
    }
}


void Spoof_SPumpRevs :: init()
{
    _name = "SPumpRevs";
    _comment = "Spoofs the delta revs safety sees for the pump specified by variable var1 to the value specified by var2";
    _timeout = 10; // 10 seconds
}

void Spoof_SPumpRevs :: help()
{
    LOG_INFO( "Include -var1 <pumpName> -var2 <revs> to spoof safety status pump revs" );
    LOG_INFO( " where pumpName is acPump, inletPump, returnPump, plasmaPump, or plateletPump" );
    LOG_INFO( " and var2 is the number of delta revs to spoof" );
    LOG_INFO( "Default time is 10 seconds" );
}

void Spoof_SPumpRevs :: run()
{
    LOG_INFO( "Spoofing AC pump to 2" );
    api().spoofData( "SHwStates", "acRevs", "2" );
    api().spoofActivate();
}

void Spoof_SPumpRevs :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||
     (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||
     (strcmp(Var1, "acPump") == 0) ) 
    {
        if (atoi(Var2) >=0  )
        {
            LOG_INFO( "Spoofing pump %s status %s revs.", Var1, Var2 );
        }
        else
        {
            LOG_ERROR( "var2 must be a number" );
            return;
        }
       
    }
    else
    {
        LOG_ERROR( "var1 must be a pump name");
        return;
    }

    if (strcmp(Var1,"acPump") == 0)
    {
        api().spoofData( "SHwStates", "acRevs", Var2 ); 
    }
    else if (strcmp(Var1,"inletPump") == 0)
    {
        api().spoofData( "SHwStates", "inletRevs", Var2 );
    }
    else if (strcmp(Var1,"plasmaPump") == 0)
    {
        api().spoofData( "SHwStates", "plasmaRevs", Var2 );
    }
    else if (strcmp(Var1,"plateletPump") == 0)
    {
        api().spoofData( "SHwStates", "plateletRevs", Var2 );
    }
    else if (strcmp(Var1,"returnPump") == 0)
    {
        api().spoofData( "SHwStates", "returnRevs", Var2 );
    }

    api().spoofActivate();
}


void Spoof_CPumpRPM :: init()
{
    _name = "CPumpRPM";
    _comment = "Spoofs the pump speed in control status to the value specified by variable var2 for pump specified by var1";
    _timeout = 30; // 30 seconds
}

void Spoof_CPumpRPM :: help()
{
    LOG_INFO( "Include -var1 <pumpName> -var2 <RPM> to spoof control status pump speed" );
    LOG_INFO( "Default timeout is 30 seconds" );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofed speed" ); 
    LOG_INFO( "for a short time after the inititating event" );
}

void Spoof_CPumpRPM :: run()
{
    int theSS;
    int* buf =&theSS;

    LOG_INFO( "Spoofing plasma pump speed to 2 RPM now" );
    api().spoofData( "CHwStates", "plasmaRPM", "2" );
    api().spoofActivate();
}

void Spoof_CPumpRPM :: run_generic(const char* Var1, const char* Var2)
{ 
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||
        (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||
        (strcmp(Var1, "acPump") == 0) ) 
    {
        if ((atoi(Var2) >=0) || (atof(Var2) >= 0) )
        {
            LOG_INFO( "Spoofing pump %s status %s RPM.", Var1, Var2 );
        }
        else
        {
            LOG_ERROR( "var2 must be an number" );
            return;
        }

    }
    else
    {
        LOG_ERROR( "var1 must be a pump name");
        return;
    }

    if (strcmp(Var1,"acPump") == 0)
    {
        api().spoofData( "CHwStates", "acRPM", Var2 ); 
    }
    else if (strcmp(Var1,"inletPump") == 0)
    {
        api().spoofData( "CHwStates", "inletRPM", Var2 );
    }
    else if (strcmp(Var1,"plasmaPump") == 0)
    {
        api().spoofData( "CHwStates", "plasmaRPM", Var2 );
    }
    else if (strcmp(Var1,"plateletPump") == 0)
    {
        api().spoofData( "CHwStates", "plateletRPM", Var2 );
    }
    else if (strcmp(Var1,"returnPump") == 0)
    {
       api().spoofData( "CHwStates", "returnRPM", Var2 );
    }

    api().spoofActivate();
}


void Spoof_CPumpMult :: init()
{
    _name = "CPumpMult";
    _comment = "Changes the commanded speed in control status for the pump specified by var1 (returnPump, plasmaPump, plateletPump, inletPump, acPump)";
	_comment = "to var2 times the original speed (new_speed = var2 X original_speed)";
    _timeout = 30; // 30 seconds
}

void Spoof_CPumpMult :: help()
{
    LOG_INFO( "Include -var1 <pumpName> -var2 <RPM> to spoof control status pump speed" );
    LOG_INFO( "Default timeout is 30 seconds" );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofed speed" ); 
    LOG_INFO( "for a short time after the inititating event" );
}

void Spoof_CPumpMult :: run()
{
    int theSS;
    int* buf =&theSS;

    LOG_INFO( "Spoofing plasma pump speed to 1.5X commanded speed now" );
    api().spoofData( "CHwStates", "plasmaRPM", 1.5, 0.0 );
    api().spoofActivate();
}

void Spoof_CPumpMult :: run_generic(const char* Var1, const char* Var2)
{ 
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||
        (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||
        (strcmp(Var1, "acPump") == 0) ) 
    {
        if ((atoi(Var2) >=0) || (atof(Var2) >= 0) )
        {
            LOG_INFO( "Spoofing pump %s status %s RPM.", Var1, Var2 );
        }
        else
        {
            LOG_ERROR( "var2 must be an number" );
            return;
        }

    }
    else
    {
        LOG_ERROR( "var1 must be a pump name");
        return;
    }

    if (strcmp(Var1,"acPump") == 0)
    {
        api().spoofData( "CHwStates", "acRPM", atof(Var2), 0.0 ); 
    }
    else if (strcmp(Var1,"inletPump") == 0)
    {
        api().spoofData( "CHwStates", "inletRPM", atof(Var2), 0.0 );
    }
    else if (strcmp(Var1,"plasmaPump") == 0)
    {
        api().spoofData( "CHwStates", "plasmaRPM", atof(Var2), 0.0 );
    }
    else if (strcmp(Var1,"plateletPump") == 0)
    {
        api().spoofData( "CHwStates", "plateletRPM", atof(Var2), 0.0 );
    }
    else if (strcmp(Var1,"returnPump") == 0)
    {
       api().spoofData( "CHwStates", "returnRPM", atof(Var2), 0.0 );
    }

    api().spoofActivate();
}

// the following spoof was written to test 7720, but always created an alarm when the plasma pump was commanded.
// leaving here for reference, but deactiving
/*
void Spoof_COPlasma_PCAMIDRUN :: init()
{
    _name = "COPlasma_PCAMIDRUN";
    _comment = "Spoofs the control hardware orders for plasma pump during PCA to var1 and during Midrun to the value specified by var2";
    _timeout = 2100; // 35 minutes
}

void Spoof_COPlasma_PCAMIDRUN :: help()
{
    LOG_INFO( "Include -var1 <PCA plsRPM> -var2 <Midrun plsRPM> to spoof control hardware pump speeds" );
    LOG_INFO( " for testing IT 7720" );
    LOG_INFO( "Default time is 35 minutes.  With no parameters spoofs to 40 in PCA and 0.5 in MidRun" );
}

void Spoof_COPlasma_PCAMIDRUN :: run()
{
    int theSS;
    int* buf = &theSS;
    LOG_INFO( "Will spoof plasma pump orders to 40 during PCA" );

    api().waitForSubstate("PCA", buf, sizeof(int), 34 * 60 * 1000); 

    LOG_INFO( "Spoofing plasma pump orders to 40 now" );
    api().spoofData( "CHwOrders", "plasmaRPM", "40" );
    api().spoofData( "CHwStates", "plasmaRPM", "40" );
    api().spoofData( "SHwStates", "plasmaRPM", "40" );
    api().spoofActivate();

    api().waitForSubstate("MIDRUN", buf, sizeof(int), 25* 60 *1000);
    LOG_INFO( "Now spoofing the plasma pump orders to 0.5" );
    api().spoofData( "CHwOrders", "plasmaRPM", "0.5" );
    api().spoofData( "CHwStates", "plasmaRPM", "0.5" );
    api().spoofActivate();
    api().sleepMsec( 2000 );
    api().spoofDisable();
}

void Spoof_COPlasma_PCAMIDRUN :: run_generic(const char* Var1, const char* Var2)
{
    int theSS;
    int* buf = &theSS;
    LOG_INFO( "Will spoof plasma pump orders to %s during PCA", Var1 );

    api().waitForSubstate("PCA", buf, sizeof(int), 34 * 60 * 1000); 

    LOG_INFO( "Spoofing plasma pump orders to %s now", Var1 );
    api().spoofData( "CHwOrders", "plasmaRPM", Var1 );
    api().spoofData( "CHwStates", "plasmaRPM", Var1 );
    api().spoofData( "SHwStates", "plasmaRPM", Var1 );
    api().spoofActivate();

    LOG_INFO("Now spoofing the plasma pump orders to 0.5 and state to %s" , Var2 );
    api().waitForSubstate("MIDRUN", buf, sizeof(int), 25* 60 *1000);
    api().spoofData( "CHwOrders", "plasmaRPM", "0.5" );
    api().spoofData( "CHwStates", "plasmaRPM", Var2 );
    api().spoofData( "SHwStates", "plasmaRPM", Var2 );
    api().spoofActivate();
    api().sleepMsec( 2000 );
    api().spoofDisable();
}
*/

void Spoof_ControlACPumpAccum :: init()
{
    _name = "ControlACPumpAccum";
    _comment = "Spoofs the control AC pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_ControlACPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_ControlACPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_ControlACPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing AC Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "acPumpAccum", atof(Var1), atof(Var2) * 20608); 
	api().spoofActivate();
}

void Spoof_ControlInletPumpAccum :: init()
{
    _name = "ControlInletPumpAccum";
    _comment = "Spoofs the control Inlet pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_ControlInletPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_ControlInletPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_ControlInletPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Inlet Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "inletPumpAccum", atof(Var1), atof(Var2) * 20608);
	api().spoofActivate();
}

void Spoof_ControlPlasmaPumpAccum :: init()
{
    _name = "ControlPlasmaPumpAccum";
    _comment = "Spoofs the control Plasma pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_ControlPlasmaPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_ControlPlasmaPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_ControlPlasmaPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Plasma Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "plasmaPumpAccum", atof(Var1), atof(Var2) * 20608); 
	api().spoofActivate();
}

void Spoof_ControlPlateletPumpAccum :: init()
{
    _name = "ControlPlateletPumpAccum";
    _comment = "Spoofs the control Plasma pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_ControlPlateletPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_ControlPlateletPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_ControlPlateletPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Platelet Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "plateletPumpAccum", atof(Var1), atof(Var2) * 20608); 
	api().spoofActivate();
}

void Spoof_ControlReturnPumpAccum :: init()
{
    _name = "ControlReturnPumpAccum";
    _comment = "Spoofs the control Return pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_ControlReturnPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_ControlReturnPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_ControlReturnPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Return Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "returnPumpAccum", atof(Var1), atof(Var2) * 20608); 
	api().spoofActivate();
}

void Spoof_SafetyACPumpAccum :: init()
{
    _name = "SafetyACPumpAccum";
    _comment = "Spoofs the safety AC pump accumulated hall counts using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_SafetyACPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum hall counts." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated hall counts." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_SafetyACPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_SafetyACPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing AC Pump accumulated hall counts by %f and %f.", atof(Var1), atof(Var2));
    api().spoofData("SHwStates", "acPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_SafetyInletPumpAccum :: init()
{
    _name = "SafetyInletPumpAccum";
    _comment = "Spoofs the safety Inlet pump accumulated hall counts using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_SafetyInletPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum hall counts." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated hall counts." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_SafetyInletPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_SafetyInletPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Inlet Pump accumulated hall counts by %f and %f.", atof(Var1), atof(Var2));
    api().spoofData("SHwStates", "inletPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_SafetyPlasmaPumpAccum :: init()
{
    _name = "SafetyPlasmaPumpAccum";
    _comment = "Spoofs the safety Plasma pump accumulated hall counts using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_SafetyPlasmaPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum hall counts." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated hall counts." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_SafetyPlasmaPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_SafetyPlasmaPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Plasma Pump accumulated hall counts by %f and %f.", atof(Var1), atof(Var2));
    api().spoofData("SHwStates", "plasmaPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_SafetyPlateletPumpAccum :: init()
{
    _name = "SafetyPlateletPumpAccum";
    _comment = "Spoofs the safety Platelet pump accumulated hall revolutions using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_SafetyPlateletPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum hall counts." );
    LOG_INFO( "IMPORTANT NOTE: Multiplier will spoof using a scalar equal to -var1 whereas the -var2 offset will be multiplied by 2 to yield revolutions rather than hall counts." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated hall counts." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_SafetyPlateletPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_SafetyPlateletPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Platelet Pump accumulated hall counts by %f times plus ( 2 * %f ) revolutions.", atof(Var1), atof(Var2));
    api().spoofData("SHwStates", "plateletPumpAccum", atof(Var1), atof(Var2) * 2); 
	api().spoofActivate();
}

void Spoof_SafetyReturnPumpAccum :: init()
{
    _name = "SafetyReturnPumpAccum";
    _comment = "Spoofs the safety Return pump accumulated hall counts using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_SafetyReturnPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum hall counts." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated hall counts." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_SafetyReturnPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_SafetyReturnPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Return Pump accum hall counts by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("SHwStates", "returnPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_BothACPumpAccum :: init()
{
    _name = "BothACPumpAccum";
    _comment = "Spoofs both the safety and control AC pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_BothACPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_BothACPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_BothACPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing AC Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "acPumpAccum", atof(Var1), atof(Var2) * 20608); 
    api().spoofData("SHwStates", "acPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_BothInletPumpAccum :: init()
{
    _name = "BothInletPumpAccum";
    _comment = "Spoofs both the safety and control Inlet pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_BothInletPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_BothInletPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_BothInletPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Inlet Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "inletPumpAccum", atof(Var1), atof(Var2) * 20608); 
    api().spoofData("SHwStates", "inletPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_BothPlasmaPumpAccum :: init()
{
    _name = "BothPlasmaPumpAccum";
    _comment = "Spoofs both the safety and control Plasma pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_BothPlasmaPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_BothPlasmaPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_BothPlasmaPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Plasma Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "plasmaPumpAccum", atof(Var1), atof(Var2) * 20608); 
    api().spoofData("SHwStates", "plasmaPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_BothPlateletPumpAccum :: init()
{
    _name = "BothPlateletPumpAccum";
    _comment = "Spoofs both the safety and control Platelet pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_BothPlateletPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_BothPlateletPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_BothPlateletPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Platelet Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "plateletPumpAccum", atof(Var1), atof(Var2) * 20608); 
    api().spoofData("SHwStates", "plateletPumpAccum", atof(Var1), atof(Var2) * 2); 
	api().spoofActivate();
}

void Spoof_BothReturnPumpAccum :: init()
{
    _name = "BothReturnPumpAccum";
    _comment = "Spoofs both the safety and control Return pump accumulated revs using given multiplier and offset.";
	_timeout = 10;
}

void Spoof_BothReturnPumpAccum :: help()
{
    LOG_INFO( "Include -var1 <multiplier> -var2 <offset> to spoof the pump accum encoder revs." );
    LOG_INFO( "Default timeout is 10 seconds." );
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofing of accumulated encoder revs." ); 
    LOG_INFO( "Without parameters spoofs the Pump to 1.015 times" );
}

void Spoof_BothReturnPumpAccum :: run()
{
	this->run_generic("1.015", "0");
}

void Spoof_BothReturnPumpAccum :: run_generic(const char* Var1, const char* Var2)
{ 
	LOG_INFO("Spoofing Return Pump accum encoder revs by %1.2f and %1.2f.", atof(Var1), atof(Var2));
    api().spoofData("CHwStates", "returnPumpAccum", atof(Var1), atof(Var2) * 20608); 
    api().spoofData("SHwStates", "returnPumpAccum", atof(Var1), atof(Var2)); 
	api().spoofActivate();
}

void Spoof_ReservoirStatus :: init()
{
    _name = "ReservoirStatus";
    _comment = "Spoofs the reservoir status";
    _timeout = 10 * 60;
}
void Spoof_ReservoirStatus :: help()
{
    LOG_INFO( "- Start spoof during draw cycle to simulate reservoir sensor high" );
}
void Spoof_ReservoirStatus :: run()
{
	this->run_generic("164", "50");
}

void Spoof_ReservoirStatus :: run_generic(const char* Var1, const char* Var2)
{
	std::vector<float> volumes;
	std::vector<int> periods;
	char delim[] = ",";
	char *result = NULL;

	ProcVolumes_CDS cds(ROLE_RO);

	result = std::strtok(const_cast<char*>(Var1), delim);
	while (result != NULL) {
		volumes.push_back(std::atof(result));
		result = strtok(NULL, delim);
	}

	result = std::strtok(const_cast<char*>(Var2), delim);
	while (result != NULL) {
		periods.push_back(std::atoi(result));
		result = strtok(NULL, delim);
	}

	if (volumes.size() == periods.size()) {
		for (int i = 0; i < volumes.size(); ++i) {
		   	while (cds.VinTotal - cds.Vreturn < volumes[i]) {
				api().sleepMsec(50); 
			}

			DataLog(log_level_spoofer_info) << "Spoofing Reservoir Status"
				<< " Volume:" << volumes[i]
				<< " Period:" << periods[i] 
				<< endmsg;
			api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_HIGH" );
			if (i == 0) {
				api().spoofActivate();
			}
			api().sleepMsec(periods[i]);
			DataLog(log_level_spoofer_info) << "Unspoofing Reservoir Status"
				<< " Volume:" << volumes[i]
				<< " Period:" << periods[i] 
				<< endmsg;
			api().unspoofData( "CHwStates", "reservoir" );
		}
	}
	else {
		LOG_ERROR( "Num volumes must equal num time frames." );
	}
}

void Spoof_ReservoirHct :: init()
{
    _name = "ReservoirHct";
    _comment = "Spoofs the reservoir HCT status";
    _timeout = 60;
}
void Spoof_ReservoirHct :: help()
{
    LOG_INFO( "- Start the spoof during a return cycle." );
}
void Spoof_ReservoirHct :: run()
{
	this->run_generic("60", "");
}

void Spoof_ReservoirHct :: run_generic(const char* Var1, const char* Var2)
{
	float value = atof(Var1);

    LOG_INFO( "Spoofing ResHct at %1.2f", value );

	api().spoofData( "ProcCycle_ResHct", "data", 0, value );
    api().spoofActivate();
}

void Spoof_SealSafeOvertemp :: init()
{
    _name = "SealSafeOvertemp";
    _comment = "Spoofs seal safe over temp";
    _timeout = 60;
}

void Spoof_SealSafeOvertemp :: help()
{
    LOG_INFO( "- Start the spoof while running STS." );
}

void Spoof_SealSafeOvertemp :: run()
{
	api().spoofData( "CHwStates", "sealSafeOvertemp", "CHW_SEAL_SAFE_OTW" );
    api().spoofActivate();
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to force an Extended 
 *  Platelet Concentration Adjustment (EPCA) phase, by adding sub state values 
 *  to PROC that may not be predicted. */
void Spoof_FORCE_EPCA::init( ){ 
	_name = "Force_EPCA";
	_comment = "Force an EPCA";
	_timeout = 180 ;			// 3 Minute timeout
}
void Spoof_FORCE_EPCA::help( ){ 
	LOG_INFO( "Used to force an Extended Plasma Concentration Adjustment Phase (EPCA) ");
}
void Spoof_FORCE_EPCA::run( ){ 

	/* Values taken from 1T02218_20050124_000_000103.dlog */

	/* Set inlet pump to draw and return */
    api().spoofData( "Predict_SubstateInfo", "QinD_9", "95.0" );
    api().spoofData( "Predict_SubstateInfo", "QinR_9", "75.0" );
	
	/* Set plasma (collect (c)) pump to draw and return */
    api().spoofData( "Predict_SubstateInfo", "QcD_9", "20.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_9", "20.0" );

	/* Give a target of 12 Ml for Platelet Collection in EPCA */
    api().spoofData( "Predict_SubstateInfo", "Vtarget_9",		"12.0" );
	api().spoofData( "Predict_SubstateInfo", "VtargetType_9",	"VCOLBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "VCOLBAG" );
    api().spoofData( "Predict_SubstateInfo", "PlateletValve_9", "HAL_VALVE_COLLECT" ); 
	
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to force an Extended 
 *  Platelet Concentration Adjustment (EPCA) phase, by adding sub state values 
 *  to PROC that may not be predicted. */
void Spoof_PLO_EPCA::init( ){ 
	_name = "PLO_EPCA";
	_comment = "Plasma Line Occlusion EPCA and RED\\GREEN values to cause WBC Flag";
	_timeout = 300 ;			// 5 Minute timeout
}
void Spoof_PLO_EPCA::help( ){ 
	LOG_INFO( "Used to for an Extended Plasma Concentration Adjustment Phase (EPCA) ");
	LOG_INFO( "  AND set the RED\\GREEN values such that a WBC flag is set on ");
	LOG_INFO( "  Platelet products. ");
	LOG_INFO( "Include -var1 xxx to set a new specific bag volume during EPCA ");
}
void Spoof_PLO_EPCA::run( ){ 
	run_generic( "300", NULL );	
}
void Spoof_PLO_EPCA::run_generic( const char *pcVar1, const char *pcVar2){ 


	char	caBagVol[] = "300";
	char	*p_caBagVol = caBagVol;

	/* Check that pcVar1 is /NOT/ null ... user has a desired volume */
	if( NULL != pcVar1 ){

		p_caBagVol = const_cast< char *> ( pcVar1 ); 
	}

	LOG_INFO( "Setting up fault R/G for EPCA at %s ml", p_caBagVol );

	/* Give a target of 300 Ml for Platelet Collection in EPCA */
	// EPCA substate
	api().spoofData( "Predict_SubstateInfo", "Vtarget_9",		p_caBagVol ); 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_9",	"VCOLBAG" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "NOCOLLECT" );
	api().spoofData( "Predict_SubstateInfo", "PlateletValve_9", "HAL_VALVE_COLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "Qac_9",			"8.99" ); 
	/* Collect and Platelet */
	api().spoofData( "Predict_SubstateInfo", "QcD_9",			"4.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QcR_9",			"4.99" ); 
	/* Inlet */
	api().spoofData( "Predict_SubstateInfo", "QinD_9",			"119.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QinR_9",			"84.99" ); 
	/* Plasma */
	api().spoofData( "Predict_SubstateInfo", "QpD_9",			"20.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QpR_9",			"20.99" ); 
	/* Return */
	api().spoofData( "Predict_SubstateInfo", "Qrp_9",			"333.99" ); 

	/* Set RED GREEN ration low ... to cause a WBC contamination flag */
	api().spoofData( "CHwStates",			"red",				"400" );
	api().spoofData( "CHwStates",			"green",			"300" );

	api().spoofActivate();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to force /NO/ Midrun
 *  phase, by adding sub state values to PROC to bypass the Midrun phase. */
void Spoof_Skip_Midrun::init( ){ 
	_name = "Skip_Midrun";
	_comment = "Used to Skip the Midrun substate";
	_timeout = 120 ;			// 2 Minute timeout
}
void Spoof_Skip_Midrun::help( ){ 

	LOG_INFO( "Used to skip the Midrun substate by setting VtargetType == NOCOLLECT(0),");
	LOG_INFO( " and Vtarget == 0 ");
}
void Spoof_Skip_Midrun::run( ){ 

	LOG_INFO( "Setting Midrun values to collect to 0 Ml and NOCOLLECT " );

	/* Give a target of 0 Ml for  Collection in MIDRUN */
	// Mid-Run substate
    api().spoofData( "Predict_SubstateInfo", "Vtarget_6",		"0" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_6",	"NOCOLLECT" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_6", "NOCOLLECT" );

	api().spoofActivate();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to generate a known
 *  RED\GREEN reflectance, and then inject a poison reflectance that should
 *  be ignored by the PLO logic as it's during an (test conductor generated) Alarm 
 *  or Recovery */
void Spoof_PLO_Poison_Midrun::init( ){ 
	_name = "PLO_Poison_Midrun";
	_comment = "Used to generate a known Red\\Green reflectance, and then inject a\n",
				" poison value before returning to a known Red\\Green value\n";	
	_timeout = 480 ;			// 8 Minute timeout
}
void Spoof_PLO_Poison_Midrun::help( ){ 

	LOG_INFO( "Specificly designed for testing Plasma Line Occlusion (PLO) flag,\n");
	LOG_INFO( " Used to generate a known Red\\Green reflectance, and then inject a \n");
	LOG_INFO( " poison value before returning to a known Red\\Green value\n");
}
void Spoof_PLO_Poison_Midrun::run( ){ 

	const char caRed[] = "650",
				caGreen[] = "625",
				caPoison[] = "2500";

	/* Set a known baseline value */
	LOG_INFO( " Spoofer : setting PLO Baseline values (during MidRun) to %s \\ %s  ", 
		caRed, caGreen );
    api().spoofData( "CHwStates", "red", caRed );
    api().spoofData( "CHwStates", "green", caGreen );
	api().spoofActivate();

	/* Thirty (30) second warning till poison; delay is from start of spoof
	 *	RED\GREEN value; will poison values 2'30" into this spoof */
	LOG_INFO( " Spoofer : 30 Seconds till Poison data \n" );
	api().sleepMsec( 1000 * (2 * 60) );

	/* Ten (10) second warning till poison */
	LOG_INFO( " Spoofer : 10 Seconds till Poison data \n" );
	api().sleepMsec( 1000 * 10 );

	/* Injecting poison values */
	LOG_INFO( " Spoofer : setting PLO POISON values (during MidRun) to %s \\ %s  ", 
		caPoison, caPoison );
    api().spoofData( "CHwStates", "red", caPoison );
    api().spoofData( "CHwStates", "green", caPoison );
	
	/* Inject poison for 15 seconds */
	api().sleepMsec( 1000 * 15 );

	/* Set a known baseline value */
	LOG_INFO( " Spoofer : setting PLO Baseline values (during MidRun) to %s \\ %s  ", 
		caRed, caGreen );
    api().spoofData( "CHwStates", "red", caRed );
    api().spoofData( "CHwStates", "green", caGreen );
	api().spoofActivate();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to force the early end of Mid-Run
 *  such that PLO does not receive the ^FULL^ baseline count of point.*/
void Spoof_PLO_Short_Midrun::init( ){ 
	_name = "PLO_Short_Midrun";
	_comment = "Used to generate a known Red\\Green reflectance, and shorten the Mid-Run\n",
				" such that PLO does not receive the ^FULL^ baseline count.\n";	
	_timeout = 600 ;			// 10 Minute timeout
}
void Spoof_PLO_Short_Midrun::help( ){ 

	LOG_INFO( "Specificly designed for testing Plasma Line Occlusion (PLO) flag,\n");
	LOG_INFO( " Used to generate a known Red\\Green reflectance, and shorten the Mid-Run\n");
	LOG_INFO( " such that PLO does not receive the ^FULL^ baseline count.\n");
}
void Spoof_PLO_Short_Midrun::run( ){ 

	/* NOTE : Overwrite the Predict values first so we guarantee we hit 
	 *	EPCA with our short Mid-Run */
	/* FORCE and EPCA */
	char	caBagVol[] = "300";
	LOG_INFO( "Scheduling an  EPCA with Platelet volume of %s ml", caBagVol  );

	/* Give a target of 300 Ml for Platelet Collection in EPCA */
	// EPCA substate
	api().spoofData( "Predict_SubstateInfo", "Vtarget_9",		caBagVol ); 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_9",	"VCOLBAG" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "NOCOLLECT" );
	api().spoofData( "Predict_SubstateInfo", "PlateletValve_9", "HAL_VALVE_COLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "Qac_9",			"8.99" ); 
	/* Collect and Platelet */
	api().spoofData( "Predict_SubstateInfo", "QcD_9",			"4.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QcR_9",			"4.99" ); 
	/* Inlet */
	api().spoofData( "Predict_SubstateInfo", "QinD_9",			"119.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QinR_9",			"84.99" ); 
	/* Plasma */
	api().spoofData( "Predict_SubstateInfo", "QpD_9",			"20.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QpR_9",			"20.99" ); 
	/* Return */
	api().spoofData( "Predict_SubstateInfo", "Qrp_9",			"333.99" ); 
	
	/* Set a known baseline value */
	const char caRed[] = "650",
				caGreen[] = "625";

	LOG_INFO( " Spoofer : setting PLO Baseline values (during MidRun) to %s \\ %s  ", 
		caRed, caGreen );
    api().spoofData( "CHwStates", "red", caRed );
    api().spoofData( "CHwStates", "green", caGreen );

	api().spoofActivate();

	/* Eight (8) Minute warning till we shorten the Mid-Run into a forced 
	 * EPCA */
	LOG_INFO( " Spoofer : 8 Minutes until early change from Mid-Run \n" );
	api().sleepMsec( 1000 * (( 7 * 60) + 30)  );

	/* Thirty (30) second warning till we shorten the Mid-Run into a forced 
	 * EPCA */
	LOG_INFO( " Spoofer : 30 Seconds till early change from Mid-Run \n" );
	api().sleepMsec( 1000 * (20) );

	/* Ten (10) second warning till we shorten the Mid-Run into a forced 
	 * EPCA 
	 *
	 * Note : After some testing, it's shown that Red\Green values are not
	 *	getting spoofed quick enough, changing timeout value to 9 seconds
	 *	and starting Red/Green spoof a hair earlier
	 * */
	LOG_INFO( " Spoofer : 10 Seconds till early change from Mid-Run \n" );
	api().sleepMsec( 1000 * (9) );

	/* Setup Red & Green for possible PLO flagging during EPCA */
	/* Set RED GREEN ration low ... to cause a WBC contamination flag */
	api().spoofData( "CHwStates",			"red",				"400" );
	api().spoofData( "CHwStates",			"green",			"300" );

	api().sleepMsec( 1000 * (1) );

	/* Force short Mid-Run */
	/* Notify 'PROC' that it's collected plenty of Volume and Platelet 
	 *	(Product) */
	api().spoofData( "ProcVolumes_Vin",				"data",	"3900.00" ); 
	api().spoofData( "ProcVolumes_PlateletVolume",	"data",	"280.00" ); 

	
}
void Spoof_IT10221 :: init()
{
    _name = "IT10221";
    _comment = "report AC in plasma bag is 1ml before the end of run summary generated";
    _timeout = 120;      //2 minutes
}

void Spoof_IT10221 :: help()
{
    LOG_INFO( "- Start spoof before donor disconnect");
}

void Spoof_IT10221 :: run()
{
    api().spoofData( "ProcVolumes_ACinPlasmaBag", "data", 0, 1 );
    api().spoofActivate();
   
}

void Spoof_PLO_PCA :: init()
{

    _name = "PLO_PCA";
    _comment = "spoofs Plasma line occlusion flag during PCA substate";
    _timeout = 1000;
}

void Spoof_PLO_PCA :: help()
{
	LOG_INFO( "start spoof at 350ml of 400ml plasma collection");
    LOG_INFO( "Tuned for donor = Male 6' 180lb 350 precount 40 crit");
	LOG_INFO( "Use product platelet=3.2e11 in 200ml + 400ml plasma");
	LOG_INFO( "Ratio=13.7 Infusion=6 Draw=fast Inlet Mgt=6 Return Mgt=6");
    LOG_INFO( "Use var1 to specify collect volume in PCA to drop R/G ratio (e.g. var1=2.0 means start at 2ml Vc)");
    LOG_INFO( "Use var2 to specify R/G drop (e.g. 100 means drop red 100 points from baseline");
	LOG_INFO( "default is var1=6.0 var2=141.0");

}

void Spoof_PLO_PCA :: run()
{
    LOG_INFO( "In PLO_PCA spoof");
	run_generic("6.0","141.0");
}

void Spoof_PLO_PCA :: run_generic(const char* Var1, const char* Var2)
{

	float var1f = (float)atof(Var1);
	float var2f = (float)atof(Var2);

	//
	//
	//    Run Qc at specific rate so we can get timing correct
	//
    api().spoofData( "Predict_SubstateInfo", "QcD_5","28.0" );
    api().spoofData( "Predict_SubstateInfo", "QcR_5","28.0" );

	//
	//   Force start of PCA substate
	//
    api().spoofData( "Predict_SubstateInfo", "Vtarget_4", "350" ); 
    api().spoofData( "Predict_SubstateInfo", "VtargetType_4", "VPBAG" ); 
    api().spoofData( "Predict_SubstateInfo", "OrVtargetType_4", "NOCOLLECT" ); 

	//
	//
	//    Set baseline red reflection
	//
	const float baselineRed = 800.0f;
	api().spoofData( "CHwStates", "red", 0, baselineRed );

    api().spoofActivate();

	//
	//
	//  Spoof baseline for desired time
	//
	LOG_INFO(" setting PLO red reflection baseline");
	int baselinetime = int((var1f/28.0f)*60.0f);
	for( int i = 1; i <= baselinetime; i++)
	{
	    api().sleepMsec( 1000 );
	}

	//
	//
	//   Now drop red reflection
	//
	LOG_INFO("dropping red reflection");
	for( int i = 1; i <= 5; i++)
	{
	    api().spoofData( "CHwStates", "red", 0, baselineRed-var2f);
	    api().sleepMsec( 1000 );
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to force an Extended 
 *  Platelet Concentration Adjustment (EPCA) phase, by adding sub state values 
 *  to PROC that may not be predicted. */
void Spoof_PLO_EPCA_1::init( ){ 
	_name = "PLO_EPCA_1";
	_comment = "Plasma Line Occlusion test for EPCA";
	_timeout = 300 ;			// 5 Minute timeout
}
void Spoof_PLO_EPCA_1::help( ){ 
	LOG_INFO( "Used to create an Extended Plasma Concentration Adjustment Phase (EPCA) ");
	LOG_INFO( "  AND set the RED\\GREEN values");
	LOG_INFO( "  R/G ratio drops at 1 ml (-var1 default) after entering EPCA.");
	LOG_INFO( "  R/G ratio returns 0.5ml later (-var2 default)");
	LOG_INFO( "Start spoof during midrun");
}
void Spoof_PLO_EPCA_1::run( ){ 
	run_generic( "1.0", "0.5" );	
}
void Spoof_PLO_EPCA_1::run_generic( const char *Var1, const char *Var2){ 

	float var1f = (float)atof(Var1);
	float var2f = (float)atof(Var2);

	LOG_INFO( "Setting up EPCA spoof at %s ml", Var1);

	//
	// Force finish of midrun substate
	// 
	api().spoofData( "Predict_SubstateInfo", "Vtarget_6",		"1" ); 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_6",	"VCOLBAG" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_6", "NOCOLLECT" );

	//
	//
	//  Get rid of any PIR substates 
	// 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_7",	"NOCOLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_7", "NOCOLLECT" );
	api().spoofData( "Predict_SubstateInfo", "VtargetType_8",	"NOCOLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_8", "NOCOLLECT" );
	
	// EPCA substate
	api().spoofData( "Predict_SubstateInfo", "Vtarget_9",		"500" ); 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_9",	"VCOLBAG" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "NOCOLLECT" );
	api().spoofData( "Predict_SubstateInfo", "PlateletValve_9", "HAL_VALVE_COLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "Qac_9",			"8.99" ); 
	/* Collect and Platelet */
	const float QcRate = 5.0f;
	api().spoofData( "Predict_SubstateInfo", "QcD_9",			"5.0");
	api().spoofData( "Predict_SubstateInfo", "QcR_9",			"5.0");
	/* Inlet */
	api().spoofData( "Predict_SubstateInfo", "QinD_9",			"119.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QinR_9",			"84.99" ); 
	/* Plasma */
	api().spoofData( "Predict_SubstateInfo", "QpD_9",			"20.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QpR_9",			"20.99" ); 
	/* Return */
	api().spoofData( "Predict_SubstateInfo", "Qrp_9",			"333.99" ); 

	// Set RBC detector status
	api().spoofData( "CHwStates",			"red",				"650" );
	api().spoofData( "CHwStates",			"green",			"625" );

	api().spoofActivate();

	//
	//
	//  Spoof baseline for desired time
	//
	LOG_INFO(" Waiting for trigger...");
	int baselinetime = int((var1f/QcRate)*60.0f*10.0f); // 100ms timeslices
	for( int i = 1; i <= baselinetime; i++)
	{
	    api().sleepMsec( 100 );
	}

	//
	//
	//   Drop red/green
	//
	LOG_INFO(" Dropping RBC detector values.");
	api().spoofData( "CHwStates",			"red",				"400" );
	api().spoofData( "CHwStates",			"green",			"300" );

	//
	//
	//  Keep reflection low for configured time
	//
	int lowtime = int((var2f/QcRate)*60.0f*10.0f); // 100ms timeslices
	for( int i = 1; i <= lowtime; i++)
	{
	    api().sleepMsec( 100 );
	}

	//
	//
	//   Return RBC detector values to initial baseline
	//
	LOG_INFO(" Restoring RBC detector values.");
	api().spoofData( "CHwStates",			"red",				"650" );
	api().spoofData( "CHwStates",			"green",			"625" );
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Written by Michael S. Chaffin for use with Trima 5.1.9, in use with testing
 *  Plasma Line Occlusion (PLO). This spoof is designed to force an Extended 
 *  Platelet Concentration Adjustment (EPCA) phase, by adding sub state values 
 *  to PROC that may not be predicted. */
void Spoof_PLO_EPCA_26::init( ){ 
	_name = "PLO_EPCA_26";
	_comment = "Plasma Line Occlusion test for EPCA";
	_timeout = 500 ;
}
void Spoof_PLO_EPCA_26::help( ){ 
	LOG_INFO( "Used to create an Extended Plasma Concentration Adjustment Phase (EPCA) ");
	LOG_INFO( "  AND set the RED\\GREEN values");
	LOG_INFO( "  R/G ratio drops at 26 ml (default) after entering EPCA.");
	LOG_INFO( "Start spoof during midrun");
}
void Spoof_PLO_EPCA_26::run( ){ 
	run_generic( "26.0", NULL );	
}
void Spoof_PLO_EPCA_26::run_generic( const char *Var1, const char *Var2){ 

	float var1f = (float)atof(Var1);

	LOG_INFO( "Setting up EPCA spoof at %s ml", Var1);

	//
	// Force finish of midrun substate
	// 
	api().spoofData( "Predict_SubstateInfo", "Vtarget_6",		"1" ); 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_6",	"VCOLBAG" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_6", "NOCOLLECT" );

	//
	//
	//  Get rid of any PIR substates 
	// 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_7",	"NOCOLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_7", "NOCOLLECT" );
	api().spoofData( "Predict_SubstateInfo", "VtargetType_8",	"NOCOLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_8", "NOCOLLECT" );
	
	// EPCA substate
	api().spoofData( "Predict_SubstateInfo", "Vtarget_9",		"500" ); 
	api().spoofData( "Predict_SubstateInfo", "VtargetType_9",	"VCOLBAG" ); 
	api().spoofData( "Predict_SubstateInfo", "OrVtargetType_9", "NOCOLLECT" );
	api().spoofData( "Predict_SubstateInfo", "PlateletValve_9", "HAL_VALVE_COLLECT" ); 
	api().spoofData( "Predict_SubstateInfo", "Qac_9",			"8.99" ); 
	/* Collect and Platelet */
	const float QcRate = 5.0f;
	api().spoofData( "Predict_SubstateInfo", "QcD_9",			"5.0");
	api().spoofData( "Predict_SubstateInfo", "QcR_9",			"5.0");
	/* Inlet */
	api().spoofData( "Predict_SubstateInfo", "QinD_9",			"119.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QinR_9",			"84.99" ); 
	/* Plasma */
	api().spoofData( "Predict_SubstateInfo", "QpD_9",			"20.99" ); 
	api().spoofData( "Predict_SubstateInfo", "QpR_9",			"20.99" ); 
	/* Return */
	api().spoofData( "Predict_SubstateInfo", "Qrp_9",			"333.99" ); 

	// Set RBC detector status
	api().spoofData( "CHwStates",			"red",				"650" );
	api().spoofData( "CHwStates",			"green",			"625" );

	api().spoofActivate();

	//
	//
	//  Spoof baseline for desired time
	//
	LOG_INFO(" Waiting for trigger...");
	int baselinetime = int((var1f/QcRate)*60.0f);
	for( int i = 1; i <= baselinetime; i++)
	{
	    api().sleepMsec( 1000 );
	}

	//
	//
	//   Drop red/green
	//
	LOG_INFO(" Updating RBC detector values.");
	api().spoofData( "CHwStates",			"red",				"400" );
	api().spoofData( "CHwStates",			"green",			"300" );
}

void Spoof_CassetteID :: init()
{
    _name = "CassetteID";
    _comment = "Spoof cassette ID to UNKNOWN via DisposableIDMsg";
    _timeout = 5;
}
void Spoof_CassetteID :: help()
{
    LOG_INFO( "- run after a cassette has been loaded" );
}
void Spoof_CassetteID :: run()
{
    api().messageSend( "DisposableIDMsg", 1 );
    api().sleepMsec( 1000 );
    api().messageSend( "DisposableIDMsg", 1 );
}



void Spoof_ProcReturnRPM :: init()
{
	_name = "ProcReturnRPM";
	_comment = "Spoofs the commanded speed in control for the return pump.";
	_comment = "Uses a enable/disable loop (3.5s enable, 1.5s disable) so as to avoid an expected vs actual alarm.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_ProcReturnRPM :: help()
{
	LOG_INFO("If no parameters are provided, will run the return pump at the default");
	LOG_INFO("scalar factor of 1.5 (new pump speed will be 1.5 times the originally commanded speed).");
	LOG_INFO("Provide a custom scalar factor with -var1 #.## (new_speed = (var1)X(original_speed))");
	LOG_INFO("Use -timeout option to override the default 30 second timeout (enable/disable loop will run with duty cycle 3.5s/1.5s until -timeout time has passed).");
}
void Spoof_ProcReturnRPM :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
	
	MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}
void Spoof_ProcReturnRPM :: run_generic(const char* Var1, const char* Var2)
{
    //pump = Var1;
    //LOG_INFO( "running inlet pump at %f x commanded speed", Var1 );
    api().spoofData( "CHwOrders", Var1, "160" );
    api().spoofActivate();

    MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}



void Spoof_S_ATD_1_0001 :: init()
{
	_name = "S-ATD-1.001";
	_comment = "Spoofs the commanded speed in control for the return pump.";
	_comment = "Uses a enable/disable loop (3.5s enable, 1.5s disable) so as to avoid an expected vs actual alarm.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_S_ATD_1_0001 :: help()
{
	LOG_INFO("Spoofs control return pump RPM to 160");
}
void Spoof_S_ATD_1_0001 :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
	
	MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}


void Spoof_S_ATD_1_0001h :: init()
{
	_name = "S-ATD-1.001h";
	_comment = "Spoofs the commanded speed in control for the return pump.";
	_comment = "Uses a enable/disable loop (3.5s enable, 1.5s disable) so as to avoid an expected vs actual alarm.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_S_ATD_1_0001h :: help()
{
	LOG_INFO("Spoofs safety and control return pump RPM to 160, inlet and AC to zero, control reservoir to middle");
}
void Spoof_S_ATD_1_0001h :: run()
{
    api().spoofData( "SHwStates", "returnRPM", "160" );
	api().spoofData( "CHwStates", "returnRPM", "160" );
	api().spoofData( "CHwOrders", "returnRPM", "160" );
	api().spoofData( "CHwOrders", "inletRPM", "0" );
	api().spoofData( "CHwOrders", "acRPM", "0" );
	api().spoofData( "CHwStates", "reservoir", "CHW_RESERVOIR_MIDDLE" );
    api().spoofActivate();
	
	MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}


void Spoof_C_DAE :: init()
{
	_name = "C-DAE";
	_comment = "Spoofs the commanded speed in control for the return pump.";
	_comment = "Uses a enable/disable loop (3.5s enable, 1.5s disable) so as to avoid an expected vs actual alarm.";
	_comment = "Default timeout is 30 seconds.";
	_timeout = 30;
}
void Spoof_C_DAE :: help()
{
	LOG_INFO("Spoofs the commanded speed in control for the return pump.");
}
void Spoof_C_DAE :: run()
{
    api().spoofData( "CHwOrders", "returnRPM", "160" );
    api().spoofActivate();
	
	MySpoofs::enableDisableLoop( 3500, 1500 ); // run spoof for 3.5 seconds at a time avoid expected vs actual alarm
}


void Spoof_C_RB_1_007a :: init()
{
	_name = "C-RB-1.007a";
	_comment = "Spoofs the plasma valve state to open.";
	_comment = "Default timeout is 50 seconds.";
	_timeout = 50;
}
void Spoof_C_RB_1_007a :: help()
{
	LOG_INFO("Run during rinseback, spoofs the plasma valve state to the open position");
}
void Spoof_C_RB_1_007a :: run()
{
    api().spoofData( "CHwStates", "plasmaValve", "HW_VALVE_OPEN" );
    api().spoofActivate();
}

void Spoof_EnableDisable :: init()
{
	_name = "EnableDisable";
	_comment =  "Enables and disables the spoofer on a loop.";
	_timeout = 30;
	
}
void Spoof_EnableDisable :: help()
{
    LOG_INFO( "With no parameters, will run with a 3.5s/1.5s enable/disable duty cycle until timeout." );
	LOG_INFO( "Use -var1 and -var2 to set the enable and disable times respectively.");
}
void Spoof_EnableDisable :: run()
{
	api().spoofActivate();
	MySpoofs::enableDisableLoop(3500,1500);
}
void Spoof_EnableDisable :: run_generic(const char* Var1, const char* Var2)
{
	api().spoofActivate();
	MySpoofs::enableDisableLoop(atoi(Var1),atoi(Var2));
}

void PFR_Index :: init()
{                                                                     
    _name = "Index";                                                  
    _comment = "Calculates offset for pfr data struct member";  
	_comment = "balbusb: As of 11/15/12 control side is not yet supported.";                     
    _timeout = 1;                                                    
                                                                      
}                                                                     
void PFR_Index :: help()
{
    LOG_INFO( "" );
}
void PFR_Index :: run()
{                                                                     
	LOG_INFO("");
}                                                                     
void PFR_Index:: run_generic(const char* Side, const char* Variable)
{   
	SPFRecovery spfr;

	unsigned char* p1 = 0;

	if (strcmp(Side, "safety")==0)
	{
		p1 = (unsigned char*)&spfr;
	}
	else if (strcmp(Side, "control")==0)
	{
		printf("Sorry, control byte finding is not yet supported.");

/*
        int nBytes = trimaSysControlPFSaveArea.nSecs * 512;                                             
        char* pData = (char *)malloc( nBytes );                                                         
        trimaSysControlPFSaveArea.pBuf = pData;                                                         
        trimaSysControlPFSaveArea.direction = 0;                                                        
                                                                                                        
        if( ataRawio( 0, 0, &trimaSysControlPFSaveArea ) == ERROR )                                     
        {                                                                                               
            DataLog(log_level_datfile_error)<<"ataRawio failed to read control power fail data"<<endmsg;
            free( pData );                                                                              
            return 1;                                                                                   
        }                                                                                               
                                                                                                        
        p1 = (unsigned char*)&trimaSysControlPFSaveArea;                                                
                                                                                                        
*/
	}
	else
	{
		printf("-var1 must be either 'control' or 'safety'");
		
	}
	unsigned char* p2 = 0;
	int index = -1;

	api().spoofEnable();

	if (strcmp(Variable, "DonorVitals")==0)
	{
		p2 = (unsigned char*)&spfr.PFR_DonorVitals;
	}
	
	index = p2 - p1;

	DataLog(log_level_spoofer_info) << "Index: "<<index<<endmsg;
	printf("byte = %d\n", index);
}

void STMBucket :: init()
{
    _name = "buck";
    _comment = "TESTING Safety ac to donor protection - inlet failure in 1st draw";
    _timeout = 13;
}
void STMBucket :: help()
{
    LOG_INFO( "- Default timeout is %d seconds", _timeout );
    LOG_INFO( "- Orders AC to pump to zero RPM for %d seconds", _timeout );
}
void STMBucket :: run()
{
    //api().spoofData( "CHwOrders", "acRPM", "0" );

    //api().spoofData( "ProcVolumes_VacTotal", "data", 0 );
    //api().spoofActivate();
    //MySpoofs::pumpMonitorDisable();

    //api().spoofData( "ProcData_ACCmd", "data", 84 );
    //api().spoofActivate();

//  api().spoofData( "CHwOrders", "returnRPM", "50" );
//  api().spoofData( "CHwOrders", "acRPM", "50" );
//  api().spoofData( "CHwOrders", "inletRPM", "50" );
//  api().spoofData( "CHwOrders", "plasmaRPM", "50" );
//  api().spoofData( "CHwOrders", "plateletRPM", "50" );
//  api().spoofActivate();
//
//  MySpoofs::enableDisableLoop( 3500, 1500 ); // avoids expected vs actual shutdown

	api().spoofData( "CHwStates", "acRevs", "0.00075" );
	//api().spoofData( "CHwStates", "inletRevs", "1.25", "0.0" );
}

void Spoof_CPumpCmdMultLoop :: init()                                                                                                                   
{                                                                                                                                                       
    _name = "CPumpCmdMultLoop";                                                                                                                         
    _comment = "Changes the commanded speed in control orders for the pump specified by var1 (returnPump, plasmaPump, plateletPump, inletPump, acPump)";
    _comment = "to var2 times the original speed (new_speed = var2 X original_speed)";                                                                  
    _comment = "NOTE: This spoof uses an enable disable loop to avoid an alarm being raised.";                                                          
    _timeout = 30; // 30 seconds                                                                                                                        
}                                                                                                                                                       
                                                                                                                                                        
void Spoof_CPumpCmdMultLoop :: help()                                                                                                                   
{                                                                                                                                                       
    LOG_INFO( "Include -var1 <pumpName> -var2 <multiplier> to spoof control commanded pump speed" );                                                           
    LOG_INFO( "Pumps will run at spoofed speed with a 3.5/1.5 duty cycle until timeout is reached.");                                                   
    LOG_INFO( "Default timeout is 30 seconds" );                                                                                                        
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );                                                 
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofed speed" );                                                          
    LOG_INFO( "for a short time after the inititating event" );                                                                                         
}                                                                                                                                                       
                                                                                                                                                        
void Spoof_CPumpCmdMultLoop :: run()                                                                                                                    
{                                                                                                                                                       
    LOG_INFO( "Spoofing plasma pump speed to 1.5X commanded speed now" );                                                                               
    api().spoofData( "CHwOrders", "plasmaRPM", 1.5, 0.0 );                                                                                              
    api().spoofActivate();                                                                                                                              
    MySpoofs::enableDisableLoop(3500,1500);                                                                                                             
}                                                                                                                                                       
                                                                                                                                                        
void Spoof_CPumpCmdMultLoop :: run_generic(const char* Var1, const char* Var2)                                                                          
{                                                                                                                                                       
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||                                                                        
        (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||                                                                        
        (strcmp(Var1, "acPump") == 0) )                                                                                                                 
    {                                                                                                                                                   
        if ((atoi(Var2) >=0) || (atof(Var2) >= 0) )                                                                                                     
        {                                                                                                                                               
            LOG_INFO( "Spoofing pump %s status %s RPM.", Var1, Var2 );                                                                                  
        }                                                                                                                                               
        else                                                                                                                                            
        {                                                                                                                                               
            LOG_ERROR( "var2 must be an number" );                                                                                                      
            return;                                                                                                                                     
        }                                                                                                                                               
                                                                                                                                                        
    }                                                                                                                                                   
    else                                                                                                                                                
    {                                                                                                                                                   
        LOG_ERROR( "var1 must be a pump name");                                                                                                         
        return;                                                                                                                                         
    }                                                                                                                                                   
                                                                                                                                                        
    if (strcmp(Var1,"acPump") == 0)                                                                                                                     
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "acRPM", atof(Var2), 0.0 );                                                                                       
    }                                                                                                                                                   
    else if (strcmp(Var1,"inletPump") == 0)                                                                                                             
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "inletRPM", atof(Var2), 0.0 );                                                                                    
    }                                                                                                                                                   
    else if (strcmp(Var1,"plasmaPump") == 0)                                                                                                            
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "plasmaRPM", atof(Var2), 0.0 );                                                                                   
    }                                                                                                                                                   
    else if (strcmp(Var1,"plateletPump") == 0)                                                                                                          
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "plateletRPM", atof(Var2), 0.0 );                                                                                 
    }                                                                                                                                                   
    else if (strcmp(Var1,"returnPump") == 0)                                                                                                            
    {                                                                                                                                                   
       api().spoofData( "CHwOrders", "returnRPM", atof(Var2), 0.0 );                                                                                    
    }                                                                                                                                                   
                                                                                                                                                        
    api().spoofActivate();                                                                                                                              
    MySpoofs::enableDisableLoop(3500,1500);                                                                                                             
}                                                                                                                                                       

void Spoof_COrdersPumpCmdLoop :: init()                                                                                                                   
{                                                                                                                                                       
    _name = "COrdersPumpCmdLoop";                                                                                                                         
    _comment = "Changes the commanded speed in control orders for the pump specified by var1 (returnPump, plasmaPump, plateletPump, inletPump, acPump) to var2 RPM";
    _timeout = 30; // 30 seconds                                                                                                                        
}                                                                                                                                                       
                                                                                                                                                        
void Spoof_COrdersPumpCmdLoop :: help()                                                                                                                   
{                                                                                                                                                       
    LOG_INFO( "Include -var1 <pumpName> -var2 <RPM> to spoof control commanded pump speed" );                                                           
    LOG_INFO( "Pumps will run at spoofed speed with a 3.5/1.5 duty cycle until timeout is reached.");                                                   
    LOG_INFO( "Default timeout is 30 seconds" );                                                                                                        
    LOG_INFO( "To test pumps at substate transitions recommend using -beginSubstate or beginMessage" );                                                 
    LOG_INFO( "along with -timeFromActive and -delta options to initiate the spoofed speed" );                                                          
    LOG_INFO( "for a short time after the inititating event" );                                                                                         
}                                                                                                                                                       
                                                                                                                                                        
void Spoof_COrdersPumpCmdLoop :: run()                                                                                                                    
{                                                                                                                                                       
    LOG_INFO( "Spoofing inlet pump speed to 100RPM" );                                                                               
    api().spoofData( "CHwOrders", "inletRPM", "100" );
    api().spoofActivate();                                                                                                                              
    MySpoofs::enableDisableLoop(3500,1500);                                                                                                             
}                                                                                                                                                       
                                                                                                                                                        
void Spoof_COrdersPumpCmdLoop :: run_generic(const char* Var1, const char* Var2)                                                                          
{                                                                                                                                                       
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||                                                                        
        (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||                                                                        
        (strcmp(Var1, "acPump") == 0) )                                                                                                                 
    {                                                                                                                                                   
        if ((atoi(Var2) >=0) || (atof(Var2) >= 0) )                                                                                                     
        {                                                                                                                                               
            LOG_INFO( "Spoofing pump %s to %s RPM.", Var1, Var2 );                                                                                  
        }                                                                                                                                               
        else                                                                                                                                            
        {                                                                                                                                               
            LOG_ERROR( "var2 must be an number" );                                                                                                      
            return;                                                                                                                                     
        }                                                                                                                                               
                                                                                                                                                        
    }                                                                                                                                                   
    else                                                                                                                                                
    {                                                                                                                                                   
        LOG_ERROR( "var1 must be a pump name");                                                                                                         
        return;                                                                                                                                         
    }                                                                                                                                                   
                                                                                                                                                        
    if (strcmp(Var1,"acPump") == 0)                                                                                                                     
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "acRPM", Var2 );
    }                                                                                                                                                   
    else if (strcmp(Var1,"inletPump") == 0)                                                                                                             
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "inletRPM", Var2 );
    }                                                                                                                                                   
    else if (strcmp(Var1,"plasmaPump") == 0)                                                                                                            
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "plasmaRPM", Var2 );
    }                                                                                                                                                   
    else if (strcmp(Var1,"plateletPump") == 0)                                                                                                          
    {                                                                                                                                                   
        api().spoofData( "CHwOrders", "plateletRPM", Var2 );
    }                                                                                                                                                   
    else if (strcmp(Var1,"returnPump") == 0)                                                                                                            
    {                                                                                                                                                   
       api().spoofData( "CHwOrders", "returnRPM", Var2 );
    }                                                                                                                                                   
                                                                                                                                                        
    api().spoofActivate();                                                                                                                              
    MySpoofs::enableDisableLoop(3500,1500);                                                                                                             
}                                                                                                                                                       

void Spoof_CPumpRevs :: init()
{
    _name = "CPumpRevs";
    _comment = "Spoofs the delta revs control sees for the pump specified by variable var1 to the value specified by var2";
    _timeout = 10; // 10 seconds
}

void Spoof_CPumpRevs :: help()
{
    LOG_INFO( "Include -var1 <pumpName> -var2 <revs> to spoof control status pump revs" );
    LOG_INFO( " where pumpName is acPump, inletPump, returnPump, plasmaPump, or plateletPump" );
    LOG_INFO( " and var2 is the number of delta revs to spoof" );
    LOG_INFO( "Default time is 10 seconds" );
}

void Spoof_CPumpRevs :: run()
{
    LOG_INFO( "Spoofing AC pump to 2" );
    api().spoofData( "CHwStates", "acRevs", "2" );
    api().spoofActivate();
}

void Spoof_CPumpRevs :: run_generic(const char* Var1, const char* Var2)
{
    if ((strcmp(Var1,"returnPump") == 0) || (strcmp(Var1, "plasmaPump") == 0) ||
     (strcmp(Var1,"plateletPump")== 0) || (strcmp(Var1, "inletPump") == 0) ||
     (strcmp(Var1, "acPump") == 0) ) 
    {
        if (atoi(Var2) >=0  )
        {
            LOG_INFO( "Spoofing pump %s status %s revs.", Var1, Var2 );
        }
        else
        {
            LOG_ERROR( "var2 must be a number" );
            return;
        }
       
    }
    else
    {
        LOG_ERROR( "var1 must be a pump name");
        return;
    }

    if (strcmp(Var1,"acPump") == 0)
    {
        api().spoofData( "CHwStates", "acRevs", Var2 ); 
    }
    else if (strcmp(Var1,"inletPump") == 0)
    {
        api().spoofData( "CHwStates", "inletRevs", Var2 );
    }
    else if (strcmp(Var1,"plasmaPump") == 0)
    {
        api().spoofData( "CHwStates", "plasmaRevs", Var2 );
    }
    else if (strcmp(Var1,"plateletPump") == 0)
    {
        api().spoofData( "CHwStates", "plateletRevs", Var2 );
    }
    else if (strcmp(Var1,"returnPump") == 0)
    {
        api().spoofData( "CHwStates", "returnRevs", Var2 );
    }

    api().spoofActivate();
}

void Spoof_AvgACRatioTooHigh :: init()
{
    _name = "AvgACRatioTooHigh";
    _comment = "Spoofs AC pump delta revs to 80% of the actual value. Uses an enable disable loop to avoid draw cycle too big alarms";
    _timeout = 1800;
}
void Spoof_AvgACRatioTooHigh :: help()
{
    LOG_INFO( "- Default timeout is %d seconds", _timeout );
    LOG_INFO( "- Begin spoof during the second draw cycle as first cycle uses a slightly different avg ac ratio calc" );
	LOG_INFO( "  that is not accounted for in this spoof." );
	LOG_INFO( "- This spoof works with management and ratio settings as high as possible." );
	}
void Spoof_AvgACRatioTooHigh :: run()
{
	api().spoofData( "CHwStates", "acRevs", 0.80 , 0.0 );
	
	api().spoofActivate();

	MySpoofs::enableDisableLoop( 11500, 11500 );
}

void Spoof_AvgACRatioTooLow :: init()
{
    _name = "AvgACRatioTooLow";
    _comment = "Spoofs inlet and AC pump delta revs to a low AC ratio. Uses an enable disable loop to avoid instantaneous AC ratio too low alarm";
    _timeout = 90;
}
void Spoof_AvgACRatioTooLow :: help()
{
    LOG_INFO( "- Default timeout is %d seconds", _timeout );
	LOG_INFO( "- Configure management and ratio settings as low as possible." );
	LOG_INFO( "- Use during a plasma only procedure (might work with procedures that include platelet collection)." );
	LOG_INFO( "- Begin spoof during the third draw cycle." );
}
void Spoof_AvgACRatioTooLow :: run()
{
	api().spoofData( "CHwStates", "acRevs", 1.625, 0.0 );
	
	api().spoofActivate();

	MySpoofs::enableDisableLoop( 2000, 2000 );
}

void Spoof_InstACRatioTooHigh :: init()
{
    _name = "InstACRatioTooHigh";
    _comment = "Spoofs inlet and AC pump delta revs to a high AC ratio.";
    _timeout = 20;
}
void Spoof_InstACRatioTooHigh :: help()
{
    LOG_INFO( "- Default timeout is %d seconds", _timeout );
    LOG_INFO( "- This spoof will spoof the AC ratio to a value greater than 15. This is done by spoofing" );
	LOG_INFO( "  the delta revs for both the AC and Inlet pumps." );
}
void Spoof_InstACRatioTooHigh :: run()
{
	api().spoofData( "CHwStates", "acRevs", "0.026" );
	api().spoofData( "CHwStates", "inletRevs", "0.40" );
	
	api().spoofActivate();
}

void Spoof_InstACRatioTooLow :: init()
{
    _name = "InstACRatioTooLow";
    _comment = "Spoofs inlet and AC pump delta revs to a low AC ratio.";
    _timeout = 10;
}
void Spoof_InstACRatioTooLow :: help()
{
    LOG_INFO( "- Default timeout is %d seconds", _timeout );
    LOG_INFO( "- This spoof will spoof the AC ratio to a value less than 5. This is done by spoofing" );
	LOG_INFO( "  the delta revs for both the AC and Inlet pumps." );
}
void Spoof_InstACRatioTooLow :: run()
{
	api().spoofData( "CHwStates", "acRevs", "0.095" );
	api().spoofData( "CHwStates", "inletRevs", "0.40" );

	api().spoofActivate();
}

void Spoof_RG_reflect :: init()
{
    _name = "Spoof_RG_reflect";
    _comment = "Spoofs the RED and GREEN halstatus reflectance values";
    _timeout = 30; // 30 seconds
}

void Spoof_RG_reflect :: help()
{
    LOG_INFO( "Use -var1 xxxx to spoof RED reflectance to xxxx, Use -var2 xxxx to spoof GREEN reflectance to xxxx.");
    LOG_INFO( "Use  -timeout tt if 30 second default timeout not desirable" );
}

void Spoof_RG_reflect :: run()
{
    LOG_INFO("Spoofing RED and GREEN reflectance values as 10 and 20, respectively.");
    api().spoofData("HalStatus_redValue", "data", 0, 10);
	api().spoofData("HalStatus_greenValue", "data", 0, 20);
    api().spoofActivate();
}

void Spoof_RG_reflect :: run_generic(const char* Var1, const char* Var2)
{
   LOG_INFO( "Spoofing RED and GREEN reflectance values as %s and %s, respectively.", Var1, Var2);
   api().spoofData("HalStatus_redValue", "data", 0, atoi(Var1));
   api().spoofData("HalStatus_greenValue", "data", 0, atoi(Var2));
   api().spoofActivate();
}

void Spoof_IT12551_1 :: init()
{
    _name = "Spoof_IT12551_1";
    _comment = "Spoofs the APS Pressure in mmHg to test IT12551";
    _timeout = 20;
}


void Spoof_IT12551_1 :: help()
{
    LOG_INFO( "Run at the very beginning of a draw cycle");
}

void Spoof_IT12551_1 :: run()
{
    LOG_INFO("Spoofing APS reading...");
    api().spoofData("HalStatus_aps", "data", 0, -300);
	api().spoofActivate();
	api().sleepMsec( 1000 );
	api().spoofData("HalStatus_aps", "data", 0, -10);
}

void Spoof_IT12551_2 :: init()
{
    _name = "Spoof_IT12551_2";
    _comment = "Spoofs the APS Pressure in mmHg to test IT12551";
    _timeout = 20;
}


void Spoof_IT12551_2 :: help()
{
    LOG_INFO( "Run at the very beginning of a draw cycle");
}

void Spoof_IT12551_2 :: run()
{
    LOG_INFO("Spoofing APS reading...");
    api().spoofData("HalStatus_aps", "data", 0, -300);
	api().spoofActivate();
	api().sleepMsec( 6000 );
	api().spoofData("HalStatus_aps", "data", 0, -10);
}

void SubstateAlert :: init()
{
    _name = "SubstateAlert";
    _comment = "Notifies tester via stdout that a substate has been entered";
    _timeout = 120;
}

void SubstateAlert :: help()
{
    LOG_INFO( "- Include -var1 xxxx in command line " );
    LOG_INFO( "- where xxxx is text you want printed" );
    LOG_INFO( "- with no parameters prints \"Substate entered\"" );
    LOG_INFO( "- default timeout is 120 seconds" );
}

void SubstateAlert :: run()
{
    LOG_INFO( "Substate entered" );
    printf("***** Requested substate entered *****\n");
}

void SubstateAlert :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO("Requested substate: %s entered.", Var1);
    printf("***** Substate: %s entered. *****\n", Var1);
}

