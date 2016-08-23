/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include "guipalette.h"

OSPaletteEntry guipalette::palette[OSPaletteSize] =
{
   {   0,   0,   0 },
   {   5,   0,   0 },
   {   0,   5,   0 },
   {   0,   0,   5 },
   {   5,   5,   5 },
   {   9,   0,   0 },
   {  13,   0,   0 },
   {   0,   9,   0 },
   {   0,  13,   0 },
   {   0,   0,   9 },
   {   0,   0,  13 },
   {   9,   9,   9 },
   {  13,  13,  13 },
   {  17,   0,   0 },
   {  21,   0,   0 },
   {  25,   0,   0 },
   {  29,   0,   0 },
   {   0,  17,   0 },
   {   0,  21,   0 },
   {   0,  25,   0 },
   {   0,  29,   0 },
   {   0,   0,  17 },
   {   0,   0,  21 },
   {   0,   0,  25 },
   {   0,   0,  29 },
   {  17,  17,  17 },
   {  21,  21,  21 },
   {  25,  25,  25 },
   {  29,  29,  29 },
   {  33,   0,   0 },
   {  37,   0,   0 },
   {  41,   0,   0 },
   {  45,   0,   0 },
   {  49,   0,   0 },
   {  53,   0,   0 },
   {  57,   0,   0 },
   {  61,   0,   0 },
   {   0,  33,   0 },
   {   0,  37,   0 },
   {   0,  41,   0 },
   {   0,  45,   0 },
   {   0,  49,   0 },
   {   0,  53,   0 },
   {   0,  57,   0 },
   {   0,  61,   0 },
   {   0,   0,  33 },
   {   0,   0,  37 },
   {   0,   0,  41 },
   {   0,   0,  45 },
   {   0,   0,  49 },
   {   0,   0,  53 },
   {   0,   0,  57 },
   {   0,   0,  61 },
   {  33,  33,  33 },
   {  37,  37,  37 },
   {  41,  41,  41 },
   {  45,  45,  45 },
   {  49,  49,  49 },
   {  53,  53,  53 },
   {  57,  57,  57 },
   {  61,  61,  61 },
   {  65,   0,   0 },
   {  69,   0,   0 },
   {  73,   0,   0 },
   {  77,   0,   0 },
   {  81,   0,   0 },
   {  85,   0,   0 },
   {  89,   0,   0 },
   {  93,   0,   0 },
   {  97,   0,   0 },
   { 101,   0,   0 },
   { 105,   0,   0 },
   { 109,   0,   0 },
   { 113,   0,   0 },
   { 117,   0,   0 },
   { 121,   0,   0 },
   { 125,   0,   0 },
   {   0,  65,   0 },
   {   0,  69,   0 },
   {   0,  73,   0 },
   {   0,  77,   0 },
   {   0,  81,   0 },
   {   0,  85,   0 },
   {   0,  89,   0 },
   {   0,  93,   0 },
   {   0,  97,   0 },
   {   0, 101,   0 },
   {   0, 105,   0 },
   {   0, 109,   0 },
   {   0, 113,   0 },
   {   0, 117,   0 },
   {   0, 121,   0 },
   {   0, 125,   0 },
   {   0,   0,  65 },
   {   0,   0,  69 },
   {   0,   0,  73 },
   {   0,   0,  77 },
   {   0,   0,  81 },
   {   0,   0,  85 },
   {   0,   0,  89 },
   {   0,   0,  93 },
   {   0,   0,  97 },
   {   0,   0, 101 },
   {   0,   0, 105 },
   {   0,   0, 109 },
   {   0,   0, 113 },
   {   0,   0, 117 },
   {   0,   0, 121 },
   {   0,   0, 125 },
   {  65,  65,  65 },
   {  69,  69,  69 },
   {  73,  73,  73 },
   {  77,  77,  77 },
   {  81,  81,  81 },
   {  85,  85,  85 },
   {  89,  89,  89 },
   {  93,  93,  93 },
   {  97,  97,  97 },
   { 101, 101, 101 },
   { 105, 105, 105 },
   { 109, 109, 109 },
   { 113, 113, 113 },
   { 117, 117, 117 },
   { 121, 121, 121 },
   { 125, 125, 125 },
   { 129,   0,   0 },
   { 133,   0,   0 },
   { 137,   0,   0 },
   { 141,   0,   0 },
   { 145,   0,   0 },
   { 149,   0,   0 },
   { 153,   0,   0 },
   { 157,   0,   0 },
   { 161,   0,   0 },
   { 165,   0,   0 },
   { 169,   0,   0 },
   { 173,   0,   0 },
   { 177,   0,   0 },
   { 181,   0,   0 },
   { 185,   0,   0 },
   { 189,   0,   0 },
   { 193,   0,   0 },
   { 197,   0,   0 },
   { 201,   0,   0 },
   { 205,   0,   0 },
   { 209,   0,   0 },
   { 213,   0,   0 },
   { 217,   0,   0 },
   { 221,   0,   0 },
   { 225,   0,   0 },
   { 229,   0,   0 },
   { 233,   0,   0 },
   { 237,   0,   0 },
   { 241,   0,   0 },
   { 245,   0,   0 },
   { 249,   0,   0 },
   { 253,   0,   0 },
   {   0, 129,   0 },
   {   0, 133,   0 },
   {   0, 137,   0 },
   {   0, 141,   0 },
   {   0, 145,   0 },
   {   0, 149,   0 },
   {   0, 153,   0 },
   {   0, 157,   0 },
   {   0, 161,   0 },
   {   0, 165,   0 },
   {   0, 169,   0 },
   {   0, 173,   0 },
   {   0, 177,   0 },
   {   0, 181,   0 },
   {   0, 185,   0 },
   {   0, 189,   0 },
   {   0, 193,   0 },
   {   0, 197,   0 },
   {   0, 201,   0 },
   {   0, 205,   0 },
   {   0, 209,   0 },
   {   0, 213,   0 },
   {   0, 217,   0 },
   {   0, 221,   0 },
   {   0, 225,   0 },
   {   0, 229,   0 },
   {   0, 233,   0 },
   {   0, 237,   0 },
   {   0, 241,   0 },
   {   0, 245,   0 },
   {   0, 249,   0 },
   {   0, 253,   0 },
   {   0,   0, 129 },
   {   0,   0, 133 },
   {   0,   0, 137 },
   {   0,   0, 141 },
   {   0,   0, 145 },
   {   0,   0, 149 },
   {   0,   0, 153 },
   {   0,   0, 157 },
   {   0,   0, 161 },
   {   0,   0, 165 },
   {   0,   0, 169 },
   {   0,   0, 173 },
   {   0,   0, 177 },
   {   0,   0, 181 },
   {   0,   0, 185 },
   {   0,   0, 189 },
   {   0,   0, 193 },
   {   0,   0, 197 },
   {   0,   0, 201 },
   {   0,   0, 205 },
   {   0,   0, 209 },
   {   0,   0, 213 },
   {   0,   0, 217 },
   {   0,   0, 221 },
   {   0,   0, 225 },
   {   0,   0, 229 },
   {   0,   0, 233 },
   {   0,   0, 237 },
   {   0,   0, 241 },
   {   0,   0, 245 },
   {   0,   0, 249 },
   {   0,   0, 253 },
   {   0, 255, 255 },
   { 128, 128, 128 },
   { 133, 133, 133 },
   { 137, 137, 137 },
   { 141, 141, 141 },
   { 145, 145, 145 },
   { 149, 149, 149 },
   { 153, 153, 153 },
   { 157, 157, 157 },
   { 161, 161, 161 },
   { 165, 165, 165 },
   { 169, 169, 169 },
   { 173, 173, 173 },
   { 177, 177, 177 },
   { 181, 181, 181 },
   { 185, 185, 185 },
   { 189, 189, 189 },
   { 193, 193, 193 },
   { 197, 197, 197 },
   { 201, 201, 201 },
   { 205, 205, 205 },
   { 209, 209, 209 },
   { 213, 213, 213 },
   { 217, 217, 217 },
   { 221, 221, 221 },
   { 225, 225, 225 },
   { 229, 229, 229 },
   { 233, 233, 233 },
   { 237, 237, 237 },
   { 241, 241, 241 },
   { 245, 245, 245 },
   { 248, 248, 248 },
   { 250, 250, 250 },
   { 252, 252, 252 },
   { 254, 254, 0   }
};

/* FORMAT HASH c37d4847773d94a4ad3d17e30a7dd482 */
