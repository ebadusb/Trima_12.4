/*****************************************************************************\
**
** VistaRecExtractor.h
**
** Extracts the vista binary records from a datalog file.
**
\*****************************************************************************/

#ifndef VISTARECEXTRACTOR_H
#define VISTARECEXTRACTOR_H

/* Includes ******************************************************************/

#include <string>
#include <set>

#include "LogReader.h"
#include "LogExtractor.h"
#include "VistaRecordUtil.h"

namespace VISTAREC {

/* Classes *******************************************************************/

class VistaRecExtractor : public DECODER::LogExtractor {
public:
	typedef std::ostream Stream;
	typedef std::string String;
	typedef DECODER::LogReader LogReader;
	typedef DECODER::Result Result;

	VistaRecExtractor(Stream &stream) : mStream(stream), 
			mProcessVistaAlarmRec(mStream),
			mProcessVistaAlarmResponseRec(mStream),
			mProcessVistaBarcodeScanRec(mStream),
			mProcessVistaDonorRejectedRec(mStream),
			mProcessVistaParamAdjustRec(mStream),
			mProcessVistaPFRStatusRec(mStream),
			mProcessVistaDonorVitalsRec(mStream),
			mProcessVistaAuditTrailRec(mStream),
			mProcessVistaCatalogItemRec(mStream),
			mProcessVistaInterfaceStateRec(mStream),
			mProcessVistaRunSummaryRec(mStream) {}

	/**
	 *
	 * Register
	 *
	 * Registers the necessary handlers with the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Register(LogReader &reader);

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(LogReader &reader);

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
	 * ImportLogData
	 *
	 * Utility function that extracts the columns from a datalog file.
	 *
	 * @param filename The filename of the data log file.
	 * @param set The column name set.
	 *
	 * @return The results. 
	 *
	 */
	static Result ImportLogData(const String &filename, Stream &stream);

private:
	Stream &mStream;

	DECODER::UTIL::ProcessVistaAlarmRec mProcessVistaAlarmRec;
	DECODER::UTIL::ProcessVistaAlarmResponseRec mProcessVistaAlarmResponseRec;
	DECODER::UTIL::ProcessVistaBarcodeScanRec mProcessVistaBarcodeScanRec;
	DECODER::UTIL::ProcessVistaDonorRejectedRec mProcessVistaDonorRejectedRec;
	DECODER::UTIL::ProcessVistaParamAdjustRec mProcessVistaParamAdjustRec;
	DECODER::UTIL::ProcessVistaPFRStatusRec mProcessVistaPFRStatusRec;
	DECODER::UTIL::ProcessVistaDonorVitalsRec mProcessVistaDonorVitalsRec;
	DECODER::UTIL::ProcessVistaAuditTrailRec mProcessVistaAuditTrailRec;
	DECODER::UTIL::ProcessVistaCatalogItemRec mProcessVistaCatalogItemRec;
	DECODER::UTIL::ProcessVistaInterfaceStateRec mProcessVistaInterfaceStateRec;
	DECODER::UTIL::ProcessVistaRunSummaryRec mProcessVistaRunSummaryRec;
};

} // namespace VISTAREC

#endif // VISTARECEXTRACTOR_H

