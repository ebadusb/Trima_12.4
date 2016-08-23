/*****************************************************************************\
**
** SourceStream.h
**
** Provides the implementation for streaming source code to a file.
**
\*****************************************************************************/

#ifndef SOURCESTREAM_H
#define SOURCESTREAM_H

/* Includes ******************************************************************/

#include <sstream>

#include "SpoofTypes.h"
#include "SpoofTags.h"

namespace BUILDER {

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

class SourceStream {
public:
	typedef std::ostringstream Stream;
	typedef std::map<String, Stream*> StreamMap;
	typedef StreamMap::const_iterator Iterator;
	
	/**
	 *
	 * SourceStream
	 *
	 * Constructor
	 *
	 */
	SourceStream(void) 
	{
		for (Uint32 i = 0; i < kSpoofTagsCount; ++i) {
			mStreamMap[kSpoofTags[i]] = new Stream();
		}
	}

	/**
	 *
	 * ~SourceStream
	 *
	 * Destructor
	 *
	 */
	~SourceStream(void) 
	{
		Iterator iter = mStreamMap.begin();
		Iterator end = mStreamMap.end();

		for (; iter != end; ++iter) {
			delete iter->second;
		}
	}

	/**
	 *
	 * GetStream
	 *
	 * Gets the stream for the given tag name.
	 *
	 * @param name The tag name.
	 *
	 * @return The stream for the tag.
	 *
	 */
	Stream &GetStream(String name)
	{ 
		Iterator iter = mStreamMap.find(name);
		Iterator end = mStreamMap.end();
		Stream *stream = NULL; 

		if (iter != end) {
			stream = iter->second;
		}
		else {
			stream = new Stream();
			mStreamMap[name] = stream;
		}

		return *stream;
	}

	/**
	 *
	 * Begin
	 *
	 * Gets an iterator to the stream map.
	 *
	 * @return An iterator to the beginning of map.
	 *
	 */
	Iterator Begin(void)
	{
		return mStreamMap.begin();
	}

	/**
	 *
	 * End
	 *
	 * Gets an iterator to the stream map.
	 *
	 * @return An iterator to the end of map.
	 *
	 */
	Iterator End(void)
	{
		return mStreamMap.end();
	}

private:
	StreamMap mStreamMap;
};

} // namespace BUILDER

#endif // SOURCESTREAM_H

