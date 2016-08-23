/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      states.h, Focussed System states include file
 *
 * ABSTRACT:   This file includes all state definitions for function of
 *                       a procedure.
 *
 */

#ifndef STATES_H
#define STATES_H

enum State_names
{
   FIRST_STATE,
   SELF_TEST,
   STARTUP_TESTS,
   DISPOSABLE_TEST,
   AC_CONNECTED,
   AC_PRIME,
   DONOR_CONNECTED,
   BLOOD_PRIME,
   BLOOD_RUN,
   BLOOD_RINSEBACK,
   DONOR_DISCONNECT,
   POST_RUN,
   METERED_STORAGE,
   METERED_STORAGE_DISCONNECT,
   SERVICE_MODE,
   DEFAULT_STATE,
   LAST_PROCEDURE_STATE=10000,

// It is important to PQIMonitor that SS_CHANNEL_SETUP be first
// and SS_RBC be last in the PltPlsRBC-set block of substates.
   SS_CHANNEL_SETUP,
   SS_PREPLATELET_PLASMA,
   SS_PREPLATELET_NOPLASMA,
   SS_PLATELET_PLASMA,
   SS_PCA,
   SS_MIDRUN,
   SS_PIR_PLASMA,
   SS_PIR_NOPLASMA,
   SS_EXTENDED_PCA,
   SS_EXTENDED_PLASMA,
   SS_RBC_PTF_SETUP_1,
   SS_RBC_PTF_SETUP_2,
   SS_RBC_PTF_PRIME,
   SS_RBC,

// It is important to PQIMonitor that SS_RP_CHANNEL_SETUP be first
// and SS_RP_RBC be last in the RBCPls-set block of substates.
   SS_RP_CHANNEL_SETUP,       // RBC-Plasma channel setup
   SS_RP_PLASMA_ONLY,         // RBC-Plasma plasma only
   SS_RP_RBC_PTF_SETUP_1,     // RBC-Plasma Setup (clear buffy coat)
   SS_RP_RBC_PTF_SETUP_2,     // RBC-Plasma Setup (bring crit down to normal)
   SS_RP_RBC,                 // RBC collection

   SS_RP_RBC_PTF_SETUP_1_DOUBLE,     // RBC-Plasma Setup (clear buffy coat)
   SS_RP_RBC_PTF_SETUP_2_DOUBLE,     // RBC-Plasma Setup (bring crit down to normal)
   SS_RP_RBC_DOUBLE,                 // RBC collection


   // metered storage states
   SS_MSSPFR,
   SS_VERIFYCLOSURE,
   SS_MSSSETUP,
   SS_RESSETUP,
   SS_VERIFYCLOSURE3,
   SS_MSSCONNECT,
   SS_MSSPRIME,
   SS_RESSETUP2,
   SS_MSSRBCPRIME1,
   SS_MSSRBCPRIMESALVAGE,
   SS_RESSETUP3,
   SS_MSSPLTPRIMESALVAGE,
   SS_MSSPLTPRIME1,
   SS_MSSPLTPRIME2,
   SS_MSSPARALLELPROC,
   SS_MSSVACPURGE,
   SS_PASFINALFLOWVERIFICATION,
   SS_MSSVACPURGEFINAL,
   SS_MSSPLTADDITIONPAUSE,
   SS_MSSADDITION,
   SS_MSSRBCADDITION,
   SS_MSSPLTADDITION,

   INVALID_SUBSTATE,
   NOCOLLECT_STATE

};

//
//
//  This structure is defined to allow both states and substates to
//  be requested simultaneously.
//
struct stateSubstateChangeRequest_t
{
   State_names state;
   State_names substate;
};

struct the_state_struct
{
   enum State_names state;
   char*            state_text;
};

#ifdef STATE_LIST
the_state_struct        state_name_table[] = {
   { FIRST_STATE, "FirstState" },
   { SELF_TEST, "SelfTests" },
   { STARTUP_TESTS, "StartupTests" },
   { DISPOSABLE_TEST, "DisposableTest" },
   { AC_CONNECTED, "ACConnected" },
   { AC_PRIME, "ACPrime" },
   { DONOR_CONNECTED, "DonorConnected" },
   { BLOOD_PRIME, "BloodPrime" },
   { BLOOD_RUN, "BloodRun" },
   { BLOOD_RINSEBACK, "BloodRinseback" },
   { DONOR_DISCONNECT, "DonorDisconnect" },
   { POST_RUN, "PostRun" },
   { METERED_STORAGE, "MeteredStorage" },
   { METERED_STORAGE_DISCONNECT, "MSSDisconnect" },
   { SERVICE_MODE, "ServiceMode" },
   { DEFAULT_STATE, "DefaultState" },
   { LAST_PROCEDURE_STATE, "LastProcedureState" },


   { SS_CHANNEL_SETUP, "ChannelSetup" },
   { SS_PREPLATELET_PLASMA, "PrePlateletPlasma" },
   { SS_PREPLATELET_NOPLASMA, "PrePlateletNoPlasma" },
   { SS_PLATELET_PLASMA, "PlateletPlasma" },
   { SS_PCA, "PlateletConcentrationAdjustment" },
   { SS_MIDRUN, "MidRun"},
   { SS_PIR_PLASMA, "PIRPlasma" },
   { SS_PIR_NOPLASMA, "PIRNoPlasma" },
   { SS_EXTENDED_PCA, "ExtendedPCA" },
   { SS_EXTENDED_PLASMA, "ExtendedPlasma" },
   { SS_RBC_PTF_SETUP_1, "RBCPTFSetup1" },
   { SS_RBC_PTF_SETUP_2, "RBCPTFSetup2" },
   { SS_RBC_PTF_PRIME,   "RBCPTFPrime" },
   { SS_RBC, "RBCCollection" },

   { SS_RP_CHANNEL_SETUP, "ChannelSetup" },                // RBC-Plasma channel setup
   { SS_RP_PLASMA_ONLY, "PlasmaOnly" },                    // RBC-Plasma plasma only
   { SS_RP_RBC_PTF_SETUP_1, "RBCPTFSetup1" },              // RBC-Plasma only
   { SS_RP_RBC_PTF_SETUP_2, "RBCPTFSetup2" },              // RBC-Plasma only
   { SS_RP_RBC, "RBCCollection" },                         // RBC collection
   { SS_RP_RBC_PTF_SETUP_1_DOUBLE, "RBCPTFSetup1Double" }, // repeat for double RBC-Plasma only
   { SS_RP_RBC_PTF_SETUP_2_DOUBLE, "RBCPTFSetup2Double" }, // repeat for double RBC-Plasma only
   { SS_RP_RBC_DOUBLE, "RBCCollectionDouble" },            // repeat for double RBC collection

   // metered storgae substates
   { SS_MSSSETUP,         "MSSSetup" },
   { SS_MSSPFR,                "MSSPFR" },
   { SS_RESSETUP,              "ResSetup" },
   { SS_RESSETUP2,             "ResSetup2" },
   { SS_RESSETUP3,             "ResSetup3" },
   { SS_VERIFYCLOSURE,         "VerifyClosure" },
   { SS_VERIFYCLOSURE3,        "VerifyClosure3" },
   { SS_MSSCONNECT,            "MSSConnect" },
   { SS_MSSPRIME,              "MSSPrime" },
   { SS_MSSRBCPRIME1,          "MSSRBCPrime1" },
   { SS_MSSRBCPRIMESALVAGE,    "MSSRBCPrimeSalvage" },
   { SS_MSSPLTPRIMESALVAGE,    "MSSPLTPrimeSalvage" },
   { SS_MSSPLTPRIME1,          "MSSPLTPrime1" },
   { SS_MSSPLTPRIME2,          "MSSPLTPrime2" },
   { SS_MSSPARALLELPROC,       "MSSParallelProcessing" },
   { SS_MSSVACPURGE,           "VacPurge" },
   { SS_PASFINALFLOWVERIFICATION, "PasFinalFlowVerification" },
   { SS_MSSVACPURGEFINAL,      "VacPurgeFinal" },
   { SS_MSSPLTADDITIONPAUSE,   "MSSPltAdditionPause" },
   { SS_MSSADDITION,           "MSSAddition" },
   { SS_MSSRBCADDITION,        "MSSRBCAddition" },
   { SS_MSSPLTADDITION,        "MSSPLTAddition" },



   { INVALID_SUBSTATE, "Invalid" },
   { NOCOLLECT_STATE, "NoCollectState" }

};
#else
extern the_state_struct state_name_table[];
#endif

#endif

/* FORMAT HASH 6f8512e0f7ba899cc739d8aa3e9dd417 */
