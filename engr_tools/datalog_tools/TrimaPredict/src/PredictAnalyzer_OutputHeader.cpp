#include "PredictAnalyzer.h"


namespace PREDICT
{

void PredictAnalyzer::OutputCommonHeader(std::ostream& stream, std::string tbv, std::string ht, std::string wt)
{
    stream << 
    "Donor No."
    ",Proc No." <<
    "," << tbv <<
    ",Cpre" 
    ",H(%)" 
    ",Gender" <<
    "," << ht  <<
    "," << wt  <<
    ",Yield"
    ",Vp"
    ",Vc-Target"
    ",Vc-Predict"
    ",Vrbc"
    ",PltConc"
    ",Vintarg"
    ",Vptarg"
    ",Ytarg"
    ",Vinpur"
    ",VinPrime"
    ",PrimeDuration"
    ",VinPIRstart"
    ",PIRLockIn"
    ",YdchPIR"
    //precision shift
    ",Ydch"
    ",Ydpur"
    //precision shift
    ",Vac"
    ",Vpmin"
    ",MaxQin"
    //precision shift
    ",Ec"
    //precision shift
    ",Qinc"
    ",Qcmr"
    ",Tc"
    ",Time(min)"
    ",Status";
}

void PredictAnalyzer::OutputSubstateHeader(std::ostream& stream, std::string substate)
{
    stream 
    << ",AccumTime("<<substate<<")"
    << ",AccumVac("<<substate<<")"
    << ",AccumVacc("<<substate<<")"
    << ",AccumVacp("<<substate<<")"
    << ",AccumVc("<<substate<<")"
    << ",AccumVp("<<substate<<")"
    << ",AccumVrep("<<substate<<")"
    << ",AvgHt("<<substate<<")"
    //precision shift
    << ",D("<<substate<<")"
    //precision shift
    << ",Csf("<<substate<<")"
    << ",EndVin("<<substate<<")"
    << ",Facb("<<substate<<")"
    << ",Facp("<<substate<<")"
    << ",Hend("<<substate<<")"
    << ",Hin("<<substate<<")"
    << ",HypovolemiaDistance("<<substate<<")"
    << ",HypovolemiaMargin("<<substate<<")"
    << ",Qacinst("<<substate<<")"
    << ",Qch("<<substate<<")"
    << ",Qcinstd("<<substate<<")"
    << ",Qcinstr("<<substate<<")"
    << ",Qininstd("<<substate<<")"
    << ",Qininstr("<<substate<<")"
    << ",Qnret("<<substate<<")"
    << ",Qpinstd("<<substate<<")"
    << ",Qpinstr("<<substate<<")"
    << ",Qrep("<<substate<<")"
    << ",Qrinst("<<substate<<")"
    << ",RPM("<<substate<<")"
    << ",Time("<<substate<<")"
    << ",Tcn("<<substate<<")"
    << ",Td("<<substate<<")"
    << ",Tr("<<substate<<")"
    << ",Vac("<<substate<<")"
    << ",Vacc("<<substate<<")"
    << ",Vacp("<<substate<<")"
    << ",Vacr("<<substate<<")"
    << ",Vc("<<substate<<")"
    << ",Vp("<<substate<<")"
    << ",Vr("<<substate<<")"
    << ",Vrep("<<substate<<")"
    << ",Vsvn("<<substate<<")";
}

}

