/*****************************************************************************\
**
** SpoofGeneratedMain.h
**
** Provides the main implementation for generated spoofs. A generated spoof
** must provide the implementation for the functions AddAssignment add DellTasks.
** These functions simply manage the content of "mAssignmentList".
**
\*****************************************************************************/

#ifndef SPOOFGENERATEDBASE_H
#define SPOOFGENERATEDBASE_H

/* Includes ******************************************************************/

#include <list>
#include <algorithm>

#include "SpoofAssignment.h"
#include "spoofbase.hpp"
#include "datalog_levels.h"
#include "spooferlog.hpp"

namespace SPOOFNAME {

/* Constants *****************************************************************/

const char *kSpoofNoParam = "No parameters defined for spoof";

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

static void Initialize(SpoofAssignment *assignment) { assignment->Initialize(); }
static void Execute(SpoofAssignment *assignment) { assignment->Execute(); }
static void Finalize(SpoofAssignment *assignment) { assignment->Finalize(); }
static void Delete(SpoofAssignment *assignment) { delete assignment; }

/* Classes *******************************************************************/

class SpoofGeneratedMain : public SpoofBase {
public:
	SpoofGeneratedMain(SpooferAPI &api);
	virtual ~SpoofGeneratedMain(void);

	virtual void run(void);
	virtual void init(void);
	virtual void end(void);
	virtual void help(void);

	virtual void run_generic(const char * Var1, const char * Var2);

private:
	typedef std::list<SpoofAssignment*> AssignmentList;

	virtual void AddAssignment(void);
	virtual void DelAssignment(void);

	AssignmentList mAssignmentList;
	SpooferAPI &API;
};

/**
 * 
 * SpoofGeneratedMain
 * 
 * Constructor. Adds the spoof tasks to the main spoof object.
 * 
 */
SpoofGeneratedMain::SpoofGeneratedMain(SpooferAPI &api) : SpoofBase(), API(api) 
{
	DataLog(log_level_spoofer_info) << "Loading custom spoof: " << kSpoofName << endmsg;

	this->AddAssignment();
	this->init();
}

/**
 * 
 * ~SpoofGeneratedMain
 * 
 * Destructor. Deletes the spoof tasks from the spoof object.
 * 
 */
SpoofGeneratedMain::~SpoofGeneratedMain(void)
{
	DataLog(log_level_spoofer_info) << "Unloading custom spoof: " << kSpoofName << endmsg;

	this->end();
	this->DelAssignment();
}

/**
 * 
 * run
 * 
 * Runs the spoof by executing each individual tasks.
 * 
 */
void SpoofGeneratedMain::run(void)
{
	DataLog(log_level_spoofer_info) << "Running custom spoof: " << kSpoofName << endmsg;

	std::for_each(mAssignmentList.begin(), mAssignmentList.end(), Execute);
    API.spoofActivate();
}

/**
 * 
 * init
 * 
 * Initializes the spoof. 
 * 
 */
void SpoofGeneratedMain::init(void)
{
	DataLog(log_level_spoofer_info) << "Initializing custom spoof: " << kSpoofName << endmsg;

	_name = kSpoofName;
	_comment = kSpoofComment;
	_timeout = kSpoofTimeout;

	std::for_each(mAssignmentList.begin(), mAssignmentList.end(), Initialize);
}

/**
 * 
 * end
 * 
 * Ends the spoof. 
 * 
 */
void SpoofGeneratedMain::end(void)
{
	std::for_each(mAssignmentList.begin(), mAssignmentList.end(), Finalize);
}

/**
 * 
 * help
 * 
 * Provides a user with use information. 
 * 
 */
void SpoofGeneratedMain::help(void)
{
	LOG_INFO(const_cast<char*>(kSpoofHelp));
}

/**
 * 
 * run_generic
 * 
 * Runs the spoof with the given parameters. 
 * 
 */
void SpoofGeneratedMain::run_generic(const char * Var1, const char * Var2) 
{ 
	LOG_INFO(const_cast<char*>(kSpoofNoParam));

	this->run();
};

}; // namespace SPOOFNAME

#endif SPOOFGENERATEDBASE_H
