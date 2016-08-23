/* FILENAME: co01.cpp
 * $Header: K:/BCT_Development/Trima/engr_tools/spoofer/test/rcs/co01.cpp 1.2 2000/07/19 15:47:12 td07711 Exp $
 * PURPOSE: main for testing spoofing of control order messages
 * CHANGELOG:
 * $Log: co01.cpp $
 * Revision 1.2  2000/07/19 15:47:12  td07711
 *   header files now all lower case.
 * Revision 1.1  2000/01/13 22:16:38  TD07711
 * Initial revision
 * 12/30/99 - dyes - initial version
 */


#include "buffmsg.hpp"
#include "chw.hpp"
#include "spooferlog.hpp"  // include last to avoid ASSERT nameclash


dispatcher* dispatch;                        // message dispatcher



class TestMessage : public focusBufferMsg<CHwOrders>
{
    public:

        TestMessage();
        ~TestMessage();
        void notify();   // process incoming message

        // new functions for TestMessage
        void setup();    // setup outgoing message
        void send() { set(&_outbuf); };     // send message
        void receive() { get(&_inbuf); };  // receive message
        void print(const CHwOrders& msgbuf);
        void diff(); // compares inbuf and outbuf

    protected:

    CHwOrders _inbuf;
    CHwOrders _outbuf;

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
: focusBufferMsg<CHwOrders>(ControlHardwareCommands)
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

    DIFF(cassettePosition);
    DIFF(centrifugeRPM);
    DIFF(centrifugeRampUpRate);
    DIFF(centrifugeRampDownRate);
    DIFF(alarmLight);
    DIFF(doorLatches);
    DIFF(doorDirection);
    DIFF(inletRPM);
    DIFF(returnRPM);
    DIFF(acRPM);
    DIFF(plateletRPM);
    DIFF(plasmaRPM);
    DIFF(rbcValve);
    DIFF(plasmaValve);
    DIFF(plateletValve);
    DIFF(redDrive);
    DIFF(greenDrive);
    DIFF(soundLevel);
    DIFF(test1);
    DIFF(highAPSLimit);
    DIFF(lowAPSLimit);
}


void TestMessage::setup()
{
    LOG_INFO("TestMessage setup");
    _outbuf.cassettePosition = CHW_CASSETTE_DOWN;
    _outbuf.centrifugeRPM = 10;
    _outbuf.centrifugeRampUpRate = 11;
    _outbuf.centrifugeRampDownRate = 12;
    _outbuf.alarmLight = HW_DISABLE;
    _outbuf.doorLatches = HW_ENABLE;
    _outbuf.doorDirection = CHW_DOOR_LOCK;
    _outbuf.inletRPM = 20;
    _outbuf.plateletRPM = 21;
    _outbuf.plasmaRPM = 22;
    _outbuf.acRPM = 23;
    _outbuf.returnRPM = 24;
    _outbuf.rbcValve = CHW_VALVE_COLLECT;
    _outbuf.plasmaValve = CHW_VALVE_OPEN;
    _outbuf.plateletValve = CHW_VALVE_RETURN;
    _outbuf.redDrive = 30;
    _outbuf.greenDrive = 31;
    _outbuf.soundLevel = CHW_SOUND_SOFT;
    _outbuf.test1 = 0;
    _outbuf.highAPSLimit = 40;
    _outbuf.lowAPSLimit = 41;

    print(_outbuf);
}


void TestMessage::print(const CHwOrders& msg)
{
    LOG_INFO("TestMessage printing");

    #define PRINT(field) LOG_INFO("%s = %d", #field, (int)msg.field)

    PRINT(cassettePosition);
    PRINT(centrifugeRPM);
    PRINT(centrifugeRampUpRate);
    PRINT(centrifugeRampDownRate);
    PRINT(alarmLight);
    PRINT(doorLatches);
    PRINT(doorDirection);
    PRINT(inletRPM);
    PRINT(plateletRPM);
    PRINT(plasmaRPM);
    PRINT(acRPM);
    PRINT(returnRPM);
    PRINT(rbcValve);
    PRINT(plasmaValve);
    PRINT(plateletValve);
    PRINT(redDrive);
    PRINT(greenDrive);
    PRINT(soundLevel);
    PRINT(test1);
    PRINT(highAPSLimit);
    PRINT(lowAPSLimit);
}

