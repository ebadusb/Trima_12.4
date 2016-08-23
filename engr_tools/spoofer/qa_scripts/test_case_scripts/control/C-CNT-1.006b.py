# FILENAME: <C-CNT-1.006b>.py
# PURPOSE: This python script will spoof the HalStatus_centActRPM.data  from 0.000 to 0.200.
#          The idea is to get the Centrifuge Shutdown Test (i.e., the zero speed test) to fail.
scriptName = "C-CNT-1.006b.py"

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
    runner.AddCommand('spoof_main "-object HalStatus_centActRPM -data data -value 0.2 -enable -trace"\n','->',False)
    runner.Run()
    runner.ResetCommands()

    print 'Pausing for 30 seconds...'
    time.sleep(30)
    runner.AddCommand('spoof_main "-end"\n','->',False)
    runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
    runner.Run()
    runner.ResetCommands()

    #**************************************************************************************************************************
    #Optional prompt to notify operator that script and all spoofs have been execute successfully.
    #raw_input("Script execution complete, press ENTER to close this window.");
    #**************************************************************************************************************************




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
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # Set number of times to iterate through the telnet connect sequence to 200 before timing out.
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    _timeout=200
    _count = 0

    while not _connected:
        try:
            _c = telnetlib.Telnet(address)
            if _c.sock is not None:
                _connected = True

        except socket.error:
            _count = _count + 1
            print "Trima socket not ready, waiting to retry, attempt #"+str(_count)
            #**************************************************************************************************************************
            # For timing purposes, decreased the time to 1/10th of a second; 5 seconds was too long to wait fo the telnet session 
            # to start.
            #**************************************************************************************************************************
            time.sleep(0.1)

    print("Trima Telnet Connection Ready")	
    
def reboot_trima(runner):
    """Sends a 'ctrl-c' keyboard input to the Trima device via TelNet which will reboot the machine"""
    runner.AddCommand("reboot\n",'',False)
    runner.Run()
    runner.ResetCommands()

if __name__ == '__main__':
    
    main()