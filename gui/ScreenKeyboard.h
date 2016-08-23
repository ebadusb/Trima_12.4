#ifndef _SCREEN_KEYBOARD_H_
#define _SCREEN_KEYBOARD_H_

#include <list>
#include <map>
#include "bwindow.hpp"
#include "button_bitmap.h"
#include "scan_structure.h"
#include "message.h"

using namespace std;

class ScreenKeyboard
   : public Base_Window
{
private:
   /*type definitions*/
   typedef void (ScreenKeyboard::* ButtonHandler)(string&, Button_Bitmap*);
   typedef map<GUI_BUTTON_ID, ButtonHandler>::iterator HIter;

   Display_Text m_text;
   list<Button_Bitmap*>               keys;
   map<GUI_BUTTON_ID, TEXT_STRING_ID> keyMap;
   string                 strData;
   /*pointer to the single instance of this class */
   static ScreenKeyboard* instance;
public:
   ScreenKeyboard();
   virtual ~ScreenKeyboard(void);

   virtual void allocate_resources (OSWindow& parent, const char* format_string);
   virtual void allocate_resources (OSWindow& parent);
   virtual void deallocate_resources ();

   void Initialize (int x, int y);

   /*accessor to the single instance of this class*/
   static ScreenKeyboard* Instance ();
   static void            Cleanup ();
   static bool            IsShown ();
protected:
   map<GUI_BUTTON_ID, ButtonHandler> handlers;
   void InitializeRowKeys (GUI_BUTTON_ID startButtonID, GUI_BUTTON_ID endButtonID,
                           int startX, int startY, BITMAP_ID upBitmap = BITMAP_KEYBOARD_KEYUP, BITMAP_ID dnBitmap = BITMAP_KEYBOARD_KEYDN);
   static void ButtonCallback (void* data);

   void UpdateView ();
   void MainHandler (Button_Bitmap* button);
   void BackHandler (string& txt, Button_Bitmap* button);
   void ClearHandler (string& txt, Button_Bitmap* button);
   void EnterHandler (string& txt, Button_Bitmap* button);
   void AlphaHandler (string& txt, Button_Bitmap* button);
   void HandleBarcodeMsg ();
   Message<BADGE_BARCODE_MSG> _msgBarcode;
};

#endif

/* FORMAT HASH d7ccad23aec38f2f7f19f0866176a9af */
