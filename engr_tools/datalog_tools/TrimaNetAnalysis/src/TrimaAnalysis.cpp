/*****************************************************************************\
**
** TrimaAnalysis.h
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>

#include "TrimaAnalysis.h"
#include "LogExecute.h"

namespace ANALYSIS {

/* Functions *****************************************************************/

/**
 *
 * Register
 *
 * Registers the necessary handlers with the reader.
 *
 * @param reader The file reader.
 *
 */
void TrimaAnalysis::Register(DECODER::LogReader &reader)
{
	reader.RegisterLogLevel(mProcessGetVxWorksInfo);
	reader.RegisterStreamOutput(mFilterLogData);
}

/**
 *
 * Unregister
 *
 * Unregisters the handlers from the reader.
 *
 * @param reader The file reader.
 *
 */
void TrimaAnalysis::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterLogLevel(mProcessGetVxWorksInfo);
	reader.UnregisterStreamOutput(mFilterLogData);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void TrimaAnalysis::PreProcess(void)
{
	mStatDataMap.clear();
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void TrimaAnalysis::PostProcess(void)
{
	StatDataMap::const_iterator iter = mStatDataMap.begin();
	StatDataMap::const_iterator end = mStatDataMap.end();

	for (; iter != end; ++iter) {
		StatDataList::const_iterator dataIter = iter->second.begin();
		StatDataList::const_iterator dataEnd = iter->second.end();

		mStream << "Network stat data for node: " << iter->first << std::endl;  

		mStream << "Time" << ","
			<< kTagTCPRetransmitTimeouts << ","
			<< kTagTCPCompletelyDuplicatePacket << ","
			<< kTagTCPOutOfOrderPacket << ","
			<< kTagTCPDataAfterWindow << ","
			<< kTagTCPWindowProbe << ","
			<< kTagTCPWindowUpdatePacket << ","
			<< kTagTCPPacketReceivedAfterClose << ","
			<< kTagTCPDiscardedForBadChecksum << ","
			<< kTagTCPDiscardedForBadHeaderOffset << ","
			<< kTagTCPDiscardedBecausePacketTooShort << ","
			<< kTagIPTotal << ","
			<< kTagIPBadsum << ","
			<< kTagIPTooshort << ","
			<< kTagIPToosmall << ","
			<< kTagIPBadhlen << ","
			<< kTagIPbadlen << ","
			<< kTagIPInfragments << ","
			<< kTagIPFragdropped << ","
			<< kTagIPFragtimeout << ","
			<< kTagIPForward << ","
			<< kTagIPCantforward << ","
			<< kTagIPRedirectsent << ","
			<< kTagIPUnknownprotocol << ","
			<< kTagIPNobuffers << ","
			<< kTagIPReassembled << ","
			<< kTagIPOutfragments << ","
			<< kTagIPNoroute << ","
			<< std::endl;

		for (; dataIter != dataEnd; ++dataIter) {
			mStream << dataIter->StatDataTime << "," 
				<< dataIter->TCPRetransmitTimeouts << "," 
				<< dataIter->TCPCompletelyDuplicatePacket << "," 
				<< dataIter->TCPOutOfOrderPacket << "," 
				<< dataIter->TCPDataAfterWindow << ","
				<< dataIter->TCPWindowProbe << ","
				<< dataIter->TCPWindowUpdatePacket << ","
				<< dataIter->TCPPacketReceivedAfterClose << ","
				<< dataIter->TCPDiscardedForBadChecksum << ","
				<< dataIter->TCPDiscardedForBadHeaderOffset << ","
				<< dataIter->TCPDiscardedBecausePacketTooShort << ","
				<< dataIter->IPTotal << ","
				<< dataIter->IPBadsum << ","
				<< dataIter->IPTooshort << ","
				<< dataIter->IPToosmall << ","
				<< dataIter->IPBadhlen << ","
				<< dataIter->IPbadlen << ","
				<< dataIter->IPInfragments << ","
				<< dataIter->IPFragdropped << ","
				<< dataIter->IPFragtimeout << ","
				<< dataIter->IPForward << ","
				<< dataIter->IPCantforward << ","
				<< dataIter->IPRedirectsent << ","
				<< dataIter->IPUnknownprotocol << ","
				<< dataIter->IPNobuffers << ","
				<< dataIter->IPReassembled << ","
				<< dataIter->IPOutfragments << ","
				<< dataIter->IPNoroute << ","
				<< std::endl;
		}	
	}
}

/**
 *
 * ExtractLogData
 *
 * Utility function that extracts the message data to a file.
 *
 * @param logFile The filename of the log file.
 * @param datFile The filename of the data file.
 *
 * @return The results. 
 *
 */
DECODER::Result TrimaAnalysis::ExtractLogData(const DECODER::String &logFile, const DECODER::String &datFile)
{
	DECODER::Result results = DECODER::LogReader::kResultNone;

	std::ofstream file;
	file.open(datFile.c_str());
	results = DECODER::ProcessLogData(logFile.c_str(), TrimaAnalysis(file));
	file.flush();
	file.close();

	return results;
}

} // namespace ANALYSIS

