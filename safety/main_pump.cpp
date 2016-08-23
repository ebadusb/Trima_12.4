#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "s_module.hpp"
#include "s_monitor.hpp"
#include "pump_monitor.hpp"


class HallsTest // : public PumpMonitor
{
public:
   HallsTest() { }
   ~HallsTest() { }

   PumpMonitor Test (std::string inputCsv);

};

PumpMonitor HallsTest::Test (std::string inputCsv)
{
   PumpMonitor pm;
   int         pumpIndex = 0;
   int         lastRet   = 0;
   int         HALLS     = PumpMonitor::HALLS;
   int         ENCODERS  = PumpMonitor::ENCODERS;
   sscanf(inputCsv.c_str(), "%d,  %l,%l,  %l,%l,  %f,  %l,%l,%l,%l,  %l,%l,%l,%l,   %l,%l,%l,%l,  %d",
          // Format: PumpType	(Index) -
          &pumpIndex,
          // CurrentProcState	PreviousProcState
          &pm.m_state.proc, &pm.m_lastState,
          // Accum	AccumRevs
          &pm.m_controlStatus.pumps[pumpIndex].Accum, &pm.m_safetyStatus.pumps[pumpIndex].accumHalls,
          // Cmd
          &pm.m_commands.pumpRpm[pumpIndex],
          // HallsStarting	HallsLast	HallsMovement	HallsWhileDisengaged
          &pm.data[pumpIndex][HALLS].starting, &pm.data[pumpIndex][HALLS].last, &pm.data[pumpIndex][HALLS].movement, &pm.data[pumpIndex][HALLS].whileDisengaged,
          // EncodersStarting	EncodersLast	EncodersMovement	EncodersWhileDisengaged
          &pm.data[pumpIndex][ENCODERS].starting, &pm.data[pumpIndex][ENCODERS].last, &pm.data[pumpIndex][ENCODERS].movement, &pm.data[pumpIndex][ENCODERS].whileDisengaged,
          // ControlTimeStampSec	ControlTimeStampNanoSec	ControlTimeStampSecRhs ControlTimeStampNanoSecRhs
          &pm.m_controlStatus.timestamp.tv_sec, &pm.m_controlStatus.timestamp.tv_nsec, &pm.m_lastControlStatusTimes[0].tv_sec, &pm.m_lastControlStatusTimes[0].tv_nsec,
          // LastReturnDirFwd
          &lastRet);
   pm.m_lastReturnDirectionForward = (lastRet == 0) ? false : true;
   pm.check();
   return pm;
}

/* externs */
int main (int argc, char* argv[])
{
   HallsTest   ht;
   PumpMonitor pm = ht.Test("a,b,c <- Modify this");
}

/* FORMAT HASH 9c036b0ca678121bab6cad822a40983a */
