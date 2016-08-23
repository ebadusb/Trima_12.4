#include <string.h>
#include "ScreenKeyboard.h"
#include "string_id.h"
#include "SoftOptUtility.h"
#include "guistring.h"
#include "callback.h"
#include "trimamessages.h"
#include "software_cds.h"
#include "statline.hpp"
#include "guiglobs.hpp"
#include "env_names.h"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "cfg_softopt.hpp"


/*static initializer*/
ScreenKeyboard* ScreenKeyboard::instance = NULL;

const int       X_INT                    = 54;
const int       Y_INT                    = 43;
const int       X_MARGIN                 = 10;
const int       Y_MARGIN                 = 10;
const int       TXT_HGT                  = 85;
const int       TXT_WDT                  = 450;

ScreenKeyboard::ScreenKeyboard()
   : m_text(textKeyboardEmptyFormatInit)
{
   Base_Window::set_screen_id(GUI_SCREEN_KEYBOARD);
   Base_Window::set_background_bitmap(BITMAP_KEYBOARD_BACKGROUND);

}  // END of Screen_CONFIG_SOFTOPT CONSTRUCTOR

ScreenKeyboard::~ScreenKeyboard(void)
{
   list<Button_Bitmap*>::iterator iter = keys.begin();
   for ( ; iter != keys.end() ; iter++)
      delete (*iter);
   instance = NULL;
}

ScreenKeyboard* ScreenKeyboard::Instance ()
{
   if ( instance == NULL )
      instance = new ScreenKeyboard();
   return instance;
}

void ScreenKeyboard::Cleanup ()
{
   if ( instance != NULL )
   {
      delete instance;
      instance = NULL;
   }
}

void ScreenKeyboard::Initialize (int x, int y)
{
   set_position(x, y);
   m_text.set_position(0, Y_MARGIN);

   SoftOptUtility::InitializeKeyMap(keyMap);

   int txtX = 2 * X_MARGIN;
   int txtY = Y_MARGIN;
   int kbX  = X_MARGIN;
   int kbY  = txtY + TXT_HGT; // + MARGIN / 2;

   InitializeRowKeys(GUI_BUTTON_KEYBOARD_0, GUI_BUTTON_KEYBOARD_9, kbX, kbY);
   InitializeRowKeys(GUI_BUTTON_KEYBOARD_A, GUI_BUTTON_KEYBOARD_J, kbX, kbY + Y_INT);
   InitializeRowKeys(GUI_BUTTON_KEYBOARD_K, GUI_BUTTON_KEYBOARD_T, kbX, kbY + 2 * Y_INT);
   InitializeRowKeys(GUI_BUTTON_KEYBOARD_U, GUI_BUTTON_KEYBOARD_Z, kbX + 2 * X_INT, kbY + 3 * Y_INT);
   InitializeRowKeys(GUI_BUTTON_KEYBOARD_CR, GUI_BUTTON_KEYBOARD_CR, kbX + 4 * X_INT, kbY + 4 * Y_INT, BITMAP_KEYBOARD_CRUP, BITMAP_KEYBOARD_CRDN);
   InitializeRowKeys(GUI_BUTTON_KEYBOARD_BK, GUI_BUTTON_KEYBOARD_BK, kbX + 9 * X_INT, txtY, BITMAP_KEYBOARD_BKUP, BITMAP_KEYBOARD_BKDN);
   InitializeRowKeys(GUI_BUTTON_KEYBOARD_CLR, GUI_BUTTON_KEYBOARD_CLR, kbX + 9 * X_INT, txtY + Y_INT);

   handlers[GUI_BUTTON_NULL_BUTTON]  = &ScreenKeyboard::AlphaHandler;
   handlers[GUI_BUTTON_KEYBOARD_BK]  = &ScreenKeyboard::BackHandler;
   handlers[GUI_BUTTON_KEYBOARD_CLR] = &ScreenKeyboard::ClearHandler;
   handlers[GUI_BUTTON_KEYBOARD_CR]  = &ScreenKeyboard::EnterHandler;
}

// ALLOCATE_RESOURCES
void ScreenKeyboard::allocate_resources (OSWindow& parent, const char* format_string)
{
   _msgBarcode.init(Callback < ScreenKeyboard >(this, &ScreenKeyboard::HandleBarcodeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   strData = string();

   // Attach the window to the parent
   list<Button_Bitmap*>::iterator iter = keys.begin();
   for ( ; iter != keys.end() ; iter++)
   {
      (*iter)->allocate_resources(*this);
      (*iter)->set_callback(ScreenKeyboard::ButtonCallback, (*iter));
   }

   m_text.allocate_resources(*this);

   attach(&parent);

   Base_Window::allocate_resources(format_string);

   // Parent the status bar onto this screen
   // guiglobs::apheresis_status_line->reparent_status_line (*this);
}

// ALLOCATE_RESOURCES
void ScreenKeyboard::allocate_resources (OSWindow& parent)
{
   _msgBarcode.init(Callback < ScreenKeyboard >(this, &ScreenKeyboard::HandleBarcodeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   strData = string();

   // Attach the window to the parent
   list<Button_Bitmap*>::iterator iter = keys.begin();
   for ( ; iter != keys.end() ; iter++)
   {
      (*iter)->allocate_resources(*this);
      (*iter)->set_callback(ScreenKeyboard::ButtonCallback, (*iter));
   }

   m_text.allocate_resources(*this);

   // Parent the status line onto this screen
   guiglobs::apheresis_status_line->reparent_status_line (*Screen_CONFIG_SOFTOPT::getInstancePtr());
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type_real(DEFAULT_STATUS_LINE, textCfgScreenLabelSoftOpt, NULL, 0);
   // attach(&parent);
}
// DEALLOCATE_RESOURCES
void ScreenKeyboard::deallocate_resources ()

{
   if (!_allocated)
      return;

   list<Button_Bitmap*>::iterator iter = keys.begin();
   for ( ; iter != keys.end() ; iter++)
      (*iter)->deallocate_resources();

   m_text.deallocate_resources();
   _msgBarcode.deregisterMsg();
   Base_Window::deallocate_resources();

   // Parent the status line onto the UR window
   guiglobs::apheresis_status_line->unparent_status_line (Screen_CONFIG_SOFTOPT::getInstancePtr()->get_screen_id());

}

/*
This initializes a row of keys given the buttons ids, location and bitmap up/dn ids
*/
void ScreenKeyboard::InitializeRowKeys (GUI_BUTTON_ID startButtonID, GUI_BUTTON_ID endButtonID,
                                        int startX, int startY, BITMAP_ID upBitmap, BITMAP_ID dnBitmap)
{
   TEXT_STRING_ID text = textEmpty;
   map<GUI_BUTTON_ID, TEXT_STRING_ID>::iterator iter;

   for ( int i = startButtonID, j = 0 ; i <= endButtonID ; i++, j++)
   {
      iter = keyMap.find((GUI_BUTTON_ID)i);
      if ( iter != keyMap.end() )
         text =  (*iter).second;
      else
         text = textEmpty;

      keys.push_back(new Button_Bitmap((GUI_BUTTON_ID)i, (j) * X_INT + startX, startY, text, upBitmap, dnBitmap));
   }
}

/*
This is the handler that is attached to the button_bitmap objects
*/
void ScreenKeyboard::ButtonCallback (void* data)
{
   Button_Bitmap*  button = (Button_Bitmap*)(data);
   ScreenKeyboard* kb     = ScreenKeyboard::Instance();
   kb->MainHandler(button);
}

/*
This is the main handler of the the keys
*/
void ScreenKeyboard::MainHandler (Button_Bitmap* data)
{
   // guiglobs::apheresis_status_line->set_current_text(textEmpty);
   HIter iter = handlers.find(data->get_button_id());
   if ( iter == handlers.end())
      iter = handlers.find(GUI_BUTTON_NULL_BUTTON);
   if ( iter == handlers.end())
      return;
   ScreenKeyboard::ButtonHandler ht = (*iter).second;
   guiglobs::apheresis_status_line->deactivate_status_line_type_real(ATTENTION_STATUS_LINE, NULL, 0); // set_current_status_line_text_by_type_real(DEFAULT_STATUS_LINE, textCfgScreenLabelSoftOpt,NULL, 0);
   (this->*ht)(strData, data);
   UpdateView();
}

void ScreenKeyboard::BackHandler (string& txt, Button_Bitmap* button)
{
   int length = txt.length() > 0 ? txt.length() - 1 : 0;
   txt = txt.substr(0, txt.length() - 1);
   // guiglobs::apheresis_status_line->set_current_text(textEmpty);
}

void ScreenKeyboard::ClearHandler (string& txt, Button_Bitmap* button)
{
   txt = string();
   // guiglobs::apheresis_status_line->set_current_text(textEmpty);
}

void ScreenKeyboard::EnterHandler (string& txt, Button_Bitmap* button)
{
   KEY_RESULT result = SUCCESS;
   if (!txt.empty())
   {
      result = Software_CDS::GetInstance().decodeKey(txt, Software_CDS::GetInstance().getCurrentAuthLevel());
   }


   if (result == SUCCESS)
   {
      this->deallocate_resources();

      Message<bool>                  doneMessage(MessageBase::SEND_LOCAL);
      doneMessage.send(true);
      RemoveMostRecentChildScreenMsg scrnmsg(MessageBase::SEND_LOCAL);
      scrnmsg.send();

   }
   else
   {
      string         logMsg;
      TEXT_STRING_ID id;
      if (result == FAILED_UNAUTHORIZED_USER)
      {
         logMsg = "Unauthorized User";
         id     = textSoftOptInvalidKeyUser;
      }
      else if (result == FAILED_UNAUTHORIZED_CUSTOMER)
      {
         logMsg = "Unauthorized Customer";
         id     = textSoftOptInvalidKeyCustomer;
      }
      else if (result == FAILED_UNAUTHORIZED_SERIAL_NUMBER)
      {
         logMsg = "Unauthorized Serial Number";
         id     = textSoftOptInvalidKeySerial;
      }
      else if (result == FAILED_UNAUTHORIZED_SITE)
      {
         logMsg = "Unauthorized Site";
         id     = textSoftOptInvalidKeySite;
      }
      else if (result == FAILED_UNAUTHORIZED_COUNTRY)
      {
         logMsg = "Unauthorized Country";
         id     = textSoftOptInvalidKeyCountry;
      }
      else
      {
         logMsg = "Invalid format";
         id     = textSoftOptInvalidKeyFormat;
      }
      DataLog(log_level_gui_info) << "Invalid key entered: " << strData << "; Reason:  " << logMsg << endmsg;

      guiglobs::apheresis_status_line->set_current_status_line_text_by_type_real(ATTENTION_STATUS_LINE, id, NULL, 0);
   }
}

void ScreenKeyboard::AlphaHandler (string& txt, Button_Bitmap* button)
{
   string tempData;
   SoftOptUtility::GetString(*button, tempData);
   txt += tempData;
}

void ScreenKeyboard::UpdateView ()
{
   string tempData, origData;
   SoftOptUtility::FormatStringDisplay(this->strData, tempData);
   m_text.set_text_direct(tempData.length(), tempData.c_str());
}

bool ScreenKeyboard::IsShown ()
{
   return Instance()->_allocated;
}

void ScreenKeyboard::HandleBarcodeMsg ()
{
   BADGE_BARCODE_MSG msg = _msgBarcode.getData();

   // Terminate the string
   msg.badgeString[msg.badgeLength] = 0;

   DataLog(log_level_gui_info) << "Software options key" << msg.badgeString << "(" << msg.badgeLength << " characters)" << endmsg;

   // Save badge string
   guistring printString;
   printString.initialize();

   sprintf(printString, msg.badgeString);
   printString.strip_unprintable_chars();

   strData += string(printString.getBufPtr());
   UpdateView();
}

/* FORMAT HASH f3ccd54da2ebf2f7cd6eb59dd7107751 */
