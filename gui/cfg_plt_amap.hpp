/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2010 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PLT_AMAP

*     File name:   cfg_plt_amap.hpp

*     Description: See prototype header descriptions below...

*************************< FOCUSSED GUI DOMAIN >****************************/


// Single include define guardian
#ifndef SYSCONFIG_AMAP_PLATELET_HPP
#define SYSCONFIG_AMAP_PLATELET_HPP


// CLASS INCLUDE


#include "screen_config.hpp"
#include "cfg_butt.hpp"
#include "button_bitmap.h"

static const int MAX_AMAP_ROWS = 3;

//
///////////////////////////////////////////////////////////////////
//

class Screen_PLT_AMAP
   : public Screen_Config
{

public:

   typedef enum
   {
      INACTIVE,
      MINYIELD,
      MAXYIELD,
      FINAL_CONC,
      YSF,
      PLASMA_PERCENT,
   } BUTTON_TYPE;

   typedef struct
   {
      int              xPos;
      int              yPos;
      void*            btnPtr;
      Button_Bitmap*   btnBmpPtr;
      Screen_PLT_AMAP* parentScreenPtr;
      BUTTON_TYPE      btnType;
      int              rowNum;
   } BTN_INFO;

   static bool _btnLock;

protected:
   bool        _pasOn;
   int         _topProduct, _callbackProduct;
   BUTTON_TYPE _callbackBtnType;

   // The display text objects
   Display_Text
      _txtMinYieldLbl,
      _txtMaxYieldLbl,
      _txtFinalConcLbl,
      _txtCollectConcLbl,
      _txtPlasmaPercentLbl,
      _txtPASRangeLbl,
      _toThe3rdCollectConc,
      _toThe3rdFinalConc,
      _toThe11thMinYield,
      _toThe11thMaxYield,
      _txtYsfLbl,
      _txtDonorsAllowedLbl,
   * _productIndicatorPtr;


   ConfigButton
      _btnYsf,
      _btnDonorsAllowed;

   // This is the set of
   struct PRODUCTS
   {
      // The two string IDs are probably the same text with different styling records.
      TEXT_STRING_ID   productIndicatorString;
      TEXT_STRING_ID   productLblString;
      CONFIG_DATA_KEYS keyMaxYield;
      CONFIG_DATA_KEYS keyMinYield;
      CONFIG_DATA_KEYS keyAmapConc;
      CONFIG_DATA_KEYS keyPasToggle;
      CONFIG_DATA_KEYS keyPasPlasmaPercent;
      float            finalConc;
   } _products[NUM_YIELD_VOLUME];

   struct GRAPHICAL_ROW
   {
      // The buttons
      BTN_INFO
         btnProdLabel,
         btnMaxYield,
         btnMinYield,
         btnFinalConc;

      // The text labels
      Display_Text
      * txtCollectConc,
      * txtMinPasVol,
      * txtPasSep,
      * txtMaxPasVol;

   } _graphicalRows[MAX_AMAP_ROWS];

   struct GRAPHICAL_PAS_ROW
   {
      // The buttons
      BTN_INFO
         btnProdLabel,
         btnPasPlasmaPercent,
         btnFinalConc;

      // The bitmaps
      Bitmap
      * bmpMagGlass;

      // The text labels
      Display_Text
      * txtMinYield,
      * txtMaxYield,
      * txtCollectConc,
      * txtMinPasVol,
      * txtMaxPasVol;

   } _graphicalPasRows[MAX_AMAP_ROWS];

   // ALLOCATE_RESOURCES
   // This method allocates any btnBmpPtr resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any btnBmpPtr resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // LABEL THE BUTTONS
   // This method calls labelButtonRow() for each rowNum, which updates
   // all of the rows to make sure they are up-to-date with changes.
   virtual void labelAllButtons ();
   virtual void labelButtonRow (const int rowNum, const int product_num, float& collect_conc, float& final_conc);

   // DISPLAY ALERT
   // Make sure that the concentration is in range, otherwise display an alert.
   virtual void displayAlert (float collect_conc, float final_conc);

   void initRowInfo ();
   void initProcedureInfo ();

   void checkForTimeOnlyConfig ();

public:


   // CONSTRUCTOR
   Screen_PLT_AMAP();


   // DESTRUCTOR
   virtual ~Screen_PLT_AMAP();

   //
   //   Methods invoked by callbacks (public so they can be used by non-class
   //   callback functions)
   virtual void processYsfButton ();
   virtual void processMaleOnlyPltBtn ();

   virtual void  update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void  update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);
   virtual void  setRowIndicator (const int rowNum);
   virtual void  awaitingCallback (const BUTTON_TYPE type, const int rowNum);
   virtual float getPercentCoMultiplier (const int rowNum);

   static float getMinAmapYield (const int row);
   static float getMaxAmapYield (const int row);



};
// End of single include define guardian
#endif

//
///////////////////////////////////////////////////////////////////
// end of cfg_platelet.hpp file

/* FORMAT HASH 918d46dc9431711df3b6f8d1f15231e8 */
