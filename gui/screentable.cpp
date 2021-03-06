#include <vxworks.h>

#include "scrntran.hpp"

// Definition of the screen/state transition table
// Current State --      Current Screen --        Next State --         Next Screen --        Reset all screens

SCREEN_STATE_TABLE screen_state_transition_table[] =
{

   {SELF_TEST, GUI_SCREEN_POWERUP, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, TRUE},                          //  done
   {STARTUP_TESTS, GUI_SCREEN_SYSDONOR, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},                 //  load sys btn
   {STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},                 //  goback
   {STARTUP_TESTS, GUI_SCREEN_SYSDISP, STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, FALSE},                     //  cont
   {STARTUP_TESTS, GUI_SCREEN_SYSDISP, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},                     //  cont

   {STARTUP_TESTS, GUI_SCREEN_PTF_MOUNT, STARTUP_TESTS, GUI_SCREEN_SYSDISP, FALSE},                   //  cont
   {STARTUP_TESTS, GUI_SCREEN_PTF_MOUNT, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},               //  goback
   {STARTUP_TESTS, GUI_SCREEN_SYSDISP, STARTUP_TESTS, GUI_SCREEN_PTF_MOUNT, FALSE},                   //  cont
   {STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, STARTUP_TESTS, GUI_SCREEN_PTF_MOUNT, FALSE},               //  cont

   {STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},                     //  canc
   {STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},                  //  canc
   {STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, STARTUP_TESTS, GUI_SCREEN_SYSDISP, FALSE},                   //  cont
   {STARTUP_TESTS, GUI_SCREEN_SYSDISP, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},                   //  goback

   // New clamp screens for additive solutions
   {STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, STARTUP_TESTS, GUI_SCREEN_PAS_INIT_SPLIT, FALSE},               //  canc
   {STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, STARTUP_TESTS, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, FALSE},      //  canc
   {STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, STARTUP_TESTS, GUI_SCREEN_RBC_SPLIT_INIT, FALSE},               //  canc
   {STARTUP_TESTS, GUI_SCREEN_PAS_INIT_SPLIT, STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, FALSE},               //  canc
   {STARTUP_TESTS, GUI_SCREEN_PAS_INIT_SPLIT, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},            //  HW failure while loading or goback (before tests)
   {STARTUP_TESTS, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, FALSE},      //  canc
   {STARTUP_TESTS, GUI_SCREEN_RBC_SPLIT_INIT, STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, FALSE},               //  canc
   {STARTUP_TESTS, GUI_SCREEN_RBC_SPLIT_INIT, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},            //  HW failure while loading or goback (before tests)
   {STARTUP_TESTS, GUI_SCREEN_SYSCLAMP, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},                    //  cont
   {STARTUP_TESTS, GUI_SCREEN_PAS_INIT_SPLIT, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},              //  cont
   {STARTUP_TESTS, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},     //  cont
   {STARTUP_TESTS, GUI_SCREEN_RBC_SPLIT_INIT, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},              //  cont
   {STARTUP_TESTS, GUI_SCREEN_SYSDONOR, BLOOD_RUN, GUI_SCREEN_RUNPROC, FALSE},                          //  pfr


   // Disposable tests:
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, STARTUP_TESTS, GUI_SCREEN_SYSDISP, FALSE},                              //  failure   // (chg'ed from Next Scrn = SYSDONOR 4/14/97 TRW)
   {DISPOSABLE_TEST, GUI_SCREEN_CLAMP_CROSSOVER, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},                    //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_CLAMP_CROSSOVER, STARTUP_TESTS, GUI_SCREEN_SYSDISP, FALSE},                      //  canc disp test from xover scrn
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, DISPOSABLE_TEST, GUI_SCREEN_CLAMP_CROSSOVER, FALSE},                    //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, DISPOSABLE_TEST, GUI_SCREEN_PAS_INIT_SPLIT, FALSE},                     //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, DISPOSABLE_TEST, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, FALSE},            //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_PAS_INIT_SPLIT, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},                     //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_PAS_INIT_SPLIT, DISPOSABLE_TEST, GUI_SCREEN_SYSCLAMP, FALSE},                    //  canc
                                                                                                                 //
                                                                                                                 //
   {STARTUP_TESTS, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},            //  canc
   //
   {DISPOSABLE_TEST, GUI_SCREEN_PAS_INIT_SPLIT, STARTUP_TESTS, GUI_SCREEN_SYSCASSETTE, FALSE},                   //  HW failure while loading cassette or unload/goback during/after tests
   {DISPOSABLE_TEST, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},            //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, DISPOSABLE_TEST,  GUI_SCREEN_RBC_SPLIT_INIT, FALSE},                    //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_RBC_SPLIT_INIT, DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, FALSE},                     //  cont
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, AC_CONNECTED, GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION, FALSE},              //  done
   {DISPOSABLE_TEST, GUI_SCREEN_SYSTEST, AC_CONNECTED, GUI_SCREEN_SYSACATT, FALSE},                              //  done
   //

   {AC_CONNECTED, GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION, AC_CONNECTED, GUI_SCREEN_SYSACATT, FALSE},                                  //  done
   {AC_CONNECTED, GUI_SCREEN_SYSACATT, AC_CONNECTED, GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION, FALSE},                                  //  canc

   {AC_CONNECTED, GUI_SCREEN_SYSACATT, DONOR_DISCONNECT, GUI_SCREEN_DISCONN, FALSE},                    //  canc
   {AC_CONNECTED, GUI_SCREEN_SYSACATT, AC_PRIME, GUI_SCREEN_SYSDONOR, FALSE},                           //  cont
                                                                                                        //
   {AC_PRIME, GUI_SCREEN_SYSDONOR, AC_PRIME, GUI_SCREEN_DONVITAL, FALSE},                               //  data btn
   {AC_PRIME, GUI_SCREEN_CONFIRM_DON_INFO, AC_PRIME, GUI_SCREEN_DONVITAL, FALSE},                       //  data btn
   {AC_PRIME, GUI_SCREEN_SYSDONOR, AC_PRIME, GUI_SCREEN_CONFIRM_DON_INFO, FALSE},                       //  data btn
   {AC_PRIME, GUI_SCREEN_DONVITAL, AC_PRIME, GUI_SCREEN_SYSDONOR, FALSE},                               //  gb
   {AC_PRIME, GUI_SCREEN_CONFIRM_DON_INFO, AC_PRIME, GUI_SCREEN_SYSDONOR, FALSE},                       //  gb
   {AC_PRIME, GUI_SCREEN_DONVITAL, AC_PRIME, GUI_SCREEN_DONINFO, FALSE},                                //  cont
   {AC_PRIME, GUI_SCREEN_DONINFO, AC_PRIME, GUI_SCREEN_DONVITAL, FALSE},                                //  gb
   {AC_PRIME, GUI_SCREEN_DONINFO, AC_PRIME, GUI_SCREEN_PREDICT, FALSE},                                 //  cont
   {AC_PRIME, GUI_SCREEN_CONFIRM_DON_INFO, AC_PRIME, GUI_SCREEN_PREDICT, FALSE},                        //  cont
   {AC_PRIME, GUI_SCREEN_PREDICT, AC_PRIME, GUI_SCREEN_DONINFO, FALSE},                                 //  gb
   {AC_PRIME, GUI_SCREEN_PREDICT, AC_PRIME, GUI_SCREEN_CONFIRM_DON_INFO, FALSE},                        //  gb
   {AC_PRIME, GUI_SCREEN_PREDICT, AC_PRIME, GUI_SCREEN_SYSDONOR, FALSE},                                //  cont
   {STARTUP_TESTS, GUI_SCREEN_SYSDONOR, STARTUP_TESTS, GUI_SCREEN_DONVITAL, FALSE},                     //  data btn
   {STARTUP_TESTS, GUI_SCREEN_SYSDONOR, STARTUP_TESTS, GUI_SCREEN_CONFIRM_DON_INFO, FALSE},             //  data btn
   {STARTUP_TESTS, GUI_SCREEN_DONVITAL, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},                     //  gb
   {STARTUP_TESTS, GUI_SCREEN_CONFIRM_DON_INFO, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},             //  gb
   {STARTUP_TESTS, GUI_SCREEN_DONVITAL, STARTUP_TESTS, GUI_SCREEN_DONINFO, FALSE},                      //  cont
   {STARTUP_TESTS, GUI_SCREEN_DONINFO, STARTUP_TESTS, GUI_SCREEN_DONVITAL, FALSE},                      //  gb
   {STARTUP_TESTS, GUI_SCREEN_DONINFO, STARTUP_TESTS, GUI_SCREEN_PREDICT, FALSE},                       //  cont
   {STARTUP_TESTS, GUI_SCREEN_CONFIRM_DON_INFO, STARTUP_TESTS, GUI_SCREEN_PREDICT, FALSE},              //  cont
   {STARTUP_TESTS, GUI_SCREEN_PREDICT, STARTUP_TESTS, GUI_SCREEN_DONINFO, FALSE},                       //  gb
   {STARTUP_TESTS, GUI_SCREEN_PREDICT, STARTUP_TESTS, GUI_SCREEN_CONFIRM_DON_INFO, FALSE},              //  gb
   {STARTUP_TESTS, GUI_SCREEN_PREDICT, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},                      //  cont
   {AC_PRIME, GUI_SCREEN_SYSDONOR, DONOR_CONNECTED, GUI_SCREEN_DONCONN, FALSE},                         //  cont
                                                                                                        //                                                                                                        //
                                                                                                        //
   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CFG_PLT_AMAP, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CFG_PLT_AMAP, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CFG_PLT_AMAP, FALSE},
   {AC_PRIME, GUI_SCREEN_CFG_PLT_AMAP, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},
   //
   //
   {DONOR_CONNECTED, GUI_SCREEN_DONCONN, DONOR_CONNECTED, GUI_SCREEN_RUNPROC, FALSE},                   //  cont
   {DONOR_CONNECTED, GUI_SCREEN_DONCONN, BLOOD_PRIME, GUI_SCREEN_RUNPROC, FALSE},                       //  cont
   {BLOOD_PRIME, GUI_SCREEN_RUNPROC, BLOOD_PRIME, GUI_SCREEN_DONINFO, FALSE},                           //  infobtn
   {BLOOD_PRIME, GUI_SCREEN_DONINFO, BLOOD_PRIME, GUI_SCREEN_RUNPROC, FALSE},                           //  cont
   {BLOOD_PRIME, GUI_SCREEN_DONINFO, BLOOD_PRIME, GUI_SCREEN_PREDICT, FALSE},                           //  predict
   {BLOOD_PRIME, GUI_SCREEN_DONINFO, BLOOD_RUN, GUI_SCREEN_DONINFO, FALSE},                             //  noop state chg.  // Added per M. Cobb (3/18/96)
   {BLOOD_PRIME, GUI_SCREEN_PREDICT, BLOOD_RUN, GUI_SCREEN_PREDICT, FALSE},                             //  noop state chg.  // Added per M. Cobb (3/18/96)
   {BLOOD_PRIME, GUI_SCREEN_RUNPROC, BLOOD_RUN, GUI_SCREEN_RUNPROC, FALSE},                             //  state chg. added: 3/27/96
   {BLOOD_PRIME, GUI_SCREEN_TROUBLE, BLOOD_PRIME, GUI_SCREEN_PREDICT, FALSE},                           //  predict
   {BLOOD_PRIME, GUI_SCREEN_PREDICT, BLOOD_PRIME, GUI_SCREEN_RUNPROC, FALSE},                           //  cont
   {BLOOD_PRIME, GUI_SCREEN_PREDICT, BLOOD_PRIME, GUI_SCREEN_TROUBLE, FALSE},                           //  goback
   {BLOOD_PRIME, GUI_SCREEN_PREDICT, BLOOD_PRIME, GUI_SCREEN_DONINFO, FALSE},                           //  goback
   // {BLOOD_PRIME        , GUI_SCREEN_TROUBLE     , BLOOD_PRIME       , GUI_SCREEN_SERV1       , FALSE},  //  service
   // {BLOOD_PRIME        , GUI_SCREEN_SERV1       , BLOOD_PRIME       , GUI_SCREEN_RUNPROC     , FALSE},  //  cont
   {BLOOD_RUN, GUI_SCREEN_RUNPROC, BLOOD_RUN, GUI_SCREEN_DONINFO, FALSE},                               //  infobtn
   {BLOOD_RUN, GUI_SCREEN_DONINFO, BLOOD_RUN, GUI_SCREEN_RUNPROC, FALSE},                               //  cont
   {BLOOD_RUN, GUI_SCREEN_DONINFO, BLOOD_RUN, GUI_SCREEN_PREDICT, FALSE},                               //  predict
   {BLOOD_RUN, GUI_SCREEN_RUNPROC, BLOOD_RUN, GUI_SCREEN_TROUBLE, FALSE},                               //  ts btn
   {BLOOD_RUN, GUI_SCREEN_RUNPROC, BLOOD_RUN, GUI_SCREEN_PREDICT, FALSE},                               //  I.T. 3448 -  this allows an adjust_scrn-initiated predict_scrn switch to happen, if the trouble screen times out and returns to runproc.
   {BLOOD_RUN, GUI_SCREEN_TROUBLE, BLOOD_RUN, GUI_SCREEN_RUNPROC, FALSE},                               //  cont
   {BLOOD_RUN, GUI_SCREEN_TROUBLE, BLOOD_RUN, GUI_SCREEN_PREDICT, FALSE},                               //  predict
   {BLOOD_RUN, GUI_SCREEN_PREDICT, BLOOD_RUN, GUI_SCREEN_RUNPROC, FALSE},                               //  cont
   {BLOOD_RUN, GUI_SCREEN_PREDICT, BLOOD_RUN, GUI_SCREEN_TROUBLE, FALSE},                               //  goback
   {BLOOD_RUN, GUI_SCREEN_PREDICT, BLOOD_RUN, GUI_SCREEN_DONINFO, FALSE},                               //  goback
   // {BLOOD_RUN          , GUI_SCREEN_TROUBLE     , BLOOD_RUN         , GUI_SCREEN_SERV1       , FALSE},  //  service
   {BLOOD_RUN, GUI_SCREEN_TROUBLE, BLOOD_RINSEBACK, GUI_SCREEN_TROUBLE, FALSE},                         //  noop state chg. // Added per M. Cobb (3/18/96)
   {BLOOD_RUN, GUI_SCREEN_RUNPROC, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                         //  state chg. added: 3/27/96
   // {BLOOD_RUN          , GUI_SCREEN_SERV1       , BLOOD_RUN         , GUI_SCREEN_RUNPROC     , FALSE},  //  cont
   {BLOOD_RUN, GUI_SCREEN_DONINFO, BLOOD_RINSEBACK, GUI_SCREEN_DONINFO, FALSE},                         //  state chg. added: 10/6/96
   {BLOOD_RUN, GUI_SCREEN_PREDICT, BLOOD_RINSEBACK, GUI_SCREEN_PREDICT, FALSE},                         //  state chg. added: 10/6/96
   {BLOOD_RINSEBACK, GUI_SCREEN_DONINFO, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                   //  state chg. added: 4/3/96
   {BLOOD_RINSEBACK, GUI_SCREEN_PREDICT, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                   //  state chg. added: 10/6/96
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, BLOOD_RINSEBACK, GUI_SCREEN_TROUBLE, FALSE},                   //  ts btn
   {BLOOD_RINSEBACK, GUI_SCREEN_TROUBLE, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                   //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, FALSE},                  //  run summary during rinseback
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, DONOR_DISCONNECT, GUI_SCREEN_DISCONN, FALSE},                  //  canc

   {DONOR_DISCONNECT, GUI_SCREEN_DISCONN,              METERED_STORAGE,     GUI_SCREEN_DISCONN,        FALSE},
   {METERED_STORAGE, GUI_SCREEN_DISCONN,               METERED_STORAGE,     GUI_SCREEN_AAS_TRANSITION_DUMMY,       FALSE},
   {DONOR_DISCONNECT, GUI_SCREEN_DISCONN,              METERED_STORAGE,     GUI_SCREEN_AAS_TRANSITION_DUMMY,       FALSE},


   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_RUNSUM_PLT,         FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC,         FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC_PROD1,           FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC_PROD2,           FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_CLAMP_CHANNEL,          FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_PAS_SPLIT,              FALSE},

   // For PFR
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_CHANNEL_TEST,           FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_PAS,            FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_RAS,            FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_PRIME,                  FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_ADDITION,               FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_TRANSITION_DUMMY,  METERED_STORAGE,     GUI_SCREEN_AAS_CLAMP_CHANNEL,          FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CHANNEL_TEST,      METERED_STORAGE,     GUI_SCREEN_AAS_TRANSITION_DUMMY,       FALSE},


   {METERED_STORAGE, GUI_SCREEN_RUNSUM_PLT,            METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC,         FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUM_PLT,            METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC_PROD1,   FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUM_PLT,            METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC_PROD2,   FALSE},

   {METERED_STORAGE, GUI_SCREEN_RUNSUM_RBC_PROD1,       METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC_PROD2,   FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUM_RBC_PROD2,       METERED_STORAGE,     GUI_SCREEN_RUNSUM_RBC_PROD1,   FALSE},

   {METERED_STORAGE, GUI_SCREEN_RUNSUM_PLT,           METERED_STORAGE,     GUI_SCREEN_AAS_CLAMP_CHANNEL,  FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUM_RBC,           METERED_STORAGE,     GUI_SCREEN_AAS_CLAMP_CHANNEL,  FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUM_RBC_PROD1,     METERED_STORAGE,     GUI_SCREEN_AAS_CLAMP_CHANNEL,  FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUM_RBC_PROD2,     METERED_STORAGE,     GUI_SCREEN_AAS_CLAMP_CHANNEL,  FALSE},

   {METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL,      METERED_STORAGE,     GUI_SCREEN_AAS_CHANNEL_TEST,  FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CHANNEL_TEST,       METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_PAS,   FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CHANNEL_TEST,       METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_RAS,   FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CONNECT_PAS,        METERED_STORAGE,     GUI_SCREEN_AAS_PRIME,         FALSE},
   // {METERED_STORAGE    , GUI_SCREEN_AAS_CONNECT_PAS,		METERED_STORAGE,	 GUI_SCREEN_AAS_CONNECT_RAS,		   FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CONNECT_RAS,        METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_PAS,           FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CONNECT_RAS,        METERED_STORAGE,     GUI_SCREEN_AAS_PRIME,         FALSE},

   {METERED_STORAGE, GUI_SCREEN_AAS_PAS_SPLIT,          METERED_STORAGE,     GUI_SCREEN_AAS_PRIME,     FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_PRIME,              METERED_STORAGE,     GUI_SCREEN_AAS_PAS_SPLIT,         FALSE},

   {METERED_STORAGE, GUI_SCREEN_AAS_PAS_SPLIT,          METERED_STORAGE,     GUI_SCREEN_AAS_ADDITION,      FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_PRIME,              METERED_STORAGE,     GUI_SCREEN_AAS_ADDITION,      FALSE},

   // END ADDITION
   {METERED_STORAGE, GUI_SCREEN_AAS_ADDITION,           METERED_STORAGE_DISCONNECT,  GUI_SCREEN_AAS_RUN_SUMMARY,    FALSE},
   {METERED_STORAGE_DISCONNECT, GUI_SCREEN_AAS_RUN_SUMMARY, METERED_STORAGE_DISCONNECT,  GUI_SCREEN_MSS_END,            FALSE},

   {METERED_STORAGE, GUI_SCREEN_AAS_ADDITION, METERED_STORAGE, GUI_SCREEN_RUNSUMM1, FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_ADDITION, POST_RUN, GUI_SCREEN_AAS_RUN_SUMMARY, FALSE},
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_AAS_RUN_SUMMARY, FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUMM1, METERED_STORAGE, GUI_SCREEN_AAS_ADDITION, FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUMM2, METERED_STORAGE, GUI_SCREEN_AAS_ADDITION, FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUMM1, METERED_STORAGE, GUI_SCREEN_RUNSUMM2, FALSE},
   {METERED_STORAGE, GUI_SCREEN_RUNSUMM2, METERED_STORAGE, GUI_SCREEN_RUNSUMM1, FALSE},
   {METERED_STORAGE_DISCONNECT, GUI_SCREEN_MSS_END, POST_RUN, GUI_SCREEN_AAS_RUN_SUMMARY, FALSE},
   {POST_RUN, GUI_SCREEN_AAS_RUN_SUMMARY, POST_RUN, GUI_SCREEN_RUNSUMM1, FALSE},
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_AAS_RUN_SUMMARY, FALSE},

   {DONOR_DISCONNECT, GUI_SCREEN_DISCONN, DONOR_DISCONNECT, GUI_SCREEN_SEAL_AND_DETACH, FALSE},                                 //  discon btn
   {DONOR_DISCONNECT, GUI_SCREEN_SEAL_AND_DETACH, POST_RUN, GUI_SCREEN_AAS_RUN_SUMMARY, FALSE},                                 //  discon btn

   // PAS PFR TRANSITIONS
   {METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL,      METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_PAS,        FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL,      METERED_STORAGE,     GUI_SCREEN_AAS_CONNECT_RAS,        FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL,      METERED_STORAGE,     GUI_SCREEN_AAS_PRIME,      FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL,      METERED_STORAGE,     GUI_SCREEN_AAS_ADDITION,       FALSE},
   {METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL,      METERED_STORAGE,     GUI_SCREEN_MSS_END,        FALSE},


#if CPU==SIMNT
   // This is just stuff so I can jump around in the simulator.
   {STARTUP_TESTS, GUI_SCREEN_SYSDONOR, DONOR_DISCONNECT, GUI_SCREEN_DISCONN, FALSE},                   //  canc
   {DONOR_DISCONNECT, GUI_SCREEN_DISCONN, METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_DISCONN, METERED_STORAGE, GUI_SCREEN_AAS_CLAMP_CHANNEL, FALSE},
#endif

   // FROM THE RINSEBACK PROCESS YOU CAN GO TO RUNSUM, AND VICE VERSA.
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                  //  gb or run complete
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, FALSE},                  //  gb or run complete
   // THE PRODUCT RUNSUM SCREENS CAN GO BACK TO RINSEBACK PROCESS
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                  //  run complete
#if CPU==SIMNT
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE},
#endif /* if CPU==SIMNT */
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},              //  run complete
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},              //  run complete

   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1,       BLOOD_RINSEBACK,     GUI_SCREEN_RUNSUM_RBC_PROD2,   FALSE},
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2,       BLOOD_RINSEBACK,     GUI_SCREEN_RUNSUM_RBC_PROD1,   FALSE},

   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                  //  run complete
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},               //  run complete
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNPROC, FALSE},                    //  run complete
   // RUNSUM1 CAN GO TO ANY OF THE PRODUCT END RUN SUM SCREENS.
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, FALSE, TRUE},                 //  if labeling needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE, TRUE},           //  if labeling needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE, TRUE},           //  if labeling needed

   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, FALSE, TRUE},                 //  if product verification needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, FALSE, TRUE},                   //  if product verification needed
   // PRODUCT END RUN SUMMARY SCREENS CAN GO BACK TO RUNSUM1.
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1,  FALSE, TRUE},                //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1,  FALSE, TRUE},                   //  if labeling needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1,  FALSE, TRUE},             //  if labeling needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1,  FALSE, TRUE},             //  if labeling needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1,  FALSE, TRUE},                   //  if product verification needed
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM1,  FALSE, TRUE},                     //  if product verification needed
   // PLATELET RUNSUM CAN GO TO PLASMA, RBC, OR RUNSUM2.
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, FALSE, TRUE},                 //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, FALSE, TRUE},                    //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, FALSE, TRUE},                      //  cont
   // PLASMA AND RBC RUNSUM CAN BACKTRACK TO PLATELET RUNSUM
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, FALSE, TRUE},                 //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, FALSE, TRUE},                    //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, FALSE, TRUE},              //  cont

   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLT, FALSE, TRUE},                      //  cont
   // PLASMA RUNSUM CAN GO TO RBC RUNSUM OR RUNSUMM2, AND VICE VERSA.
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, FALSE, TRUE},                 //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE, TRUE},           //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE, TRUE},           //  cont

   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, FALSE, TRUE},                 //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, FALSE, TRUE},               //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, FALSE, TRUE},         //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, FALSE, TRUE},         //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_PLASMA, FALSE, TRUE},                 //  cont
   // RBC RUNSUM CAN GO TO RUNSUMM2 AND VICE-VERSA
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, FALSE, TRUE},                    //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC, FALSE, TRUE},                    //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE, TRUE},              //  cont
   {BLOOD_RINSEBACK, GUI_SCREEN_RUNSUMM2, BLOOD_RINSEBACK, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE, TRUE},              //  cont

   // RUNSUM1 CAN GO TO ANY OF THE PRODUCT END RUN SUM SCREENS.
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_RUNSUM_RBC, FALSE},               //  if labeling needed
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE},         //  if labeling needed
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE},         //  if labeling needed
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_RUNSUM_PLT, FALSE},               //  if product verification needed
   {POST_RUN, GUI_SCREEN_RUNSUMM1, POST_RUN, GUI_SCREEN_RUNSUMM2, FALSE},                 //  if product verification needed
   // PRODUCT END RUN SUMMARY SCREENS CAN GO BACK TO RUNSUM1.
   {POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, POST_RUN, GUI_SCREEN_RUNSUMM1,  FALSE},               //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC, POST_RUN, GUI_SCREEN_RUNSUMM1,  FALSE},                  //  if labeling needed
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, POST_RUN, GUI_SCREEN_RUNSUMM1,  FALSE},            //  if labeling needed
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, POST_RUN, GUI_SCREEN_RUNSUMM1,  FALSE},            //  if labeling needed

   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1,       POST_RUN,    GUI_SCREEN_RUNSUM_RBC_PROD2,   FALSE},
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2,       POST_RUN,    GUI_SCREEN_RUNSUM_RBC_PROD1,   FALSE},

   {POST_RUN, GUI_SCREEN_RUNSUM_PLT, POST_RUN, GUI_SCREEN_RUNSUMM1,  FALSE},                  //  if product verification needed
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_RUNSUMM1,  FALSE},                    //  if product verification needed
   // PLATELET RUNSUM CAN GO TO PLASMA OR RBC RUNSUM, OR RUNSUMM2.
   {POST_RUN, GUI_SCREEN_RUNSUM_PLT, POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, FALSE},               //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLT, POST_RUN, GUI_SCREEN_RUNSUM_RBC, FALSE},                  //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLT, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLT, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLT, POST_RUN, GUI_SCREEN_RUNSUMM2, FALSE},                    //  cont
   // PLASMA, RBC, AND RUNSUMM2 CAN BACKTRACK TO PLATELET RUNSUM
   {POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, POST_RUN, GUI_SCREEN_RUNSUM_PLT, FALSE},                 //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC, POST_RUN, GUI_SCREEN_RUNSUM_PLT, FALSE},                    //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, POST_RUN, GUI_SCREEN_RUNSUM_PLT, FALSE},              //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, POST_RUN, GUI_SCREEN_RUNSUM_PLT, FALSE},              //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_RUNSUM_PLT, FALSE},                      //  cont
   // PLASMA RUNSUM CAN GO TO RBC RUNSUM, AND VICE VERSA.
   {POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, POST_RUN, GUI_SCREEN_RUNSUM_RBC, FALSE},               //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE},         //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE},         //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, POST_RUN, GUI_SCREEN_RUNSUMM2, FALSE},                 //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC, POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, FALSE},               //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, FALSE},         //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, FALSE},         //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_RUNSUM_PLASMA, FALSE},                 //  cont
   // RBC RUNSUM CAN GO TO RUNSUMM2 AND VICE-VERSA
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC, POST_RUN, GUI_SCREEN_RUNSUMM2, FALSE},                  //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, POST_RUN, GUI_SCREEN_RUNSUMM2, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, POST_RUN, GUI_SCREEN_RUNSUMM2, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_RUNSUM_RBC, FALSE},                  //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD2, FALSE},            //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, TRUE},                //  cont
   {POST_RUN, GUI_SCREEN_RUNSUMM2, POST_RUN, GUI_SCREEN_POWERDOWN, TRUE},                    //  cont
   {POST_RUN, GUI_SCREEN_POWERDOWN, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, TRUE},               //  cont
    #if CPU==SIMNT
   {POST_RUN, GUI_SCREEN_RUNSUM_RBC, POST_RUN, GUI_SCREEN_RUNSUM_RBC_PROD1, FALSE},
    #endif /* if CPU==SIMNT */

   ///////////////////////////////////////////////////////////////////////////////
   // Configuration Screens
   {STARTUP_TESTS, GUI_SCREEN_SYSDONOR, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},                                   //  load config screen
   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_SYSDONOR, FALSE},                                   //  return from config screen
   {AC_PRIME, GUI_SCREEN_SYSDONOR, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},                                             //  load config screen
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_SYSDONOR, FALSE},                                             //  return from config screen

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_MACHINE, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_MACHINE, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_MACHINE, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_MACHINE, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_LANGUAGE, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_LANGUAGE, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_LANGUAGE, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_LANGUAGE, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_PLASMA, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_PLASMA, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_PLASMA, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_PLASMA, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_PLATELET, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_PLATELET, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_PLATELET, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_PLATELET, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_PROCEDURE, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_PROCEDURE, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_PROCEDURE, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_PROCEDURE, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_RBC, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_RBC, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_RBC, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_RBC, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_DONOR, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_DONOR, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_DONOR, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_DONOR, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_VOLUME_LIMITS, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_VOLUME_LIMITS, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_VOLUME_LIMITS, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_VOLUME_LIMITS, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_PRIORITY, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_PRIORITY, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_PRIORITY, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_PRIORITY, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_METERED_SOLUTIONS, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_METERED_SOLUTIONS, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_METERED_SOLUTIONS, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_METERED_SOLUTIONS, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},

   {STARTUP_TESTS, GUI_SCREEN_CONFIG, STARTUP_TESTS, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, FALSE},
   {STARTUP_TESTS, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, STARTUP_TESTS, GUI_SCREEN_CONFIG, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG, AC_PRIME, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, FALSE},
   {AC_PRIME, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, AC_PRIME, GUI_SCREEN_CONFIG, FALSE},
   // end of Configuration Screens
   ///////////////////////////////////////////////////////////////////////////////

   // DO NOT add any states after the "table end" entry
   {TABLE_END,           GUI_SCREEN_NULL_SCREEN,  TABLE_END,          GUI_SCREEN_NULL_SCREEN} /// TABLE END ///
};


// Definition of the default screen table: Procedure State ==> Default Screen
DEFAULT_SCREEN_TABLE default_screen_tab[] =
{
   // Default screens
   {STARTUP_TESTS,    GUI_SCREEN_SYSDONOR},
   {DISPOSABLE_TEST,  GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION},
   {AC_CONNECTED,     GUI_SCREEN_SYSACATT},
   {AC_PRIME,         GUI_SCREEN_SYSDONOR},
   {DONOR_CONNECTED,  GUI_SCREEN_DONCONN},
   {BLOOD_PRIME,     GUI_SCREEN_RUNPROC},
   {BLOOD_RUN,        GUI_SCREEN_RUNPROC},
   {BLOOD_RINSEBACK,  GUI_SCREEN_RUNPROC},
   {DONOR_DISCONNECT, GUI_SCREEN_DISCONN},
   {POST_RUN,         GUI_SCREEN_AAS_RUN_SUMMARY},
   {DEFAULT_STATE,    GUI_SCREEN_POWERUP},
   {SELF_TEST,        GUI_SCREEN_POWERUP},
   {METERED_STORAGE,  GUI_SCREEN_AAS_TRANSITION_DUMMY},
   {METERED_STORAGE_DISCONNECT, GUI_SCREEN_AAS_RUN_SUMMARY},

   // Acceptable screens (if we are already on these we don't need to force a
   // change.
   {METERED_STORAGE,  GUI_SCREEN_RUNSUMM1},
   {METERED_STORAGE,  GUI_SCREEN_RUNSUMM2},

   {BLOOD_RINSEBACK,  GUI_SCREEN_RUNSUMM1},
   {BLOOD_RINSEBACK,  GUI_SCREEN_RUNSUMM2},

   // DO NOT add any states after the "table end" entry
   {TABLE_END}
};

/* FORMAT HASH a61b247d615cf70ae46974b3f73e8c07 */
