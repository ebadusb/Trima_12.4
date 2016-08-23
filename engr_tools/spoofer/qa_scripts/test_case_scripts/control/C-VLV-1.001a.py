# FILENAME: C-VLV-1.001a.py
# PURPOSE: Used to test requirement "The system shall alarm if the Platelet, Plasma, or RBC valve position does not match the commanded position within 10 seconds as seen by the optical sensor".
scriptName = "C-VLV-1.001a.py"

import time
import os
import sys
import telnetlib
import socket
import ftplib
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner


"""
    USAGE
    1. Save a copy of this file to your computer using the test case ID as the file name (<TestCaseID>.py).
    2. Change the filename in the first line of your document to reflect the name chosen in step 1.
    3. Set the 'scriptName' variable below to be the same as the file name.
    4. Do not edit the "Automated Trima connection" section below.
    5. For each command to be sent to the spoofer, add a "runner.AddCommand" line using the format given in the examples.
    6. If multiple sets of commands need to be executed the runner command cache will need to be reset between command sets by using the runner.ResetCommands() function.
"""


def load_and_run(runner):
    """
    This function uses the runner.py module (passed from main()) to interact with the spoofer.
    Commands are cached in the runner object, then executed in the order in 
    which they are cached without pause."""

    runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
    
    runner.AddCommand('spoof_main "-object HalStatus_PlasmaValveStatus -data data -value HAL_VALVE_UNKNOWN -enable -trace"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    runner.AddCommand('spoof_main "-end"\n','->',False)
    raw_input("After the machine alarms, press ENTER clear the spoof: ")
    runner.Run()
    runner.ResetCommands()

    runner.AddCommand('spoof_main "-object HalStatus_CollectValveStatus -data data -value HAL_VALVE_UNKNOWN -enable -trace"\n','->',False)
    raw_input("Clear the alarm, then press ENTER to spoof next alarm state: ")
    runner.Run()
    runner.ResetCommands()
    runner.AddCommand('spoof_main "-end"\n','->',False)
    raw_input("After the machine alarms, press ENTER clear the spoof: ")
    runner.Run()
    runner.ResetCommands()
    
    
    runner.AddCommand('spoof_main "-object HalStatus_RBCValveStatus -data data -value HAL_VALVE_UNKNOWN -enable -trace"\n','->',False)
    raw_input("Clear the alarm, then press ENTER to spoof next alarm state: ")
    runner.Run()
    runner.ResetCommands()
    runner.AddCommand('spoof_main "-end"\n','->',False)
    raw_input("After the machine alarms, press ENTER clear the spoof: ")
    runner.Run()
    runner.ResetCommands()

    runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
    runner.Run()
    runner.ResetCommands()
    raw_input("Script execution complete, now back to your regularly scheduled programming.")


def main():
    # Instantiate a Runner object and connect it to the device under test
    _address = calculate_trima_address(int(raw_input("\nEnter the Trima Machine ID: ")))
    auto_connect(_address)
    runner = Runner.Commander(_address)
    load_and_run(runner)

def calculate_trima_address(testMachine):
    """Calculates Trima IP address based on Trima Machine ID"""
    _machineBase = int(testMachine/256)
    _machineRemainder = int(testMachine-(_machineBase*256))
    _machineBase = str(_machineBase)
    _machineRemainder = str(_machineRemainder)
    _address = "172.21."+_machineBase+"."+_machineRemainder
    
    return _address
    
def auto_connect(address):
    """Persistently tests the Trima telnet socket, exits when socket is available"""
    _connected=False
    _timeout=100
    _count = 0

    while not _connected:
        try:
            _c = telnetlib.Telnet(address)
            if _c.sock is not None:
                _connected = True

        except socket.error:
            _count = _count + 1
            print "Trima socket not ready, waiting to retry, attempt #"+str(_count)
            time.sleep(5)

    print("Trima Telnet Connection Ready")	
    
def reboot_trima(runner):
    """Sends a 'ctrl-c' keyboard input to the Trima device via TelNet which will reboot the machine"""
    runner.AddCommand("reboot\n",'',False)
    runner.Run()
    runner.ResetCommands()

if __name__ == '__main__':
    
    main()