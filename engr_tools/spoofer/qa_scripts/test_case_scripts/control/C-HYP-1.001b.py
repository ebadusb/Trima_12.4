# FILENAME: C-HYP-1.001b.py
# PURPOSE: .
scriptName = "C-HYP-1.001b.py"

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

    #**************************************************************************************************************************
    #Cache commands to be executed.
    #**************************************************************************************************************************
    runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
    runner.Run()
    runner.ResetCommands()
    
    limit = float(raw_input("Enter the configured volumetric limit for Procedures with plasma: ")) #The product volume that can be removed from the donor before the "Procedures with plasma" limit is exceeded
    offset = 15  # Amount [ml] that the spoof will undershoot and overshoot the configured volume limit by
    
    print 'Spoofing the collected product volume to the configured limit - 15 ml...'
    
    runner.AddCommand('spoof_main "-object CHwStates -data plasmaValve -value HW_VALVE_COLLECT"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data plateletValve -value HW_VALVE_COLLECT"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data rbcValve -value HW_VALVE_COLLECT"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_Vac -data data -value 200"\n','->',False) # spoof 200 mL AC accum to get away from safety side mimic
    runner.AddCommand('spoof_main "-object ProcVolumes_PlasmaVolume -data data -value '+ str((limit/3) - offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_PlateletVolume -data data -value '+ str((limit/3) - offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_RBCVolume -data data -value '+ str((limit/3) - offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-enable -trace"\n','->',False)
    
    #**************************************************************************************************************************
    #Execute cached commands, then clear cached commands to get runner ready for next set of commands.
    #Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
    #The raw_input method will display the string then the operator will have to press ENTER to proceed.
    #raw_input("<Prompt to present to operator>");
    #**************************************************************************************************************************
    runner.Run()
    runner.ResetCommands()

    #**************************************************************************************************************************
    #(Optional) Pause here with additional details if necessary (i.e. instructions, timing, etc.).
    #**************************************************************************************************************************
    print 'Pausing for 10 seconds...'
    time.sleep(10)
    runner.AddCommand("Log_Test_Info(\"Collected product volume spoofed below limit. Total Product Volume: " + str((limit) - offset) + "\")\n", "->", False)
    runner.AddCommand('spoof_main "-disable"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    
    #**************************************************************************************************************************
    #(Optional) Next set of commands to be executed.
    #**************************************************************************************************************************
    print 'Spoofing the collected product volume to the configured limit + 15 ml...'
    runner.AddCommand('spoof_main "-object CHwStates -data plasmaValve -value HW_VALVE_COLLECT"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data plateletValve -value HW_VALVE_COLLECT"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data rbcValve -value HW_VALVE_COLLECT"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_Vac -data data -value 200"\n','->',False) # spoof 200 mL AC accum to get away from safety side mimic
    runner.AddCommand('spoof_main "-object ProcVolumes_PlasmaVolume -data data -value '+ str((limit/3) + offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_PlateletVolume -data data -value '+ str((limit/3) + offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_RBCVolume -data data -value '+ str((limit/3) + offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-enable -trace"\n','->',False)
    
    #**************************************************************************************************************************
    #(Optional) Execute next set of cached commands.
    #**************************************************************************************************************************
    runner.Run()
    runner.ResetCommands()
    print 'Pausing for 10 seconds...'
    time.sleep(10)
    runner.AddCommand("Log_Test_Info(\"Collected product volume spoofed above limit. Total Product Volume: " + str((limit) + offset) + "\")\n", "->", False)

    #**************************************************************************************************************************
    #If desired, add a -disable -end or -unspoof command to disable the spoofer or simply unspoof spoofed data. 
    #**************************************************************************************************************************
    runner.AddCommand('spoof_main "-end"\n','->',False)
    runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
    runner.Run()
    runner.ResetCommands()
    
    #**************************************************************************************************************************
    #Optional prompt to notify operator that script and all spoofs have been execute successfully.
    #**************************************************************************************************************************
    raw_input("Script execution complete, press ENTER to close this window.");

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