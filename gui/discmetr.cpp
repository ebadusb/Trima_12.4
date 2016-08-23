/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Discrete_Meter

*     File name:   discmetr.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/
#include <vxworks.h>
#include <math.h>

// Trima #INCLUDES


// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "discmetr.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Discrete_Meter::Discrete_Meter (BITMAP_ID p_frame_bitmap_ID,
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

                                ) :

   frame_bitmap_ID                       (p_frame_bitmap_ID),
   frame_location_X_pixel                (p_frame_location_X_pixel),
   frame_location_Y_pixel                (p_frame_location_Y_pixel),

   meter_offset_X_pixel                  (p_meter_offset_X_pixel),
   meter_offset_Y_pixel                  (p_meter_offset_Y_pixel),
   meter_total_height_pixel              (p_meter_total_height_pixel),
   meter_total_positive_tics             (p_meter_total_positive_tics),
   meter_total_negative_tics             (p_meter_total_negative_tics),

   positive_pressure_value_limit         (p_positive_pressure_value_limit),
   negative_pressure_value_limit         (p_negative_pressure_value_limit),

   last_pressure_mode_enabled            (p_last_pressure_mode_enabled),

   positive_inactive_tic_bitmap_ID       (p_positive_inactive_tic_bitmap_ID),
   positive_active_tic_bitmap_ID         (p_positive_active_tic_bitmap_ID),
   positive_last_active_tic_bitmap_ID    (p_positive_last_active_tic_bitmap_ID),
   positive_offscale_tic_bitmap_ID       (p_positive_offscale_tic_bitmap_ID),

   negative_inactive_tic_bitmap_ID       (p_negative_inactive_tic_bitmap_ID),
   negative_active_tic_bitmap_ID         (p_negative_active_tic_bitmap_ID),
   negative_last_active_tic_bitmap_ID    (p_negative_last_active_tic_bitmap_ID),
   negative_offscale_tic_bitmap_ID       (p_negative_offscale_tic_bitmap_ID),


// Initializations

   first_display (false),
   current_pressure_value_is_reset (1),                                // 1 == yes, reset
   last_pressure_value_is_reset (1),                                   // 1 == yes, reset

// Constituent object(s)

   frame_bitmap (p_frame_location_X_pixel,
                 p_frame_location_Y_pixel,
                 p_frame_bitmap_ID),

   meter_bitmaps(NULL)

{
   set_positive_pressure_value_limit (positive_pressure_value_limit) ;
   set_negative_pressure_value_limit (negative_pressure_value_limit) ;

   pixel_incr = meter_total_height_pixel / get_total_tics () ;

} // END of Discrete_Meter CONSTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Discrete_Meter::~Discrete_Meter ()
{

   // attempt to deallocate resources in case the owner forgot to
   deallocate_resources ();

} // END of Discrete_Meter DESTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Discrete_Meter::allocate_resources (OSWindow& parent)
{
   // Allocate frame
   frame_bitmap.allocate_resources (parent);

   // Create an array of pointers to the tic bitmaps and populate the array
   const int bitmdex = get_total_tics();

   meter_bitmaps = (Bitmap**)GUI_POOL_MALLOC(bitmdex * sizeof(Bitmap*));

   if (meter_bitmaps == NULL)
   {
      anAlarmMsg alarm(BITMAP_LOAD_FAILURE);
      alarm.setAlarm("bitmap alloc failed");
      return;
   }

   BITMAP_ID pos_or_neg;

   for (int i = 0; i < bitmdex; i++)
   {
      if ( (i + 1) <= meter_total_positive_tics)
      {
         pos_or_neg = positive_inactive_tic_bitmap_ID;
      }
      else
      {
         pos_or_neg = negative_inactive_tic_bitmap_ID;
      }

      // do the c++ allocation
      meter_bitmaps[i] = new Bitmap (frame_location_X_pixel + meter_offset_X_pixel,
                                     frame_location_Y_pixel + meter_offset_Y_pixel + (pixel_incr * i),
                                     pos_or_neg);

      // do the photon resource allocation and tie it to the container parent
      meter_bitmaps[i]->allocate_resources(parent);
   }

   first_display = true;

}




////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Discrete_Meter::deallocate_resources ()
{
   if (meter_bitmaps != NULL)
   {

      // delete all the bitmaps
      int bitmdex = get_total_tics () ;

      for (int i = 0; i < bitmdex; i++)
      {
         // then delete the bitmap
         delete meter_bitmaps[i];
         meter_bitmaps[i] = NULL;

      }  // End of: for (int i = 0; i < bitmdex; i++)

      // delete the tic bitmap array
      GUI_POOL_FREE(meter_bitmaps);
      meter_bitmaps = NULL ;

      // Deallocate frame
      frame_bitmap.deallocate_resources () ;
   }


} // END of Discrete_Meter deallocate_resources




////////////////////////////////////////////////////////////////////////////////
// GET_TIC_ARRAY_INDEX

int Discrete_Meter::get_tic_array_index (float pressure_value)
{
   int   tic ;
   float float_val_per_tic ;

   // Calculate the value of a tic (either pos or neg)
   if (pressure_value >= 0.0)
   {
      // if there's no meter for this range, return a -1
      if (get_positive_tics() <= 0)
      {
         return (-1) ;
      }
      else
      {
         // calculate the value per tic
         float_val_per_tic = get_positive_pressure_value_limit() / get_positive_tics() ;

         // calculate how many of these are needed to represent the pressure_value

         tic = (int)ceil (pressure_value / float_val_per_tic) ;

         // limit the tic range to the number of meter tics
         if (tic > get_positive_tics())
         {
            tic = get_positive_tics() ;
         }

         // 0 tic becomes tic 1
         if (tic <= 0)
         {
            tic = 1 ;
         }

         // transform it to go positive "up" the bitmap array instead of "down" the array
         tic =  get_positive_tics() - tic ;

         return (tic) ;
      }

   }
   else  // its the negative end of the scale
   {
      // if there's no meter for this range, return a -1
      if (get_negative_tics() <= 0)
      {
         return (-1) ;
      }
      else
      {
         // calculate the value per tic
         float_val_per_tic = get_negative_pressure_value_limit() / get_negative_tics() ;

         // calculate how many of these are needed to represent the pressure_value

         tic = (int)ceil (pressure_value / float_val_per_tic) ;

         // limit the tic range to the number of meter tics
         if (tic > get_negative_tics())
         {
            tic = get_negative_tics() ;
         }

         // 0 tic becomes tic 1
         if (tic <= 0)
         {
            tic = 1 ;
         }

         // transform it to go negative "down" the bitmap array offset from the positive array
         tic =  get_positive_tics() + tic - 1 ;

         return (tic) ;
      }
   }

} // END of Discrete_Meter get_tic_array_index




////////////////////////////////////////////////////////////////////////////////
// SET_TIC_BITMAP

void Discrete_Meter::set_tic_bitmap (int display_bar_index, BITMAP_ID display_graphic)
{

   if ( meter_bitmaps != NULL &&
        display_bar_index >= 0 &&
        display_bar_index < get_total_tics() )
   {
      meter_bitmaps[display_bar_index] -> set_id (display_graphic) ;
   }

}  // END of Discrete_Meter set_tic_bitmap




////////////////////////////////////////////////////////////////////////////////
// RESET_ALL_VALUES

void Discrete_Meter::reset_all_values ()
{

   if (!current_pressure_value_is_reset)
   {
      if (current_pressure_value >= 0.0)
      {
         set_tic_bitmap (get_tic_array_index (current_pressure_value), positive_inactive_tic_bitmap_ID) ;
      }
      else
      {
         set_tic_bitmap (get_tic_array_index (current_pressure_value), negative_inactive_tic_bitmap_ID) ;
      }

      current_pressure_value          = 0.0 ;
      current_pressure_value_is_reset = 1 ;
   }


   if (!last_pressure_value_is_reset && last_pressure_mode_enabled)
   {
      if (last_pressure_value >= 0.0)
      {
         set_tic_bitmap (get_tic_array_index (last_pressure_value), positive_inactive_tic_bitmap_ID) ;
      }
      else
      {
         set_tic_bitmap (get_tic_array_index (last_pressure_value), negative_inactive_tic_bitmap_ID) ;
      }

      last_pressure_value          = 0.0 ;
      last_pressure_value_is_reset = 1 ;
   }


} // END of Discrete_Meter reset_all_values




////////////////////////////////////////////////////////////////////////////////
// SET_METER_VALUE

int Discrete_Meter::set_meter_value (float meter_value)
{
   float save_current ;
   int   off_meter = 1 ;

   save_current = get_meter_value () ;


   // If both the new and the currently displayed values yield the same tic index
   // don't refresh the meter UNLESS: we're working the ends of the meter because
   // the maximum meter value and the out-of-range meter values resolve to the
   // same tic. If we don't make exceptions with these then movement between the
   // max meter value and the out-of-range values won't change the tic's bitmap.
   // Also, if this is the first call to the meter, unconditionally display the
   // value.

   if ((get_tic_array_index (meter_value) == get_tic_array_index (save_current))
       && !out_of_range (meter_value)
       && !out_of_range (save_current)
       && !first_display
       )
   {
      return (1) ; // no change in meter indication, so just return
   }


   // reset the meter in prep for a change
   reset_all_values() ;

   if (!first_display)
   {
      // first draw the last value bar using the current value
      set_last_meter_value (save_current) ;
   }
   else
   {
      first_display = false;
   }


   // now draw the new value (as the current)

   if (meter_value >= 0.0) // positive
   {
      if (out_of_range (meter_value))
      {
         set_tic_bitmap (get_tic_array_index (get_positive_pressure_value_limit ()), positive_offscale_tic_bitmap_ID) ;
         off_meter = 0;
      }
      else
      {
         set_tic_bitmap (get_tic_array_index (meter_value), positive_active_tic_bitmap_ID) ;
      }
   }
   else // negative
   {
      if (out_of_range (meter_value))
      {
         set_tic_bitmap (get_tic_array_index (get_negative_pressure_value_limit ()), negative_offscale_tic_bitmap_ID) ;
         off_meter = 0;
      }
      else
      {
         set_tic_bitmap (get_tic_array_index (meter_value), negative_active_tic_bitmap_ID) ;
      }

   }

   current_pressure_value          = meter_value ;
   current_pressure_value_is_reset = 0 ;

   return (off_meter) ;


} // END of Discrete_Meter set_meter_value




////////////////////////////////////////////////////////////////////////////////
// SET_LAST_METER_VALUE

int Discrete_Meter::set_last_meter_value (float meter_value)
{

   // See if the last meter function is enabled.
   if (!last_pressure_mode_enabled)
   {
      return (0) ;
   }

   // now draw the new value (as the current)
   int off_meter = 1 ;

   if (meter_value >= 0.0)   // positive
   {
      if (out_of_range (meter_value))
      {
         // display nothing
         off_meter = 0;
      }
      else
      {
         set_tic_bitmap (get_tic_array_index (meter_value), positive_last_active_tic_bitmap_ID) ;
         last_pressure_value_is_reset = 0 ;
      }
   }
   else   // negative
   {
      if (out_of_range (meter_value))
      {
         // display nothing
         off_meter = 0;
      }
      else
      {
         set_tic_bitmap (get_tic_array_index (meter_value), negative_last_active_tic_bitmap_ID) ;
         last_pressure_value_is_reset = 0 ;
      }

   }

   last_pressure_value = meter_value ;

   return (off_meter) ;


} // END of Discrete_Meter set_last_meter_value



////////////////////////////////////////////////////////////////////////////////
// OUT_OF_RANGE

int Discrete_Meter::out_of_range (float meter_value)
{
   if (meter_value > get_positive_pressure_value_limit () ||
       meter_value < get_negative_pressure_value_limit ()
       )
   {
      return (1) ;
   }
   else
   {
      return (0) ;
   }

} // END of Discrete_Meter out_of_range

/* FORMAT HASH 01b8543d489a76f57a7b34fbab99aa3e */
