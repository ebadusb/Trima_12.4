/************************> FOCUSSED GUI DOMAIN <****************************
*
*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved
*
*     Author:       Dan Slauson
*
*     Class name:   Screen_MSS_END
*
*     File name:    mssEnd.cpp
*
*     Contents:     See prototype header descriptions below...
*
*     Description:	See description in header file.
*
*************************< FOCUSSED GUI DOMAIN >***************************
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/mssEnd.cpp 1.11 2008/10/03 16:14:26Z dslausb Exp dslausb $
*
****************************************************************************/


#include <vxworks.h>


// BASE CLASS INCLUDE

#include "mssEnd.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

#if (CPU==SIMNT) // Include unit testing definitions if compiled for simulator.
#include "guiUnitTestMessageDefs.h"
#endif

////////////////////////////////////////////////////////////////////////////////
Screen_MSS_END::Screen_MSS_END()
   : Base_Apheresis_Window   (MAGENTA, GUI_SCREEN_MSS_END),
     _txtSealDetach          (textMiscAasSealDetach),
     _txtRemoveSet           (textMiscInstructionAasRemoveTubingSet),
     _bmpGraphic             (25, 150, BITMAP_NULL),
     _currentEndMssState     (END_MSS_SCREEN_STATE_NULL),
     _bmpIdSeal(BITMAP_NULL),
     _bmpIdRemove(BITMAP_NULL)

{}


////////////////////////////////////////////////////////////////////////////////
Screen_MSS_END::~Screen_MSS_END()
{
   Screen_MSS_END::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_MSS_END::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_cancel.deallocate_resources();
   btn_goback.deallocate_resources();

   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_SOLUTIONS);
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_UNLOAD);

   selectGraphics();

   btn_continue.allocate_resources(*this);

   // Now link the status bar/line to this window
   link_status_bar_line();

   _msgCassetteRaised.init(Callback<Screen_MSS_END>(this, &Screen_MSS_END::procSaysCassetteIsRaised));

   transitionGraphicsByScreenState(END_MSS_SCREEN_STATE_SEAL_DETACH);
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_MSS_END::deallocate_resources ()
{
   // deallocate all constituent objects first

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   _msgCassetteRaised.deregisterMsg();

   // clean up the text lines
   _txtSealDetach.deallocate_resources();
   _txtRemoveSet.deallocate_resources();

   _bmpGraphic.deallocate_resources();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////

void Screen_MSS_END::process_continue_button ()
{
   // Disable all the standard buttons on this screen

   // set the metering product flags
   DataLog(log_level_gui_info) << "Screen_MSS_END::process_continue_button().  State == " << printState(_currentEndMssState) << endmsg;

   switch (_currentEndMssState)
   {
      case END_MSS_SCREEN_STATE_SEAL_DETACH :
         completionAck();
         transitionGraphicsByScreenState(END_MSS_SCREEN_STATE_REMOVE);
         break;

      case END_MSS_SCREEN_STATE_REMOVE :
         confirmUnload();
         break;

      default :
      {
         DataLog(log_level_gui_info) << "Unknown state for Contiunue button:" << endmsg;
         DataLog(log_level_gui_info) << " State == " << _currentEndMssState << endmsg;
      }
   }
}   // END of Screen_MSSCLAMP PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////
void Screen_MSS_END::completionAck ()
{
   MSSAckCompletionMsg _ackCompleted(MessageBase::SEND_GLOBAL);
   _ackCompleted.send(0);
   DataLog(log_level_gui_info) << "MSSAckCompletionMsg sent" << endmsg;
}

////////////////////////////////////////////////////////////////////////////
void Screen_MSS_END::transitionGraphicsByScreenState (END_MSS_STATE newState)
{
   if (newState == _currentEndMssState) return;

   _currentEndMssState = newState;

   DataLog(log_level_gui_info) << "transition to "
                               << printState(_currentEndMssState) << "("
                               << _currentEndMssState << ")" << endmsg;

   switch (_currentEndMssState)
   {
      case END_MSS_SCREEN_STATE_SEAL_DETACH :
      {
         guiglobs::apheresis_status_line->set_current_text (textStatAasSealDetach);

         _bmpGraphic.set_id(_bmpIdSeal);
         _bmpGraphic.allocate_resources(*this);
         _txtSealDetach.allocate_resources(*this);
         _txtSealDetach.set_color(YELLOW);

         _txtRemoveSet.deallocate_resources();
      }
      break;

      case END_MSS_SCREEN_STATE_REMOVE :
      {
         btn_continue.deallocate_resources();

         guiglobs::apheresis_status_line->set_current_text (textStatAasRemoveDisp);

         _bmpGraphic.set_id(_bmpIdRemove);
         _bmpGraphic.allocate_resources(*this);
         _txtSealDetach.allocate_resources(*this);
         _txtSealDetach.set_color(GREY_LIGHT);

         _txtRemoveSet.allocate_resources(*this);
      }
      break;

      default :
         DataLog (log_level_gui_info) << "Transition " << newState << " makes no sense." << endmsg;
         break;
   }
}

/////////////////////////////////////////////////
// Take an END_MSS_STATE and return it as a string for outputting.
const char* Screen_MSS_END::printState (END_MSS_STATE state)
{
   switch (state)
   {
      case END_MSS_SCREEN_STATE_NULL :
         return "END_MSS_SCREEN_STATE_NULL";
         break;
      case END_MSS_SCREEN_STATE_SEAL_DETACH :
         return "END_MSS_SCREEN_STATE_SEAL_DETACH";
         break;
      case END_MSS_SCREEN_STATE_REMOVE :
         return "END_MSS_SCREEN_STATE_REMOVE";
         break;
      default :
         return "UNDEFINED STUFF";
         break;
   }
}

void Screen_MSS_END::confirmUnload ()
{
   DataLog(log_level_gui_info) << "Screen_MSSBEGIN::unloadConfirmed ()" << endmsg;

   MSSConfirmUnloadMsg _confirmUnload(MessageBase::SEND_GLOBAL);
   _confirmUnload.send(0);

   DataLog(log_level_gui_info) << "MSSConfirmUnloadMsg Sent " << endmsg;
}

void Screen_MSS_END::procSaysCassetteIsRaised ()
{
   DataLog (log_level_gui_info) << "Proc says the cassette is raised." << endmsg;
   btn_continue.allocate_resources(*this);
}


void Screen_MSS_END::selectGraphics ()
{
   bool isWhiteStamp = guiglobs::cassette_mgr->currentSetIsWhiteStamp();
   bool isPas        = guiglobs::cassette_mgr->currentSetIsPas();
   bool isRas        = guiglobs::cassette_mgr->currentSetIsRas();

   selectGraphics(isWhiteStamp, isPas, isRas);
}

void Screen_MSS_END::selectGraphics (bool isWhiteStamp, bool isPas, bool isRas)
{
   _bmpIdSeal   = BITMAP_NULL;
   _bmpIdRemove = BITMAP_NULL;

   if (isWhiteStamp)
   {
      if (isPas && isRas)
      {
         _bmpIdSeal   = BITMAP_AAS_SEAL_WHITE_BOTH;
         _bmpIdRemove = BITMAP_AAS_REMOVE_WHITE_BOTH;
      }

      if ((!isPas) && isRas)
      {
         _bmpIdSeal   = BITMAP_AAS_SEAL_WHITE_RAS;
         _bmpIdRemove = BITMAP_AAS_REMOVE_WHITE_RAS;
      }

      if (isPas && (!isRas))
      {
         _bmpIdSeal   = BITMAP_AAS_SEAL_WHITE_PAS;
         _bmpIdRemove = BITMAP_AAS_REMOVE_WHITE_PAS;
      }
   }
   else
   {
      _bmpIdSeal   = BITMAP_AAS_SEAL_BLACK;
      _bmpIdRemove = BITMAP_AAS_REMOVE_BLACK;

   }

   // If the graphics didn't get set, error.
   if ((_bmpIdSeal == BITMAP_NULL) ||
       (_bmpIdRemove == BITMAP_NULL))
   {
      DataLog (log_level_gui_error) << "ERROR: No bitmap that works with current set." << endmsg;
      anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
      alarm.setAlarm();
   }
}

void Screen_MSS_END::initialize_status_line ()
{
   // Set the status line to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (MAGENTA);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty);
}

#if (CPU==SIMNT)
void Screen_MSS_END::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      MSS_END_DISPLAY_TYPE mssEndType      = (MSS_END_DISPLAY_TYPE)traverseInfo.parameters[0];

      END_MSS_STATE        transitionState = END_MSS_SCREEN_STATE_SEAL_DETACH;
      bool                 isPas           = true;
      bool                 isRas           = true;
      bool                 isWhite         = true;

      DataLog (log_level_sim_gui_side_info) << "Recieved SIMNT screen change msg with change value " << mssEndType << "." << endmsg;

      switch (mssEndType)
      {
         case MSS_END_WHITE_RAS_SEAL : isPas              = false; break;
         case MSS_END_WHITE_RAS_REMOVE : isPas            = false; transitionState = END_MSS_SCREEN_STATE_REMOVE; break;
         case MSS_END_WHITE_PAS_SEAL : isRas              = false; break;
         case MSS_END_WHITE_PAS_REMOVE : isRas            = false; transitionState = END_MSS_SCREEN_STATE_REMOVE; break;
         case MSS_END_WHITE_BOTH_SEAL : /* do nothing */ break;
         case MSS_END_WHITE_BOTH_REMOVE : transitionState = END_MSS_SCREEN_STATE_REMOVE; break;
         case MSS_END_BLACK_SEAL : isPas                  = false; isWhite = false; break;
         case MSS_END_BLACK_REMOVE : isPas                = false; isWhite = false; transitionState = END_MSS_SCREEN_STATE_REMOVE; break;
         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown MSS End Display Type: " << mssEndType << endmsg;
            return;
      }

      selectGraphics(isWhite, isPas, isRas);
      transitionGraphicsByScreenState(transitionState);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH 367bd20e041f98aebbae48f023e2d0df */
