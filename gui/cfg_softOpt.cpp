#include <vxworks.h>
#include "cfg_softOpt.hpp"
#include "ScreenKeyboard.h"
#include "SoftOptUtility.h"
#include "feature_string_util.h"
#include "callback.h"
#include "guiglobs.hpp"

#include "env_names.h"
/*
these are the definition of the the keyboard origin
*/
const int ORIG_X               = 20;
const int ORIG_Y               = 110;

const int SCROLL_BUTTON_HEIGHT = 29;
const int SCROLL_BUTTON_WIDTH  = 36;
const int LIST_ITEM_HEIGHT     = 31;
const int LIST_HEIGHT          = 235;
const int LIST_WIDTH           = 280;
const int TEXT_FIELD_WIDTH     = 280;
const int RIGHT_COL_MARGIN     = 10;
const int TEXT_VALUE_MARGIN    = 20;
const int LABEL_HEIGHT         = 20;
const int TEXT_VALUE_HEIGHT    = 25;
const int KB_BUTTON_OFFSET     = 90;

// const int X1 = ORIG_X + SCROLL_BUTTON_WIDTH + 6;
// const int Y1 = ORIG_Y + 9;

const int X2 = ORIG_X + SCROLL_BUTTON_WIDTH + LIST_WIDTH + RIGHT_COL_MARGIN;
const int Y2 = ORIG_Y + 5 * LABEL_HEIGHT + 4 * TEXT_VALUE_HEIGHT;

const int Y3 = Y2 + (2 * TEXT_VALUE_HEIGHT + 10); // 10, adjusted //100 x 48


Screen_CONFIG_SOFTOPT* Screen_CONFIG_SOFTOPT::_instance = NULL;
///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CONFIG_SOFTOPT::Screen_CONFIG_SOFTOPT()
   : Screen_Config (TEAL, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, textCfgScreenLabelSoftOpt),
     m_confirmBg   (X2, Y2, BITMAP_BG_CONFIG_SOFTOPT_CONFIRM_CODE),
     m_listBg      (ORIG_X, ORIG_Y, BITMAP_BG_CONFIG_SOFTOPT_LIST),
     m_keyboard    (NULL),
     m_listView    (MAX_ITEM),
     m_kbButton    (GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD,
                    X2 + KB_BUTTON_OFFSET,
                    Y3,
                    textEmpty,
                    BITMAP_BUTTON_CONFIG_SOFTOPT_KEYBOARD,
                    BITMAP_BUTTON_CONFIG_SOFTOPT_KEYBOARD_DN,
                    true, false),
     m_kbAllowed   (false),

     m_serialNumberLabel     (textCfgSoftOptSerialNumberLabel),
     m_serialNumber          (textCfgSoftOptSerialNumberFormat),
     m_customerLabel         (textCfgSoftOptCustomerLabel),
     m_customer              (textCfgSoftOptCustomerFormat),
     m_siteLabel             (textCfgSoftOptSiteLabel),
     m_site                  (textCfgSoftOptSiteFormat),
     m_countryLabel          (textCfgSoftOptCountryLabel),
     m_country               (textCfgSoftOptCountryFormat),
     m_confirmationCodeLabel (textCfgSoftOptConfirmationCodeLabel),
     m_confirmationCode      (textCfgSoftOptConfirmationCodeFormat),

     m_featureChanged  (Callback<Screen_CONFIG_SOFTOPT>(this,
                                                        &Screen_CONFIG_SOFTOPT::FeatureChangedNotification),
                        MessageBase::SNDRCV_LOCAL),
     m_compPls (false),
     m_extraWBCMsg (false)
{

   Initialize();

   _instance = this;
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_SOFTOPT::~Screen_CONFIG_SOFTOPT()
{}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_SOFTOPT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   AllocateChildren();

}

//////////////////////////////////////////////
// This method is added to allow ease of unittest
void Screen_CONFIG_SOFTOPT::AllocateChildren ()
{
   m_listBg.allocate_resources(*this);

   m_listView.allocate_resources(*this);

   if (m_kbAllowed)
   {
      m_kbButton.allocate_resources(*this);

      m_kbButton.set_callback(ButtonCallback, &m_kbButton);
   }

   m_serialNumberLabel     .allocate_resources(*this);
   m_serialNumber          .allocate_resources(*this);
   m_customerLabel         .allocate_resources(*this);
   m_customer              .allocate_resources(*this);
   m_siteLabel             .allocate_resources(*this);
   m_site                  .allocate_resources(*this);
   m_countryLabel          .allocate_resources(*this);
   m_country               .allocate_resources(*this);
   m_confirmationCodeLabel .allocate_resources(*this);
   m_confirmationCode      .allocate_resources(*this);

   m_confirmBg.allocate_resources(*this);

}

//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_SOFTOPT::deallocate_resources ()
{
   // deactivate the status line before leaving
   // uiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();

   m_listView.deallocate_resources();

   if (m_kbAllowed)
   {
      m_kbButton.deallocate_resources();
   }

   m_serialNumberLabel     .deallocate_resources();
   m_serialNumber          .deallocate_resources();
   m_customerLabel         .deallocate_resources();
   m_customer              .deallocate_resources();
   m_siteLabel             .deallocate_resources();
   m_site                  .deallocate_resources();
   m_countryLabel          .deallocate_resources();
   m_country               .deallocate_resources();
   m_confirmationCodeLabel .deallocate_resources();
   m_confirmationCode      .deallocate_resources();

   m_confirmBg.deallocate_resources();
   m_listBg.deallocate_resources();
}

/*
This is the handler that is attached to the button_bitmap objects
*/
void Screen_CONFIG_SOFTOPT::ButtonCallback (void* data)
{
   Button_Bitmap* button = (Button_Bitmap*)(data);
   if ( _instance == NULL )
      return;

   BIter iter = _instance->m_handlers.find(button->get_button_id());
   if ( iter == _instance->m_handlers.end())
      return;
   Screen_CONFIG_SOFTOPT::ButtonHandler ht = (*iter).second;
   (_instance->*ht)(NULL);
}

void Screen_CONFIG_SOFTOPT::Initialize ()
{
   InitializeHandlers();

   m_listView.Initialize(ORIG_X, ORIG_Y);
   m_confirmationCodeReturn.init(Callback<Screen_CONFIG_SOFTOPT>(this, &Screen_CONFIG_SOFTOPT::DoneEnterConfirmCodeHandler), MessageBase::SNDRCV_RECEIVE_ONLY);

   InitializeFeatureList(true);

   SetSerialNumber(getenv(MachineEnumName));
   SetSite(Software_CDS::GetInstance().getSiteName());
   SetCustomer(Software_CDS::GetInstance().getCustomerName());
   SetCountry(Software_CDS::GetInstance().getCountryName());


   // If no machine id info set (i.e. no machine.id file)
   if (Software_CDS::GetInstance().getSiteId() == -1 &&
       Software_CDS::GetInstance().getCountryId() == -1 &&
       Software_CDS::GetInstance().getCustomerId() == -1)
   {
      m_kbAllowed = false;
   }
   else
   {
      m_kbAllowed = true;
   }

#if CPU==SIMNT
   SetSite(string("Lakewood Campus"));
   SetCustomer(string("Terumo BCT"));
   SetCountry(string("United States"));
   SetCountry(string("United States"));

   m_kbAllowed = true;
#endif

   /////////////////////////////////////////////////////////////////////
}

typedef struct ip_address
{
   unsigned char b0;
   unsigned char b1;
   unsigned char b2;
   unsigned char b3;
} ip_address;

typedef union
{
   uint32_t   i;
   ip_address ip;
} ip_translate;

void Screen_CONFIG_SOFTOPT::InitializeFeatureList (bool firstTime)
{
   list<TEXT_STRING_ID> items;
   DataLog(log_level_gui_info) << "Populating feature list" << endmsg;

   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      if (Software_CDS::GetInstance().getFeature((FEATURE_ID)i))
      {
         DataLog(log_level_gui_info) << "Feature: " << i << " "
                                     << Software_CDS::GetInstance().getFeatureConfigFileLabel((FEATURE_ID)i) << " =on" << endmsg;

         items.push_back(convertFeatureIdToStringId((FEATURE_ID)i));

         if (firstTime)
         {
            if (i == ComplementaryPlasmaAvailable)
            {
               m_compPls = true;
            }
            else if (i == AllowExtraWbcMessages)
            {
               m_extraWBCMsg = true;
            }
         }
         else
         {
            if (i == ComplementaryPlasmaAvailable)
            {
               if (!m_compPls)
               {
                  guiglobs::cdsData.rwConfig.predict.key_complementary_plasma = false;
                  DataLog(log_level_gui_info) << "key_complementary_plasma is turned off " << endmsg;

                  m_compPls = true;
               }
            }
            else if (i == AllowExtraWbcMessages)
            {
               if (!m_extraWBCMsg)
               {
                  guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags = true;
                  DataLog(log_level_gui_info) << "AllowExtraWbcMessages is turned on " << endmsg;

                  m_extraWBCMsg = true;
               }
            }
         }
      }
      else
      {
         if (i == ComplementaryPlasmaAvailable)
         {
            m_compPls = false;
         }
         else if (i == AllowExtraWbcMessages)
         {
            m_extraWBCMsg = false;
         }
      }
   }

   /*for (int i = 0; i < VALUE_FEATURE_DEF_END; i++)
   {
      printf("Value Feature: %d (%s)\n", i, Software_CDS::GetInstance().getValueFeatureConfigFileLabel((VALUE_FEATURE_ID)i));
      uint32_t value = Software_CDS::GetInstance().getValueFeature((VALUE_FEATURE_ID)i);
      printf("Value Feature: %d (%s) = %u\n", i, Software_CDS::GetInstance().getValueFeatureConfigFileLabel((VALUE_FEATURE_ID)i), value);

      const char* dataItem = convertValueFeatureIdToDataItem((VALUE_FEATURE_ID)i).c_str();

      if (i == VistaIP)
      {
         ip_translate val;
         val.i = Software_CDS::GetInstance().getValueFeature((VALUE_FEATURE_ID)i);

         m_ipAddress.str("");
         m_ipAddress << (int)val.ip.b3 << "." << (int)val.ip.b2 << "." << (int)val.ip.b1 << "." << (int)val.ip.b0;
         printf("Value Feature: %d (%s) = %s\n", i, Software_CDS::GetInstance().getValueFeatureConfigFileLabel((VALUE_FEATURE_ID)i), m_ipAddress.str().c_str());

         if (!guiglobs::setTextDataItem(dataItem,
                                        m_ipAddress.str().c_str()))
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Unable to access feature data: ");
         }
      }
      else
      {
         int val = Software_CDS::GetInstance().getValueFeature((VALUE_FEATURE_ID)i);

         if (!guiglobs::setIntDataItem(dataItem,
                                       val))
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Unable to access feature data: ");
         }
      }

      items.push_back(convertValueFeatureIdToStringId((VALUE_FEATURE_ID)i));

   }*/

   m_listView.Initialize(items);

   SetConfirmationCode(Software_CDS::GetInstance().getShortConfirmationCode());
}

void Screen_CONFIG_SOFTOPT::InitializeHandlers ()
{
   m_handlers[GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD] = &Screen_CONFIG_SOFTOPT::EnterConfirmCode;

}

///////////////////////////////////////////////////////
// This is the method that handles the button click on
// entering confimation code
void Screen_CONFIG_SOFTOPT::EnterConfirmCode (void* data)
{
   m_keyboard =  ScreenKeyboard::Instance();
   if ( m_keyboard != NULL )
   {
      m_keyboard->Initialize(ORIG_X, ORIG_Y);
      m_keyboard->allocate_resources(*this);
   }

   btn_continue.deallocate_resources();

   guistring str;
   str.initialize();
   goto_child_screen(GUI_SCREEN_KEYBOARD, str);
}


void Screen_CONFIG_SOFTOPT::SetSerialNumber (const string& serialNumber)
{
   m_serialNumber.set_text_direct(serialNumber.length(), serialNumber.c_str());
}

void Screen_CONFIG_SOFTOPT::SetConfirmationCode (const string& confirmationCode)
{
   m_confirmationCode.set_text_direct(confirmationCode.length(), confirmationCode.c_str());
}

void Screen_CONFIG_SOFTOPT::SetCustomer (const string& customer)
{
   m_customer.set_text_direct(customer.length(), customer.c_str());
}

void Screen_CONFIG_SOFTOPT::SetSite (const string& site)
{
   m_site.set_text_direct(site.length(), site.c_str());
}

void Screen_CONFIG_SOFTOPT::SetCountry (const string& country)
{
   m_country.set_text_direct(country.length(), country.c_str());
}


void Screen_CONFIG_SOFTOPT::FeatureChangedNotification (void* data)
{
   InitializeFeatureList();

}

void Screen_CONFIG_SOFTOPT::DoneEnterConfirmCodeHandler ()
{
   btn_continue.allocate_resources(*this);
}

Screen_CONFIG_SOFTOPT* Screen_CONFIG_SOFTOPT::getInstancePtr ()
{
   return _instance;
}

/* FORMAT HASH f85eb84184f01013e1bfca227ee06202 */
