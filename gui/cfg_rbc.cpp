/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  cfg_rbc.cpp

*     File name:   Screen_CONFIG_RBC

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************


$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_rbc.cpp 1.99 2009/07/13 20:56:15Z dslausb Exp dslausb $
$Log: cfg_rbc.cpp $
Revision 1.99  2009/07/13 20:56:15Z  dslausb
IT 9333 - Fix swapped RBC button locations
Revision 1.98  2009/06/18 21:18:54Z  dslausb
Move and rename DRBC dose threshold
Revision 1.97  2009/05/20 19:33:34Z  dslausb
IT 9241 - RBC config not properly limiting RAS vol.
Revision 1.96  2009/04/24 22:46:31Z  dslausb
IT 9113 - RAS volume min changed to 80 for a single, 160 for a double.
Revision 1.95  2009/04/24 20:55:15Z  dslausb
RAS validation vars no longer needed
Revision 1.94  2009/04/10 16:40:29Z  jsylusb
Created a new CDS for software.dat settings.
Revision 1.93  2009/04/08 16:02:41Z  dslausb
Remove extra logging
Revision 1.92  2009/04/08 14:38:33Z  dslausb
IT 9061
Revision 1.91  2008/10/06 22:25:33Z  dslausb
Only one validated RAS volume
Revision 1.90  2008/08/26 22:50:38Z  dslausb
IT 8114 - Change RAS validated range behavior
Revision 1.89  2008/08/20 15:59:39Z  dslausb
IT 8770 - RAS volume warning ranges
Revision 1.88  2008/08/20 15:42:29Z  dslausb
IT 8770 - RAS storage limits changed
Revision 1.87  2008/07/14 20:26:21Z  dslausb
More stringId changes requested by localization
Revision 1.86  2008/07/09 16:29:37Z  dslausb
IT 8735 - Fix a procedure bug
Revision 1.85  2008/07/07 18:25:16Z  dslausb
More localization string id standardization
Revision 1.84  2008/07/03 20:16:58Z  dslausb
Further standardization of string IDs for localization
Revision 1.83  2008/07/03 15:01:58Z  mereusb
IT 8107 - Improved screenshot coverage.
Revision 1.82  2008/06/25 15:20:20  dslausb
Swapped two bitmaps that I mistakenly swapped before.
Revision 1.81  2008/06/25 19:00:42Z  dslausb
Combine RAS toggle with procedure label to buy more room for localization
Revision 1.80  2008/05/14 16:42:12Z  dslausb
Nothing much.
Revision 1.79  2008/05/14 16:27:33Z  dslausb
Added descriptive comments and removed unnecessary logging
Revision 1.78  2008/05/14 16:11:22Z  dslausb
IT 8088 - Fix problem with DRBC split configuration
Revision 1.77  2008/05/09 20:42:29Z  dslausb
IT 8088 - Lock down bad DRBC split configurations
Revision 1.76  2008/04/08 17:03:32Z  dslausb
IT 8451 - Implement Time-Only Config Mode
Revision 1.75  2008/02/19 23:41:52Z  dslausb
First batch of layout fixes for string.info files
Revision 1.74  2008/02/18 20:17:09Z  dslausb
Updates for string.info files
Revision 1.73  2008/01/23 17:26:56Z  dslausb
IT 8435 - VxWorks 5.5 Checkin
Revision 1.72  2007/12/07 20:45:59Z  dslausb
IT 8241
Revision 1.71  2007/10/31 16:20:29Z  dslausb
IT 8245 - Change RBC config screen so that it shows the PTF RSF offset, just like the procedure selection screen.
Revision 1.70  2007/10/25 16:08:46Z  dslausb
Rearrange RBC config screen to accomdate new order (see IT 8244)
Revision 1.69  2007/10/25 15:32:44Z  dslausb
IT 8244 - Swap RBC collect volume column with solution volume column.
Revision 1.68  2007/09/06 22:30:06Z  dslausb
IT 8198 - Fixed CFG RBC stat line warning glitch
Revision 1.67  2007/08/24 19:29:35Z  dslausb
- Fix Hct seen by predict when procedure is RAS.
- DRBC keypad only prepopulates when going from off to on.
Revision 1.66  2007/08/21 21:19:01Z  dslausb
Add logging
Revision 1.65  2007/08/21 18:54:49Z  dslausb
Changes to DRBC split config functionality
Revision 1.64  2007/08/14 18:18:03Z  dslausb
IT7986: Combined cassette bit for PTF and RAS into one bit
Revision 1.63  2007/08/13 17:00:42Z  dslausb
- Made DRBC split notification automatically on if ANY procedure is configured PTF.
- Removed references to obsolete config values
Revision 1.62  2007/08/10 22:24:48Z  dslausb
RBC config screen changes for DRBC auto split


*/

#include <vxworks.h>

/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "math.h"                // for rounding functions
#include "software_cds.h"
#include "scrncomm.hpp"
#include "statline.hpp"
#include "display_formatter.h"

#define RBC_BUTTON_LABEL_LINE(row) ((row==0) ? (GUI_BUTTON_CONFIG_RBC_LABEL_LINE_1) : ((row==1) ? (GUI_BUTTON_CONFIG_RBC_LABEL_LINE_2) : (GUI_BUTTON_CONFIG_RBC_LABEL_LINE_3)))
#define RBC_BUTTON_PROD(row) ((row==0) ? (GUI_BUTTON_CONFIG_RBC_PROD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_RBC_PROD_2) : (GUI_BUTTON_CONFIG_RBC_PROD_3)))
#define RBC_BUTTON_CRIT(row) ((row==0) ? (GUI_BUTTON_CONFIG_RBC_CRIT_1) : ((row==1) ? (GUI_BUTTON_CONFIG_RBC_CRIT_2) : (GUI_BUTTON_CONFIG_RBC_CRIT_3)))
#define RBC_BUTTON_DOSE(row) ((row==0) ? (GUI_BUTTON_CONFIG_RBC_DOSE_1) : ((row==1) ? (GUI_BUTTON_CONFIG_RBC_DOSE_2) : (GUI_BUTTON_CONFIG_RBC_DOSE_3)))
#define RBC_BUTTON_VOLUME(row) ((row==0) ? (GUI_BUTTON_CONFIG_RBC_VOLUME_1) : ((row==1) ? (GUI_BUTTON_CONFIG_RBC_VOLUME_2) : (GUI_BUTTON_CONFIG_RBC_VOLUME_3)))

static void cb_button (void* data);
static void cb_up_button (void* data);
static void cb_down_button (void* data);
static void cb_drbc_alert_button (void* data);
static void cb_rbc_ysf_button (void* data);
static void cb_toggle_button (void* data);

// BASE CLASS INCLUDE
#include "cfg_rbc.hpp"
int Screen_CONFIG_RBC::_topProductNum = 0;
int Screen_CONFIG_RBC::_maxProdVol    = 0;

/** defines **/
//  Note positions used from platelet screen to get similar look
#define DOSE_COLUMN         155
#define CRIT_COLUMN         290
#define VOLUME_COLUMN       405
#define DRBC_ICON_COLUMN    478
#define SS_COLUMN           520

#define DEFAULT_DRBC_SPLIT_CRIT 80.0
#define DEFAULT_SPLIT_VALUE     275.0

#define ROW0 BITMAP_BUTTON_CFGPLT_ROW1_Y
#define ROW1 BITMAP_BUTTON_CFGPLT_ROW2_Y
#define ROW2 BITMAP_BUTTON_CFGPLT_ROW3_Y

#define LABEL_AREA 125

#define PROCEDURE_INDICTOR_X    300
#define PROCEDURE_INDICTOR_Y    40

#define PR_KEYPAD_X 5
#define PR_KEYPAD_Y 150

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CONFIG_RBC::Screen_CONFIG_RBC()
   : Screen_Config       (TEAL, GUI_SCREEN_CONFIG_RBC, textCfgScreenLabelRbc),
     _txtDoseLbl         (textCfgColLabelCfgRbcDose),
     _txtCritLbl         (textCfgColLabelCfgRbcCrit),
     _txtCollectVolLbl   (textCfgColLabelCfgRbcVol),
     _txtSolVolLbl       (textCfgColLabelCfgRbcRas),
     _txtProductIndicator(textCfgRowLabelCfgRbc1),
     _txtMinValLbl       (textEmptyFormatCfgNumericTextFormatNull),
     _txtMaxValLbl       (textEmptyFormatCfgNumericTextFormatNull),

     _txtSplitRangeBottomMin (textMiscCfgSplitRangeBottomMin),
     _txtSplitRangeBottomMax (textMiscCfgSplitRangeBottomMax),
     _txtSplitRangeTopMin    (textMiscCfgSplitRangeTopMin),
     _txtSplitRangeTopMax    (textMiscCfgSplitRangeTopMax),

     _txtSplitRangeBottomLbl (textMiscCfgSplitRangeBottomLabel),
     _txtSplitRangeTopLbl    (textMiscCfgSplitRangeTopLabel),

     _btnScrollUp    (NULL),
     _btnScrollDown  (NULL),

     _btnDrbcAlert((DOSE_COLUMN + CRIT_COLUMN) / 2,
                   367,
                   guiglobs::cdsData.rwConfig.procedure.key_drbc_alert,
                   KEY_DRBC_ALERT,
                   CFG_Look::BUTTON_UP_NAME,
                   CFG_Look::BUTTON_DOWN_NAME,
                   GUI_BUTTON_CONFIG_DRBC_ALERT,
                   textButtonLabelCfgLeftRbcSplitAlert,
                   CFG_Look::FONT_COLOR,
                   CFG_Look::FONT_ALIGN,
                   CFG_Look::FONT_SIZE),

     _btnYsf(450,
             367,
             guiglobs::cdsData.rwConfig.predict.key_rsf,
             KEY_RSF,
             CFG_Look::BUTTON_UP_NAME,
             CFG_Look::BUTTON_DOWN_NAME,
             GUI_BUTTON_CONFIG_RSF,
             textButtonLabelCfgLeftRbcYsf,
             CFG_Look::FONT_COLOR,
             CFG_Look::FONT_ALIGN,
             CFG_Look::FONT_SIZE),

     _btnScrollUpGray(NULL),
     _btnScrollDownGray(NULL),

     _callbackType(INACTIVE),
     _callbackProduct(0),
     _srbcDoseMax(0.0f)
{
   _topProductNum = 0;

   _maxProdVol    = Software_CDS::GetInstance().getRbcMaxProdVol();

   if (_maxProdVol <= 0)
   {
      DataLog (log_level_gui_error) << "Features.bin returned nonsense value " << _maxProdVol
                                    << " for rbc_max_prod_vol. Set value to 1000.0" << endmsg;

      _maxProdVol = 1000;
   }


   // Fill in most of the row info
   for (int row = 0; row < MAX_RBC_ROWS; row++)
   {
      _rowInfo[row].rasToggleAndProdNum.info.x_pos    = BITMAP_CFGRBC_LABEL_BOX_X;
      _rowInfo[row].rasToggleAndProdNum.btnPtr        = NULL;
      _rowInfo[row].rasToggleAndProdNum.info.parent   = this;
      _rowInfo[row].rasToggleAndProdNum.info.btn_type = MS;
      _rowInfo[row].rasToggleAndProdNum.info.row      = row;

      _rowInfo[row].rowLine.info.x_pos                = BITMAP_CFGRBC_LABEL_LINE_X;
      _rowInfo[row].rowLine.btnPtr                    = NULL;
      _rowInfo[row].rowLine.info.parent               = this;
      _rowInfo[row].rowLine.info.btn_type             = INACTIVE;
      _rowInfo[row].rowLine.info.row                  = row;

      _rowInfo[row].doseBtn.info.x_pos                = DOSE_COLUMN;
      _rowInfo[row].doseBtn.btnPtr                    = NULL;
      _rowInfo[row].doseBtn.info.parent               = this;
      _rowInfo[row].doseBtn.info.btn_type             = DOSE;
      _rowInfo[row].doseBtn.info.row                  = row;

      _rowInfo[row].critBtn.info.x_pos                = CRIT_COLUMN;
      _rowInfo[row].critBtn.btnPtr                    = NULL;
      _rowInfo[row].critBtn.info.parent               = this;
      _rowInfo[row].critBtn.info.btn_type             = CRIT;
      _rowInfo[row].critBtn.info.row                  = row;

      _rowInfo[row].solVolBtn.info.x_pos              =  SS_COLUMN;
      _rowInfo[row].solVolBtn.btnPtr                  = NULL;
      _rowInfo[row].solVolBtn.info.parent             = this;
      _rowInfo[row].solVolBtn.info.btn_type           = SS_VOL;
      _rowInfo[row].solVolBtn.info.row                = row;

      _rowInfo[row].drbcIcon.info.x_pos               =  DRBC_ICON_COLUMN;
      _rowInfo[row].drbcIcon.bmpPtr                   = NULL;
      _rowInfo[row].drbcIcon.info.parent              = this;
      _rowInfo[row].drbcIcon.info.btn_type            = INACTIVE;
      _rowInfo[row].drbcIcon.info.row                 = row;

      _rowInfo[row].collectVolTxt                     = 0;
   }

   // Fill in the row Ys
   _rowInfo[0].rasToggleAndProdNum.info.y_pos = BITMAP_CFGRBC_LABEL_BOX1_Y;
   _rowInfo[0].rowLine.info.y_pos             = BITMAP_CFGRBC_LABEL_LINE1_Y;
   _rowInfo[0].doseBtn.info.y_pos             = ROW0;
   _rowInfo[0].critBtn.info.y_pos             = ROW0;
   _rowInfo[0].solVolBtn.info.y_pos           = ROW0;
   _rowInfo[0].drbcIcon.info.y_pos            = ROW0 + 5;

   _rowInfo[1].rasToggleAndProdNum.info.y_pos = BITMAP_CFGRBC_LABEL_BOX2_Y;
   _rowInfo[1].rowLine.info.y_pos             = BITMAP_CFGRBC_LABEL_LINE2_Y;
   _rowInfo[1].doseBtn.info.y_pos             = ROW1;
   _rowInfo[1].critBtn.info.y_pos             = ROW1;
   _rowInfo[1].solVolBtn.info.y_pos           = ROW1;
   _rowInfo[1].drbcIcon.info.y_pos            = ROW1 + 5;

   _rowInfo[2].rasToggleAndProdNum.info.y_pos = BITMAP_CFGRBC_LABEL_BOX3_Y;
   _rowInfo[2].rowLine.info.y_pos             = BITMAP_CFGRBC_LABEL_LINE3_Y;
   _rowInfo[2].doseBtn.info.y_pos             = ROW2;
   _rowInfo[2].critBtn.info.y_pos             = ROW2;
   _rowInfo[2].solVolBtn.info.y_pos           = ROW2;
   _rowInfo[2].drbcIcon.info.y_pos            = ROW2 + 5;

   // Fill in the data info
   _dataInfo[0].indicatorLbl = textMiscCfgRbc1Indicator;
   _dataInfo[0].procLbl      = textCfgRowLabelCfgRbc1;
   _dataInfo[0].doseKey      = KEY_RBC_DOSE_1;
   _dataInfo[0].critKey      = KEY_RBC_CRIT_1;
   _dataInfo[0].solToggleKey = KEY_RBC_MSS_1;
   _dataInfo[0].solVolKey    = KEY_RBC_MSS_VOLUME_1;

   _dataInfo[1].indicatorLbl = textMiscCfgRbc2Indicator;
   _dataInfo[1].procLbl      = textCfgRowLabelCfgRbc2;
   _dataInfo[1].doseKey      = KEY_RBC_DOSE_2;
   _dataInfo[1].critKey      = KEY_RBC_CRIT_2;
   _dataInfo[1].solToggleKey = KEY_RBC_MSS_2;
   _dataInfo[1].solVolKey    = KEY_RBC_MSS_VOLUME_2;

   _dataInfo[2].indicatorLbl = textMiscCfgRbc3Indicator;
   _dataInfo[2].procLbl      = textCfgRowLabelCfgRbc3;
   _dataInfo[2].doseKey      = KEY_RBC_DOSE_3;
   _dataInfo[2].critKey      = KEY_RBC_CRIT_3;
   _dataInfo[2].solToggleKey = KEY_RBC_MSS_3;
   _dataInfo[2].solVolKey    = KEY_RBC_MSS_VOLUME_3;

   _dataInfo[3].indicatorLbl = textMiscCfgRbc4Indicator;
   _dataInfo[3].procLbl      = textCfgRowLabelCfgRbc4;
   _dataInfo[3].doseKey      = KEY_RBC_DOSE_4;
   _dataInfo[3].critKey      = KEY_RBC_CRIT_4;
   _dataInfo[3].solToggleKey = KEY_RBC_MSS_4;
   _dataInfo[3].solVolKey    = KEY_RBC_MSS_VOLUME_4;

   _dataInfo[4].indicatorLbl = textMiscCfgRbc5Indicator;
   _dataInfo[4].procLbl      = textCfgRowLabelCfgRbc5;
   _dataInfo[4].doseKey      = KEY_RBC_DOSE_5;
   _dataInfo[4].critKey      = KEY_RBC_CRIT_5;
   _dataInfo[4].solToggleKey = KEY_RBC_MSS_5;
   _dataInfo[4].solVolKey    = KEY_RBC_MSS_VOLUME_5;

   _dataInfo[5].indicatorLbl = textMiscCfgRbc6Indicator;
   _dataInfo[5].procLbl      = textCfgRowLabelCfgRbc6;
   _dataInfo[5].doseKey      = KEY_RBC_DOSE_6;
   _dataInfo[5].critKey      = KEY_RBC_CRIT_6;
   _dataInfo[5].solToggleKey = KEY_RBC_MSS_6;
   _dataInfo[5].solVolKey    = KEY_RBC_MSS_VOLUME_6;
}  // END of Screen_CONFIG_RBC CONSTRUCTOR



//
////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_RBC::~Screen_CONFIG_RBC()
{
   Screen_CONFIG_RBC::deallocate_resources ();
}  // END of Screen_CONFIG_RBC DESTRUCTOR



//
////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_RBC::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   if (_srbcDoseMax <= 0)
   {
      // Cassette object from trima_common_reentrant (the same one predict uses)
      cassette cs;
      _srbcDoseMax = cs.maxRbcDose(cassette::PLTPLSRBC);
      // DataLog (log_level_gui_info) << "SRBC Dose Max is " << _srbcDoseMax << endmsg;
   }

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.
   OSRegion widget_positions(0, 0, LABEL_AREA, 70);

   _txtDoseLbl.allocate_resources(*this);
   _txtCritLbl.allocate_resources(*this);
   _txtCollectVolLbl.allocate_resources(*this);
   _txtSolVolLbl.allocate_resources(*this);

   _btnDrbcAlert.allocate_resources(*this);
   _btnDrbcAlert.set_callback(cb_drbc_alert_button, (void*)this);

   _btnYsf.allocate_resources(*this);
   _btnYsf.set_callback(cb_rbc_ysf_button, (void*)this);


   _btnScrollUpGray = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                         BITMAP_BUTTON_CFGPLT_UP_ARROW_X,
                                         BITMAP_BUTTON_CFGPLT_UP_ARROW_Y,
                                         textEmpty,
                                         BITMAP_CONFIG_SCROLL_GRAY_UP,
                                         BITMAP_CONFIG_SCROLL_GRAY_UP);

   _btnScrollDownGray = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                           BITMAP_BUTTON_CFGPLT_DN_ARROW_X,
                                           BITMAP_BUTTON_CFGPLT_DN_ARROW_Y,
                                           textEmpty,
                                           BITMAP_CONFIG_SCROLL_GRAY_DN,
                                           BITMAP_CONFIG_SCROLL_GRAY_DN);

   _btnScrollDown = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                       BITMAP_BUTTON_CFGPLT_DN_ARROW_X,
                                       BITMAP_BUTTON_CFGPLT_DN_ARROW_Y,
                                       textEmpty,
                                       BITMAP_CONFIG_DOWN_SCROLL,
                                       BITMAP_CONFIG_SCROLL_DOWN);


   _btnScrollUpGray->allocate_resources(*this);
   _btnScrollUpGray->disable_callbacks ();

   _btnScrollDownGray->allocate_resources(*this);
   _btnScrollDownGray->disable_callbacks ();

   _btnScrollDown->allocate_resources (*this) ;
   _btnScrollDown->set_callback(cb_down_button, (void*)this);

   for (int row = 0; row < MAX_RBC_ROWS; row++)
   {
      _rowInfo[row].rowLine.btnPtr =
         new Button_Bitmap (RBC_BUTTON_LABEL_LINE(row),
                            (short)_rowInfo[row].rowLine.info.x_pos,
                            (short)_rowInfo[row].rowLine.info.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CFGPLT_PLT_LABEL_LINE,
                            BITMAP_NULL);

      _rowInfo[row].rowLine.btnPtr->allocate_resources(*this);
      _rowInfo[row].rowLine.btnPtr->disable_callbacks();

      _rowInfo[row].doseBtn.btnPtr =
         new Button_Bitmap (RBC_BUTTON_DOSE(row),
                            (short)_rowInfo[row].doseBtn.info.x_pos,
                            (short)_rowInfo[row].doseBtn.info.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      _rowInfo[row].doseBtn.btnPtr->allocate_resources(*this);
      _rowInfo[row].doseBtn.btnPtr->set_callback(cb_button, (void*)&_rowInfo[row].doseBtn);

      _rowInfo[row].critBtn.btnPtr =
         new Button_Bitmap (RBC_BUTTON_CRIT(row),
                            (short)_rowInfo[row].critBtn.info.x_pos,
                            (short)_rowInfo[row].critBtn.info.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      _rowInfo[row].critBtn.btnPtr->allocate_resources(*this);
      _rowInfo[row].critBtn.btnPtr->set_callback(cb_button, (void*)&_rowInfo[row].critBtn);

      _rowInfo[row].rasToggleAndProdNum.btnPtr =
         new Button_Bitmap (RBC_BUTTON_PROD(row),
                            (short)_rowInfo[row].rasToggleAndProdNum.info.x_pos,
                            (short)_rowInfo[row].rasToggleAndProdNum.info.y_pos,
                            textCfgRowLabelCfgRbc2,
                            BITMAP_MSS_BUTTON_RAS_ICON_UP,
                            BITMAP_MSS_BUTTON_RAS_ICON_DOWN,
                            false, false);

      _rowInfo[row].rasToggleAndProdNum.btnPtr->allocate_resources(*this);
      _rowInfo[row].rasToggleAndProdNum.btnPtr->set_callback(cb_toggle_button, (void*)&_rowInfo[row].rasToggleAndProdNum);

      _rowInfo[row].solVolBtn.btnPtr =
         new Button_Bitmap (RBC_BUTTON_VOLUME(row),
                            (short)_rowInfo[row].solVolBtn.info.x_pos,
                            (short)_rowInfo[row].solVolBtn.info.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      _rowInfo[row].solVolBtn.btnPtr->allocate_resources(*this);
      _rowInfo[row].solVolBtn.btnPtr->set_callback(cb_button, (void*)&_rowInfo[row].solVolBtn);

      _rowInfo[row].collectVolTxt   = new Display_Text(textEmptyFormatCfgRbcVolData);

      _rowInfo[row].drbcIcon.bmpPtr = new Bitmap(_rowInfo[row].drbcIcon.info.x_pos,
                                                 _rowInfo[row].drbcIcon.info.y_pos,
                                                 BITMAP_CFG_RBC_DRBC_ICON);

      _rowInfo[row].collectVolTxt->allocate_resources(*this);
      widget_positions   = _rowInfo[row].collectVolTxt->getRegion();
      widget_positions.y = (short)(_rowInfo[row].doseBtn.info.y_pos);
      _rowInfo[row].collectVolTxt->set_requested_region(widget_positions);
   }
   _topProductNum = 0;

   updateDrbcButton();
   label_the_buttons();

   checkForTimeOnlyConfig();
} // END of Screen_CONFIG_RBC ALLOCATE_RESOURCES


//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_RBC::deallocate_resources ()
{
   for (int row = 0; row < MAX_RBC_ROWS; row++)
   {
      delete _rowInfo[row].rowLine.btnPtr;
      _rowInfo[row].rowLine.btnPtr = NULL;

      delete _rowInfo[row].doseBtn.btnPtr;
      _rowInfo[row].doseBtn.btnPtr = NULL;

      delete _rowInfo[row].critBtn.btnPtr;
      _rowInfo[row].critBtn.btnPtr = NULL;

      delete _rowInfo[row].rasToggleAndProdNum.btnPtr;
      _rowInfo[row].rasToggleAndProdNum.btnPtr = NULL;

      delete _rowInfo[row].solVolBtn.btnPtr;
      _rowInfo[row].solVolBtn.btnPtr = NULL;

      delete _rowInfo[row].collectVolTxt;
      _rowInfo[row].collectVolTxt = NULL;

      delete _rowInfo[row].drbcIcon.bmpPtr;
      _rowInfo[row].drbcIcon.bmpPtr = NULL;
   }

   _txtDoseLbl.deallocate_resources();
   _txtCritLbl.deallocate_resources();
   _txtCollectVolLbl.deallocate_resources();
   _txtSolVolLbl.deallocate_resources();
   _txtProductIndicator.deallocate_resources();
   _btnDrbcAlert.deallocate_resources();
   _btnYsf.deallocate_resources();

   // Make sure range strings are clear.
   clearCustomDoubleRange();

   delete _btnScrollUp;
   _btnScrollUp = NULL;

   delete _btnScrollDown;
   _btnScrollDown = NULL;

   delete _btnScrollUpGray;
   _btnScrollUpGray = NULL;

   delete _btnScrollDownGray;
   _btnScrollDownGray = NULL;

   delete _btnScrollDown;
   _btnScrollDown = NULL;

   // deactivate the status line before leaving
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}  // END of Screen_CONFIG_RBC DEALLOCATE_RESOURCES


///////////////////////////////////////////////////////////////////////////////
void cb_toggle_button (void* data)
{
   Screen_CONFIG_RBC::BTN_INFO* btn_info = (Screen_CONFIG_RBC::BTN_INFO*)data;

   (btn_info->info.parent)->awaiting_callback(btn_info->info.btn_type, btn_info->info.row);
   btn_info->info.parent->process_toggle_button(btn_info);
}

///////////////////////////////////////////////////////////////////////////////

void Screen_CONFIG_RBC::putUpDoseKeypad (int prodRow)
{
   // If there's a bad gap, display split stuff.
   bool useCustomDoubleRange = isBadSplitConfigurationPossible(prodRow);

   // The bool parameter decides whether or not we should show the custom double
   // range.  See IT 8088 for background on this.

   float minDose = guiglobs::config_var_list.Var(KEY_RBC_DOSE_1).Min();
   float maxDose = getMaxDose(prodRow);

   // DataLog (log_level_gui_info) << "maxDose == " << maxDose << endmsg;

   if (useCustomDoubleRange)
   {
      float bottomMin = guiglobs::config_var_list.Var(KEY_RBC_DOSE_1).Min();
      float bottomMax = _srbcDoseMax;
      float topMin    = guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold;

      // DataLog (log_level_gui_info) << "putUpDoseKeypad(" << prodRow << ") => double range [("
      //	<< bottomMin << "; " << bottomMax << "); (" << topMin << "; " << maxDose << ")]" << endmsg;

      displayCustomDoubleRange(bottomMin, bottomMax, topMin, maxDose, "%.0f");
   }
   else
   {
      // DataLog (log_level_gui_info) << "putUpDoseKeypad(" << prodRow << ") => single range ("
      //	<< minDose << "; " << maxDose << ")" << endmsg;

      display_min_max_keypad_limits(minDose, maxDose, "%.0f");
   }

   build_keypad_string (keypad_string,
                        KEY_RBC_DOSE_1,  // unused
                        PR_KEYPAD_X,
                        PR_KEYPAD_Y,
                        "###",
                        maxDose,
                        minDose);

   guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

void Screen_CONFIG_RBC::putUpDrbcTresholdKeypad ()
{
   static const float minThresh = guiglobs::config_var_list.Var(KEY_DRBC_THRESHOLD).Min();
   static const float maxThresh = guiglobs::config_var_list.Var(KEY_DRBC_THRESHOLD).Max();

   // We only want to prepopulate the calculator if the the alert
   // was off and we're turning it on.
   bool prepopCalc = !drbcAlertIsOn();

   guiglobs::cdsData.rwConfig.procedure.key_drbc_alert = 1;

   guistring tempStr;
   tempStr.initialize();

   sprintf(tempStr, "%d", guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold);

   display_min_max_keypad_limits(minThresh, maxThresh, "%.0f");

   if (prepopCalc)
      Screen_Config::build_keypad_string (keypad_string,
                                          KEY_DRBC_THRESHOLD,
                                          PR_KEYPAD_X,
                                          PR_KEYPAD_Y,
                                          "###",
                                          maxThresh,
                                          minThresh,
                                          tempStr);
   else
      Screen_Config::build_keypad_string (keypad_string,
                                          KEY_DRBC_THRESHOLD,
                                          PR_KEYPAD_X,
                                          PR_KEYPAD_Y,
                                          "###",
                                          maxThresh,
                                          minThresh);

   awaiting_callback (DRBC, 0);

   guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


void cb_button (void* data)
{
   float     min, max;
   guistring keypad_string;
   keypad_string.initialize();

   Screen_CONFIG_RBC::BTN_INFO* btn_info = (Screen_CONFIG_RBC::BTN_INFO*)data;

   (btn_info->info.parent)->set_indicator(btn_info->info.row);
   (btn_info->info.parent)->awaiting_callback(btn_info->info.btn_type, btn_info->info.row);

   switch (btn_info->info.btn_type)
   {

      case Screen_CONFIG_RBC::SS_VOL :
         min = Screen_CONFIG_RBC::getMinRasVol(btn_info->info.row);
         max = Screen_CONFIG_RBC::getMaxRasVol(btn_info->info.row);

         // Deal with the corner case where max is less than min setting min to max (only allowing current value)
         if (max < min) min = max;

         (btn_info->info.parent)->display_min_max_keypad_limits(min, max, "%.0f");

         Screen_Config::build_keypad_string (keypad_string,
                                             KEY_RBC_MSS_VOLUME_1,   // unused
                                             PR_KEYPAD_X,
                                             PR_KEYPAD_Y,
                                             "###",
                                             max,
                                             min);

         guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen());

         (btn_info->info.parent)->goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
         break;

      case Screen_CONFIG_RBC::DOSE :  // assumes min and max are the same for all doses
         // (or we'd have to ask the parent)
         guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
         (btn_info->info.parent)->putUpDoseKeypad(btn_info->info.row);
         break;

      case Screen_CONFIG_RBC::CRIT :
         min = Screen_CONFIG_RBC::getMinCrit(btn_info->info.row);
         max = guiglobs::config_var_list.Var(KEY_RBC_CRIT_1).Max();

         (btn_info->info.parent)->display_min_max_keypad_limits(min, max, "%.0f");

         Screen_Config::build_keypad_string (keypad_string,
                                             KEY_RBC_CRIT_1,    // unused
                                             PR_KEYPAD_X,
                                             PR_KEYPAD_Y,
                                             "##",
                                             max,
                                             min);

         guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen());

         (btn_info->info.parent)->goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "UNEXPECTED SCREEN CHOICE" << endmsg;
         break;
   }

}


////////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_RBC::awaiting_callback (const BUTTON_TYPE type, const int row)
{
   _callbackType    = type;
   _callbackProduct = row + _topProductNum;
}


////////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_RBC::process_toggle_button (const BTN_INFO* btn_info)
{
   guiglobs::cdsData.rwConfig.templates.key_rbc_mss[btn_info->info.row + _topProductNum] =
      (guiglobs::cdsData.rwConfig.templates.key_rbc_mss[btn_info->info.row + _topProductNum] == 0);

   // If we just turned MSS on, we may need to update the DRBC button.
   // We also may need to put up an MSS volume warning or something.
   updateDrbcButton();
   checkForBadRasConfig(btn_info->info.row);
   label_the_buttons_by_row(btn_info->info.row);
}

////////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG_RBC::set_indicator (const int row)
{
   if (row >= 0)
   {
      _txtProductIndicator.set_string_id(_dataInfo[row + _topProductNum].indicatorLbl);
      _txtProductIndicator.allocate_resources(*this);
   }
}

float Screen_CONFIG_RBC::getDose (int product)
{
   return guiglobs::cdsData.rwConfig.templates.key_rbc_dose[product];
}

float Screen_CONFIG_RBC::getCrit (int product)
{
   return (guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[product]) ?
          DEFAULT_DRBC_SPLIT_CRIT : guiglobs::cdsData.rwConfig.templates.key_rbc_crit[product];
}

float Screen_CONFIG_RBC::getCollectVol (int product)
{
   return (getDose(product) * 100.0f / guiglobs::cdsData.rwConfig.predict.key_rsf) / getCrit(product);
}

//
///////////////////////////////////////////////////////////////////////////////
// label_the_buttons
void Screen_CONFIG_RBC::label_the_buttons ()
{
   for (int row = 0; row < MAX_RBC_ROWS; row++)
      label_the_buttons_by_row(row);
}

bool Screen_CONFIG_RBC::updateRasToggle (int row)
{
   // this assumes rbc1-rbcn numbered 0 to n-1
   _rowInfo[row].rasToggleAndProdNum.btnPtr->set_string_id(_dataInfo[row + _topProductNum].procLbl);

   // Set pas toggle
   _rowInfo[row].rasToggleAndProdNum.btnPtr->set_up_bitmap((guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on) ? ((guiglobs::cdsData.rwConfig.templates.key_rbc_mss[row + _topProductNum]) ? BITMAP_MSS_BUTTON_RAS_ICON_DOWN : BITMAP_MSS_BUTTON_RAS_ICON_UP) : BITMAP_MSS_BUTTON_RAS_ICON_GRAY);
   _rowInfo[row].rasToggleAndProdNum.btnPtr->set_button_text_color((guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on) ? YELLOW : GREY_LIGHT);
   _rowInfo[row].rasToggleAndProdNum.btnPtr->set_disabled(!guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on);

   return guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on &&
          guiglobs::cdsData.rwConfig.templates.key_rbc_mss[row + _topProductNum];
}

float Screen_CONFIG_RBC::updateDose (int row)
{
   char  tmpTxtStr[20];

   float dose     = getDose(row + _topProductNum);

   int   text_len = sprintf(tmpTxtStr, "%.0f", dose);
   _rowInfo[row].doseBtn.btnPtr->set_text_direct (text_len, tmpTxtStr);

   return dose;
}


float Screen_CONFIG_RBC::updateCrit (int row)
{
   char  tmpTxtStr[20];

   bool  ptfOn    = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[row + _topProductNum];
   float crit     = getCrit(row + _topProductNum);

   int   text_len = sprintf(tmpTxtStr, "%.0f", crit);
   _rowInfo[row].critBtn.btnPtr->set_text_direct (text_len, tmpTxtStr);
   _rowInfo[row].critBtn.btnPtr->set_disabled(ptfOn);
   _rowInfo[row].critBtn.btnPtr->set_up_bitmap(ptfOn ? BITMAP_CONFIG_BUTTON_GRAY : CFG_Look::BUTTON_UP_NAME);
   _rowInfo[row].critBtn.btnPtr->set_button_text_color (ptfOn ? GREY_LIGHT : YELLOW);

   return crit;
}

void Screen_CONFIG_RBC::updateAllCollectVolumes ()
{
   for (int row = 0; row < MAX_RBC_ROWS; row++)
      updateCollectVol(row);
}


float Screen_CONFIG_RBC::updateCollectVol (int row)
{
   char  tmpTxtStr[20];
   float collectVol = getCollectVol(row + _topProductNum);

   int   text_len   = sprintf(tmpTxtStr, "%.0f", collectVol);
   _rowInfo[row].collectVolTxt->set_text_direct (text_len, tmpTxtStr);

   return collectVol;
}

void Screen_CONFIG_RBC::label_the_buttons_by_row (int row)
{
   int product_num = row + _topProductNum;

   // Update RAS toggle
   updateRasToggle(row);

   updateDose(row);
//   updateRasToggle(row);
   updateCrit(row);
   updateCollectVol(row);

   updateRasVol(row);

   updateDrbcIcon(row);
}

void Screen_CONFIG_RBC::updateAllDrbcIcons ()
{
   for (int row = 0; row < MAX_RBC_ROWS; row++)
      updateDrbcIcon(row);
}

bool Screen_CONFIG_RBC::isDrbcProduct (int row)
{
   return getDose(row + _topProductNum) >=
          (drbcAlertIsOn() ? guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold : DEFAULT_SPLIT_VALUE);
}

void Screen_CONFIG_RBC::updateDrbcIcon (int row)
{
   if (isDrbcProduct(row)) _rowInfo[row].drbcIcon.bmpPtr->allocate_resources(*this);
   else _rowInfo[row].drbcIcon.bmpPtr->deallocate_resources();
}

bool Screen_CONFIG_RBC::checkForBadRasConfig (int row)
{
   int minRasVol    = (int)(getMinRasVol(row));
   int actualRasVol = guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum];

   DataLog (log_level_gui_info) << "Row " << row << ": RAS vol " << actualRasVol
                                << ((actualRasVol < minRasVol) ? " < " : " >= ") << "minRasVol " << minRasVol
                                << ((actualRasVol < minRasVol) ? ". Set new RAS vol to " : ". Keep RAS vol at ")
                                << ((actualRasVol < minRasVol) ? minRasVol : actualRasVol)
                                << "." << endmsg;

   if (actualRasVol < minRasVol)
   {
      guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum] = minRasVol;
      updateRasVol(row);
      return true;
   }

   return false;
}

int Screen_CONFIG_RBC::updateRasVol (int row)
{
   DataLog (log_level_gui_info) << "updateRasVol(" << row << ");" << endmsg;
   char tmpTxtStr[10];
   int  text_len;

   int  maxRasVol = (int)getMaxRasVol(row);
   if (guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum] > maxRasVol)
      guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum] = maxRasVol;

   if (guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on &&
       guiglobs::cdsData.rwConfig.templates.key_rbc_mss[row + _topProductNum])
   {
      DataLog (log_level_gui_info) << "Set RAS for row " << row << " to "
                                   << guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum] << endmsg;

      text_len = sprintf(tmpTxtStr, "%d", guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum]);
      _rowInfo[row].solVolBtn.btnPtr->set_text_direct (text_len, tmpTxtStr);
      _rowInfo[row].solVolBtn.btnPtr->set_disabled(false);
      _rowInfo[row].solVolBtn.btnPtr->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
      _rowInfo[row].solVolBtn.btnPtr->set_button_text_color (YELLOW);

      return guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row + _topProductNum];
   }
   else
   {
      DataLog (log_level_gui_info) << "RAS is not on for row " << row << endmsg;

      text_len = sprintf(tmpTxtStr, "--");
      _rowInfo[row].solVolBtn.btnPtr->set_text_direct (text_len, tmpTxtStr);
      _rowInfo[row].solVolBtn.btnPtr->set_disabled(true);
      _rowInfo[row].solVolBtn.btnPtr->set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);
      _rowInfo[row].solVolBtn.btnPtr->set_button_text_color (GREY_LIGHT);

      return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
// callback for the up button
void cb_up_button (void* data)
{
   ((Screen_CONFIG_RBC*)data)->process_up_button();
}

///////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_RBC::process_up_button ()
{
   if (_topProductNum > 0)
   {
      _topProductNum--;
      label_the_buttons();

      if (_topProductNum == 0)
      {
         delete _btnScrollUp;
         _btnScrollUp = NULL;
      }
   }

   if (_btnScrollDown == NULL)
   {
      _btnScrollDown = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                          BITMAP_BUTTON_CFGPLT_DN_ARROW_X,
                                          BITMAP_BUTTON_CFGPLT_DN_ARROW_Y,
                                          textEmpty,
                                          BITMAP_CONFIG_DOWN_SCROLL,
                                          BITMAP_CONFIG_SCROLL_DOWN);

      _btnScrollDown->allocate_resources(*this);
      _btnScrollDown->set_callback (cb_down_button, (void*)this);
   }
}


//
///////////////////////////////////////////////////////////////////////////////
// callback for the down button
void cb_down_button (void* data)
{
   ((Screen_CONFIG_RBC*)data)->process_down_button();
}


///////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_RBC::process_down_button ()
{
   if (_topProductNum < (NUM_CRIT_DOSE - MAX_RBC_ROWS))
   {
      ++_topProductNum;
      label_the_buttons();

      if (_topProductNum >= (NUM_CRIT_DOSE - MAX_RBC_ROWS))
      {
         delete _btnScrollDown;
         _btnScrollDown = NULL;
      }
   }

   if (_btnScrollUp == NULL)
   {
      _btnScrollUp = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                        BITMAP_BUTTON_CFGPLT_UP_ARROW_X,
                                        BITMAP_BUTTON_CFGPLT_UP_ARROW_Y,
                                        textEmpty,
                                        BITMAP_CONFIG_UP_SCROLL,
                                        BITMAP_CONFIG_SCROLL_DOWN);

      _btnScrollUp->allocate_resources(*this);
      _btnScrollUp->set_callback(cb_up_button, (void*)this);
   }

}


///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_CONFIG_RBC::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   _txtProductIndicator.deallocate_resources();
   turn_off_min_max_keypad_text();
   clearCustomDoubleRange();

   // Sanity check to prevent negative array index conditions reported by static analysis
   const int  row      = _callbackProduct - _topProductNum;
   const bool badIndex = (_callbackProduct < 0 || row < 0);

   if (badIndex || return_keypad_buffer.keypad_return_value < 0)
   {
      return;
   }
   float the_data = (float)atof(return_keypad_buffer.keypad_return_string);

   switch (_callbackType)
   {
      case SS_VOL :
         guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[_callbackProduct] = (int)the_data;
         updateRasVol(row);
         break;

      case DOSE :
         // DataLog (log_level_gui_info) << "Check new dose of " << return_keypad_buffer.keypad_return_value << endmsg;

         // This is a hack.  The keypad, as implemented, isn't capable of enforcing a
         // range with a gap in the middle like we needed for IT 8088.  Rather than
         // modifying the keypad class to allow for this, let's just redisplay the keypad
         // and make the operator select a new number if the number falls into the bad range.
         if (checkForBadSplitConfiguration(row, return_keypad_buffer.keypad_return_value))
         {
            // If this is a bad split configuration, put the keypad back up.
            DataLog (log_level_gui_info) << "Bad split configuration: re-display dose keypad." << endmsg;

            set_indicator(_callbackProduct);
            putUpDoseKeypad(_callbackProduct);
            return;
         }

         // DataLog (log_level_gui_info) << "Configuration OK" << endmsg;

         guiglobs::cdsData.rwConfig.templates.key_rbc_dose[_callbackProduct] = the_data;
         _rowInfo[row].doseBtn.btnPtr->set_text_direct (10, return_keypad_buffer.keypad_return_string);

         updateCollectVol(row);
         updateDrbcIcon(row);
         checkForBadRasConfig(row);
         display_alert(the_data);
         break;

      case CRIT :
         guiglobs::cdsData.rwConfig.templates.key_rbc_crit[_callbackProduct] = the_data;
         _rowInfo[row].critBtn.btnPtr->set_text_direct (10, return_keypad_buffer.keypad_return_string);
         updateCollectVol(row);
         break;

      case RSF :
         guiglobs::cdsData.rwConfig.predict.key_rsf = the_data;

         { // Brackets purely for scoping
            guistring rsfStr;
            int       rsfStrLen = sprintf_l10n(rsfStr, "%.2f", the_data);
            _btnYsf.set_text_direct(rsfStrLen, rsfStr);
         }

         updateAllCollectVolumes();
         break;

      case DRBC :
         guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold = (int)the_data;
         updateDrbcButton();

         checkForBadRasConfig(row);

         // If changing the threshold left us with a product in the
         // unconfigurable gap (see IT 8088), then give the operator a
         // message telling them so.
         if (checkForBadSplitConfiguration())
         {
            // If this is a bad split configuration, put the keypad back up.
            guiglobs::apheresis_status_line->reparent_status_line(*this);
            guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatDoseRangeError);
         }

         // This is not row-specific, so check all rows.
         updateAllDrbcIcons();

         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();
         DataLog(log_level_gui_error) << "UNEXPECTED BUTTON TYPE" << endmsg;
         break;
   }
} // end of method


////////////////////////////////////////////////////////////////////////////////
// callbacks
void cb_drbc_alert_button (void* data)
{
   ((Screen_CONFIG_RBC*)data)->process_drbc_threshold_button();
}

void Screen_CONFIG_RBC::process_drbc_threshold_button ()
{
   // If drbc alert is on, turn it off (unless we're running a PTF alert).
   if ((!atLeastOnePtfIsOn()) && (guiglobs::cdsData.rwConfig.procedure.key_drbc_alert != 0))
   {
      guiglobs::cdsData.rwConfig.procedure.key_drbc_alert = 0;
      updateDrbcButton();
   }
   else
   {
      guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
      putUpDrbcTresholdKeypad();
   }
}


void cb_rbc_ysf_button (void* data)
{
   ((Screen_CONFIG_RBC*)data)->process_rbc_ysf_button();
}

void Screen_CONFIG_RBC::process_rbc_ysf_button ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_RSF).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_RSF).Max();

   display_min_max_keypad_limits(min, max, "%.2f");

   Screen_Config::build_keypad_string (keypad_string,
                                       KEY_RSF,
                                       PR_KEYPAD_X,
                                       PR_KEYPAD_Y,
                                       "#.##",
                                       max,
                                       min);

   awaiting_callback (RSF, 0);
   guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


float Screen_CONFIG_RBC::updateDrbcButton ()
{
   int drbcThreshold = guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold;

   if (drbcAlertIsOn())
   {
      // Make sure DRBC is on
      _btnDrbcAlert.set_up_bitmap(CFG_Look::BUTTON_DOWN_NAME);

      guistring thresholdText;
      thresholdText.initialize();

      int stringLen = sprintf(thresholdText, "%d", drbcThreshold);
      _btnDrbcAlert.set_text_direct(thresholdText.buffersize, thresholdText);

      return drbcThreshold;
   }
   else
   {
      _btnDrbcAlert.set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
      _btnDrbcAlert.set_string_id(textListboxItemCfgOff);

      return DEFAULT_SPLIT_VALUE;
   }
}

float Screen_CONFIG_RBC::getMinRasVol (int row)
{
   float minRasVol = guiglobs::config_var_list.Var(KEY_RBC_MSS_VOLUME_1).Min();

   if (isDrbcProduct(row)) minRasVol *= 2.0;

   return minRasVol;
}


float Screen_CONFIG_RBC::getMaxRasVol (int row)
{
   float collectVol = (getDose(row + _topProductNum) * 100.0f
                       / guiglobs::cdsData.rwConfig.predict.key_rsf)
                      / DEFAULT_DRBC_SPLIT_CRIT;

   float defaultMax = guiglobs::config_var_list.Var(KEY_RBC_MSS_VOLUME_1).Max();
//	float calcMax = floor(_maxProdVol - collectVol);
   float calcMax    = floor(_maxProdVol - DisplayFormatter::CalculateCollectVolForRAS(collectVol));

   return min(defaultMax, calcMax);
}

float Screen_CONFIG_RBC::getMinCrit (int row)
{
   float minCritCalc = ceil((getDose(row + _topProductNum) * 100.0f
                             / guiglobs::cdsData.rwConfig.predict.key_rsf) / (_maxProdVol));

   float minCritDefault = guiglobs::config_var_list.Var(KEY_RBC_CRIT_1).Min();

   return max(minCritCalc, minCritDefault);
}

float Screen_CONFIG_RBC::getMaxDose (int row)
{
   bool rasOn = false;

   if ((row >= 0) && (row < NUM_CRIT_DOSE) && (guiglobs::cdsData.rwConfig.templates.key_rbc_mss[row]) && (guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on))
   {
      rasOn = true;
   }

   float rasVol = rasOn ?  // If RAS is on, this is RAS volume.  Otherwise, it's zero.
                  guiglobs::cdsData.rwConfig.templates.key_rbc_mss_volume[row]
                  : 0.0f;

   float crit        = getCrit(row + _topProductNum);

   float maxDoseCalc = floor((guiglobs::cdsData.rwConfig.predict.key_rsf
                              * crit
                              * (_maxProdVol - rasVol) ) / 100.0f);

   float maxDoseDefault = guiglobs::config_var_list.Var(KEY_RBC_DOSE_1).Max();

   /*DataLog (log_level_gui_info) << "getMaxDose(" << row << ") --> calcMax == floor((rsf["
       << guiglobs::cdsData.rwConfig.predict.key_rsf << "] * crit[" << crit << "] * (maxVol["
       << _maxProdVol << "] - rasVol[" << rasVol << "])) / 100) == maxDoseCalc["
       << maxDoseCalc << "]; defMax=="
       << maxDoseDefault << endmsg;*/

   return MIN(maxDoseCalc, maxDoseDefault);
}

bool Screen_CONFIG_RBC::drbcAlertIsOn ()
{
   return guiglobs::cdsData.rwConfig.procedure.key_drbc_alert ||
          atLeastOnePtfIsOn();
}


bool Screen_CONFIG_RBC::atLeastOnePtfIsOn ()
{
   if (guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on)
      for (int procIndex = 0; procIndex < NUM_CRIT_DOSE; procIndex++)
         if (guiglobs::cdsData.rwConfig.templates.key_rbc_mss[procIndex]) return true;

   return false;
}

void Screen_CONFIG_RBC::verifyDrbcBitReflectsPtfToggles ()
{
   bool newDrbcAlert = drbcAlertIsOn();

   if (guiglobs::cdsData.rwConfig.procedure.key_drbc_alert != newDrbcAlert)
   {
      DataLog (log_level_gui_info) << "One or more RBC procedures are configured as PTF. Toggling DRBC split notification before config write." << endmsg;
      guiglobs::cdsData.rwConfig.procedure.key_drbc_alert = newDrbcAlert;
   }

   for (int rbcProdIndex = 0; rbcProdIndex < NUM_CRIT_DOSE; rbcProdIndex++)
   {
      bool ptfOn = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[rbcProdIndex];

      if ( ptfOn && (guiglobs::cdsData.rwConfig.templates.key_rbc_crit[rbcProdIndex] != DEFAULT_DRBC_SPLIT_CRIT))
      {
         DataLog (log_level_gui_info) << "PTF is on for procedure " << rbcProdIndex + 1 << ". Adjusting crit to "
                                      << DEFAULT_DRBC_SPLIT_CRIT << "." << endmsg;
         guiglobs::cdsData.rwConfig.templates.key_rbc_crit[rbcProdIndex] = DEFAULT_DRBC_SPLIT_CRIT;
      }
   }
}

Screen_CONFIG_RBC* Screen_CONFIG_RBC::getInstancePtr ()
{
   return ((Screen_CONFIG_RBC*)guiglobs::scrnComm->getScreenInstancePointer(GUI_SCREEN_CONFIG_RBC));
}

void Screen_CONFIG_RBC::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      disableConfigBtn(_btnDrbcAlert);
      disableConfigBtn(_btnYsf);

      for (int row = 0; row < MAX_RBC_ROWS; row++)
      {
         bool rasOn =
            guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on &&
            guiglobs::cdsData.rwConfig.templates.key_rbc_mss[row + _topProductNum];

         if (_rowInfo[row].doseBtn.btnPtr)
            disableConfigBtn(*_rowInfo[row].doseBtn.btnPtr);

         if (_rowInfo[row].critBtn.btnPtr)
            disableConfigBtn(*_rowInfo[row].critBtn.btnPtr);

         if (_rowInfo[row].rasToggleAndProdNum.btnPtr)
            disableConfigBtn(*_rowInfo[row].rasToggleAndProdNum.btnPtr, ((rasOn) ? BITMAP_MSS_BUTTON_RAS_ICON_GRAY_DOWN : BITMAP_MSS_BUTTON_RAS_ICON_GRAY));

         if (_rowInfo[row].solVolBtn.btnPtr)
            disableConfigBtn(*_rowInfo[row].solVolBtn.btnPtr);

         if (_rowInfo[row].collectVolTxt)
            disableConfigTxt(*_rowInfo[row].collectVolTxt);
      }

      disableConfigTxt(_txtDoseLbl);
      disableConfigTxt(_txtCritLbl);
      disableConfigTxt(_txtCollectVolLbl);
      disableConfigTxt(_txtSolVolLbl);
      disableConfigTxt(_txtProductIndicator);
      disableConfigTxt(_txtMinValLbl);
      disableConfigTxt(_txtMaxValLbl);
   }
}

bool Screen_CONFIG_RBC::isBadSplitConfigurationPossible (int prodRow = -1)
{
   if (!drbcAlertIsOn())
   {
      // DataLog (log_level_gui_info) << "isBadSplitConfigurationPossible() == false (drbc alert not on)" << endmsg;
      return false;
   }
   else
   {
      float bottomMax = _srbcDoseMax;
      float topMin    = guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold;
      // 1000 is just a high number that's always greather than topMin.
      float topMax    = (prodRow < 0) ? 1000.0 : getMaxDose(prodRow);

      // Is there a gap in the configureable range,
      // and can they actually configure a double with
      // the current RAS setting?

      /*DataLog (log_level_gui_info) << "isBadSplitConfigurationPossible("
          << prodRow << ") == (bMax[" << bottomMax << "] < tMin["
          << topMin << "] && tMin[" << topMin << "] < tMax["
          << topMax << "]) == " << (bottomMax < topMin && topMin < topMax) << endmsg;*/

      return bottomMax < topMin && topMin < topMax;
   }
}

bool Screen_CONFIG_RBC::checkForBadSplitConfiguration (int updatedProdRow = -1, float updatedProdDose = -1.0)
{
   // Configuring a product for which the dose is greater than
   // the max allowable single product, but less than the DRBC
   // threshold (or less than 300 if DRBC threshold is off)
   // results in that product getting thrown out at predict.
   // In IT 8088, we decided we want to prevent against allowing
   // these sorts of products to be configured, for some reason.
   // This function checks for that condition.

   // If no row was provided,
   if (updatedProdRow >= 0 && !isBadSplitConfigurationPossible(updatedProdRow))
   {
      // DataLog (log_level_gui_info) << "Bad split configuration not possible." << endmsg;
      return false;
   }

   float bottomMin = guiglobs::config_var_list.Var(KEY_RBC_DOSE_1).Min();
   float bottomMax = _srbcDoseMax;
   float topMin    = guiglobs::cdsData.rwConfig.procedure.key_drbc_threshold;

   // If a valid row wasn't provided, check all the rows.
   if (updatedProdDose < 0)
   {
      for (int procIndex = 0; procIndex < NUM_CRIT_DOSE; procIndex++)
      {
         float prodDose = getDose(procIndex);

         float topMax   = getMaxDose(procIndex);

         /*DataLog (log_level_gui_info) << "Index " << procIndex << " dose: " << prodDose << "; range: [("
             << bottomMin << "; " << bottomMax << "); (" << topMin << "; " << topMax << ")]" << endmsg;*/

         if (!( ((prodDose >= bottomMin) && (prodDose <= bottomMax)) ||
                ((prodDose >= topMin)    && (prodDose <= topMax) )) )
         {
            return true;
         }
      }

      // DataLog (log_level_gui_info) << "No bad split configurations." << endmsg;
   }
   else if (updatedProdRow < 0)
   {
      DataLog(log_level_gui_error) << "Invalid Product Row index: " << updatedProdRow << endmsg;
   }
   // A valid row was provided, so check just the current row.
   else
   {
      float topMax = getMaxDose(updatedProdRow);

      /*DataLog (log_level_gui_info) << "Index " << updatedProdRow << " dose: " << updatedProdDose << "; range: [("
         << bottomMin << "; " << bottomMax << "); (" << topMin << "; " << topMax << ")]" << endmsg;*/

      return !( ((updatedProdDose >= bottomMin) && (updatedProdDose <= bottomMax)) ||
                ((updatedProdDose >= topMin)    && (updatedProdDose <= topMax)) );
   }


   return false;
}

// This function displays two ranges (an upper and a lower).  See IT 8088 for
// the reason for this.
void Screen_CONFIG_RBC::displayCustomDoubleRange (const float bottomRangeMin,
                                                  const float bottomRangeMax,
                                                  const float topRangeMin,
                                                  const float topRangeMax,
                                                  const char* format)
{
   char      tmpTxtStr[80];
   guistring btmMinStr, btmMaxStr, topMinStr, topMaxStr;

   // Set all DisplayText objects to initial strings
   _txtSplitRangeBottomMin.set_string_id(textMiscCfgSplitRangeBottomMin);
   _txtSplitRangeBottomMax.set_string_id(textMiscCfgSplitRangeBottomMax);
   _txtSplitRangeTopMin.set_string_id(textMiscCfgSplitRangeTopMin);
   _txtSplitRangeTopMax.set_string_id(textMiscCfgSplitRangeTopMax);

   // Get the strings from the DisplayText objects
   _txtSplitRangeBottomMin.get_text_direct(btmMinStr);
   _txtSplitRangeBottomMax.get_text_direct(btmMaxStr);
   _txtSplitRangeTopMin.get_text_direct(topMinStr);
   _txtSplitRangeTopMax.get_text_direct(topMaxStr);

   // Convert the data to strings and append it
   // to the guistrings
   sprintf(tmpTxtStr, format, bottomRangeMin);
   strcat(btmMinStr, tmpTxtStr);

   sprintf(tmpTxtStr, format, bottomRangeMax);
   strcat(btmMaxStr, tmpTxtStr);

   sprintf(tmpTxtStr, format, topRangeMin);
   strcat(topMinStr, tmpTxtStr);

   sprintf(tmpTxtStr, format, topRangeMax);
   strcat(topMaxStr, tmpTxtStr);

   // Set the DisplayText objects with the new string values.
   _txtSplitRangeBottomMin.set_text_direct(btmMinStr.length(), btmMinStr);
   _txtSplitRangeBottomMax.set_text_direct(btmMaxStr.length(), btmMaxStr);
   _txtSplitRangeTopMin.set_text_direct(topMinStr.length(), topMinStr);
   _txtSplitRangeTopMax.set_text_direct(topMaxStr.length(), topMaxStr);

   // Allocate all of 'em.
   _txtSplitRangeBottomMin.allocate_resources(*this);
   _txtSplitRangeBottomMax.allocate_resources(*this);
   _txtSplitRangeTopMin.allocate_resources(*this);
   _txtSplitRangeTopMax.allocate_resources(*this);
   _txtSplitRangeBottomLbl.allocate_resources(*this);
   _txtSplitRangeTopLbl.allocate_resources(*this);
}

void Screen_CONFIG_RBC::clearCustomDoubleRange ()
{   // Deallocate all these text strings.
   _txtSplitRangeBottomMin.deallocate_resources();
   _txtSplitRangeBottomMax.deallocate_resources();
   _txtSplitRangeTopMin.deallocate_resources();
   _txtSplitRangeTopMax.deallocate_resources();
   _txtSplitRangeBottomLbl.deallocate_resources();
   _txtSplitRangeTopLbl.deallocate_resources();
}

//
///////////////////////////////////////////////////////////////////////////////
// display the alert message
void Screen_CONFIG_RBC::display_alert (const float dose)
{
   if (dose < MinRBCProductDose /*150*/ ||
       (dose > MaxRBCProductDose /*250*/ && dose < MinDRBCProductDose /*300*/) ||
       dose > MaxDRBCProductDose /*500*/)
   {
      guiglobs::apheresis_status_line->reparent_status_line(*this);
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatDoseRangeError);
   }
   else guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
}


//
///////////////////////////////////////////////////////////////////////////////
// end of cfg_rbc.cpp file

/* FORMAT HASH 50f4e8c99f6e8f86048fe59735c8c231 */
