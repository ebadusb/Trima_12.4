/* FILENAME: cs01.cpp
 * $Header: L:/BCT_Development/Trima/engr_tools/spoofer/test/rcs/cs01.cpp 1.1 2000/01/13 22:16:40 TD07711 Exp td07711 $
 * PURPOSE: main for testing spoofing of control status messages
 * CHANGELOG:
 * $Log: cs01.cpp $
 * Revision 1.1  2000/01/13 22:16:40  TD07711
 * Initial revision
 * 01/07/2000 - dyes - initial version
 */


#include "buffmsg.hpp"
#include "chw.hpp"
#include "spooferlog.hpp"  // include last to avoid ASSERT nameclash


dispatcher* dispatch;                        // message dispatcher



class TestMessage : public focusBufferMsg<CHwStates>
{
    public:

        TestMessage();
        ~TestMessage();
        void notify();   // process incoming message

        // new functions for TestMessage
        void setup();    // setup outgoing message
        void send() { set(&_outbuf); };     // send message
        void receive() { get(&_inbuf); };  // receive message
        void print(const CHwStates& msgbuf);
        void diff(); // compares inbuf and outbuf

    protected:

    CHwStates _inbuf;
    CHwStates _outbuf;

};



main(int argc, char** argv)
{
    Logger::Init(argc, argv);

    dispatch = new dispatcher(argc, argv);

    // register the TestMessage with the dispatcher
    TestMessage msg;

    msg.setup();  // sets up message data
    alarm(10);   // timeout if echo not done within 10 seconds
    msg.send();  // sends the message

    dispatch->dispatchLoop();

    LOG_INFO("exiting");

    return 0;
}


TestMessage::TestMessage()
: focusBufferMsg<CHwStates>(ControlHardwareStatus)
{
    // zero msg buffers
    memset(&_inbuf, 0, sizeof(_inbuf)); 
    memset(&_outbuf, 0, sizeof(_outbuf));
    LOG_DEBUG("TestMessage constructed");
}


TestMessage::~TestMessage()
{
    LOG_DEBUG("TestMessage destroyed");
}


void TestMessage::notify()
{
    LOG_INFO("TestMessage notify");

    // receive and print one message, then exit dispatch loop
    receive();
    diff();
    kill(getpid(), SIGQUIT);
}


void TestMessage::diff()
{
    // convert all fields to ints for logging
    #define DIFF(field)                         \
    if(_outbuf.field != _inbuf.field)               \
    {                                               \
        LOG_INFO("diff: %s out=%d in=%d", #field,   \
                 (int)_outbuf.field, (int)_inbuf.field);      \
    }

    DIFF(accessPressure);
    DIFF(batteryCurrent);
    DIFF(batteryVoltage);
    DIFF(emiTemperature);
    DIFF(centrifugeCurrent);
    DIFF(centrifugePressure);
    DIFF(fiveVoltSupply);
    DIFF(leakDetector);
    DIFF(minusTwelvePS);
    DIFF(sixtyFourVoltSupply);
    DIFF(sixtyFourVoltSwitched);
    DIFF(twelveVoltSupply);
    DIFF(twentyFourVoltCurrent);
    DIFF(twentyFourVoltSupply);
    DIFF(twentyFourVoltSwitched);
    DIFF(cassettePosition);
    DIFF(cassetteError);
    DIFF(centrifugeRPM);
    DIFF(centrifugeError);
    DIFF(doorLocks);
    DIFF(pauseSwitch);
    DIFF(stopSwitch);
    DIFF(pauseSwitchLatch);
    DIFF(stopSwitchLatch);
    DIFF(vibration);
    DIFF(valveFault);
    DIFF(motorFault);
    DIFF(solenoidFault);
    DIFF(red);
    DIFF(green);
    DIFF(rbcCommError);
    DIFF(acDetector);
    DIFF(reservoir);
    DIFF(plasmaValve);
    DIFF(plateletValve);
    DIFF(rbcValve);
    DIFF(fanSense0);
    DIFF(fanSense1);
    DIFF(fanSense2);
    DIFF(tempError);
    DIFF(psTempWarning);
    DIFF(soundLevel);
    DIFF(event);
    DIFF(msgCount);
    DIFF(test1);
    DIFF(lowAGC);
    DIFF(highAGC);

    DIFF(acPump.errors);
    DIFF(acPump.RPM);
    DIFF(acPump.Revs);
    DIFF(acPump.Accum);
    DIFF(acPump.Integral);
    DIFF(acPump.AcummIntegral);

    DIFF(inletPump.errors);
    DIFF(inletPump.RPM);
    DIFF(inletPump.Revs);
    DIFF(inletPump.Accum);
    DIFF(inletPump.Integral);
    DIFF(inletPump.AcummIntegral);

    DIFF(returnPump.errors);
    DIFF(returnPump.RPM);
    DIFF(returnPump.Revs);
    DIFF(returnPump.Accum);
    DIFF(returnPump.Integral);
    DIFF(returnPump.AcummIntegral);

    DIFF(plateletPump.errors);
    DIFF(plateletPump.RPM);
    DIFF(plateletPump.Revs);
    DIFF(plateletPump.Accum);
    DIFF(plateletPump.Integral);
    DIFF(plateletPump.AcummIntegral);

    DIFF(plasmaPump.errors);
    DIFF(plasmaPump.RPM);
    DIFF(plasmaPump.Revs);
    DIFF(plasmaPump.Accum);
    DIFF(plasmaPump.Integral);
    DIFF(plasmaPump.AcummIntegral);
}


void TestMessage::setup()
{
    LOG_INFO("TestMessage setup");
    _outbuf.accessPressure = 100;
    _outbuf.batteryCurrent = 101;
    _outbuf.batteryVoltage = 102;
    _outbuf.emiTemperature = 103;
    _outbuf.centrifugeCurrent = 104;
    _outbuf.centrifugePressure = 105;
    _outbuf.fiveVoltSupply = 106;
    _outbuf.leakDetector = 107;
    _outbuf.minusTwelvePS = 108;
    _outbuf.sixtyFourVoltSupply = 109;
    _outbuf.sixtyFourVoltSwitched = 110;
    _outbuf.twelveVoltSupply = 111;
    _outbuf.twentyFourVoltCurrent = 112;
    _outbuf.twentyFourVoltSupply = 113;
    _outbuf.twentyFourVoltSwitched = 114;
    _outbuf.cassettePosition = HW_CASSETTE_UNKNOWN;
    _outbuf.cassetteError = HW_NO_FAULT;
    _outbuf.centrifugeRPM = 120;
    _outbuf.centrifugeError = HW_CENT_HARDWARE_ERROR;
    _outbuf.doorLocks = HW_DOOR_CLOSED_STATE;
    _outbuf.pauseSwitch = HW_SWITCH_ENABLED;
    _outbuf.pauseSwitchLatch = HW_SWITCH_ENABLED;
    _outbuf.stopSwitchLatch = HW_SWITCH_DISABLED;
    _outbuf.vibration = HW_FAULT;
    _outbuf.valveFault = HW_NO_FAULT;
    _outbuf.motorFault = HW_FAULT;
    _outbuf.red = 200;
    _outbuf.green = 201;
    _outbuf.rbcCommError = HW_FAULT;
    _outbuf.acDetector = CHW_AC_DETECTOR_FLUID;
    _outbuf.reservoir = CHW_RESERVOIR_EMPTY;
    _outbuf.plasmaValve = HW_VALVE_COLLECT;
    _outbuf.plateletValve = HW_VALVE_RETURN;
    _outbuf.rbcValve = HW_VALVE_OPEN;
    _outbuf.fanSense0 = HW_NO_FAULT;
    _outbuf.fanSense1 = HW_FAULT;
    _outbuf.fanSense2 = HW_NO_FAULT;
    _outbuf.tempError = HW_FAULT;
    _outbuf.psTempWarning = CHW_PS_OTW;
    _outbuf.soundLevel = CHW_SOUND_SOFT;
    _outbuf.event = CHW_DOOR_EVENT;
    _outbuf.msgCount = 300;
    _outbuf.test1 = 301;
    _outbuf.lowAGC = 302;
    _outbuf.highAGC = 303;
    
    _outbuf.acPump.errors = CHW_PUMP_NO_FAULT;
    _outbuf.acPump.RPM = 50;
    _outbuf.acPump.Revs = 51;
    _outbuf.acPump.Accum = 52;
    _outbuf.acPump.Integral = 53;
    _outbuf.acPump.AcummIntegral = 54;

    _outbuf.inletPump.errors = CHW_PUMP_FAIL_TO_MAINTAIN_SPEED;
    _outbuf.inletPump.RPM = 55;
    _outbuf.inletPump.Revs = 56;
    _outbuf.inletPump.Accum = 57;
    _outbuf.inletPump.Integral = 58;
    _outbuf.inletPump.AcummIntegral = 59;

    _outbuf.returnPump.errors = CHW_PUMP_INTEGRAL_ERROR;
    _outbuf.returnPump.RPM = 60;
    _outbuf.returnPump.Revs = 61;
    _outbuf.returnPump.Accum = 62;
    _outbuf.returnPump.Integral = 63;
    _outbuf.returnPump.AcummIntegral = 64;

    _outbuf.plateletPump.errors = CHW_PUMP_NO_FAULT;
    _outbuf.plateletPump.RPM = 65;
    _outbuf.plateletPump.Revs = 66;
    _outbuf.plateletPump.Accum = 67;
    _outbuf.plateletPump.Integral = 68;
    _outbuf.plateletPump.AcummIntegral = 69;

    _outbuf.plasmaPump.errors = CHW_PUMP_FAIL_TO_MAINTAIN_SPEED;
    _outbuf.plasmaPump.RPM = 70;
    _outbuf.plasmaPump.Revs = 71;
    _outbuf.plasmaPump.Accum = 72;
    _outbuf.plasmaPump.Integral = 73;
    _outbuf.plasmaPump.AcummIntegral = 74;

    print(_outbuf);
}


void TestMessage::print(const CHwStates& msg)
{
    LOG_INFO("TestMessage printing");

    #define PRINT(field) LOG_INFO("%s = %d", #field, (int)msg.field)

    PRINT(accessPressure);
    PRINT(batteryCurrent);
    PRINT(batteryVoltage);
    PRINT(emiTemperature);
    PRINT(centrifugeCurrent);
    PRINT(centrifugePressure);
    PRINT(fiveVoltSupply);
    PRINT(leakDetector);
    PRINT(minusTwelvePS);
    PRINT(sixtyFourVoltSupply);
    PRINT(sixtyFourVoltSwitched);
    PRINT(twelveVoltSupply);
    PRINT(twentyFourVoltCurrent);
    PRINT(twentyFourVoltSupply);
    PRINT(twentyFourVoltSwitched);
    PRINT(cassettePosition);
    PRINT(cassetteError);
    PRINT(centrifugeRPM);
    PRINT(centrifugeError);
    PRINT(doorLocks);
    PRINT(pauseSwitch);
    PRINT(stopSwitch);
    PRINT(pauseSwitchLatch);
    PRINT(stopSwitchLatch);
    PRINT(vibration);
    PRINT(valveFault);
    PRINT(motorFault);
    PRINT(solenoidFault);
    PRINT(red);
    PRINT(green);
    PRINT(rbcCommError);
    PRINT(acDetector);
    PRINT(reservoir);
    PRINT(plasmaValve);
    PRINT(plateletValve);
    PRINT(rbcValve);
    PRINT(fanSense0);
    PRINT(fanSense1);
    PRINT(fanSense2);
    PRINT(tempError);
    PRINT(psTempWarning);
    PRINT(soundLevel);
    PRINT(event);
    PRINT(msgCount);
    PRINT(test1);
    PRINT(lowAGC);
    PRINT(highAGC);

    PRINT(acPump.errors);
    PRINT(acPump.RPM);
    PRINT(acPump.Revs);
    PRINT(acPump.Accum);
    PRINT(acPump.Integral);
    PRINT(acPump.AcummIntegral);

    PRINT(inletPump.errors);
    PRINT(inletPump.RPM);
    PRINT(inletPump.Revs);
    PRINT(inletPump.Accum);
    PRINT(inletPump.Integral);
    PRINT(inletPump.AcummIntegral);

    PRINT(returnPump.errors);
    PRINT(returnPump.RPM);
    PRINT(returnPump.Revs);
    PRINT(returnPump.Accum);
    PRINT(returnPump.Integral);
    PRINT(returnPump.AcummIntegral);

    PRINT(plateletPump.errors);
    PRINT(plateletPump.RPM);
    PRINT(plateletPump.Revs);
    PRINT(plateletPump.Accum);
    PRINT(plateletPump.Integral);
    PRINT(plateletPump.AcummIntegral);

    PRINT(plasmaPump.errors);
    PRINT(plasmaPump.RPM);
    PRINT(plasmaPump.Revs);
    PRINT(plasmaPump.Accum);
    PRINT(plasmaPump.Integral);
    PRINT(plasmaPump.AcummIntegral);
}

