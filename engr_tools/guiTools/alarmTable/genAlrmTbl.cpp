/************************> ENGINEER TOOLS DOMAIN <****************************
 *
 *     Copyright(c) 2006 GAMBRO BCT, Inc. All rights reserved
 *
 *     Author: I'm not sure who wrote the original version of this, but
 *     it's being updated and maintained by Dan Slauson (x4664)
 *
 *     Class name:  N/A
 *
 *     File name:   genAlrmTbl.cpp
 *
 * Description: This file provides the entry point to a program that outputs all the relevant
 * information on all the Trima alarms into a nicely formatted CSV table.
 *
 *****************************************************************************************/


#include "stdio.h"
#include <iostream>
#include "trimver.h"
#include "alarmtb.h"
#include "alarmcontainer.h"
#include "stringTable.h"
#include "alarmEnumStrings.h"

TABLE_CONTAINER stringTable::_tables; // Must be here before langTableHeader.h so static globals
// are initialized in the right order

#include "langTableHeader.h"

extern ALARM_TABLE alarms[];
const ALARM_TABLE * alarmTable = alarms;

string getAlarmEnumString(ALARM_VALUES id)
{
   if (id < FIRST_PROCEDURE_ALARM ||
       id > NULL_ALARM ||
       (id > LAST_FATAL_ERROR_ALARM && id < FIRST_SAFETY_ALARM))
      {
         fprintf(stderr, "Alarm ID %d out of range", id);
         return "bummer";
      }

   int index = (id <= LAST_FATAL_ERROR_ALARM)? id : id - FIRST_SAFETY_ALARM + LAST_FATAL_ERROR_ALARM +1;

   return alarmEnumStrings[index];
}

// Given an alarm ID, return it's body text string.
void getAlarmBodyText(char* langHashString, const ALARM_TABLE* alarmTableEntry, char* returnString)
{
   if (alarmTableEntry == NULL)
      {
         sprintf(returnString,"No Entry.");
         return;
      }

   TEXT_STRING_ID stringId = alarmTableEntry->alarm_body_text;

   if (stringId == TEXT_NULL)
      {
         sprintf(returnString,"NULL TEXT");
         return;
      }

   if (!langHashString)
      {
         fprintf(stderr, "NULL string for hash.\n");
         return;
      }

   string bodyTextString = stringTable::getStringFromLang(langHashString, stringId);

   sprintf(returnString, "%s", bodyTextString.c_str());
}

// Confert an alarm type enumeration value to a string.
const char * getAlarmPriority (const AlarmStruct& alarmItem)
{
   switch (alarmItem.getType())
      {
      case A2:
         return "A2";
      case A1:
         return "A1";
      case R2:
         return "R2";
      case R1:
         return "R1";
      case OPERATOR_ATTENTION:
         return "OPERATOR ATTENTION";
      default:
         return "UNKNOWN";
      }
}

// Given an alarm value, look up and return it's name string.
const char * getAlarmName(const ALARM_TABLE* alarmTableEntry)
{
   if (alarmTableEntry == NULL) return "No Entry.";
   return alarmTableEntry->alarm_name_literal;
}

const char* getAlarmDisplayType(const ALARM_TABLE* alarmTableEntry)
{
   if (alarmTableEntry)
      {
         if (alarmTableEntry->goto_screen_gui > GUI_SCREEN_NULL_SCREEN &&
             alarmTableEntry->goto_screen_gui < GUI_SCREEN_TOTAL_SCREENS)
            return "Operator Info Prompt";

         if (alarmTableEntry->alarm_bitmap > BITMAP_NULL &&
             alarmTableEntry->alarm_bitmap < BITMAP_ID_COUNT)
            return "Text and Bitmap";
      }

   return "Text Only";
}


// Given an alarm value, return its status line text string
void getStatusLineText(const char* langHashString, const ALARM_TABLE* alarmTableEntry, char* retString)
{
   if (alarmTableEntry == NULL)
      {
         sprintf(retString, "No Entry. Displayed as System Test Failure.");
         return;
      }

   TEXT_STRING_ID stringId = alarmTableEntry->alarm_status_line_text;

   if (stringId == TEXT_NULL)
      {
         sprintf(retString, "NULL TEXT");
         return;
      }

   sprintf(retString, stringTable::getStringFromLang(langHashString, stringId).c_str());
}

string GetValidString(const char *text)
{
   const char xmlInvalidChars[] = { '<', '>', 0 };

   string content(text);
   string::size_type index = content.find_first_of(xmlInvalidChars);

   while(index != string::npos) {
      switch (content[index]) {
      case '<':
         content.replace(index, 1, "&lt;");
         index += 4;
         break;
      case '>':
         content.replace(index, 1, "&gt;");
         index += 4;
         break;
      }
      index = content.find_first_of(xmlInvalidChars, index);
   }

   return content;
}

// The main entry point
int main(int argc, char* argv[])
{
   char csvFileName[256];

   if (argc < 3)
      {
         fprintf(stderr, "Useage: genAlrmTbl -CSV <name>\ngenAlrmTbl -XML <names>\n", argc);
         return -1;
      }

   if (strcmp(argv[1], "-CSV") == 0) {
      sprintf(csvFileName, "%s.csv", argv[2]);

      fprintf(stdout, "Generating alarm file %s.\n", csvFileName);

      // Create the File Name.
      FILE *outFile = fopen(csvFileName,"w");

      // Make sure it opened OK.
      if (outFile == 0)
         {
            fprintf(stderr, "Unable to open CSV file %s\n", csvFileName);
            return -2;
         }

      // Create an alarm table
      AlarmTable alarmTable;

      // Label the top of the CSV file.
      fprintf(outFile, "Trima %s Alarms\n\n", DCL_TEXT_PWRUP_VERSION);

      // Create an Alarm table iterator.
      AlarmTable::iterator alarmItem = alarmTable.begin();

      // Output column labels
      fprintf(outFile, "Alarm Id, Alarm Number, Alarm Name, Alarm Type, Data Log Alarm String, Display Order, Display Type, Status Line, Alarm Body Text\n");

      // Start the main loop; continue until you hit a null alarm item.
      while (alarmItem != alarmTable.end())
         {
            // Undisplayable alarms should will not leave an entry.
            if (alarmItem->getDisplayType() != DISP_NONE)
               {
                  GuiAlarmTable guiAlarmTable;
                  int i = 1;

                  const ALARM_VALUES alarmId = alarmItem->getName();

                  const ALARM_TABLE* alarmTableEntry = guiAlarmTable.GetEntry(alarmId);

                  char alarmBodyText[2048];
                  char statLineText[512];

                  getAlarmBodyText(argv[2], alarmTableEntry, alarmBodyText);
                  getStatusLineText(argv[2], alarmTableEntry, statLineText);

                  fprintf(outFile, "%s, %d, %s, %s, %s, 1, %s, %s, %s\n", getAlarmEnumString(alarmId).c_str(), alarmId, getAlarmName(alarmTableEntry), getAlarmPriority(*alarmItem), alarmItem->getDescription(), getAlarmDisplayType(alarmTableEntry), statLineText, alarmBodyText);

                  while (alarmTableEntry && guiAlarmTable.PeekNextEntry() &&
                         guiAlarmTable.PeekNextEntry()->alarm_name == alarmTableEntry->alarm_name)
                     {
                        i++;
                        alarmTableEntry = guiAlarmTable.GetNextEntry();

                        getAlarmBodyText(argv[2], alarmTableEntry, alarmBodyText);
                        getStatusLineText(argv[2], alarmTableEntry, statLineText);

                        fprintf(outFile, "(more info),,,,, %d, %s, %s, %s\n", i, getAlarmDisplayType(alarmTableEntry), statLineText, alarmBodyText);
                     }
               }

            ++alarmItem;
         }

      fprintf(stdout, "Alarm file %s generated successfully.\n", csvFileName);

      //close the file
      fclose (outFile);
   }
   else if (strcmp(argv[1], "-XML") == 0) {
      FILE *outFile = fopen("alarm_table.xml", "w");

      if (outFile == 0) {
         fprintf(stderr, "Unable to open XML file allarm_table.xml\n");
         return -2;
      }

      AlarmTable alarmTable;
      AlarmTable::iterator alarmItem = alarmTable.begin();

      fprintf(outFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fprintf(outFile, "<dita>\n");

      while (alarmItem != alarmTable.end())
         {
            if (alarmItem->getDisplayType() != DISP_NONE) {
               GuiAlarmTable guiAlarmTable;

               const ALARM_VALUES alarmId = alarmItem->getName();
               const ALARM_TABLE* alarmTableEntry = guiAlarmTable.GetEntry(alarmId);

               char alarmBodyText[2048];
               char statLineText[512];

               fprintf(outFile, "<reference id=\"%s\">\n", getAlarmEnumString(alarmId).c_str());
               fprintf(outFile, "<title/>\n");
               fprintf(outFile, "<refbody>\n");

               fprintf(outFile, "<section id=\"number\">%d</section>\n", alarmId);
               fprintf(outFile, "<section id=\"info\">%s</section>\n", getAlarmName(alarmTableEntry));
               fprintf(outFile, "<section id=\"type\">%s</section>\n", getAlarmPriority(*alarmItem));
               fprintf(outFile, "<section id=\"log\">%s</section>\n", GetValidString(alarmItem->getDescription()).c_str());

               fprintf(outFile, "<section id=\"name\">\n");
               for (int i = 2; i < argc; ++i) {
                  getStatusLineText(argv[i], alarmTableEntry, statLineText);
                  if (strlen(statLineText) == 0) {
                     sprintf(statLineText, "%s", getAlarmDisplayType(alarmTableEntry));
                  }
                  fprintf(outFile, "<ph id=\"name\" otherprops=\"%s\">%s</ph>\n", stringTable::getLanguageStr(argv[i]), statLineText);
               }
               fprintf(outFile, "</section>\n");

               fprintf(outFile, "<section id=\"explanation\">\n");
               for (int i = 2; i < argc; ++i) {
                  getAlarmBodyText(argv[i], alarmTableEntry, alarmBodyText);
                  if (strlen(alarmBodyText) == 0) {
                     sprintf(alarmBodyText, "%s", getAlarmDisplayType(alarmTableEntry));
                  }
                  fprintf(outFile, "<ph id=\"explanation\" otherprops=\"%s\">%s</ph>\n", stringTable::getLanguageStr(argv[i]), alarmBodyText);
               }
               fprintf(outFile, "</section>\n");

               int j = 0;

               while (alarmTableEntry && guiAlarmTable.PeekNextEntry() &&
                      guiAlarmTable.PeekNextEntry()->alarm_name == alarmTableEntry->alarm_name)
                  {
                     alarmTableEntry = guiAlarmTable.GetNextEntry();

                     fprintf(outFile, "<section id=\"moreinfo%d\">\n", j);
                     for (int i = 2; i < argc; ++i) {
                        getAlarmBodyText(argv[i], alarmTableEntry, alarmBodyText);
                        if (strlen(alarmBodyText) == 0) {
                           sprintf(alarmBodyText, "%s", getAlarmDisplayType(alarmTableEntry));
                        }
                        fprintf(outFile, "<ph id=\"moreinfo%d\" otherprops=\"%s\">%s</ph>\n", j, stringTable::getLanguageStr(argv[i]), alarmBodyText);
                     }
                     fprintf(outFile, "</section>\n");

                     j++;
                  }

               fprintf(outFile, "</refbody>\n");
               fprintf(outFile, "</reference>\n");
            }

            ++alarmItem;
         }

      fprintf(outFile, "</dita>\n");
      fclose (outFile);
   }

   return 0;
}
