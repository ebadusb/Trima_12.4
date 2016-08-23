#ifndef SYSCONFIG_SOFTOPT_HPP
#define SYSCONFIG_SOFTOPT_HPP

#include <map>
#include <sstream>
#include "screen_config.hpp"
#include "cfg_butt.hpp"
#include "button_bitmap.h"
#include "guistring.h"
#include "listview.hpp"
#include "trimamessages.h"

class ScreenKeyboard;

const int MAX_ITEM = 9;

class Screen_CONFIG_SOFTOPT
   : public Screen_Config
{
protected:

   typedef void (Screen_CONFIG_SOFTOPT::* ButtonHandler)(void*);
   typedef map<GUI_BUTTON_ID, ButtonHandler>::iterator BIter;
   typedef void (Screen_CONFIG_SOFTOPT::* CallbackMethod)(void*);

   map<GUI_BUTTON_ID, ButtonHandler> m_handlers;
   Bitmap m_confirmBg;
   Bitmap m_listBg;

   // keyboard
   ScreenKeyboard* m_keyboard;
   ListView        m_listView;
   // buttons
   Button_Bitmap   m_kbButton;
   bool            m_kbAllowed; // true when kb is enabled
   // text
   Display_Text    m_serialNumberLabel;
   Display_Text    m_serialNumber;
   Display_Text    m_customerLabel;
   Display_Text    m_customer;
   Display_Text    m_siteLabel;
   Display_Text    m_site;
   Display_Text    m_countryLabel;
   Display_Text    m_country;
   Display_Text    m_confirmationCodeLabel;
   Display_Text    m_confirmationCode;
   Message<bool>   m_confirmationCodeReturn;
   ///this is temporary
   // list<Feature*> m_features;
   std::stringstream m_ipAddress;

public:

   static Screen_CONFIG_SOFTOPT* getInstancePtr ();

protected:
   virtual void allocate_resources (const char* allocation_parameter);

   virtual void deallocate_resources ();

   void        AllocateChildren ();
   void        Initialize ();
   void        InitializeHandlers ();
   void        EnterConfirmCode (void* data);
   void        DoneEnterConfirmCodeHandler ();
   void        UpdateView ();
   static void ButtonCallback (void* data);
   static Screen_CONFIG_SOFTOPT* _instance;

   FeatureChanged                m_featureChanged;
   bool m_compPls;
   bool m_extraWBCMsg;

   void FeatureChangedNotification (void* data);

   void InitializeFeatureList (bool firstTime = false);

public:
   Screen_CONFIG_SOFTOPT();

   virtual ~Screen_CONFIG_SOFTOPT();

   void SetSerialNumber (const string& serialNumber);
   void SetCustomer (const string& customer);
   void SetSite (const string& site);
   void SetCountry (const string& country);
   void SetConfirmationCode (const string& confirmationCode);
};

#endif

/* FORMAT HASH 615916f251b75e8b3a4c0ecf4b51bbf5 */
