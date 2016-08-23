# FILENAME: S-STM-1.001a.py
# PURPOSE: This script will walk through several state transitions across serveral boot cycles. In each boot cycle, spoofs will be executed to sequently progress through any prerequisite state 
# transitions (including spoofing conditions which will allow passage through various checkpoints between states) before finally spoofing a transition to an invalid state. After the transition 
# to the invalid state, the script will reboot the machine and persistently attemp to reconnect with the device. Once the script is able to reconnect with the Trima device, it will beging 
# walking through the set of spoofs for testing the next invalid state transition.

""" This module was generated using the code from Trima software version 6.3 spiral 0 as a reference. For any future implementations of this module (i.e. in a test case or test protocol) there is a possiblity that the specific things that are done in v.6.3.0 in order to allow passage between states will no longer be sufficient or valid. 
"""

_scriptName = "S-STM-1.001a.py"

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
    
    #Here we define all of the states that we want to start in before attempting an invalid state transition
    #To add new states one must add a definition in the stateSwitchers dictionary and also define a function to transition sequentially
    #into all of the prerequisite states and then finally enter the last valid state (see go_to_donor_connect below for an example of this)

    stateSwitchers = {'switch_DISPOSABLE_TEST':go_to_disp_test, 'switch_DONOR_CONNECTED':go_to_donor_connect,
                      'switch_BLOOD_PRIME':go_to_blood_prime, 'switch_BLOOD_RUN':go_to_blood_run} 
                        # This state has been removed from the dictionary due to difficulties getting past the doFirstDrawCheck() method
                            #, 'switch_BLOOD_RINSEBACK':go_to_blood_rinseback}

    invalids = {}
    invalids['switch_DISPOSABLE_TEST'] = (SystemStates.DONOR_CONNECTED)
    invalids['switch_DONOR_CONNECTED'] = (SystemStates.DISPOSABLE_TEST, SystemStates.AC_PRIME, SystemStates.BLOOD_RUN, SystemStates.BLOOD_RINSEBACK, SystemStates.METERED_STORAGE)
    invalids['switch_BLOOD_PRIME'] = (SystemStates.DISPOSABLE_TEST, SystemStates.AC_PRIME, SystemStates.METERED_STORAGE)
    invalids['switch_BLOOD_RUN'] = (SystemStates.DISPOSABLE_TEST, SystemStates.AC_PRIME, SystemStates.METERED_STORAGE)
    #These states have been removed from the dictionary due to difficulties getting past the doFirstDrawCheck() method
    #invalids['switch_BLOOD_RINSEBACK'] = (States.DISPOSABLE_TEST, States.AC_PRIME, States.METERED_STORAGE)
    #invalids['fromMeteredStorage'] = (States.DONOR_CONNECTED)
    
    state_mangler(stateSwitchers, invalids, runner, _address)
    remove_corrupt_dat(_address)
    raw_input("\n\n\nScript execution complete, press ENTER to EXIT:")

   
class SystemStates:
    FIRST_STATE = 0
    SELF_TEST = 1
    STARTUP_TESTS = 2
    DISPOSABLE_TEST = 3
    AC_CONNECTED = 4
    AC_PRIME = 5
    DONOR_CONNECTED = 6
    BLOOD_PRIME = 7
    BLOOD_RUN = 8
    BLOOD_RINSEBACK = 9
    DONOR_DISCONNECT = 10
    POST_RUN = 11
    METERED_STORAGE = 12
    METERED_STORAGE_DISCONNECT = 13
    SERVICE_MODE = 14
    DEFAULT_STATE = 15
    #LAST_PROCEDURE_STATE = 10000

def state_mangler(stateSwitchers, invalids, runner, address):
    """
    Will call functions, as defined in the dictionaries in main(), to spoof valid transitions (i.e. T1 tests passed)
    to <stateSwitcher> state then invalid transition to <invalid> state
    """

    for _fromState in stateSwitchers:
        
        _invalidStatesList = invalids[_fromState]

        if type(_invalidStatesList) is not int: #If there is only state define in the invalids dictionary for the given _fromState, _invalidStatesList will be an int, else a list
            for _invalidState in _invalidStatesList:
                runner.AddCommand("Log_Test_Info(\"Running spoof script " + _scriptName + "\")\n", "->", False)
                #Call "go_to_..." function (defined in stateSwitchers) to enter prerequisite state
                stateSwitchers[_fromState](runner)
                #We should now have transitioned into our last valid state and are ready to transition into our invalid state
                runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int ' + str(_invalidState) + '"\n','->',False)
                runner.Run()
                runner.ResetCommands()
                reboot_trima(runner)
                #Pause here to prevent weird telnet issue where the connection does not close (i.e. the mahcine does not reboot fast enough) before auto_connect() is able to reconnect
                print("Machine Reset pause")
                time.sleep(2)
                auto_connect(address)
                #raw_input("sa;ldjf")
        else: #We only have one invalid transition to test (type(_invalidStatesList) == int)
            runner.AddCommand("Log_Test_Info(\"Running spoof script " + _scriptName + "\")\n", "->", False)
            #Call "go_to_..." function (defined in stateSwitchers) to enter prerequisite state
            stateSwitchers[_fromState](runner)
            #We should now have transitioned into our last valid state and are ready to transition into our invalid state
            runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int ' + str(_invalidStatesList) + '"\n','->',False)
            runner.Run()
            runner.ResetCommands()
            reboot_trima(runner)
            #Pause here to prevent weird telnet issue where the connection does not close (i.e. the mahcine does not reboot fast enough) before auto_connect() is able to reconnect
            print("Machine Reset pause")
            time.sleep(20)
            auto_connect(address)
        
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
    runner.AddCommand("reboot\n",'',False)
    runner.Run()
    runner.ResetCommands()

def remove_corrupt_dat(address):
    """Deletes the config.dat file from /machine/tools if present"""
    ftp = ftplib.FTP(address,'david','david')
    print("Deleting corrupt.dat (if present)")
    try:
        ftp.delete('/machine/tools/corrupt.dat')
    except ftplib.error_perm:
        raw_input("\n**Error deleting corrupt.dat. Delete manually. Press ENTER to continue.**")
    
def go_to_disp_test(runner, T1 = True):
    """Enters disposable test state, with (default T1 = True) or without T1 safe passage"""
    enter_disp_test_state(runner, T1)
    
def go_to_donor_connect(runner, test = True):
    """Enters donor connect state, with (default test = True) or without donor vitals test safe passage"""
    go_to_disp_test(runner)
    enter_ac_connected_state(runner)
    enter_ac_prime_state(runner)
    enter_donor_donnected_state(runner, True)

def go_to_blood_prime(runner):
    """Enters blood prime state"""
    go_to_donor_connect(runner)
    enter_blood_prime_state(runner)
    
def go_to_blood_run(runner):
    """Enters blood run state"""
    go_to_blood_prime(runner)
    enter_blood_run_state(runner)

def go_to_blood_rinseback(runner):
    """Enters blood run state"""
    go_to_blood_run(runner)
    enter_blood_rinseback_state(runner)

def enter_disp_test_state(runner, T1 = True):	
    """First spoofs all necessary conditions in order to pass all T1 tests required before entry to DISPOSABLE_TEST, then enters DISPOSABLE_TEST"""
    # Just to be safe, we will empty out all spoofer data and disable the spoofer before
    runner.AddCommand('spoof_main "-end"\n','->',False)

    if T1:
        # Run pumps so that they have spun more than five times for cassette_monitor.cpp T1 test safe passage
        runner.AddCommand('spoof_main "-object CHwOrders -data returnRPM -value 105"\n','->',False)
        runner.AddCommand('spoof_main "-object CHwOrders -data acRPM -value 105"\n','->',False)
        runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 105"\n','->',False)
        runner.AddCommand('spoof_main "-object CHwOrders -data plasmaRPM -value 105"\n','->',False)
        runner.AddCommand('spoof_main "-object CHwOrders -data plateletRPM -value 105"\n','->',False)
        runner.AddCommand('spoof_main "-id EnableDisable -var1 3000 -var2 2000 -timeout 4"\n','->',False)
        runner.AddCommand('spoof_main "-end"\n','->',False)
        runner.AddCommand('spoof_main "-enable -serverDebug -debug"\n','->',False)
    
        # Set disposable type
        runner.AddCommand('spoof_main "-send DisposableIDMsg -int 2"\n','->',False)		#Set set type
    
        # Spoof cassette positions for safety T1 test safe passage
        runner.AddCommand('spoof_main "-object SHwStates -data cassettePosition -value HW_CASSETTE_UP"\n','->',False)		
        runner.AddCommand('spoof_main "-object SHwStates -data cassettePosition -value HW_CASSETTE_UNKNOWN"\n','->',False)		
        runner.AddCommand('spoof_main "-object SHwStates -data cassettePosition -value HW_CASSETTE_DOWN"\n','->',False)		

    # Change system state to state 3 (DISPOSABLE_TEST)
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 3"\n','->',False)
    runner.Run()
    runner.ResetCommands()

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
    
    
def enter_donor_donnected_state(runner, test = True):
    """First spoofs all necessary conditions in order to pass all tests required before entry to DONOR_CONNECTED, then enters DONOR_CONNECTED"""
    
    if test:
        #Lock the door so we are allowed to enable centrifuge power.
        runner.AddCommand('spoof_main "-object CHwStates -data doorLocks -value HW_DOOR_CLOSED_AND_LOCKED"\n','->',False)
        runner.AddCommand('spoof_main "-object SHwStates -data doorLocks -value HW_DOOR_CLOSED_AND_LOCKED"\n','->',False)
    
        # Turn on 24V and 64V power so safety can complete its power T1 testing. 
        runner.AddCommand('spoof_main "-send RequestSafetyPowerMsg -data ENABLE_BOTH_POWER"\n','->',False)
    
        # Load donor vitals
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorSex -value 1"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorWeightUnits -value 1"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorWeight -value 200"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorHeightUnits -value 1"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorHeight -value 72"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorHct -value 45"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data PltPrecount -value 200"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data ProcDonorTBV -value 5768.449"\n','->',False)
        runner.AddCommand('spoof_main "-object DVitalsFromProc -data DonorVitalStatus -value 1 -enable"\n','->',False)
        # Still need to find a way to have the system automagically let safety know that there are vitals, for now:
        runner.Run()
        runner.ResetCommands()	
        raw_input("Press the Donor Info button, then press the ENTER key to continue:")
    
    # Change system state to state 6 (DONOR_CONNECTED)
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 6"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    
def enter_blood_prime_state(runner):
    """Change system state to state 7 (BLOOD_PRIME)"""
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 7"\n','->',False)
    runner.AddCommand('spoof_main "-object SHwStates -data centrifugeRPM -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data centrifugeRPM -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object HalStatus_centActRPM -data data -value 201"\n','->',False)
    runner.AddCommand('spoof_main "-object ProcVolumes_Vreturn -data data -value -25"\n','->',False)
    runner.Run()
    runner.ResetCommands()

def enter_blood_run_state(runner):
    """Change system state to state 8 (BLOOD_RUN)"""
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 8"\n','->',False)
    runner.Run()
    runner.ResetCommands()

def enter_blood_rinseback_state(runner):
    """Change system state to state 9 (BLOOD_RINSEBACK)"""
        #The code block below constitutes the remnants of my failed attempts at validly entering BLOOD_RINSEBACK (passing doFirstDrawCheck())
        #this will be left out for now in hopes that spoofer re-architecture will make this more plausable
            #runner.AddCommand('spoof_main "-object CHwStates -data inletRevs -value 13.4808"\n','->',False)
            #runner.AddCommand('spoof_main "-object CHwStates -data acRevs -value 1.69616"\n','->',False)
            #runner.Run()
            #runner.ResetCommands()

            #print("Accumulating Inlet volume")
            #time.sleep(7)

            #runner.AddCommand('spoof_main "-unspoof -object CHwStates -data acRevs"\n','->',False)
            #runner.AddCommand('spoof_main "-unspoof -object CHwStates -data inletRevs"\n','->',False)
            #runner.AddCommand('spoof_main "-send SystemStateChangeRequestMsg -int 10001"\n','->',False)
            #runner.AddCommand('spoof_main "-object CHwStates -data reservoir -value CHW_RESERVOIR_HIGH"\n','->',False)
            ##runner.AddCommand('spoof_main "-object ProcTrimaSet_CassetteStateType -data data -value 8"\n','->',False)
            ##runner.AddCommand('spoof_main "-id EnableDisable -var1 6500 -var2 1000 -timeout 7"\n','->',False)
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 9"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    
def enterPostRunState(runner):
    """Change system state to state 11 (POST_RUN)"""
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 11"\n','->',False)
    runner.Run()
    runner.ResetCommands()
    
def enterMeteredStorageState(runner):
    """Change system state to state 12 (METERED_STORAGE)"""
    runner.AddCommand('spoof_main "-send SystemStateChangeMsg -int 12"\n','->',False)
    runner.Run()
    runner.ResetCommands()

    
if __name__ == '__main__':
    
    main()