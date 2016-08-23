# @(#) $Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/alarm_table/rcs/alarmEnumStrings.sed 1.1 2009/05/01 23:10:24Z dslausb Exp $
#
#************************> ENGINEER TOOLS DOMAIN <****************************
#
#     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved
#
#     Author:      Dan Slauson
#
#     Class name:  N/A
#
#     File name:   alarmEnumStrings.sed
#
#     Description:
#
#	This is a sed script that builds a table of string literals based on
#	the alarms enum so that they can be output into the csv as strings.
#
#*************************< ENGINEER TOOLS DOMAIN >*****************************
#
# $Log: alarmEnumStrings.sed $
# Revision 1.1  2009/05/01 23:10:24Z  dslausb
# Initial revision
#
#*******************************************************************************
1i// This file is auto-generated by the alarm_table utility.
1i//   DO NOT EDIT this file directly!
1i
1i#include "alarms.h"
1i
1ichar* alarmEnumStrings[] = {
s/[\t\ ][\t\ ]*/ /g
s/^[\t\ ]//g
s/^enum.*//g
s/^[^a-zA-Z_].*//g
s/^\([A-Za-z_][A-Za-z0-9_]*\).*/\t"\1",/p
$i};
$i

