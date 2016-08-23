/*****************************************************************************\
**
** SpoofTags.h
**
** Provides a set of spoof tags.
**
\*****************************************************************************/

#ifndef SPOOFTAGS_H
#define SPOOFTAGS_H

/* Includes ******************************************************************/

#include "SpoofTypes.h"

namespace BUILDER {

/* Constants ******************************************************************/

static const String kTagName = "<Name>";
static const String kTagComment = "<Comment>";
static const String kTagDatalog = "<Datalog>";
static const String kTagHelp = "<Help>";
static const String kTagDate = "<Date>";
static const String kTagNamespace = "<Namespace>";
static const String kTagFilename = "<Filename>";
static const String kTagTimeout = "<Timeout>";
static const String kTagTypes = "<Types>";
static const String kTagConstants = "<Constants>";
static const String kTagIncludes = "<Includes>";
static const String kTagPopulate = "<Populate>";

static const String kSpoofTags[] = {
		kTagName,
		kTagComment,
		kTagDatalog,
		kTagHelp,
		kTagDate,
		kTagNamespace,
		kTagFilename,
		kTagTimeout,
		kTagTypes,
		kTagConstants,
		kTagIncludes,
		kTagPopulate,
};

static const Uint32 kSpoofTagsCount = sizeof(kSpoofTags) / sizeof (String);

} // namespace BUILDER

#endif // SPOOFTAGS_H