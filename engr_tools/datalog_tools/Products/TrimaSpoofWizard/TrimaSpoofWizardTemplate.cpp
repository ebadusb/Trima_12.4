/*****************************************************************************\
**
** <Filename>.cpp
**
** Auto generated file for spoofer configurations.
**
** Name:	<Name>
** DLog:	<Datalog>
** Date:	<Date>
**
\*****************************************************************************/

#define SPOOFNAME <Namespace>

/* Spoof Types ***************************************************************/

namespace SPOOFNAME {

<Types>

}; // namespace SPOOFNAME

/* Spoof Configs *************************************************************/

namespace SPOOFNAME {

const char *kSpoofName = "<Name>"; 
const char *kSpoofComment = "<Comment>";
const char *kSpoofHelp = "<Help>";
const int kSpoofTimeout = <Timeout>;

<Constants>

}; // namespace SPOOFNAME

/* Spoof Configs *************************************************************/

#include "SpoofGeneratedMain.h"

<Includes>

/* Spoof Functions ***********************************************************/

/**
 * 
 * Create<Filename>
 * 
 * Utility function for creating an instance to the spoof.
 * 
 */
extern "C" SpoofBase *Create<Filename>(SpooferAPI &api)
{
	 return new SPOOFNAME::SpoofGeneratedMain(api);
}

namespace SPOOFNAME {

/**
 * 
 * AddAssignment
 * 
 * Adds the tasks to the task list.
 * 
 */
void SpoofGeneratedMain::AddAssignment(void)
{
	<Populate>
}

/**
 * 
 * DelAssignment
 * 
 * Deletes the tasks from the task list.
 * 
 */
void SpoofGeneratedMain::DelAssignment(void)
{
 	 std::for_each(mAssignmentList.begin(), mAssignmentList.end(), Delete);
}

}; // namespace SPOOFNAME


