/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Discrete_Meter

*     File name:    discmetr.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents a meter that will display values using discrete
         rectangular boxes within a meter framework -- much like a dynamic range
         meter on a stereo using LEDs. The constructor (and subsequently method
         calls) will define the graphical elements to be used and the dynamic range
         of the values to be portrayed in the meter.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: K:/BCT_Development/vxWorks/Trima/gui/rcs/discmetr.hpp 1.3 2003/01/06 18:31:07 jl11312 Exp jl11312 $
$Log: discmetr.hpp $
Revision 1.3  2003/01/06 18:31:07  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.2  2002/08/15 22:14:24  sb07663
Compiling Vx version
Revision 1.1  2000/11/01 00:40:20  tw09453
Initial revision
*/


// Single include guardian define
#ifndef DISCMETR_HPP
#define DISCMETR_HPP


// Focussed #INCLUDES

#include "bitmap.h"
#include "display_text.h"
#include "aphwind.hpp"


class Discrete_Meter
{

private:

   // The meter frame bitmap and parameters
   Bitmap    frame_bitmap ;
   BITMAP_ID frame_bitmap_ID ;
   int       frame_location_X_pixel ;
   int       frame_location_Y_pixel ;

   // The meter parameters
   int  meter_offset_X_pixel ;         // offset from upper-left frame location
   int  meter_offset_Y_pixel ;         // offset from upper-left frame location
   int  meter_total_height_pixel ;     // total vertical screen dimension for meter (pixels)
   int  meter_total_positive_tics ;    // number of positive meter bars
   int  meter_total_negative_tics ;    // number of negative meter bars
   bool first_display;                 // true on first display of meter after construction

   // Analog range of meter
   float positive_pressure_value_limit ;                // positive range 0 to +limit
   float negative_pressure_value_limit ;                // negative range 0 to -limit

   // Current meter values
   float current_pressure_value ;
   int   current_pressure_value_is_reset ;                // if true, the current pressure value is not valid -- don't try to display
   float last_pressure_value ;
   int   last_pressure_value_is_reset ;                // if true, the last pressure value is not valid -- don't try to display

   // Last pressure value display mode
   int last_pressure_mode_enabled ;                  // if true, then the last pressure should be shown using the last active tic graphic.
                                                     //    otherwise, don't show any indication of the last pressure.

   // The various meter element bitmap ID's
   BITMAP_ID positive_inactive_tic_bitmap_ID ;
   BITMAP_ID positive_active_tic_bitmap_ID ;
   BITMAP_ID positive_last_active_tic_bitmap_ID ;
   BITMAP_ID positive_offscale_tic_bitmap_ID ;

   BITMAP_ID negative_inactive_tic_bitmap_ID ;
   BITMAP_ID negative_active_tic_bitmap_ID ;
   BITMAP_ID negative_last_active_tic_bitmap_ID ;
   BITMAP_ID negative_offscale_tic_bitmap_ID ;

   // Meter bar array pointer
   Bitmap** meter_bitmaps ;             // the pointer to the "newed" array of bitmaps

   int      pixel_incr ;             // distance between tic bitmaps on the meter.


   // Disallow no-parm constructor; copies, assignments
   Discrete_Meter () ;
   Discrete_Meter(Discrete_Meter const&) ;
   Discrete_Meter& operator = (Discrete_Meter const&) ;


   // SET_LAST_METER_VALUE
   // This method sets the value that the meter will display as the previous value.
   // If the value is "off" the meter, this method will return a 0, otherwise it will return a 1.
   int set_last_meter_value (float meter_value) ;

   // GET_TIC_ARRAY_INDEX
   // This method is passed a value within the positive and negative pressure limits and
   // returns the index of the display bar or "tic" that represents that value. if the value
   // is within the limits and an index can be calculated, this routine returns the positive
   // index, otherwise it returns a -1.
   int get_tic_array_index (float pressure_value) ;

   // SET_TIC_BITMAP
   // This method is passed the index to a display bar or "tic" and the bitmap ID of the
   // graphic it should display. This method will set the corresponding bitmap object to
   // that bitmap.
   void set_tic_bitmap (int display_bar_index, BITMAP_ID display_graphic) ;

   // OUT_OF_RANGE
   // This method will test the value passed and determine whether the value is in or out
   // of range of the meter. If it is out-of-range, it will return a 1, otherwise it will
   // return a 0.
   int out_of_range (float meter_value) ;

   // GET_POSITIVE_TICS
   // This method will return the number of positive tics on the meter
   int get_positive_tics ()
   {
      if (meter_total_positive_tics > 0)
      {
         return (meter_total_positive_tics) ;
      }
      else
      {
         return (0) ;
      }
   }

   // GET_NEGATIVE_TICS
   // This method will return the number of negative tics on the meter
   int get_negative_tics ()
   {
      if (meter_total_negative_tics > 0)
      {
         return (meter_total_negative_tics) ;
      }
      else
      {
         return (0) ;
      }
   }




public:


   /////////////////////////// con/de struction /////////////////////////////

   // CONSTRUCTOR
   Discrete_Meter (BITMAP_ID p_frame_bitmap_ID,
                   int p_frame_location_X_pixel,
                   int p_frame_location_Y_pixel,

                   int p_meter_offset_X_pixel,
                   int p_meter_offset_Y_pixel,
                   int p_meter_total_height_pixel,
                   int p_meter_total_positive_tics,
                   int p_meter_total_negative_tics,

                   float p_positive_pressure_value_limit,
                   float p_negative_pressure_value_limit,

                   int p_last_pressure_mode_enabled,

                   BITMAP_ID p_positive_inactive_tic_bitmap_ID,
                   BITMAP_ID p_positive_active_tic_bitmap_ID,
                   BITMAP_ID p_positive_last_active_tic_bitmap_ID,
                   BITMAP_ID p_positive_offscale_tic_bitmap_ID,

                   BITMAP_ID p_negative_inactive_tic_bitmap_ID,
                   BITMAP_ID p_negative_active_tic_bitmap_ID,
                   BITMAP_ID p_negative_last_active_tic_bitmap_ID,
                   BITMAP_ID p_negative_offscale_tic_bitmap_ID

                   );

   // DESTRUCTOR
   virtual ~Discrete_Meter ();



   /////////////////////////// inline gets and sets /////////////////////////////

   // SET_POSITIVE_PRESSURE_VALUE_LIMIT
   // This method sets the value of the upper or positive pressure represented by the
   // meter. If the set works, this routine returns a 1, otherwise a 0.
   int set_positive_pressure_value_limit (float positive_pressure_value)
   {
      if (positive_pressure_value >= 0.0 && positive_pressure_value != positive_pressure_value_limit)
      {
         // First reset the meter using the old range values
         reset_all_values () ;

         // Now assign the new value
         positive_pressure_value_limit =  positive_pressure_value ;
         return (1) ;
      }
      else
      {
         return (0) ;
      }
   }

   // GET_POSITIVE_PRESSURE_VALUE_LIMIT
   // This method gets the value of the upper or positive pressure represented by the
   // meter.
   float get_positive_pressure_value_limit ()
   {
      return (positive_pressure_value_limit) ;
   }


   // SET_NEGATIVE_PRESSURE_VALUE_LIMIT
   // This method sets the value of the lower or negative pressure represented by the
   // meter. If the set works, this routine returns a 1, otherwise a 0.
   int set_negative_pressure_value_limit (float negative_pressure_value)
   {
      if (negative_pressure_value <= 0.0 && negative_pressure_value != negative_pressure_value_limit)
      {
         // First reset the meter using the old range values
         reset_all_values () ;

         // Now assign the new value
         negative_pressure_value_limit =  negative_pressure_value ;
         return (1) ;
      }
      else
      {
         return (0) ;
      }
   }


   // GET_NEGATIVE_PRESSURE_VALUE_LIMIT
   // This method gets the value of the lower or negative pressure represented by the
   // meter.
   float get_negative_pressure_value_limit ()
   {
      return (negative_pressure_value_limit) ;
   }


   // GET_TOTAL_TICS
   // This method returns the total number of meter bars or "tics" displayed on the
   // meter.
   int get_total_tics ()
   {
      if ((meter_total_positive_tics + meter_total_negative_tics) > 0)
      {
         return (meter_total_positive_tics + meter_total_negative_tics) ;
      }
      else
      {
         return (1) ;           // to allay any divide-by-zero problems -- yes it will be a lousy meter
      }
   }


   // GET_METER_VALUE
   // This method will return the current meter value. If the meter value hasn't been
   // set or is logically invalid, this method will return a 0.0.
   float get_meter_value ()
   {
      if (current_pressure_value_is_reset)
      {
         return (0.0) ;
      }
      else
      {
         return (current_pressure_value) ;
      }
   }


   // GET_LAST_METER_VALUE
   // This method will return the previous meter value. If the meter value hasn't been
   // set or is logically invalid, this method will return a 0.0.
   float get_last_meter_value ()
   {
      if (last_pressure_value_is_reset)
      {
         return (0.0) ;
      }
      else
      {
         return (last_pressure_value) ;
      }
   }


   // SET_LAST_PRESSURE_MODE
   // This method sets the "mode" flag ON for showing the previous value behind the current
   // value on the meter.
   void set_last_pressure_mode ()
   {
      last_pressure_mode_enabled = 1 ;
   }


   // RESET_LAST_PRESSURE_MODE
   // This method resets the "mode" flag OFF for showing the previous value behind the current
   // value on the meter.
   void reset_last_pressure_mode ()
   {
      last_pressure_mode_enabled = 0 ;

   }



   /////////////////////////// method declarations /////////////////////////////

   // RESET_ALL_VALUES
   // This method resets the current and last value and marks the validity flags as false.
   // Any attempt to display these values will be ignored. This will also "blank" the meter
   // so that is shows no value at all.
   void reset_all_values () ;


   // SET_METER_VALUE
   // This method sets the value that the meter will display. If the value is "off" the
   // the meter, this method will return a 0, otherwise it will return a 1.
   int set_meter_value (float meter_value) ;


   // ALLOCATE RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources and the basic window widget.
   void allocate_resources (OSWindow& parent);


   // DEALLOCATE RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources and the basic window widget.
   void deallocate_resources ();



}; // END of Discrete_Meter Class Declaration


// End of single include guardian define
#endif /* ifndef DISCMETR_HPP */

/* FORMAT HASH 94b943bad87a9b382eeed99243fababd */
