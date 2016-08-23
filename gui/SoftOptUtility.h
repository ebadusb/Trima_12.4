/*
   This is the utility class to support Screen_CONFIG_SOFTOPT class
*/

#ifndef _SOFT_OPT_UTLITY_
#define _SOFT_OPT_UTLITY_

#include <map>
#include <string>
#include "string_id.h"
#include "gui.h"
#include "guistring.h"
#include "display_text.h"
#include "button_bitmap.h"

using namespace std;

class SoftOptUtility
{
public:
   SoftOptUtility(void);
   ~SoftOptUtility(void);

   static void InitializeKeyMap (map<GUI_BUTTON_ID, TEXT_STRING_ID>& keyMap);
   static void FormatDisplay (guistring& source, guistring& out);
   static void FormatStringDisplay (string& source, string& out);
   static void GetString (Display_Text& source, string& out);
   static void GetString (Button_Bitmap& source, string& out);
};

#endif

/* FORMAT HASH 0841d7ae38ae9f5fd263376b826f54aa */
