// FILENAME: dupmsg.cpp
// PURPOSE: main for dupmsg tool
// CHANGELOG:
// $Header: E:/BCT_Development/vxWorks/Trima/engr_tools/dupmsg/rcs/dupmsg.cpp 6.2 2002/05/24 21:42:08Z js70375 Exp js70375 $
// $Log: dupmsg.cpp $
// Revision 6.2  2002/05/24 21:42:08Z  js70375
// added globals struct
// Revision 6.1  2002/05/17 19:01:08Z  js70375
// Initial port to vxworks


#include <new.h>
#include <tasklib.h>
#include <usrLib.h>
#include <sigLib.h>
#include <map>
#include <string>
#include <typeinfo>
#include <time.h>
#include <iostream.h>
#include <stdio.h>

#include "messagesystem.h"
#include "message.h"
#include "trimamessages.h"
#include "logger.hpp"
#include "optionparser.hpp"
#include "usage.hpp"

typedef map<string, Message<int> *> map_t;



//we'll use a struct to contain these "globals" to avoid name collisions
struct dupmsg_globals 
{
	 // globals that user can specify via command line
	char* Msg_type;    // type of message to dup
	int Send_count; // how many times to dup it
	int Send_delay; // how many msec to wait between dups
	bool Debug_flag;
	char* Send_string;
	int Send_data;
	int Timeout;   // max seconds to run, then exit
	bool Show_types;
	int Priority;
	int Notify_count;
	Message<int> *msgptr;

	//FUNCTION: populate_map
	//PURPOSE: populate map with all possible message types.
	void populate_map(map<string, Message<int>* > &);
	
} dmg;

void notify(void);
void alarm_handler(int);




int dupmsg_main(char *args)
{
	dmg.Notify_count = 0;

	MessageSystem ms;
	ms.initBlocking();

    set_new_handler(Logger::outOfMemory);
	copyStreams(1,2); // copy stderr to stdout

    Usage usage("can either resend or originate INT32 messages");
    Logger::Init(args, &usage);

    // parse command line
    OptionParser commandline(args, usage);
    commandline.parse("-n", "number of times to send the message", &dmg.Send_count, 1, 1, 10000);
    commandline.parse("-delay", "msec delay between message sends", &dmg.Send_delay, 0, 0, 1000000);
    commandline.parse("-debug", "enables debug messages", &dmg.Debug_flag);
    commandline.parse("-data", "send this data, default duplicates data received",
                      &dmg.Send_string, 0);
    commandline.parse("-priority", "set scheduling priority", &dmg.Priority, 10, 0, 255);
	commandline.parse("-showtypes", "display the message types", &dmg.Show_types);
    commandline.parse("-timeout", "max seconds before exiting, 0=never", &dmg.Timeout, 3600, 0, 100000);
    commandline.parse("-type", "message type to send", &dmg.Msg_type, "RequestSafetyPower");
    commandline.done();

	//create a map of all possible INT_32 messages
    map_t msgmap;

	//place a list of all messages inside a map container
	dmg.populate_map(msgmap);


    if(dmg.Debug_flag)
        Logger::Enable_debug();

    if(dmg.Send_string)
    {
        if(sscanf(dmg.Send_string, "%d", &dmg.Send_data) != 1)
        {
            LOG_ERROR("-data value must be an int, value was %s", dmg.Send_string);
            exit(1);
        }
    }
    //Loop through the msgmap container and display each message type.
	if(dmg.Show_types)
    {
		//iterate through the map displaying each of the messages
		map_t::const_iterator iter;
        LOG_INFO("\nmessage types supported");

		for (iter = msgmap.begin(); iter != msgmap.end(); iter++)
			cout << iter->first << '\n';
		
		exit(0);
    }
    //set process priority
    if(taskPrioritySet(taskIdSelf(), dmg.Priority) == -1)
    {
        LOG_ERROR("taskPrioritySet %d failed", dmg.Priority);
        exit(1);
    } 
 
    if (signal(SIGALRM, alarm_handler) == SIG_ERR)
    {
        LOG_ERROR("signal failed");
        exit(1);
    }
	//create a timer, settime on the timer, timer when time expires will send SIGALRM
	timer_t pTimer;
	int RELATIVE_TIME = 0;
    int timerRetVal = timer_create(CLOCK_REALTIME, NULL, &pTimer);

    itimerspec value; //create itimerspec struct to hold timespec structs
	value.it_value.tv_sec = dmg.Timeout;
	value.it_value.tv_nsec = 0;

	value.it_interval.tv_sec = 0;
	value.it_interval.tv_nsec = 0;

	if(timer_settime(pTimer, RELATIVE_TIME, &value, NULL) == -1)
	{
		LOG_INFO("Set time failed");
		exit(-1);
	}

	//search the msgmap for the desired msg.
	dmg.msgptr = msgmap[dmg.Msg_type];
	ASSERT(dmg.msgptr);

    //
    // either wait for msgs to dup, or send msgs outright
    if(dmg.Send_string == 0)
    {
        dmg.msgptr->init(CallbackBase(notify), MessageBase::SNDRCV_GLOBAL);
		ms.dispatchMessages();
    }
    else
    {
        LOG_INFO("delay %d msec between sends", dmg.Send_delay);
		time_t delay_sec = dmg.Send_delay/1000;
        time_t delay_nsec = dmg.Send_delay%1000 * 1000000;
        for(int i=1; i <= dmg.Send_count; i++)
        {
			timespec delay;
			delay.tv_sec = delay_sec;
			delay.tv_nsec = delay_nsec;

            nanosleep(&delay, NULL);
            LOG_INFO("send %d data=%d", i, dmg.Send_data); 
            dmg.msgptr->send(dmg.Send_data);
        }
    }

    LOG_INFO("exiting");
    return 0;
}


// FUNCTION: notify
// PURPOSE: resends received msg specified number of times with specified delays
void notify(void)
{
    dmg.Notify_count++;

    // receive msg
    int data = dmg.msgptr->get();

    // duplicate the msg loop
    LOG_INFO("notify %d  delay %d msec between sends", dmg.Notify_count, dmg.Send_delay);
	time_t delay_sec = dmg.Send_delay/1000;
    time_t delay_nsec = dmg.Send_delay%1000 * 1000000;
    for(int i=1; i <= dmg.Send_count; i++)
    {
        timespec delay;
	   	delay.tv_sec = delay_sec;
		delay.tv_nsec = delay_nsec;

        nanosleep(&delay, NULL);
        LOG_INFO("send %d data=%d", i, data); 
        dmg.msgptr->set(data);
    }
	
}

// FUNCTION: alarm_handler
// PURPOSE: handles SIGALRM signal, signals self with SIGTERM to force dispatcher cleanup
void alarm_handler(int dummy)
{
    int i = dummy; // use to avoid warning
    kill(taskIdSelf(), SIGTERM);
}


void dupmsg_globals::populate_map(map<string, Message<int> *> &msgmap)
{	
	msgmap[string("ACConnected")]				= new ACConnected(MessageBase::SEND_GLOBAL);
	msgmap[string("ACPrimeCompleted")] 			= new ACPrimeCompleted(MessageBase::SEND_GLOBAL);
	msgmap[string("AckRunComplete")] 			= new AckRunComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("AckTestCompleted")] 			= new AckTestCompleted(MessageBase::SEND_GLOBAL);
	msgmap[string("AirInPlasmaLine")] 			= new AirInPlasmaLine(MessageBase::SEND_GLOBAL);
    msgmap[string("AlarmResponse")] 			= new AlarmResponse(MessageBase::SEND_GLOBAL);
	msgmap[string("AlarmStateChange")] 			= new AlarmStateChange(MessageBase::SEND_GLOBAL);
	msgmap[string("AnswerGUIAlive")] 			= new AnswerGUIAlive(MessageBase::SEND_GLOBAL);
	msgmap[string("AnswerSafetyAlive")] 		= new AnswerSafetyAlive(MessageBase::SEND_GLOBAL);
	msgmap[string("ButtonPressedID")] 			= new ButtonPressedID(MessageBase::SEND_GLOBAL);
	msgmap[string("CancelProcedure")] 			= new CancelProcedure(MessageBase::SEND_GLOBAL);
	msgmap[string("CentrifugePressureStatus")] 	= new CentrifugePressureStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("ClampedForTesting")] 		= new ClampedForTesting(MessageBase::SEND_GLOBAL);
	msgmap[string("ConfirmDisposableLoaded")] 	= new ConfirmDisposableLoaded(MessageBase::SEND_GLOBAL);
	msgmap[string("ConfirmDonorDisconnected")] 	= new ConfirmDonorDisconnected(MessageBase::SEND_GLOBAL);
	msgmap[string("ConfirmEnableLoadSystem")] 	= new ConfirmEnableLoadSystem(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcConfigUpdateMsg")] 		= new ProcConfigUpdateMsg(MessageBase::SEND_GLOBAL);
	msgmap[string("DisconnectComplete")] 		= new DisconnectComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("DisposableLowered")] 		= new DisposableLowered(MessageBase::SEND_GLOBAL);
	msgmap[string("DisposableRaised")] 			= new DisposableRaised(MessageBase::SEND_GLOBAL);
	msgmap[string("DonorConnected")] 			= new DonorConnected(MessageBase::SEND_GLOBAL);
	msgmap[string("DonorDisconnected")] 		= new DonorDisconnected(MessageBase::SEND_GLOBAL);
	msgmap[string("DoPrediction")] 				= new DoPrediction (MessageBase::SEND_GLOBAL);
	msgmap[string("EnableLoadSystem ")] 		= new EnableLoadSystem (MessageBase::SEND_GLOBAL);
	msgmap[string("EndProcedure ")] 			= new EndProcedure (MessageBase::SEND_GLOBAL);
	msgmap[string("FinishedViewingStats ")] 	= new FinishedViewingStats (MessageBase::SEND_GLOBAL);
	msgmap[string("GUIRequestBeginRun ")] 		= new GUIRequestBeginRun (MessageBase::SEND_GLOBAL);
	msgmap[string("GUIRequestRaiseCassette")] 	= new GUIRequestRaiseCassette(MessageBase::SEND_GLOBAL);
	msgmap[string("GUIRequestStartDraw ")] 		= new GUIRequestStartDraw (MessageBase::SEND_GLOBAL);
	msgmap[string("InitializeInternalStates ")] = new InitializeInternalStates (MessageBase::SEND_GLOBAL);
	msgmap[string("InletPressureStatus ")] 		= new InletPressureStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("LogEvent")] 					= new LogEvent(MessageBase::SEND_GLOBAL);
	msgmap[string("NewControlData")] 			= new NewControlData(MessageBase::SEND_GLOBAL);
	msgmap[string("AlarmHandlerStatus")] 		= new AlarmHandlerStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcAlarm")] 				= new ProcAlarm(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcState")] 				= new ProcState(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcedureAdjustment")] 		= new ProcedureAdjustment(MessageBase::SEND_GLOBAL);
	msgmap[string("QueryGUIAlive")] 			= new QueryGUIAlive(MessageBase::SEND_GLOBAL);
	msgmap[string("QuerySafetyAlive ")] 		= new QuerySafetyAlive (MessageBase::SEND_GLOBAL);
	msgmap[string("RemoveAllChildScreens ")] 	= new RemoveAllChildScreens (MessageBase::SEND_GLOBAL);
	msgmap[string("RemoveAllStatelessScreens")] = new RemoveAllStatelessScreens(MessageBase::SEND_GLOBAL);
	msgmap[string("RemoveMostRecentChildScreen")] = new RemoveMostRecentChildScreen(MessageBase::SEND_GLOBAL);
	msgmap[string("RemoveMostRecentStatelessScreen")] =	new RemoveMostRecentStatelessScreen(MessageBase::SEND_GLOBAL);
	msgmap[string("RequestEndRunStats")] 		= new RequestEndRunStats(MessageBase::SEND_GLOBAL);
	msgmap[string("RequestProcedureStatus")] 	= new RequestProcedureStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("RequestSafetyPower")] 		= new RequestSafetyPower(MessageBase::SEND_GLOBAL);
	msgmap[string("RunComplete")] 				= new RunComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("SafetyPFRFileStatus")] 		= new SafetyPFRFileStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("SafetyTestDone")] 			= new SafetyTestDone(MessageBase::SEND_GLOBAL);
	msgmap[string("ScreenChangeRequest")] 		= new ScreenChangeRequest(MessageBase::SEND_GLOBAL);
	msgmap[string("Sound")] 					= new Sound(MessageBase::SEND_GLOBAL);
	msgmap[string("SpilloverRecovery")] 		= new SpilloverRecovery(MessageBase::SEND_GLOBAL);
	msgmap[string("SystemStateChange")] 		= new SystemStateChange(MessageBase::SEND_GLOBAL);
	msgmap[string("SystemStateChangeRequest")] 	= new SystemStateChangeRequest(MessageBase::SEND_GLOBAL);
	msgmap[string("TaskManAlive")] 				= new TaskManAlive(MessageBase::SEND_GLOBAL);
	msgmap[string("TestCompleted")] 			= new TestCompleted(MessageBase::SEND_GLOBAL);
	msgmap[string("IntTestMessage1 ")] 			= new IntTestMessage1 (MessageBase::SEND_GLOBAL);
	msgmap[string("IntTestMessage2 ")] 			= new IntTestMessage2 (MessageBase::SEND_GLOBAL);
	msgmap[string("TransitionValidation")] 		= new TransitionValidation(MessageBase::SEND_GLOBAL);
	msgmap[string("UpdateProcedureConfig ")] 	= new UpdateProcedureConfig (MessageBase::SEND_GLOBAL);
	msgmap[string("SwitchToPreviousScreen")] 	= new SwitchToPreviousScreen(MessageBase::SEND_GLOBAL);
	msgmap[string("RequestSafetyDriverStatus")] = new RequestSafetyDriverStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("UnsuccessfulCassetteLowering")] = new UnsuccessfulCassetteLowering(MessageBase::SEND_GLOBAL);
	msgmap[string("SafetyServiceMode")] 		= new SafetyServiceMode(MessageBase::SEND_GLOBAL);
	msgmap[string("MovePointerService")] 		= new MovePointerService(MessageBase::SEND_GLOBAL);
	msgmap[string("StateChangeComplete")] 		= new StateChangeComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("RinsebackStatus")] 			= new RinsebackStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("SendMachineMeterStatus")] 	= new SendMachineMeterStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("DisposableID")] 				= new DisposableID(MessageBase::SEND_GLOBAL);
	msgmap[string("DonorDataStatus")] 			= new DonorDataStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("PFRToOccur")] 				= new PFRToOccur(MessageBase::SEND_GLOBAL);
	msgmap[string("PFRForceAirToDonorMonitor")] = new PFRForceAirToDonorMonitor(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcSubstateChange")] 		= new ProcSubstateChange(MessageBase::SEND_GLOBAL);
	msgmap[string("MidCycleSwitch")] 			= new MidCycleSwitch(MessageBase::SEND_GLOBAL);
	msgmap[string("RequestHardwareVersion")] 	= new RequestHardwareVersion(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcToGuiPredictStatus")] 	= new ProcToGuiPredictStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("GuiToProcPredictRequest")] 	= new GuiToProcPredictRequest(MessageBase::SEND_GLOBAL);
	msgmap[string("GuiToProcDonorTime")] 		= new GuiToProcDonorTime(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcToGuiCassetteInfo")] 	= new ProcToGuiCassetteInfo(MessageBase::SEND_GLOBAL);
	msgmap[string("DeleteProduct")] 			= new DeleteProduct(MessageBase::SEND_GLOBAL);
	msgmap[string("OptimizerAlive")] 			= new OptimizerAlive(MessageBase::SEND_GLOBAL);
	msgmap[string("RepFluidConnectStatus")] 	= new RepFluidConnectStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("OperatorSetIdentification")] = new OperatorSetIdentification(MessageBase::SEND_GLOBAL);
	msgmap[string("DateTimeSet")] 				= new DateTimeSet(MessageBase::SEND_GLOBAL);
	msgmap[string("TimeJumpControl")] 			= new TimeJumpControl(MessageBase::SEND_GLOBAL);
	msgmap[string("ProductSelection")] 			= new ProductSelection(MessageBase::SEND_GLOBAL);
	msgmap[string("OptimizerSelected")] 		= new OptimizerSelected(MessageBase::SEND_GLOBAL);
	msgmap[string("OptimizerAreYouOk")] 		= new OptimizerAreYouOk(MessageBase::SEND_GLOBAL);
	msgmap[string("PredictNextProcedure")] 		= new PredictNextProcedure(MessageBase::SEND_GLOBAL);
	msgmap[string("PredictDebug")] 				= new PredictDebug(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcedureDebug")] 			= new ProcedureDebug(MessageBase::SEND_GLOBAL);
	msgmap[string("Rinseback")] 				= new Rinseback(MessageBase::SEND_GLOBAL);
	msgmap[string("AKO")] 						= new AKO(MessageBase::SEND_GLOBAL);
	msgmap[string("ConfigUpdateComplete")] 		= new ConfigUpdateComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcDescStartMsg")] 			= new ProcDescStartMsg(MessageBase::SEND_GLOBAL);
	msgmap[string("ProcDescEndMsg")] 			= new ProcDescEndMsg(MessageBase::SEND_GLOBAL);
	msgmap[string("BasinTemp")] 				= new BasinTemp(MessageBase::SEND_GLOBAL);
	msgmap[string("NotifySafetyOfA2")] 			= new NotifySafetyOfA2(MessageBase::SEND_GLOBAL);
	msgmap[string("RBCCalibrationRequest")] 	= new RBCCalibrationRequest(MessageBase::SEND_GLOBAL);
	msgmap[string("SendOrders")] 				= new SendOrders(MessageBase::SEND_GLOBAL);
	msgmap[string("EnableFastData")] 			= new EnableFastData(MessageBase::SEND_GLOBAL);
	msgmap[string("CentrifugeStop")] 			= new CentrifugeStop(MessageBase::SEND_GLOBAL);
	msgmap[string("SalineBolus")] 				= new SalineBolus(MessageBase::SEND_GLOBAL);
	msgmap[string("RBCChamberPurge")] 			= new RBCChamberPurge(MessageBase::SEND_GLOBAL);
	msgmap[string("SpilloverDetected")] 		= new SpilloverDetected(MessageBase::SEND_GLOBAL);
	msgmap[string("APSOutOfBounds")] 			= new APSOutOfBounds(MessageBase::SEND_GLOBAL);
	msgmap[string("RBCDetectorUncalibrated")] 	= new RBCDetectorUncalibrated(MessageBase::SEND_GLOBAL);
	msgmap[string("PendingCassetteAlarm")] 		= new PendingCassetteAlarm(MessageBase::SEND_GLOBAL);
	msgmap[string("CassettePositionStatus")] 	= new CassettePositionStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("RBCSetupNeeded")] 			= new RBCSetupNeeded(MessageBase::SEND_GLOBAL);
	msgmap[string("BarcodeScanned")] 			= new BarcodeScanned(MessageBase::SEND_GLOBAL);
	msgmap[string("GUISysDonorState")] 			= new GUISysDonorState(MessageBase::SEND_GLOBAL);
	msgmap[string("VistaPFRRequest")] 			= new VistaPFRRequest(MessageBase::SEND_GLOBAL);
	msgmap[string("VistaPFRStatus")] 			= new VistaPFRStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("VistaDonorCommitted")] 		= new VistaDonorCommitted(MessageBase::SEND_GLOBAL);
	msgmap[string("VistaDonorRejected")] 		= new VistaDonorRejected(MessageBase::SEND_GLOBAL);
	msgmap[string("RinsebackComplete")] 		= new RinsebackComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("RBCRestoreCalibration")] 	= new RBCRestoreCalibration(MessageBase::SEND_GLOBAL);
	msgmap[string("RetOccMonitorStatus")] 		= new RetOccMonitorStatus(MessageBase::SEND_GLOBAL);
	msgmap[string("SettleChannel")] 			= new SettleChannel(MessageBase::SEND_GLOBAL);
	msgmap[string("ClearLine")] 				= new ClearLine(MessageBase::SEND_GLOBAL);
	msgmap[string("ClearLineComplete")] 		= new ClearLineComplete(MessageBase::SEND_GLOBAL);
	msgmap[string("PFRSubstate")] 				= new PFRSubstate(MessageBase::SEND_GLOBAL);

}

