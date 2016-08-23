# FILENAME: S-PM-1.004a.py
# PURPOSE: .
scriptName = "S-PM-1.004a.py"

import time
import os
import sys
import telnetlib
import socket
import ftplib
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner
import fileinput
from math import ceil
from math import floor


"""
    USAGE
    1. Save a copy of this file to your computer using the test case ID as the file name (<TestCaseID>.py).
    2. Change the filename in the first line of your document to reflect the name chosen in step 1.
    3. Set the 'scriptName' variable below to be the same as the file name.
    4. Do not edit the "Automated Trima connection" section below.
    5. For each command to be sent to the spoofer, add a "runner.AddCommand" line using the format given in the examples.
    6. If multiple sets of commands need to be executed the runner command cache will need to be reset between command sets by using the runner.ResetCommands() function.
"""


def load_and_run(runner,_address):
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
    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(399)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3581" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(400)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3581" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)

    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(400)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3582" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(450)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3582" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)

    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(-249)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3583" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(-250)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3583" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)

    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(-250)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3584" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwStates -data accessPressure -value ' + str(calc_raw_from_mmHg(-300)) + '" \n','->',False)
    runner.AddCommand('spoof_main "-object CHwOrders -data inletRPM -value 112.3584" \n','->',False)
    runner.AddCommand('spoof_main "-id EnableDisable -timeout 4.5" \n','->',False)

    #**************************************************************************************************************************
    #Execute cached commands, then clear cached commands to get runner ready for next set of commands.
    #Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
    #The raw_input method will display the string then the operator will have to press ENTER to proceed.
    #raw_input("<Prompt to present to operator>");
    #**************************************************************************************************************************
    raw_input("Press ENTER when prompted by test: ")
    runner.Run()
    runner.ResetCommands()
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
    #raw_input("Script execution complete, press ENTER to close this window.")
    #**************************************************************************************************************************
    raw_input("\n\nScript execution complete, press ENTER to close this window: ")



def main():
    #global cal_data
    global _address
    # Instantiate a Runner object and connect it to the device under test
    _address = calculate_trima_address(int(raw_input("\nEnter the Trima Machine ID: ")))
    auto_connect(_address)
    runner = Runner.Commander(_address)
    cal_data.load(_address)
    load_and_run(runner,_address)

def calc_raw_from_mmHg(mmHg):
    #global cal_data

    if mmHg >= 0:
        raw = ceil((mmHg/cal_data.aps_gain_pos) + cal_data.new_raw_ref) 
    else:
        raw = floor((mmHg/cal_data.aps_gain_neg) + cal_data.new_raw_ref) #- 1

    return raw


class APS_cal_data:
    def __init__(self):
        self.aps_raw_pos = -999
        self.aps_mmHg_pos = -999
        self.aps_raw_ref = -999
        self.aps_mmHg_ref = -999
        self.aps_raw_neg = -999
        self.aps_mmHg_neg = -999
        self.aps_gain_pos = -999
        self.aps_gain_neg = -999
        self.new_raw_ref = -999
        
    def load(self,_address):
        ftp = ftplib.FTP(_address,'david','david')
        ftp.retrbinary('RETR /config/cal.dat', open('cal.dat','wb').write)
    
        for line in fileinput.input('cal.dat'):
            if not '#' in line[0]:
                if '=' in line:
                    if 'aps_raw_pos' in line:
                        self.aps_raw_pos = float(line[line.find('=')+1:line.find('\n')])
                    elif 'aps_mmHg_pos' in line:
                        self.aps_mmHg_pos = float(line[line.find('=')+1:line.find('\n')])
                    elif 'aps_raw_ref' in line:
                        self.aps_raw_ref = float(line[line.find('=')+1:line.find('\n')])
                    elif 'aps_mmHg_ref' in line:
                        self.aps_mmHg_ref = float(line[line.find('=')+1:line.find('\n')])
                    elif 'aps_raw_neg' in line:
                        self.aps_raw_neg = float(line[line.find('=')+1:line.find('\n')])
                    elif 'aps_mmHg_neg' in line:
                        self.aps_mmHg_neg = float(line[line.find('=')+1:line.find('\n')])
                
        ftp.close

        self.aps_gain_pos = round(( self.aps_mmHg_pos - self.aps_mmHg_ref ) / ( self.aps_raw_pos - self.aps_raw_ref ),5)
        self.aps_gain_neg = round(( self.aps_mmHg_ref - self.aps_mmHg_neg ) / ( self.aps_raw_ref - self.aps_raw_neg ),5)

        self.new_raw_ref = float(raw_input("Enter raw reference value and press ENTER: "))


    

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
    runner.AddCommand("\x03",'',False)
    runner.Run()
    runner.ResetCommands()

cal_data = APS_cal_data()
_address = "pi"


if __name__ == '__main__':
    
    main()