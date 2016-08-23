#if !defined(_DebugLogScanner_INCLUDE)
#define _DebugLogScanner_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLogFormat.h"
#include "zlib.h"

class DebugLogScanner
{
public:
   DebugLogScanner(void);
   virtual ~DebugLogScanner();
   bool Convert(HANDLE fileHandle, DWORD node);

   	enum { MaxTaskNameSize = 16 };

protected:
   bool ReadDLogHeader(void);

   bool ReadDLogData(void * data, size_t size);
   void SkipDLogData(size_t size);
   bool ReadUncompressedData(void * data, size_t size);

   bool SkipRecord(WORD recordType);
   bool SkipLogLevel(void);
   bool SkipPrintOutput(void);
   bool SkipStreamOutput(void);
   bool SkipPeriodicOutput(void);
   bool SkipPeriodicSet(void);
   bool SkipPeriodicItem(void);
   bool SkipTaskCreate(void);
   bool SkipTaskDelete(void);
   bool SkipNetworkHeader(void);

   bool ProcessBinary(void);
   bool ProcessTaskCreate(void);
   bool ProcessDbgTaskData(size_t size);
   bool ProcessDbgMsgData(size_t size);
   bool ProcessDbgPacketData(const char * type, size_t size);
   bool ProcessDbgNetworkStatData(size_t size);

private:
   HANDLE   m_fileHandle;        // file handle for dlog file
   DWORD		m_node;
   DWORD    m_baseTime;          // machine time at start of log file

   enum { DLogInputBufferSize = 512 };
   z_stream m_dlogStream;        // stream struct needed by ZLib to handle decompression
   bool     m_dlogStreamInitialized;   // true once ZLib initialization calls for m_dlogStream are complete
   Bytef    m_dlogInputBuffer[DLogInputBufferSize];   // buffer for m_dlogStream

	struct TaskEntry
	{
		DWORD	id;
		char	name[MaxTaskNameSize];
	};

	enum { MaxTaskEntry = 50 };
	TaskEntry	m_taskEntry[MaxTaskEntry];
	int	m_lastTaskEntry;

private:

	void GetTaskName(DWORD id, char * name);
   bool GetNextFormatArg(const char * format, size_t& currentIndex, unsigned int& starModifierCount, size_t& size);
   void ParsePrint(const char * format);
};

#endif /* if !defined(_DebugLogScanner_INCLUDE) */

