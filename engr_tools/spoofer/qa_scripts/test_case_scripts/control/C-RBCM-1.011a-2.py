# FILENAME: C-RBCM-1.011a-2.py
# PURPOSE: .
scriptName = "C-RBCM-1.011a-2.py"

import time
import os
import sys
import telnetlib
import socket
import ftplib
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner
from datetime import datetime



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

    #**************************************************************************************************************************
    #Cache commands to be executed.
    #If using -object spoofs, -enable option needs to be added as either a standalone command or an option in another command.
    #Note: -enable is not required for named spoofs (-id).
    #Example command: runner.AddCommand('spoof_main "-enable -object CHwOrders -data acRPM -value 0.33 -enable"\n','->',False)
    #Templates:
    #runner.AddCommand('spoof_main "-enable"\n','->',False)
    #runner.AddCommand('spoof_main "-id  -var1  -var2 "\n','->',False)
    #runner.AddCommand('spoof_main "-enable -object  -data  -value "\n','->',False)
    #runner.AddCommand('<enter any telnet command here>\n','->',False)
    #**************************************************************************************************************************
    red = 700
    green = 650
    strRed = ""
    strGreen = ""
    sleeptime = 4
    Multiplier = 5
    RedGreenRunLevel = "800"
    RedGreenBaseLine = "1600"
    runner.AddCommand('Log_Test_Info("Running spoof script ' + scriptName + '")\n', "->", False)
    runner.AddCommand('spoof_main "-id Spoof_RG_reflect -var1 '+RedGreenBaseLine+' -var2 '+RedGreenBaseLine+' -timeout 525"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    time.sleep(sleeptime)
    for num in range(0,30):  
     if (num % 2) :
      strRed = str(red+(num*Multiplier))
      strGreen = str(green+(Multiplier*num))
     else :
      strRed = str(red-(num*Multiplier))
      strGreen = str(green-(Multiplier*num))
     strAddCmd = 'spoof_main "-id Spoof_RG_reflect -var1 ' +strRed+ ' -var2 ' +strGreen+ ' -timeout 3"\n'
     runner.AddCommand(strAddCmd,'->',False)
     runner.Run()
     runner.ResetCommands()
    time.sleep(sleeptime)
    print str(datetime.now())
    runner.AddCommand('spoof_main "-id Spoof_RG_reflect -var1 '+RedGreenRunLevel+' -var2 '+RedGreenRunLevel+' -timeout 600"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    #**************************************************************************************************************************
    #If desired, add a -disable -end or -unspoof command to disable the spoofer or simply unspoof spoofed data. 
	#time.sleep(10)
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
    


if __name__ == '__main__':
    
    main()