# FILENAME: S-PFR-1.004a.py
# PURPOSE: 

""" This module was generated using the code from Trima software version 6.3 spiral 0 as a reference. For any future implementations of this module (i.e. in a test case or test protocol) there is a possiblity that the specific things that are done in v.6.3.0 in order to allow passage between states will no longer be sufficient or valid. 
[USAGE]
"""

_scriptName = "S-PFR-1.004a.py"

import time
import os
import sys
import telnetlib
import socket
import ftplib
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner

def main():
    # Instantiate a Runner object and connect it to the device under test
    _address = calculate_trima_address(int(raw_input("\nEnter the Trima Machine ID: ")))
    auto_connect(_address)
    runner = Runner.Commander(_address)
    
    go_to_ac_prime(runner)
    
    raw_input("\n\n\nScript execution complete, press ENTER to EXIT:")

        
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
            print "Connection not ready, waiting 5 secs to retry, attempt #"+str(_count)
            time.sleep(5)

    print("Trima Telnet Connection Ready")	
    
def reboot_trima(runner):
    """Sends a 'ctrl-c' keyboard input to the Trima device via TelNet which will reboot the machine"""
    runner.AddCommand("\x03",'',False)
    runner.Run()
    runner.ResetCommands()

def go_to_disp_test(runner):
    """Enters disposable test state, with (default T1 = True) or without T1 safe passage"""
    enter_disp_test_state(runner)
    
def go_to_ac_prime(runner, test = True):
    """Enters donor connect state, with (default test = True) or without donor vitals test safe passage"""
    go_to_disp_test(runner)
    enter_ac_connected_state(runner)
    enter_ac_prime_state(runner)

def enter_disp_test_state(runner):	
    """First spoofs all necessary conditions in order to pass all T1 tests required before entry to DISPOSABLE_TEST, then enters DISPOSABLE_TEST"""
    # Just to be safe, we will empty out all spoofer data and disable the spoofer before
    runner.AddCommand('spoof_main "-end"\n','->',False)

    # Run pumps so that they have spun more than five times for cassette_monitor.cpp T1 test safe passage
    runner.AddCommand('spoof_main "-object CHwOrders -data returnRPM -value 110"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data acRPM -value 110"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 110"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data plasmaRPM -value 110"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data plateletRPM -value 110"\n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -var1 3500 -var2 2000 -timeout 4"\n','->',False)
    runner.AddCommand('spoof_main "-end"\n','->',False)
    runner.AddCommand('spoof_main "-enable -trace"\n','->',False)
    
    # Set disposable type
    runner.AddCommand('spoof_main "-send DisposableIDMsg -int 2"\n','->',False)		#Set set type
    
    # Change system state to state 3 (DISPOSABLE_TEST)
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 3"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    
    runner.AddCommand('spoof_main "-object CHwOrders -data cassettePosition -value CHW_CASSETTE_DOWN"\n','->',False)
    runner.Run()
    runner.ResetCommands()

    print("Pausing 15 seconds to allow cassette to lower...")
    time.sleep(15)


def enter_ac_connected_state(runner):
    """Change system state to state 4 (DISPOSABLE_TEST)"""
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 4"\n','->',False)
    runner.Run()
    runner.ResetCommands()


def enter_ac_prime_state(runner):
    """Change system state to state 5 (AC_PRIME)"""
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 5"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    

    
if __name__ == '__main__':
    
    main()