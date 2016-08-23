# FILENAME: S-PM-1.001.py
# PURPOSE: .
scriptName = "S-PM-1.001.py"

import time
import os
import sys
import telnetlib
import socket
import ftplib
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner


def load_and_run(runner):

    runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
    runner.AddCommand('spoof_main "-object CHwStates -data returnPumpAccum -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data inletPumpAccum -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data plasmaPumpAccum -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data acPumpAccum -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data plateletPumpAccum -value 201"\n','->',False)

    runner.Run()
    runner.ResetCommands()

    raw_input("Press ENTER to spoof control pump encoder data: ");
    
    runner.AddCommand("Log_Test_Info(\"Enabling spoofed control data\")\n", "->", False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4"\n','->',False)
    

    runner.Run()
    runner.ResetCommands()

    runner.AddCommand('spoof_main "-object SHwStates -data returnPumpAccum -value 4"\n','->',False)
    runner.AddCommand('spoof_main "-object SHwStates -data inletPumpAccum -value 4"\n','->',False)
    runner.AddCommand('spoof_main "-object SHwStates -data plasmaPumpAccum -value 4"\n','->',False)
    runner.AddCommand('spoof_main "-object SHwStates -data acPumpAccum -value 4"\n','->',False)
    runner.AddCommand('spoof_main "-object SHwStates -data plateletPumpAccum -value 4"\n','->',False)

    runner.Run()
    runner.ResetCommands()

    raw_input("Press ENTER to spoof safety pump hall data: ");

    runner.AddCommand("Log_Test_Info(\"Enabling spoofed safety data\")\n", "->", False)    
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4"\n','->',False)
    

    runner.Run()
    runner.ResetCommands()

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