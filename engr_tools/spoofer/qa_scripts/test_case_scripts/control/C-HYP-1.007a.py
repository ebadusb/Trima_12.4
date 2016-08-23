# FILENAME: C-HYP-1.007a.py
# PURPOSE: .
scriptName = "C-HYP-1.007a.py"

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
 #------------------------------------------------------------------------------------------------------------------------------------------------
    runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
    runner.Run()
    runner.ResetCommands()
 #------------------------------------------------------------------------------------------------------------------------------------------------   
    #Setting up testing variables
    setTBV = float(raw_input('Enter the Nadler TBV [ml]: '))  #Nadler TBV
    TBVLimit = 0.20               #The ECV that can be removed from the donor before the TBV% limit is exceeded
    
    ECvol = 130                   #EC volume of the set [ml] (white stamp) CC_constant
    #ECvolume = 112               #EC volume of the set [ml] (black stamp) CC_constant
    DrawMarg = 20                 #MAX draw cycle volume - Reservoir volume [ml]  CC_constant
     
    Margin = 10                   #Control allows you to exceed hypovolemia by 10 ml before alarm is thrown 
    offset = 3                    # Volume [ml] that the spoof will undershoot and overshoot the configured volume limit by  
    ACvol = 50                    # Spoofed volume [ml] of AC pumped to the set
   
    ECVlimit = setTBV * TBVLimit                          #The net ECV that can be removed from the donor before the TBV% limit is exceeded
    limit = ECVlimit + ACvol - ECvol - DrawMarg +Margin   #The remaining ECV that can be removed from the donor before the TBV% limit is exceeded
    
#------------------------------------------------------------------------------------------------------------------------------------------------    
    # Spoofing the ECV to the configured limit - 3 ml
    print 'Spoofing the ECV to the configured limit - 3 ml...'

    #spoofing AC in product bags to avoid control's %TBV bag limits
    runner.AddCommand('spoof_main "-object ProcVolumes_ACinPlasmaBag -data data -value '+ str((limit/3)) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_ACinPlateletBag -data data -value '+ str((limit/3)) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_ACinRBCBag -data data -value '+ str((limit/3)) +'"\n','->',False)    
 
    #spoofing variables used in Control's safety hypovolemia mimic
    runner.AddCommand('spoof_main "-object ProcVolumes_Vac -data data -value '+ str(ACvol) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_PlateletVolume -data data -value '+ str((limit/3) - offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_RBCVolume -data data -value '+ str((limit/3) - offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_PlasmaVolume -data data -value '+ str((limit/3) - offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-enable -trace"\n','->',False)

    runner.Run()
    runner.ResetCommands()

    print 'Pausing for 10 seconds...'
    time.sleep(10)
    
    runner.AddCommand("Log_Test_Info(\"ECV spoofed below limit.\")\n", "->", False) 
    runner.Run()
    runner.ResetCommands()

    #Disabling current bag vol spoofs so that we violate hypo limit in one fell swoop...
    runner.AddCommand('spoof_main "-disable"\n','->',False)
    runner.Run()
    runner.ResetCommands()

#------------------------------------------------------------------------------------------------------------------------------------------------    
    # Spoofing the ECV to the configured limit + 3 ml
    print 'Spoofing the collected product volume to the configured limit + 3 ml...'
    runner.AddCommand('spoof_main "-object ProcVolumes_PlasmaVolume -data data -value '+ str((limit/3) + offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_PlateletVolume -data data -value '+ str((limit/3) + offset/3) +'"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_RBCVolume -data data -value '+ str((limit/3) + offset/3) +'"\n','->',False)
    
    runner.Run()
    runner.ResetCommands()
    
    runner.AddCommand('spoof_main "-enable"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    
    print 'Pausing for 10 seconds...'
    time.sleep(10)
    
    runner.AddCommand("Log_Test_Info(\"ECV spoofed above limit.\")\n", "->", False) 
    runner.Run()
    runner.ResetCommands()

    runner.AddCommand('spoof_main "-end"\n','->',False)
    runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
    runner.Run()
    runner.ResetCommands()

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