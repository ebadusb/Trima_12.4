/*****************************************************************************\
**
** TimeFrameMgr.cpp
**
** Provides the implementation for the time frame manager.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "TimeFrameMgr.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

/**
 *
 * TimeFrameMgr
 *
 * Constructor
 *
 */ 
TimeFrameMgr::TimeFrameMgr(void)
{
}

/**
 *
 * ~TimeFrameMgr
 *
 * Destructor
 *
 */ 
TimeFrameMgr::~TimeFrameMgr(void) 
{
}

/**
 *
 * IncludeTimeFrame
 *
 * Includes the time frame.
 *
 * @param The time frame.
 *
 */
void TimeFrameMgr::IncludeTimeFrame(const TimeFrame &timeFrame)
{
	if (timeFrame.first < timeFrame.second) {
		if (mTimeFrameList.empty()) {
			mTimeFrameList.push_back(timeFrame);
		}
		else {
			TimeFrameList::iterator iter = mTimeFrameList.begin();
			TimeFrameList::iterator end = mTimeFrameList.end();
			TimeFrameList::iterator loc = mTimeFrameList.end();

			for (; iter != end; ++iter) {
				if ((*iter).first > timeFrame.first) {
					loc = mTimeFrameList.insert(iter, timeFrame);
					break;
				}
				else if ((*iter).second >= timeFrame.first) {
					(*iter).second = timeFrame.second;
					loc = iter++;
					break;
				}
			}
			
			if (loc == end) {
				loc = mTimeFrameList.insert(iter, timeFrame);
			}

			for (; iter != end; ) {
				if ((*loc).second >= (*iter).second) {
					iter = mTimeFrameList.erase(iter);
				}
				else if ((*loc).second >= (*iter).first) {
					(*loc).second = (*iter).second;
					iter = mTimeFrameList.erase(iter);
				}
				else {
					break;
				}
			}
		}
	}
}

/**
 *
 * IncludeAllTimeFrame
 *
 * Includes the entire time frame.
 *
 */
void TimeFrameMgr::IncludeAllTimeFrame(void)
{
	mTimeFrameList.clear();
	this->IncludeTimeFrame(std::make_pair(0x00000000, 0xFFFFFFFF));
}

/**
 *
 * ExcludeTimeFrame
 *
 * Excludes the time frame.
 *
 * @param The time frame.
 *
 */
void TimeFrameMgr::ExcludeTimeFrame(const TimeFrame &timeFrame)
{
	if (timeFrame.first < timeFrame.second) {
		if (!mTimeFrameList.empty()) {
			TimeFrameList::iterator iter = mTimeFrameList.begin();
			TimeFrameList::iterator end = mTimeFrameList.end();

			for (; iter != end; ++iter) {
				if ((*iter).first < timeFrame.first && (*iter).second >= timeFrame.first) {
					TimeFrame newTimeFrame(timeFrame.second, (*iter).second);

					(*iter).second = timeFrame.first;
					iter = mTimeFrameList.insert(++iter, newTimeFrame);
					break;
				}
				else if ((*iter).first > timeFrame.first) {
					break;
				}
				else if ((*iter).second >= timeFrame.first) {
					(*iter).second = timeFrame.first;
					++iter;
					break;
				}
			}

			for (; iter != end; ) {
				if (timeFrame.second >= (*iter).second) {
					iter = mTimeFrameList.erase(iter);
				}
				else if (timeFrame.second >= (*iter).first) {
					(*iter).first = timeFrame.second;
					break;
				}
				else {
					break;
				}
			}
		}
	}
}

/**
 *
 * ExcludeAllTimeFrame
 *
 * Excludes the entire time frame.
 *
 */
void TimeFrameMgr::ExcludeAllTimeFrame(void)
{
	mTimeFrameList.clear();
}

/**
 *
 * GetTimeFrameListBegin
 *
 * Gets an iterator to the beginning of the time fram list.
 *
 * @return The iterator to the beginning of the list.
 *
 */
TimeFrameMgr::TimeFrameList::const_iterator TimeFrameMgr::GetTimeFrameListBegin(void) const
{
	return mTimeFrameList.begin();
}

/**
 *
 * GetTimeFrameListEnd
 *
 * Gets an iterator to the end of the time fram list.
 *
 * @return The iterator to the end of the list.
 *
 */
TimeFrameMgr::TimeFrameList::const_iterator TimeFrameMgr::GetTimeFrameListEnd(void) const
{
	return mTimeFrameList.end();
}

/**
 *
 * GetBeginTime
 *
 * Gets the lower bounds of the red/green data that will be processed.
 *
 * @return The lower bounds.
 *
 */
Uint32 TimeFrameMgr::GetBeginTime(void) const
{
	return mTimeFrameList.begin() != mTimeFrameList.end()? (*mTimeFrameList.begin()).first : 0;
}

/**
 *
 * GetEndTime
 *
 * Gets the upper bounds of the red/green data that will be processed.
 *
 * @return The upper bounds.
 *
 */
Uint32 TimeFrameMgr::GetEndTime(void) const
{
	return mTimeFrameList.rbegin() != mTimeFrameList.rend()? (*mTimeFrameList.rbegin()).second : 0;
}

/**
 *
 * GetTotalTime
 *
 * Gets the total time from begin to end.
 *
 * @return The total time.
 *
 */
Uint32 TimeFrameMgr::GetTotalTime(void) const
{
	return this->GetEndTime() - this->GetBeginTime();

}

/**
 *
 * GetFrameTime
 *
 * Gets the total from the frames.
 *
 * @return The total time.
 *
 */
Uint32 TimeFrameMgr::GetFrameTime(void) const
{
	Uint32 total = 0;

	TimeFrameList::const_iterator iter = mTimeFrameList.begin();
	TimeFrameList::const_iterator end = mTimeFrameList.end();

	for (; iter != end; ++iter) {
		total += (*iter).second - (*iter).first;
	}

	return total; 
}

} // namespace BUILDER

