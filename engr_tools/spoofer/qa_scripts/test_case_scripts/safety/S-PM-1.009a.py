# FILENAME: S-PM-1.009a.py
# PURPOSE: .
scriptName = "S-PM-1.009a.py"

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
    #If using -object spoofs, -enable option needs to be added as either a standalone command or an option in another command.
    #Note: -enable is not required for named spoofs (-id).
    #Example command: runner.AddCommand('spoof_main "-enable -object CHwOrders -data acRPM -value 0.33 -enable"\n','->',False)
    #Templates:
    #runner.AddCommand('spoof_main "-enable"\n','->',False)
    #runner.AddCommand('spoof_main "-id  -var1  -var2 "\n','->',False)
    #runner.AddCommand('spoof_main "-enable -object  -data  -value "\n','->',False)
    #runner.AddCommand('<enter any telnet command here>\n','->',False)
    #**************************************************************************************************************************
    runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
    runner.AddCommand('spoof_main "-end"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data acRPM -value 125"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 140"\n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -trace"\n','->',False)
    


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
    #time.sleep for no operator prompt, raw_input for prompt.
    #The raw_input method will display the string then the operator will have to press ENTER to proceed.
    #time.sleep(30)
    #raw_input("<Prompt to present to operator>, press ENTER to continue.");
    #**************************************************************************************************************************

    

    #**************************************************************************************************************************
    #(Optional) Next set of commands to be executed.
    #If more sets of commands are needed copies of this section and the "Execute" section below may be inserted after the "Execute" section below.
    #If data needs to be unspoofed prior to next spoof use command below.
    #runner.AddCommand('spoof_main "-unspoof -object <Spoofed Object Here> -data <Spoofed Data Here>"\n','->',False)
    #Example command: runner.AddCommand('spoof_main "-enable -object CHwOrders -data acRPM -value 0.33 -enable"\n','->',False)
    #**************************************************************************************************************************



    #**************************************************************************************************************************
    #(Optional) Execute next set of cached commands.
    #Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
    #The raw_input method will display the string then the operator will have to press ENTER to proceed.
    #raw_input("<Prompt to operator>");
    #runner.Run()
    #runner.ResetCommands()
    #**************************************************************************************************************************



    #**************************************************************************************************************************
    #If desired, add a -disable -end or -unspoof command to disable the spoofer or simply unspoof spoofed data. 
    #runner.AddCommand('spoof_main "-unspoof -object <Spoofed Object Here> -data <Spoofed Data Here>"\n','->',False)
    #runner.AddCommand('spoof_main "-disable"\n','->',False)
    #runner.AddCommand('spoof_main "-end"\n','->',False)
    #**************************************************************************************************************************
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