/*****************************************************************************\
**
** VistaRecExtractor.h
**
** Extracts the vista binary records from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>

#include "VistaRecExtractor.h"
#include "LogExecute.h"

///this is the single instance of the recordlogheader
DECODER::RecordLogHeader DECODER::RecordLogHeader::LastRecordLogHeader;

namespace VISTAREC {

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
void VistaRecExtractor::Register(LogReader &reader)
{
	reader.RegisterBinary(mProcessVistaAlarmRec);
	reader.RegisterBinary(mProcessVistaAlarmResponseRec);
	reader.RegisterBinary(mProcessVistaBarcodeScanRec);
	reader.RegisterBinary(mProcessVistaDonorRejectedRec);
	reader.RegisterBinary(mProcessVistaParamAdjustRec);
	reader.RegisterBinary(mProcessVistaPFRStatusRec);
	reader.RegisterBinary(mProcessVistaDonorVitalsRec);
	reader.RegisterBinary(mProcessVistaAuditTrailRec);
	reader.RegisterBinary(mProcessVistaCatalogItemRec);
	reader.RegisterBinary(mProcessVistaInterfaceStateRec);
	reader.RegisterBinary(mProcessVistaRunSummaryRec);
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
void VistaRecExtractor::Unregister(LogReader &reader)
{
	reader.UnregisterBinary(mProcessVistaAlarmRec);
	reader.UnregisterBinary(mProcessVistaAlarmResponseRec);
	reader.UnregisterBinary(mProcessVistaBarcodeScanRec);
	reader.UnregisterBinary(mProcessVistaDonorRejectedRec);
	reader.UnregisterBinary(mProcessVistaParamAdjustRec);
	reader.UnregisterBinary(mProcessVistaPFRStatusRec);
	reader.UnregisterBinary(mProcessVistaDonorVitalsRec);
	reader.UnregisterBinary(mProcessVistaAuditTrailRec);
	reader.UnregisterBinary(mProcessVistaCatalogItemRec);
	reader.UnregisterBinary(mProcessVistaInterfaceStateRec);
	reader.UnregisterBinary(mProcessVistaRunSummaryRec);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void VistaRecExtractor::PreProcess(void)
{
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void VistaRecExtractor::PostProcess(void)
{
}

/**
 *
 * ImportLogData
 *
 * Inports the data from the data log file.
 *
 * @param filename The filename of the data log file.
 *
 */
VistaRecExtractor::Result VistaRecExtractor::ImportLogData(const String &filename, Stream &stream)
{
	return DECODER::ProcessLogData(filename.c_str(), VistaRecExtractor(stream));
}

} // namespace VISTAREC

