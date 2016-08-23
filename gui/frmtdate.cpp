/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Format_date

*     File name:   frmtdate.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.6  2003/01/06 18:31:12Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.5  2002/08/15 22:16:39  sb07663
Compiling version for Vx
Revision 1.4  2001/01/24 23:05:39  tw09453
Format the year to four (instead of two) digits
Revision 1.3  2000/10/08 04:02:18  sb07663
User configuration changes for single stage clinicals
Revision 1.2  1998/10/12 23:22:52  tw09453
Change the interpretation of the config date format: YEAR_DATE =
dd/mm/yy. DAY_DATE = yy/mm/dd.
Revision 1.1  1998/09/04 16:38:20  tw09453
Initial revision
*/
#include <vxworks.h>


// Focussed #INCLUDES

#include "frmtdate.hpp"

// External References

#include "guiglobs.hpp"
#include "error.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Format_date::Format_date ()
{}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Format_date::~Format_date ()
{}

////////////////////////////////////////////////////////////////////////////////
// FORMAT_DATE

void Format_date::format_date (char* str_date)
{
   // The expected length of date
   #define DATE_LENGTH 8


   // Given the format of the str_date, these constants locate the
   // individual date components for isolation.
   // YYYYMMDD
   // 01234567
   #define DAY_LOC 6
   #define MON_LOC 4
   #define YR_LOC  0


   // Define the lengths of the individual date components
   #define DAY_LEN 2
   #define MON_LEN 2
   #define YR_LEN 4

   #define DATE_DELIM "/"


   char day[DAY_LEN + 1] = "" ;
   char mon[MON_LEN + 1] = "" ;
   char yr [YR_LEN + 1]  = "" ;


   // Make sure that the date passed is the correct length and all numeric
   if (strspn(str_date, "0123456789") != DATE_LENGTH)
   {
      char msg [200];
      sprintf(msg, "Invalid date passed to this routine during date-formatting : %s", str_date) ;
      _FATAL_ERROR (__FILE__, __LINE__,  msg);
   }

   // Isolate the day
   strncpy (day, &(str_date[DAY_LOC]), DAY_LEN) ;
   day[DAY_LEN] = '\0';


   // Isolate the month
   strncpy (mon, &(str_date[MON_LOC]), MON_LEN) ;
   mon[MON_LEN] = '\0';


   // Isolate the year
   strncpy (yr,  &(str_date[YR_LOC]),   YR_LEN) ;
   yr[YR_LEN] = '\0';


   // Null the date
   *str_date = '\0' ;

   do
   {
      if (guiglobs::cdsData.config.LangUnit.Get().key_date_format == (int)MMDDYY_DATE)
      {
         strcpy (str_date, mon);
         strcat (str_date, DATE_DELIM);
         strcat (str_date, day);
         strcat (str_date, DATE_DELIM);
         strcat (str_date, yr);
         break;
      }


      if (guiglobs::cdsData.config.LangUnit.Get().key_date_format == (int)DDMMYY_DATE)
      {
         strcpy (str_date, day);
         strcat (str_date, DATE_DELIM);
         strcat (str_date, mon);
         strcat (str_date, DATE_DELIM);
         strcat (str_date, yr);
         break;
      }

      if (guiglobs::cdsData.config.LangUnit.Get().key_date_format == (int)YYMMDD_DATE)
      {
         strcpy (str_date, yr);
         strcat (str_date, DATE_DELIM);
         strcat (str_date, mon);
         strcat (str_date, DATE_DELIM);
         strcat (str_date, day);
         break;
      }

      // default
      _FATAL_ERROR (__FILE__, __LINE__, "Unknown date format encountered in the configuration file.");

   } while (false);


}   // END of format_date FORMAT_DATE

void Format_date::appendDateAndTime (guistring& result, char x, char y, char z)
{
   // Build the format string, but use x, y, and z as the delimiters
   char dateAndTime[] = "YYYY-mm-dd_HH:MM:SS";
   char format[]      = "%%Y-%%m-%%d_%%H:%%M:%%S";
   sprintf(format, "%%Y%c%%m%c%%d%c%%H%c%%M%c%%S", x, x, y, z, z);

   // Get the current time
   struct tm    local_tm;
   const time_t now = time(NULL);
   localtime_r(&now, &local_tm);

   // Generate the date/time stamp and append to the input
   strftime(dateAndTime, sizeof(dateAndTime), format, &local_tm);
   result += dateAndTime;
}

/* FORMAT HASH e3ecd59c95b1dec45a53b888dfe67688 */
