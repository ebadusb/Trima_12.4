/* FILENAME: ss01.cpp
 * $Header: L:/BCT_Development/Trima/engr_tools/spoofer/test/rcs/ss01.cpp 1.1 2000/01/13 22:16:56 TD07711 Exp td07711 $
 * PURPOSE: main for testing spoofing of safety status messages
 * CHANGELOG:
 * $Log: ss01.cpp $
 * Revision 1.1  2000/01/13 22:16:56  TD07711
 * Initial revision
 * 01/07/2000 - dyes - initial version
 */


#include "buffmsg.hpp"
#include "shw.hpp"
#include "spooferlog.hpp"  // include last to avoid ASSERT nameclash


dispatcher* dispatch;                        // message dispatcher



class TestMessage : public focusBufferMsg<SHwStates>
{
    public:

        TestMessage();
        ~TestMessage();
        void notify();   // process incoming message

        // new functions for TestMessage
        void setup();    // setup outgoing message
        void send() { set(&_outbuf); };     // send message
        void receive() { get(&_inbuf); };  // receive message
        void print(const SHwStates& msgbuf);
        void diff(); // compares inbuf and outbuf

    protected:

    SHwStates _inbuf;
    SHwStates _outbuf;

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
: focusBufferMsg<SHwStates>(SafetyHardwareStatus)
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

    DIFF(inletRPM);
    DIFF(inletRevs);
    DIFF(inletAccumRevs);
    DIFF(plateletRPM);
    DIFF(plateletRevs);
    DIFF(plateletAccumRevs);
    DIFF(plasmaRPM);
    DIFF(plasmaRevs);
    DIFF(plasmaAccumRevs);
    DIFF(acRPM);
    DIFF(acRevs);
    DIFF(acAccumRevs);
    DIFF(returnRPM);
    DIFF(returnRevs);
    DIFF(returnAccumRevs);
    DIFF(returnDirection);
    DIFF(rbcValve);
    DIFF(plasmaValve);
    DIFF(plateletValve);
    DIFF(valveLedTest);
    DIFF(cassettePosition);
    DIFF(reservoir);
    DIFF(centrifugeRPM);
    DIFF(centrifugeError);
    DIFF(doorLocks);
    DIFF(pauseSwitch);
    DIFF(stopSwitch);
    DIFF(event);
    DIFF(msgCount);
    DIFF(returnPumpDirChgTime);
    DIFF(test1);
    DIFF(test2);
    DIFF(serviceEnableMode);
}


void TestMessage::setup()
{
    LOG_INFO("TestMessage setup");
    _outbuf.inletRPM = 100;
    _outbuf.inletRevs = 101;
    _outbuf.inletAccumRevs = 102;
    _outbuf.plateletRPM = 103;
    _outbuf.plateletRevs = 104;
    _outbuf.plateletAccumRevs = 105;
    _outbuf.plasmaRPM = 106;
    _outbuf.plasmaRevs = 107;
    _outbuf.plasmaAccumRevs = 108;
    _outbuf.acRPM = 109;
    _outbuf.acRevs = 110;
    _outbuf.acAccumRevs = 111;
    _outbuf.returnRPM = 112;
    _outbuf.returnRevs = 113;
    _outbuf.returnAccumRevs = 114;
    _outbuf.returnDirection = SHW_DRAW;
    _outbuf.rbcValve = HW_VALVE_OPEN;
    _outbuf.plasmaValve = HW_VALVE_RETURN;
    _outbuf.plateletValve = HW_VALVE_UNKNOWN;
    _outbuf.valveLedTest = 1;
    _outbuf.cassettePosition = HW_CASSETTE_UP;
    _outbuf.reservoir = SHW_RESERVOIR_EMPTY;
    _outbuf.centrifugeRPM = 200;
    _outbuf.centrifugeError = HW_CENT_NO_FAULT;
    _outbuf.doorLocks = HW_DOOR_CLOSED_STATE;
    _outbuf.pauseSwitch = HW_SWITCH_DISABLED;
    _outbuf.stopSwitch = HW_SWITCH_ENABLED;
    _outbuf.event = SHW_RESERVOIR_EVENT;
    _outbuf.msgCount = 1000;
    _outbuf.returnPumpDirChgTime = 2000;
    _outbuf.test1 = 3000;
    _outbuf.test2 = 4000;
    _outbuf.serviceEnableMode = HW_DISABLE;
    
    print(_outbuf);
}


void TestMessage::print(const SHwStates& msg)
{
    LOG_INFO("TestMessage printing");

    #define PRINT(field) LOG_INFO("%s = %d", #field, (int)msg.field)

    PRINT(inletRPM);
    PRINT(inletRevs);
    PRINT(inletAccumRevs);
    PRINT(plateletRPM);
    PRINT(plateletRevs);
    PRINT(plateletAccumRevs);
    PRINT(plasmaRPM);
    PRINT(plasmaRevs);
    PRINT(plasmaAccumRevs);
    PRINT(acRPM);
    PRINT(acRevs);
    PRINT(acAccumRevs);
    PRINT(returnRPM);
    PRINT(returnRevs);
    PRINT(returnAccumRevs);
    PRINT(returnDirection);
    PRINT(rbcValve);
    PRINT(plasmaValve);
    PRINT(plateletValve);
    PRINT(valveLedTest);
    PRINT(cassettePosition);
    PRINT(reservoir);
    PRINT(centrifugeRPM);
    PRINT(centrifugeError);
    PRINT(doorLocks);
    PRINT(pauseSwitch);
    PRINT(stopSwitch);
    PRINT(event);
    PRINT(msgCount);
    PRINT(returnPumpDirChgTime);
    PRINT(test1);
    PRINT(test2);
    PRINT(serviceEnableMode);
}

