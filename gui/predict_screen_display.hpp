/************************> FOCUSSED GUI DOMAIN <****************************

  Here we pull out some of the more mechanical and less interesting parts of the procedure select
  screen.  This is still rather notional, but is a big improvement on the way things used to be.
*/

// Single include define guardian
#ifndef PREDICT_SCREEN_DISPLAY_HPP
#define PREDICT_SCREEN_DISPLAY_HPP


// GUI class includes
#include "string_id.h"
#include "display_text.h"
#include "guistring.h"
#include "button_bitmap.h"

class Screen_PREDICT;

class predict_buttons
{
public:
   predict_buttons();

   // a reasonably complete set of predict-specific button ops
   void allocate_prev (Screen_PREDICT& parent);
   void allocate_next (Screen_PREDICT& parent);
   void allocate_flowrate (Screen_PREDICT& parent);
   void allocate_time (Screen_PREDICT& parent);
   void allocate_MoreOrLess (Screen_PREDICT& parent); // as 'plus'
   void allocate_AF_Buttons (Screen_PREDICT& parent);

   void deallocate_prev ();
   void deallocate_next ();
   void deallocate_flowrate ();
   void deallocate_time ();
   void deallocate_MoreOrLess ();
   void deallocate_AF_Buttons ();

   void deallocate_all ();

   void enable_prev (); // enable callbacks
   void enable_next ();
   void enable_flowrate ();
   void enable_time ();
//   void enable_MoreOrLess();
//
//
   void showActiveMuteBtn ();
   void showDeactiveMuteBtn ();

   void freeze (bool amap);
   void thaw (bool amap, int state);

   void disable_prev (); // disable callbacks
   void disable_next ();
   // void disable_time();
   // void disable_flowrate();
   void disable_MoreOrLess ();


   void set_MoreOrLess_plus ();
   void set_MoreOrLess_minus ();


private:  // data

   Button_Bitmap _btnMoreOrLess;
   Button_Bitmap _btnTime;
   Button_Bitmap _btnFlowRate;
   Button_Bitmap _btnPrev;
   Button_Bitmap _btnNext;
   Button_Bitmap _btnAdjust_AF;
   Button_Bitmap _btnMute_AF;

   Bitmap        _bmpFlowBtnBed;
   Bitmap        _bmpTimeAndFlowIcons;

};

struct predict_keypad
{
   predict_keypad();

   void init_strings ();

   // transitions to keypad...

   const guistring& time_keypad_stateless () { return _gstrTimeKeypadAllocStateless; }
   const guistring& time_keypad_static   () { return _gstrTimeKeypadAllocStatic;   }
   const guistring& flowrate () { return _gstrCalcStringFlowRate; }

   ////////////// data
   guistring _gstrCalcStringFlowRate;
   guistring _gstrTimeKeypadAllocStatic;
   guistring _gstrTimeKeypadAllocStateless;

};

struct predict_units
{
   predict_units();

   void allocate_resources ();

   Display_Text _txtUnitsInitPltX10,
                _txtUnitsInitPltVol,
                _txtUnitsInitPltExp,
                _txtUnitsInitPlas,
                _txtUnitsInitRbc,
                _txtUnitsInitTime,
                _txtUnitsInitFlowRate;



   static guistring _gstrPltUnits;
   static guistring _gstrPltVolUnits;
   static guistring _gstrPltExponent;
   static guistring _gstrPlasUnits;
   static guistring _gstrRbcUnits;
   static guistring _gstrTimeUnits;
   static guistring _gstrFlowRateUnits;

};


//////////////////////////////////////////////////////////////////////

struct display_value // notional, holds Display_Text for a value and its units
{
   display_value(TEXT_STRING_ID data_init,
                 TEXT_STRING_ID unit_init);

   virtual ~display_value() {}

   Display_Text data;
   Display_Text units;

   void allocate_resources (Screen_PREDICT& parent);
   void deallocate_resources ();

   void set (const guistring& d);
   void clear (); // set to blank

   // used for setting ids to alternate values
   void set_string_id (TEXT_STRING_ID data_alt,
                       TEXT_STRING_ID unit_alt);

private:

   virtual void set_units () {}

};

// display values of specific physical values
struct display_plt_vol
   : public display_value
{
   display_plt_vol (TEXT_STRING_ID data_init, TEXT_STRING_ID unit_init)
      : display_value(data_init, unit_init) {}
   virtual void set_units () { units.set_text_direct(predict_units::_gstrPltVolUnits); }
};

struct display_plasma
   : public display_value
{
   display_plasma (TEXT_STRING_ID data_init, TEXT_STRING_ID unit_init)
      : display_value(data_init, unit_init) {}
   virtual void set_units () { units.set_text_direct(predict_units::_gstrPlasUnits); }
};

struct display_rbc
   : public display_value
{
   display_rbc (TEXT_STRING_ID data_init, TEXT_STRING_ID unit_init)
      : display_value(data_init, unit_init) {}
   virtual void set_units () { units.set_text_direct(predict_units::_gstrRbcUnits); }
};

struct display_time
   : public display_value
{
   display_time (TEXT_STRING_ID data_init, TEXT_STRING_ID unit_init)
      : display_value(data_init, unit_init) {}
   virtual void set_units () { units.set_text_direct(predict_units::_gstrTimeUnits); }
};

struct display_flowrate
   : public display_value
{
   display_flowrate (TEXT_STRING_ID data_init, TEXT_STRING_ID unit_init)
      : display_value(data_init, unit_init) {}
   virtual void set_units () { units.set_text_direct(predict_units::_gstrFlowRateUnits); }
};





struct PlateletText
{
   PlateletText(TEXT_STRING_ID data_init,
                TEXT_STRING_ID unit_init,
                TEXT_STRING_ID exp_init);


   void allocate_resources (Screen_PREDICT& p);
   void deallocate_resources ();

   void PlateletText::set (const guistring& _gstrDataPlt);
   void clear ();

   Display_Text data,
                units,
                exponent;

};



class procedure_value // collects guistring and the value it displays.
{
public:

   // default constructed procedure_values will be rounded as in procedure_status.cpp.
   procedure_value();

   // Use this ctor to use a format string to do the conversion
   procedure_value(const char* format_str, unsigned max_length);

   procedure_value& operator = (float); // assign from float in the most natural manner

   operator const char*() const;           // allow implicit conversions back to the
   operator const guistring&() const;      // type of the original data
   operator float() const { return d; }


   bool operator > (float f); // The only math function we need at the moment.


   void reset ();             // Reset values to 'null' but retain the format string

private:

   void convert () const;

   mutable guistring s;      // the guistring is recomputed only when necessary
   float             d;

   const char*       format; // null indicates to do the simple conversion
   unsigned          max_len;

};


struct ProductOptionStruct;

struct procedure_text  // guistrings used to display procedures in the selection window
{
   procedure_value platelets;
   procedure_value plate_vol; // amap
   procedure_value plasma;
   procedure_value rbc;
   procedure_value time;
   procedure_value flowrate;
   procedure_value comp_plasma;
   procedure_value ac_plasma;      // ac plasma volume

   procedure_text();

   procedure_text& operator = (const ProductOptionStruct&);

   void clear ();

};


#endif /* ifndef PREDICT_HPP */

/* FORMAT HASH bff901451dd284c58617267dd6c4e4fa */
