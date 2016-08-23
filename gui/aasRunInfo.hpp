#ifndef __AAS_RUN_INFO_HPP__
#define __AAS_RUN_INFO_HPP__

#include "trimamessages.h"
#include "gui.h"
#include "guiproc.h"

class AasRunInfo
{
private:

   bool _readyForPASAddition;
   bool _readyForRASAddition;


   bool _mssSetupComplete;

   bool _oldIsRasProcedure,
        _oldIsPasProcedure,
        _oldTargetedAsSplit;

   int _isRasProcedureCheckCount,
       _isPasProcedureCheckCount,
       _targetedAsSplitCheckCount;

   void* _attachedObj;

   // Messages
   AasAdditionVolumeUpdate _msgAasAdded;
   CancelMssMsg            _msgCancelAas;
   MSSCompletionMsg        _msgAasComplete;
   ProcToGuiAasPfrMsg      _msgPfrForceTransition;
   MSSPASAdditionMsg       _msgReadyForPASAddition;
   MSSRASAdditionMsg       _msgReadyForRASAddition;
   PartialCancellationMsg  _msgPartialCancelation;
   MSSSetupCompleteMsg     _msgMssSetupComplete;

   CallbackBase            _aasVolAddedCallback;
   CallbackBase            _mssSetupCompleteCallback;

   void gotoChannelTest ();
   void gotoAasPrime ();
   void makePfrScreenTransition ();

   GUI_SCREEN_ID convertPfrEnumToScreenId (AAS_PFR_SCREEN pfrValue);

public:
   AasRunInfo();
   virtual ~AasRunInfo();
   void init ();

   // Getters
   float getPasNeeded ();
   float getRasNeeded ();
   float getAasNeeded ();
   float getPasTotalVolNeeded ();
   float getAasAdded ();

   float getMeterPercentage ();

   int getPasSalvageVol ();
   int getRasSalvageVol ();

   // Setters
   void setAasAdded (float aasAdded);

   bool rasBlockedDueToCpsAlarms ();
   bool mssSetupComplete ();

   void aasAdditionVolumeUpdate (void);
   void cancelAas (void);
   void aasComplete (void);

   void receivedRASAddition (void);
   void receivedPASAddition (void);

   void gotoAddition (void);
   void handlePartialCancelation (void);
   void handleMssSetupComplete (void);


   void detach (void* obj);
   void attach (void* obj);

   void setAasAddedCallback (const CallbackBase& callback);
   void setMssSetupCompleteCallback (const CallbackBase& callback);

   bool isPasProcedure (bool ignoreAlarms = false);
   bool isRasProcedure (bool ignoreAlarms = false);

   bool readyForAddition ();         // Has proc told us that priming is complete and we're ready for addition?
   bool productTargetedAsDrbcPtf (); // Was this product initially targeted for DRBC autosplit? (May or may not have actually split.)

    #if CPU==SIMNT
    #define __AAS_RUN_INFO_SIMNT_HEADER_EXTRAS__
    #include "../unit_test/gui/h/simAasRunInfo.hpp"
    #undef __AAS_RUN_INFO_SIMNT_HEADER_EXTRAS__
    #endif
};

#endif // #ifndef __AAS_RUN_INFO_HPP__

/* FORMAT HASH f4617ffbe1693dd66dd43f47096b912c */
