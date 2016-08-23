/*****************************************************************************\
**
** TimeFrameMgr.h
**
** Provides the implementation for the time frame manager.
**
\*****************************************************************************/

#ifndef TIMEFRAMEMGR_H
#define TIMEFRAMEMGR_H

/* Includes ******************************************************************/

#include <list>

#include "SpoofTypes.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

class TimeFrameMgr {
public:
	typedef std::pair<Uint32, Uint32> TimeFrame;
	typedef std::list<TimeFrame> TimeFrameList;

	/**
	 *
	 * TimeFrameMgr
	 *
	 * Constructor
	 *
	 */ 
	TimeFrameMgr(void);

	/**
	 *
	 * ~TimeFrameMgr
	 *
	 * Destructor
	 *
	 */ 
	~TimeFrameMgr(void);

	/**
	 *
	 * IncludeTimeFrame
	 *
	 * Includes the time frame.
	 *
	 * @param The time frame.
	 *
	 */
	void IncludeTimeFrame(const TimeFrame &timeFrame);

	/**
	 *
	 * IncludeAllTimeFrame
	 *
	 * Includes the entire time frame.
	 *
	 */
	void IncludeAllTimeFrame(void);

	/**
	 *
	 * ExcludeTimeFrame
	 *
	 * Excludes the time frame.
	 *
	 * @param The time frame.
	 *
	 */
	void ExcludeTimeFrame(const TimeFrame &timeFrame);

	/**
	 *
	 * ExcludeAllTimeFrame
	 *
	 * Excludes the entire time frame.
	 *
	 */
	void ExcludeAllTimeFrame(void);

	/**
	 *
	 * GetTimeFrameListBegin
	 *
	 * Gets an iterator to the beginning of the time fram list.
	 *
	 * @return The iterator to the beginning of the list.
	 *
	 */
	TimeFrameList::const_iterator GetTimeFrameListBegin(void) const;

	/**
	 *
	 * GetTimeFrameListEnd
	 *
	 * Gets an iterator to the end of the time fram list.
	 *
	 * @return The iterator to the end of the list.
	 *
	 */
	TimeFrameList::const_iterator GetTimeFrameListEnd(void) const;

	/**
	 *
	 * GetBeginTime
	 *
	 * Gets the lower bounds of the red/green data that will be processed.
	 *
	 * @return The lower bounds.
	 *
	 */
	Uint32 GetBeginTime(void) const;

	/**
	 *
	 * GetEndTime
	 *
	 * Gets the upper bounds of the red/green data that will be processed.
	 *
	 * @return The upper bounds.
	 *
	 */
	Uint32 GetEndTime(void) const;

	/**
	 *
	 * GetTotalTime
	 *
	 * Gets the total time from begin to end.
	 *
	 * @return The total time.
	 *
	 */
	Uint32 GetTotalTime(void) const;

	/**
	 *
	 * GetFrameTime
	 *
	 * Gets the total from the frames.
	 *
	 * @return The total time.
	 *
	 */
	Uint32 GetFrameTime(void) const;

private:
	TimeFrameList mTimeFrameList;
};

} // namespace BUILDER

#endif // TIMEFRAMEMGR_H

