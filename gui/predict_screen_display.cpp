#include "predict_screen_display.hpp"

#include "predict_screen.hpp"

#include "frmtdec.hpp"
#include "frmtstr.hpp"

#include "predict_msgs.h" // holds defn of ProductOptionStruct

// "C" callback stubs for redirection to set callback method
static void cbPrevBtn             (void* data);
static void cbNextBtn             (void* data);
static void cbTimeBtn             (void* data);
static void cbAdjustFlowRate      (void* data);
static void callback_more_or_less (void* data);
static void cbAFadjustBtn (void* data);
static void cbAFmuteBtn (void* data);


predict_buttons::predict_buttons()
   : _btnMoreOrLess (GUI_BUTTON_PREDICT_MORE_OR_LESS,
                     56,
                     BUTN_SELECT_NEXT_Y,
                     textEmpty,
                     BITMAP_BUTTON_PLUS,
                     BITMAP_BUTTON_SELECT_NEXT_DN),


     _btnTime (GUI_BUTTON_PRED_DONOR_TIME,
               495, 366, textEmpty,
               BITMAP_BUTTON_DONOR_TIME_UP,
               BITMAP_BUTTON_SMALL_BOT_TEAL_DOWN),


     _btnFlowRate (GUI_BUTTON_PRED_FLOW_RATE,
                   59, 366, textEmpty,
                   BITMAP_BUTTON_FLOW_RATE_UP,
                   BITMAP_BUTTON_SMALL_BOT_TEAL_DOWN),

     _btnPrev (GUI_BUTTON_PRED_SELECT_PREVIOUS,
               BUTN_SELECT_PREVIOUS_X,
               BUTN_SELECT_PREVIOUS_Y,
               textButtonLongPredScrollPrev,
               BITMAP_BUTTON_SELECT_PREVIOUS_UP,
               BITMAP_BUTTON_SELECT_PREVIOUS_DN),


     _btnNext (GUI_BUTTON_PRED_SELECT_NEXT,
               BUTN_SELECT_NEXT_X,
               BUTN_SELECT_NEXT_Y,
               textButtonLongPredScrollNext,
               BITMAP_BUTTON_SELECT_NEXT_UP,
               BITMAP_BUTTON_SELECT_NEXT_DN),

     _btnAdjust_AF  (GUI_BUTTON_PRED_AF_ADJUST,
                     112, 434,
                     textEmpty,
                     BITMAP_AUTO_FLOW_PREDICT_ADJ_UP,
                     BITMAP_AUTO_FLOW_PREDICT_ADJ_DOWN),



     _btnMute_AF  (GUI_BUTTON_PRED_AF_MUTE,
                   53, 366,
                   textEmpty,
                   BITMAP_BUTTON_ACTIVATE_SOUND,
                   BITMAP_BUTTON_SOUND_DN),



     _bmpFlowBtnBed        (55, 363, BITMAP_BUTTON_FLOW_RATE_SINK),
     _bmpTimeAndFlowIcons  (490, 245, BITMAP_PRED_TIME_AND_FLOW_ICONS)

{}


void predict_buttons::allocate_AF_Buttons (Screen_PREDICT& parent)
{
   if (!_btnAdjust_AF.allocated())
   {
      _btnAdjust_AF.allocate_resources (parent);
      _btnAdjust_AF.set_callback (cbAFadjustBtn, (void*)&parent);
   }

//   if (!_btnMute_AF.allocated())
//   {
//      _btnMute_AF.allocate_resources (parent);
//      _btnMute_AF.set_callback (cbAFmuteBtn, (void*)&parent);
//   }
}



void predict_buttons::allocate_prev (Screen_PREDICT& parent)
{
   if (!_btnPrev.allocated())
   {
      _btnPrev.allocate_resources (parent);
      _btnPrev.set_callback (cbPrevBtn, (void*)&parent);
   }
}

void predict_buttons::allocate_next (Screen_PREDICT& parent)
{
//   if (_btnNext.allocated()) return;
   _btnNext.allocate_resources (parent);
   _btnNext.set_callback (cbNextBtn, (void*)&parent);
}



void predict_buttons::allocate_flowrate (Screen_PREDICT& parent)
{
   _bmpTimeAndFlowIcons.allocate_resources(parent);
   _bmpFlowBtnBed.allocate_resources(parent);
   _btnFlowRate.allocate_resources(parent);

   _btnFlowRate.set_callback(cbAdjustFlowRate, (void*)&parent);

}

void predict_buttons::allocate_time (Screen_PREDICT& parent)
{
   _btnTime.allocate_resources (parent);
   _btnTime.set_callback (cbTimeBtn, (void*)&parent);
}


void predict_buttons::allocate_MoreOrLess (Screen_PREDICT& parent)
{
   if (!_btnMoreOrLess.allocated())
   {
      DataLog(log_level_gui_info) << "BITMAP_BUTTON_PLUS" << endmsg;
      _btnMoreOrLess.set_up_bitmap(BITMAP_BUTTON_PLUS);
      _btnMoreOrLess.allocate_resources(parent);
      _btnMoreOrLess.set_callback(callback_more_or_less, (void*)&parent);
   }

}


void predict_buttons::deallocate_prev () {_btnPrev.deallocate_resources(); }
void predict_buttons::deallocate_next () {_btnNext.deallocate_resources(); }

void predict_buttons::deallocate_flowrate ()
{
   _btnFlowRate.deallocate_resources();
   _bmpFlowBtnBed.deallocate_resources();
   _bmpTimeAndFlowIcons.deallocate_resources();
}

void predict_buttons::deallocate_AF_Buttons ()
{
   _btnAdjust_AF.deallocate_resources();
   _btnMute_AF.deallocate_resources ();

}



void predict_buttons::deallocate_time ()
{
   _btnTime.deallocate_resources();
}

void predict_buttons::deallocate_MoreOrLess ()
{
   _btnMoreOrLess.deallocate_resources();
}

void predict_buttons::deallocate_all ()
{
   _btnTime.deallocate_resources();
   _btnPrev.deallocate_resources();
   _btnNext.deallocate_resources();

   _bmpFlowBtnBed.deallocate_resources();
   _bmpTimeAndFlowIcons.deallocate_resources();
   _btnFlowRate.deallocate_resources();

   _btnMoreOrLess.deallocate_resources();

}

void predict_buttons::enable_prev    () {_btnPrev.    enable_callbacks(); }
void predict_buttons::enable_next    () {_btnNext.    enable_callbacks(); }
void predict_buttons::enable_flowrate () {_btnFlowRate.enable_callbacks(); }
void predict_buttons::enable_time    () {_btnTime.    enable_callbacks(); }

// void predict_buttons::enable_MoreOrLess();

enum AMAP_STATE
{
   SHOW_ONE,
   SHOW_ALL
};

void predict_buttons::freeze (bool amap) // disable all buttons
{
   _btnTime.disable_callbacks();
   _btnPrev.disable_callbacks();
   _btnNext.disable_callbacks();
   _btnFlowRate.disable_callbacks();

   if (amap)
   {
      DataLog (log_level_gui_info) << "amapp: disable more or less call back" << endmsg;
      _btnMoreOrLess.disable_callbacks();
   }

}

void predict_buttons::thaw (bool amap, int amapState)
{
   // Enable the rest
   //
   _btnTime.enable_callbacks();
   _btnNext.enable_callbacks();
   _btnPrev.enable_callbacks();


   ///////////////  AMAP plt ///////////////////////////
   if (amap)
   {
      // _btnMoreOrLess.allocate_resources(*this);
      _btnMoreOrLess.enable_callbacks();

      if (amapState == SHOW_ALL)
      {

         DataLog (log_level_gui_info) << "BITMAP_BUTTON_MINUS" << endmsg;
         _btnMoreOrLess.set_up_bitmap(BITMAP_BUTTON_MINUS);
      }
      else
      {
         DataLog (log_level_gui_info) << "BITMAP_BUTTON_PLUS" << endmsg;
         _btnMoreOrLess.set_up_bitmap(BITMAP_BUTTON_PLUS);
      }
   }

}


// void predict_buttons::disable_time();
// void predict_buttons::disable_flowrate();

void predict_buttons::disable_prev () {_btnPrev.disable_callbacks(); }
void predict_buttons::disable_next () {_btnNext.disable_callbacks(); }

void predict_buttons::disable_MoreOrLess ()
{
   DataLog (log_level_gui_info) << "amapp: disable more or less call back" << endmsg;
   _btnMoreOrLess.disable_callbacks();
}


void predict_buttons::set_MoreOrLess_plus ()
{
   DataLog (log_level_gui_info) << "BITMAP_BUTTON_PLUS" << endmsg;
   _btnMoreOrLess.set_up_bitmap(BITMAP_BUTTON_PLUS);

}

void predict_buttons::set_MoreOrLess_minus ()
{
   DataLog (log_level_gui_info) << "BITMAP_BUTTON_MINUS" << endmsg;
   _btnMoreOrLess.set_up_bitmap(BITMAP_BUTTON_MINUS);
}

void predict_buttons::showActiveMuteBtn ()
{
   if ( _btnMute_AF.allocated() )
   {
      _btnMute_AF.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
   }
}

void predict_buttons::showDeactiveMuteBtn ()
{

   if ( _btnMute_AF.allocated() )
   {
      _btnMute_AF.set_up_bitmap (BITMAP_BUTTON_DISABLE_SOUND);
   }
}


//////////////////////////////////////////////////////////////////////
//
//  Predict button callback functions


void cbAFmuteBtn (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND, SOUND_CLEAR);

   ((Screen_PREDICT*)data) -> muteBeepingSound ();
   // Actually call the process button routine for the system setup button

}  // END of CALLBACK_PREVIOUS_OPTION

void cbAFadjustBtn (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_PREDICT*)data) ->processAFAdjustBtn ();

}  // END of CALLBACK_PREVIOUS_OPTION


static void cbPrevBtn (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_PREDICT*)data) -> processPrevBtn ();

}  // END of CALLBACK_PREVIOUS_OPTION



static void cbTimeBtn (void* data)

{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_PREDICT*)data) -> processTimeBtn ();

}  // END of CALLBACK_BLOOD_TYPE

static void cbNextBtn (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_PREDICT*)data) -> processNextBtn ();

}  // END of CALLBACK_NEXT_OPTION


void cbAdjustFlowRate (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_PREDICT*)data) -> processAdjustFlowRateBtn ();
}


void callback_more_or_less (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_PREDICT*)data) -> processMoreOrLess();

}  // END of CALLBACK_BLOOD_TYPE


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//  KEYPAD


predict_keypad::predict_keypad() {}

void predict_keypad::init_strings ()
{
   // initialize the allocation buffers
   // There are two because if this is the stateless version of
   // this screen, we need to take special measures to ensure
   // that the keypad appears on top.
   _gstrTimeKeypadAllocStatic.initialize();
   _gstrTimeKeypadAllocStateless.initialize();
   _gstrCalcStringFlowRate.initialize();

   // convert keypad type into corresponding string value
   sprintf(_gstrTimeKeypadAllocStatic, "%d", (int)DONOR_TIME);

   // this keypad allocation string gets the "child of stateless" string attached
   // so as to take the aforementioned special measures.  Keypad should
   // ignore this, but scrncomm should deal with it accordingly.
   sprintf(_gstrTimeKeypadAllocStateless, "%d~%s", (int)DONOR_TIME, CHILD_OF_STATELESS);

   char temp_str[80] = "";

   sprintf(temp_str, "%d", STANDARD);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%f", guiglobs::flowAdjMax);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%f", guiglobs::flowAdjMin);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%d", 40);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%d", 166);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%d", textKeypadUnitsFlow);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);
   strcpy(temp_str, "");

   strcat(_gstrCalcStringFlowRate, "###");
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, " ");
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%d", (CONFIG_DATA_KEYS)0);
   strcat(_gstrCalcStringFlowRate, temp_str);
   strcat(_gstrCalcStringFlowRate, STRING_TOKEN);

   sprintf(temp_str, "%d", 1);
   strcat(_gstrCalcStringFlowRate, temp_str);

   sprintf(temp_str, "%s%d%s0%s%d%s %s %s0%s1",
           STRING_TOKEN,
           RIGHTTOLEFT,
           STRING_TOKEN,
           STRING_TOKEN,
           textEmpty,
           STRING_TOKEN,
           STRING_TOKEN,
           STRING_TOKEN,
           STRING_TOKEN);

   strcat(_gstrCalcStringFlowRate, temp_str);

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//  predict unit strings

predict_units::predict_units()
   : _txtUnitsInitPltX10    (textDataUnitsPredPltYield),
     _txtUnitsInitPltVol    (textDataUnitsMl),
     _txtUnitsInitPltExp    (textDataUnitsPredUnitPltYieldExp),
     _txtUnitsInitPlas      (textDataUnitsMl),
     _txtUnitsInitRbc       (textDataUnitsMl),
     _txtUnitsInitTime      (textDataUnitsPredDuration),
     _txtUnitsInitFlowRate  (textDataUnitsPredFlowRate)
{}



guistring predict_units::_gstrPltUnits;
guistring predict_units::_gstrPltVolUnits;
guistring predict_units::_gstrPltExponent;
guistring predict_units::_gstrPlasUnits;
guistring predict_units::_gstrRbcUnits;
guistring predict_units::_gstrTimeUnits;
guistring predict_units::_gstrFlowRateUnits;

void predict_units::allocate_resources ()
{
   // set string id's for units
   _txtUnitsInitPltX10.set_string_id   (textDataUnitsPredPltYield);
   _txtUnitsInitPltVol.set_string_id   (textDataUnitsMl);
   _txtUnitsInitPltExp.set_string_id   (textDataUnitsPredUnitPltYieldExp);
   _txtUnitsInitPlas.set_string_id     (textDataUnitsMl);
   _txtUnitsInitRbc.set_string_id      (textDataUnitsMl);
   _txtUnitsInitTime.set_string_id     (textDataUnitsPredDuration);
   _txtUnitsInitFlowRate.set_string_id (textDataUnitsPredFlowRate);

   // now get the text that will be used in product labels (e.g. the unit text) out of the units
   // display text and into gui strings.
   _txtUnitsInitPltX10   .get_text_direct (_gstrPltUnits);
   _txtUnitsInitPltVol   .get_text_direct (_gstrPltVolUnits);
   _txtUnitsInitPltExp   .get_text_direct (_gstrPltExponent);
   _txtUnitsInitPlas     .get_text_direct (_gstrPlasUnits);
   _txtUnitsInitRbc      .get_text_direct (_gstrRbcUnits);
   _txtUnitsInitTime     .get_text_direct (_gstrTimeUnits);
   _txtUnitsInitFlowRate .get_text_direct (_gstrFlowRateUnits);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///
///  display_value (notional)

#define PREDICT_NULL_STR " "


display_value::display_value(TEXT_STRING_ID data_init,
                             TEXT_STRING_ID unit_init)
   : data(data_init),
     units(unit_init)
{}


void display_value::allocate_resources (Screen_PREDICT& parent)
{
   data.allocate_resources(parent);
   units.allocate_resources(parent);
}

void display_value::deallocate_resources ()
{
   data.deallocate_resources();
   units.deallocate_resources();

}

void display_value::set (const guistring& d)
{
   data.set_text_direct(d);
   set_units();
}

void display_value::clear ()
{
   data.set_text_direct (BLANK_STRING);
   units.set_text_direct(PREDICT_NULL_STR);
}

void display_value::set_string_id (TEXT_STRING_ID d_id,
                                   TEXT_STRING_ID u_id)
{
   data.set_string_id(d_id);
   units.set_string_id(u_id);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//  predict text strings

PlateletText::PlateletText(TEXT_STRING_ID data_init,
                           TEXT_STRING_ID unit_init,
                           TEXT_STRING_ID exp_init)

   : data     (data_init),     // exists
     units    (unit_init),     // 3.3a new
     exponent (exp_init)       // 3.3a new

{}


void PlateletText::allocate_resources (Screen_PREDICT& parent)
{
   data    .allocate_resources (parent);
   units   .allocate_resources (parent);
   exponent.allocate_resources (parent);
}

void PlateletText::deallocate_resources ()
{
   data    .deallocate_resources();
   units   .deallocate_resources();
   exponent.deallocate_resources();
}

void PlateletText::set (const guistring& _gstrDataPlt)
{
   data    .set_text_direct (_gstrDataPlt);
   units   .set_text_direct (predict_units::_gstrPltUnits);
   exponent.set_text_direct (predict_units::_gstrPltExponent);
}

void PlateletText::clear ()
{
   data    .set_text_direct (BLANK_STRING);
   units   .set_text_direct (PREDICT_NULL_STR);
   exponent.set_text_direct (PREDICT_NULL_STR);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//  process_value objects

procedure_value::procedure_value()
   : s(), d(0), format(NULL), max_len(0) {}

procedure_value::procedure_value(const char* format_str, unsigned max_length)
   : s(),
     d(0),
     format(format_str),
     max_len(max_length)
{}


procedure_value& procedure_value::operator = (float f)
{
   d = f;
   return *this;
}

procedure_value::operator const char* () const
{
   convert();
   return (const char*)(s);
}

procedure_value::operator const guistring& () const
{
   convert();
   return s;
}

bool procedure_value::operator > (float f)
{
   return d > f;
}


void procedure_value::convert () const
{
   static format_string convert_to_string;

   if (format)
   {
      convert_to_string.convert_number_to_string(s, format, d, max_len);
   }
   else
   {
      sprintf(s, "%d", (int)(d + 0.5));
   }
}


void procedure_value::reset ()
{
   d = 0;
   s = guistring();
}


// Here, we hardcode the formatting for each piece of procedure text.  This used to be hard-coded in
// the Screen_PREDICT::formatScreenText member function.
procedure_text::procedure_text()
   : platelets   ("%3.1f",  MAX_LENGTH_PLATELET_ENTRY),
     plate_vol   ("%.0f",  MAX_LENGTH_PLATELET_ENTRY),
     plasma      (),
     rbc         (),
     time        ("%3.0f", MAX_LENGTH_TIME_ENTRY),
     flowrate    ("%3.0f", MAX_LENGTH_FLOW_RATE_ENTRY),
     comp_plasma (),
     ac_plasma   ()

{}

procedure_text& procedure_text::operator = (const ProductOptionStruct& predictData)
{
   platelets = predictData.estimated_platelets;
   plate_vol = predictData.estimated_platelet_volume;
   plasma    = predictData.estimated_plasma;

   // RBC -- different!

   time        = predictData.procedure_time;
   flowrate    = predictData.estimated_qin_max;
   comp_plasma = predictData.comp_pls_volume;
   ac_plasma   = predictData.estimated_plasma_no_ac ;

   return *this;
}

void procedure_text::clear ()
{
   platelets   .reset();
   plate_vol   .reset();
   plasma      .reset();
   rbc         .reset();
   time        .reset();
   flowrate    .reset();
   comp_plasma .reset();
   ac_plasma   .reset();
}

/* FORMAT HASH efaaa1e4f9db0f82f808bf0fb984d670 */
