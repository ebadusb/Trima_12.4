1i// This file is auto-generated by the build_string_info utility.
1i// DO NOT EDIT this file directly!
1i
1i#ifndef _STRING_DATA_INCLUDE_
1i#define _STRING_DATA_INCLUDE_
1i
1i#include "trima_text_item.h"
1i#include "trima_string_data.h"
1i
1i
1istruct StringTable
1i{
1i\   TrimaTextItem &item;
1i\   const char *id;
1i\   int localize;
1i};
1i
1i
1istruct StringData : public TrimaStringData
1i{
1i\   StringData();
1i\   ~StringData();
1i
s/STRING_FILE_TABLE_START//
s/STRING_FILE_TABLE_END//
s/^\([A-Za-z_][A-Za-z0-9_]*\).*/   static TrimaTextItem \1;/p
$i};
$i
$i#endif // _STRING_DATA_INCLUDE_
