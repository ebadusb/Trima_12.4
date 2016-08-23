#include "SoftOptUtility.h"

const int GROUP_COUNT = 5;

SoftOptUtility::SoftOptUtility(void)
{}


SoftOptUtility::~SoftOptUtility(void)
{}

void SoftOptUtility::InitializeKeyMap (map<GUI_BUTTON_ID, TEXT_STRING_ID>& keyMap)
{
   // numbers
   keyMap[GUI_BUTTON_KEYBOARD_0]   = textButtonKeyboard0;
   keyMap[GUI_BUTTON_KEYBOARD_1]   = textButtonKeyboard1;
   keyMap[GUI_BUTTON_KEYBOARD_2]   = textButtonKeyboard2;
   keyMap[GUI_BUTTON_KEYBOARD_3]   = textButtonKeyboard3;
   keyMap[GUI_BUTTON_KEYBOARD_4]   = textButtonKeyboard4;
   keyMap[GUI_BUTTON_KEYBOARD_5]   = textButtonKeyboard5;
   keyMap[GUI_BUTTON_KEYBOARD_6]   = textButtonKeyboard6;
   keyMap[GUI_BUTTON_KEYBOARD_7]   = textButtonKeyboard7;
   keyMap[GUI_BUTTON_KEYBOARD_8]   = textButtonKeyboard8;
   keyMap[GUI_BUTTON_KEYBOARD_9]   = textButtonKeyboard9;
   keyMap[GUI_BUTTON_KEYBOARD_A]   = textButtonKeyboardA;
   keyMap[GUI_BUTTON_KEYBOARD_B]   = textButtonKeyboardB;
   keyMap[GUI_BUTTON_KEYBOARD_C]   = textButtonKeyboardC;
   keyMap[GUI_BUTTON_KEYBOARD_D]   = textButtonKeyboardD;
   keyMap[GUI_BUTTON_KEYBOARD_E]   = textButtonKeyboardE;
   keyMap[GUI_BUTTON_KEYBOARD_F]   = textButtonKeyboardF;
   keyMap[GUI_BUTTON_KEYBOARD_G]   = textButtonKeyboardG;
   keyMap[GUI_BUTTON_KEYBOARD_H]   = textButtonKeyboardH;
   keyMap[GUI_BUTTON_KEYBOARD_I]   = textButtonKeyboardI;
   keyMap[GUI_BUTTON_KEYBOARD_J]   = textButtonKeyboardJ;
   keyMap[GUI_BUTTON_KEYBOARD_K]   = textButtonKeyboardK;
   keyMap[GUI_BUTTON_KEYBOARD_L]   = textButtonKeyboardL;
   keyMap[GUI_BUTTON_KEYBOARD_M]   = textButtonKeyboardM;
   keyMap[GUI_BUTTON_KEYBOARD_N]   = textButtonKeyboardN;
   keyMap[GUI_BUTTON_KEYBOARD_O]   = textButtonKeyboardO;
   keyMap[GUI_BUTTON_KEYBOARD_P]   = textButtonKeyboardP;
   keyMap[GUI_BUTTON_KEYBOARD_Q]   = textButtonKeyboardQ;
   keyMap[GUI_BUTTON_KEYBOARD_R]   = textButtonKeyboardR;
   keyMap[GUI_BUTTON_KEYBOARD_S]   = textButtonKeyboardS;
   keyMap[GUI_BUTTON_KEYBOARD_T]   = textButtonKeyboardT;
   keyMap[GUI_BUTTON_KEYBOARD_U]   = textButtonKeyboardU;
   keyMap[GUI_BUTTON_KEYBOARD_V]   = textButtonKeyboardV;
   keyMap[GUI_BUTTON_KEYBOARD_W]   = textButtonKeyboardW;
   keyMap[GUI_BUTTON_KEYBOARD_X]   = textButtonKeyboardX;
   keyMap[GUI_BUTTON_KEYBOARD_Y]   = textButtonKeyboardY;
   keyMap[GUI_BUTTON_KEYBOARD_Z]   = textButtonKeyboardZ;
   keyMap[GUI_BUTTON_KEYBOARD_CLR] = textButtonKeyboardCLR;
}

void SoftOptUtility::FormatDisplay (guistring& source, guistring& out)
{
   for ( int i = 0, group = 0 ; i < source.length(); i++, group++ )
   {
      if ( group == GROUP_COUNT )
      {
         out  += " - ";
         group = 0;
      }
      out += source.getBufPtr()[i];
   }
}

void SoftOptUtility::FormatStringDisplay (string& source, string& out)
{
   for ( int i = 0, group = 0 ; i < source.length(); i++, group++ )
   {
      if ( group == GROUP_COUNT )
      {
         out  += " - ";
         group = 0;
      }
      out += source.at(i);
   }
}

void SoftOptUtility::GetString (Display_Text& source, string& out)
{
   char buffer[source.get_text_length()];
   source.get_text_direct(buffer);
   out = buffer;
}

void SoftOptUtility::GetString (Button_Bitmap& source, string& out)
{
   guistring str; str.initialize();
   source.get_text_direct(str);
   out = string(str.getBufPtr());
}

/* FORMAT HASH d1dd7bf97d295d2379f0e433e80cda23 */
