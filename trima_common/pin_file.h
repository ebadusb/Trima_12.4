#ifndef _PIN_FILE_DAT_H_
#define _PIN_FILE_DAT_H_

#include <map>
#include <string>

#include "datalog.h"


typedef map<string, string> PIN_LIST_TYPE;
typedef PIN_LIST_TYPE::iterator PIN_LIST_ITERATOR;


class PinDat
{
public:
   PinDat();
   virtual ~PinDat();

   static void setLoggingLevel (bool logPins) { _logPins = logPins; }

   static void read ();
   static void update ();

   // Exported iterator methods
   static PIN_LIST_ITERATOR begin ();
   static PIN_LIST_ITERATOR end ();
   static void              erase (const PIN_LIST_ITERATOR& it);

   static unsigned int size (void);
   static void         clear (void);

   static void addPin (const char* badge, const char* pin);

   static PIN_LIST_ITERATOR find (const char* badge);

protected:
   static bool writeDataToFile ();

private:
   static DataLog_SetHandle _set;
   static bool              _logPins;
};

#endif

/* FORMAT HASH 4a1e0269e3e658effd287d0a258cbbb4 */
