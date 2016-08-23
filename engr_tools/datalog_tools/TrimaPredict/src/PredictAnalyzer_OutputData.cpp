#include "PredictAnalyzer.h"
#include "PredictDataUtil.h"

namespace PREDICT
{

void PredictAnalyzer::OutputCommonData(std::ostream& stream, procedureDetailPltPlsRBC& proc, int procNum, DECODER::UTIL::Donor& donor, const std::string& gender)
{
    stream 
	 << donor.id 
    <<"," << procNum
    <<"," << donor.tbv
    <<"," << donor.precount
    <<"," << donor.hct
    <<"," << gender
    <<"," << donor.height
    <<"," << donor.weight
    <<"," << proc.getDesiredYield()
	 <<"," << proc.getPredVp()
    <<"," << proc.getProcFixedCollectVol()
	 <<"," << proc.getPredVc()
	 <<"," << proc.getPredRBCVolume()
    <<"," << proc.getProcPlateletProdCon()
	 <<"," << proc.getVintarg()
    <<"," << proc.getVptarg()
    <<"," << proc.getYtarg()
    <<"," << proc.getVinpur()
	 <<"," << proc.getVinPrime()
	 <<"," << proc.getPrimeDuration()
	 <<"," << proc.VinPIRStart()
	 <<"," << proc.getPIRLockin()
	 <<"," << proc.YdchPIR();
    stream.precision(4);
    stream <<"," << proc.getYdch()
	 <<"," << proc.getYdpur();
    stream.precision(2);
	 stream <<"," << proc.getPredVac()
    <<"," << proc.getVpmin()
    <<"," << proc.getMaxQin();
    stream.precision(4);
    stream <<"," << proc.getEc();
    stream.precision(2);
    stream <<"," << proc.getQinc()
    <<"," << proc.getQcmr()
    <<"," << proc.getTc()
	 <<"," << ((proc.getPredTp() > 0) ? (proc.getPredTp()): 0)
	 <<"," << proc.getStatus(); 
}

void PredictAnalyzer::OutputSubstateHeader(std::ostream& stream, std::string substate, IntermediateResults result)
{
    stream
	 << "," << result.AccumTime	
	 << "," << result.AccumVac
	 << "," << result.AccumVacc
	 << "," << result.AccumVacp
	 << "," << result.AccumVc
	 << "," << result.AccumVp
	 << "," << result.AccumVrep
	 << "," << result.AvgHt;
    stream.precision(3);
	 stream << "," << result.D;
    stream.precision(2);
	 stream << "," << result.Csf
	 << "," << result.EndVin
	 << "," << result.Facb
	 << "," << result.Facp
    << "," << result.Hend
	 << "," << result.Hin
	 << "," << result.HypovolemiaDistance
	 << "," << result.HypovolemiaMargin
	 << "," << result.Qacinst
	 << "," << result.Qch
	 << "," << result.Qcinstd
	 << "," << result.Qcinstr
	 << "," << result.Qininstd
	 << "," << result.Qininstr
	 << "," << result.Qnret
	 << "," << result.Qpinstd
	 << "," << result.Qpinstr
	 << "," << result.Qrep
	 << "," << result.Qrinst
	 << "," << result.Rpm
	 << "," << result.T
	 << "," << result.Tcn
	 << "," << result.Td
	 << "," << result.Tr
	 << "," << result.Vac
	 << "," << result.Vacc
	 << "," << result.Vacp
	 << "," << result.Vacr
	 << "," << result.Vc
	 << "," << result.Vp
	 << "," << result.Vr
	 << "," << result.Vrep
	 << "," << result.Vsvn;
}









void PredictAnalyzer::OutputCommonData(std::ostream& stream, procedureDetailRBCPls& proc, int procNum, DECODER::UTIL::Donor& donor, const std::string& gender)
{
    stream 
	 << donor.id 
    <<"," << procNum
    <<"," << donor.tbv
    <<"," << donor.precount
    <<"," << donor.hct
    <<"," << gender
    <<"," << donor.height
    <<"," << donor.weight
    <<"," << proc.getDesiredYield()
	 <<"," << proc.getPredVp()
    <<"," << proc.getProcFixedCollectVol()
	 <<"," << proc.getPredVc()
	 <<"," << proc.getPredRBCVolume()
    <<"," << proc.getProcPlateletProdCon()
	 <<"," << proc.getVintarg()
    <<"," << proc.getVptarg()
    <<"," << proc.getYtarg()
    <<"," << "NA"
	 <<"," << proc.getVinPrime()
	 <<"," << proc.getPrimeDuration()
	 <<"," << proc.VinPIRStart()
	 <<"," << proc.getPIRLockin()
	 <<"," << proc.YdchPIR()
    //precision shift
	 <<"," << "NA"
	 <<"," << proc.getYdpur()
    //precision shift
	 <<"," << proc.getPredVac()
    <<"," << proc.getVpmin()
    <<"," << proc.getMaxQin()
    //precision shift
    <<"," << "NA"
    //precision shift
    <<"," << "NA"
    <<"," << "NA"
    <<"," << proc.getTc()
	 <<"," << ((proc.getPredTp() > 0) ? (proc.getPredTp()): 0)
	 <<"," << proc.getStatus(); 
}

}