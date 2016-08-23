/* FILENAME: so01.cpp
 * $Header: L:/BCT_Development/Trima/engr_tools/spoofer/test/rcs/so01.cpp 1.1 2000/01/13 22:16:54 TD07711 Exp td07711 $
 * PURPOSE: main for testing spoofing of safety order messages
 * CHANGELOG:
 * $Log: so01.cpp $
 * Revision 1.1  2000/01/13 22:16:54  TD07711
 * Initial revision
 * 12/30/99 - dyes - initial version
 */


#include "buffmsg.hpp"
#include "shw.hpp"
#include "spooferlog.hpp"  // include last to avoid ASSERT nameclash


dispatcher* dispatch;                        // message dispatcher



class TestMessage : public focusBufferMsg<SHwOrders>
{
    public:

        TestMessage();
        ~TestMessage();
        void notify();   // process incoming message

        // new functions for TestMessage
        void setup();    // setup outgoing message
        void send() { set(&_outbuf); };     // send message
        void receive() { get(&_inbuf); };  // receive message
        void print(const SHwOrders& msgbuf);
        void diff(); // compares inbuf and outbuf

    protected:

    SHwOrders _inbuf;
    SHwOrders _outbuf;

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
: focusBufferMsg<SHwOrders>(SafetyHardwareCommands)
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

    DIFF(pumpPower);
    DIFF(centrifugePower);
    DIFF(valveLED);
    DIFF(doorSolenoidPower);
    DIFF(alarmLight);
    DIFF(serviceEnableMode);
    DIFF(donorConnectMode);
    DIFF(test1);
    DIFF(test2);
}


void TestMessage::setup()
{
    LOG_INFO("TestMessage setup");
    _outbuf.pumpPower = HW_ENABLE;
    _outbuf.centrifugePower = HW_DISABLE;
    _outbuf.valveLED = HW_ENABLE;
    _outbuf.doorSolenoidPower = HW_DISABLE;
    _outbuf.alarmLight = HW_ENABLE;
    _outbuf.serviceEnableMode = HW_DISABLE;
    _outbuf.donorConnectMode = HW_ENABLE;
    _outbuf.test1 = 1000;
    _outbuf.test2 = 2000;

    print(_outbuf);
}


void TestMessage::print(const SHwOrders& msg)
{
    LOG_INFO("TestMessage printing");

    #define PRINT(field) LOG_INFO("%s = %d", #field, (int)msg.field)

    PRINT(pumpPower);
    PRINT(centrifugePower);
    PRINT(valveLED);
    PRINT(doorSolenoidPower);
    PRINT(alarmLight);
    PRINT(serviceEnableMode);
    PRINT(donorConnectMode);
    PRINT(test1);
    PRINT(test2);
}

