/*****************************************************************************\
**
** TrimaAnalysis.h
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

#ifndef TRIMAANALYSIS_H
#define TRIMAANALYSIS_H

/* Includes ******************************************************************/

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>

#include "LogReader.h"
#include "LogExtractor.h"
#include "ColumnDataUtil.h"

namespace ANALYSIS {

/* Constants *****************************************************************/

static const char *kTagTCPRetransmitTimeouts = "retransmit timeouts";
static const char *kTagTCPCompletelyDuplicatePacket = "completely duplicate packet";
static const char *kTagTCPOutOfOrderPacket = "out-of-order packet";
static const char *kTagTCPDataAfterWindow = "data after window";
static const char *kTagTCPWindowProbe = "window probe";
static const char *kTagTCPWindowUpdatePacket = "window update packet";
static const char *kTagTCPPacketReceivedAfterClose = "packet received after close";
static const char *kTagTCPDiscardedForBadChecksum = "discarded for bad checksum";
static const char *kTagTCPDiscardedForBadHeaderOffset = "discarded for bad header offset field";
static const char *kTagTCPDiscardedBecausePacketTooShort = "discarded because packet too short";

static const char *kTagIPTotal = "total";
static const char *kTagIPBadsum = "badsum";
static const char *kTagIPTooshort = "tooshort";
static const char *kTagIPToosmall = "toosmall";
static const char *kTagIPBadhlen = "badhlen";
static const char *kTagIPbadlen = "badlen";
static const char *kTagIPInfragments = "infragments";
static const char *kTagIPFragdropped = "fragdropped";
static const char *kTagIPFragtimeout = "fragtimeout";
static const char *kTagIPForward = "forward";
static const char *kTagIPCantforward = "cantforward";
static const char *kTagIPRedirectsent = "redirectsent";
static const char *kTagIPUnknownprotocol = "unknownprotocol";
static const char *kTagIPNobuffers = "nobuffers";
static const char *kTagIPReassembled = "reassembled";
static const char *kTagIPOutfragments = "outfragments";
static const char *kTagIPNoroute = "noroute";

/* Types *********************************************************************/

struct StatData {
	double StatDataTime;

	int TCPRetransmitTimeouts;
	int TCPCompletelyDuplicatePacket;
	int TCPOutOfOrderPacket;
	int TCPDataAfterWindow;
	int TCPWindowProbe;
	int TCPWindowUpdatePacket;
	int TCPPacketReceivedAfterClose;
	int TCPDiscardedForBadChecksum;
	int TCPDiscardedForBadHeaderOffset;
	int TCPDiscardedBecausePacketTooShort;

	int IPTotal;
	int IPBadsum;
	int IPTooshort;
	int IPToosmall;
	int IPBadhlen;
	int IPbadlen;
	int IPInfragments;
	int IPFragdropped;
	int IPFragtimeout;
	int IPForward;
	int IPCantforward;
	int IPRedirectsent;
	int IPUnknownprotocol;
	int IPNobuffers;
	int IPReassembled;
	int IPOutfragments;
	int IPNoroute;
};

typedef std::vector<StatData> StatDataList;
typedef std::map<DECODER::Uint32, StatDataList> StatDataMap;

/* Classes *******************************************************************/

class ProcessLogData {
public:
	ProcessLogData(StatDataMap &map) : mStatDataMap(map) {}

	void operator()(const DECODER::RecordStreamOutput &record)
	{
		if (record.mMessage.find("TCP:") != std::string::npos) {
			StatData data = { 0 };

			data.StatDataTime = record.mTimeSeconds + (record.mTimeNanosec / 1000000000.0);

			mStatDataMap[record.mNodeID].push_back(data);
		}
		else if (record.mMessage.find(kTagTCPRetransmitTimeouts) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPRetransmitTimeouts = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPCompletelyDuplicatePacket) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPCompletelyDuplicatePacket = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPOutOfOrderPacket) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPOutOfOrderPacket = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPDataAfterWindow) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPDataAfterWindow = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPWindowProbe) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPWindowProbe = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPWindowUpdatePacket) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPWindowUpdatePacket = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPPacketReceivedAfterClose) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPPacketReceivedAfterClose = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPDiscardedForBadChecksum) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPDiscardedForBadChecksum = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPDiscardedForBadHeaderOffset) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPDiscardedForBadHeaderOffset = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagTCPDiscardedBecausePacketTooShort) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->TCPDiscardedBecausePacketTooShort = this->GetValueFromBegin(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPTotal) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPTotal = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPBadsum) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPBadsum = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPTooshort) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPTooshort = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPToosmall) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPToosmall = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPBadhlen) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPBadhlen = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPbadlen) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPbadlen = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPInfragments) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPInfragments = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPFragdropped) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPFragdropped = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPFragtimeout) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPFragtimeout = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPForward) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPForward = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPCantforward) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPCantforward = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPRedirectsent) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPRedirectsent = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPUnknownprotocol) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPUnknownprotocol = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPNobuffers) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPNobuffers = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPReassembled) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPReassembled = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPOutfragments) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPOutfragments = this->GetValueFromEnd(record.mMessage);
		}
		else if (record.mMessage.find(kTagIPNoroute) != std::string::npos) {
			mStatDataMap[record.mNodeID].rbegin()->IPNoroute = this->GetValueFromEnd(record.mMessage);
		}
	}

	int GetValueFromBegin(std::string text)
	{
		return std::atoi(text.substr(0, text.find(' ')).c_str());
	}

	int GetValueFromEnd(std::string text)
	{
		return std::atoi(text.substr(text.find_last_of(' ') + 1).c_str());
	}

private:
	StatDataMap &mStatDataMap;

	ProcessLogData(const ProcessLogData &o);
};

class TrimaAnalysis : public DECODER::LogExtractor {
public:
	typedef std::ostream Stream;

	TrimaAnalysis(Stream &stream) :
			mStream(stream),
			mProcessLogData(mStatDataMap),
			mProcessGetVxWorksInfo("vxworks_info", mLevelVxWorksInfo),
			mFilterLogData(mLevelVxWorksInfo, mProcessLogData) 
	{
	}

	/**
	 *
	 * Register
	 *
	 * Registers the necessary handlers with the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Register(DECODER::LogReader &reader);

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(DECODER::LogReader &reader);

	/**
	 *
	 * PreProcess
	 *
	 * Perform the procedures before the import process.
	 *
	 */
	virtual void PreProcess(void);

	/**
	 *
	 * PostProcess
	 *
	 * Perform the procedures after the import process.
	 *
	 */
	virtual void PostProcess(void);

	/**
	 *
	 * ExtractLogData
	 *
	 * Utility function that extracts the network data to a file.
	 *
	 * @param logFile The filename of the log file.
	 * @param datFile The filename of the data file.
	 *
	 * @return The results. 
	 *
	 */
	static DECODER::Result ExtractLogData(const DECODER::String &logFile, const DECODER::String &datFile);

private:
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, ProcessLogData> FilterLogData;

	Stream &mStream;

	StatDataMap mStatDataMap;

	DECODER::RecordLogLevel mLevelVxWorksInfo;
	DECODER::ProcessGetLevelID mProcessGetVxWorksInfo;

	FilterLogData mFilterLogData;
	ProcessLogData mProcessLogData;
};

} // namespace ANALYSIS

#endif // TRIMAANALYSIS_H

