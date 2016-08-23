# FILENAME: C-PM-1.002.py
# PURPOSE: spoof red/green values to control flagging during a platelet-only procedure

scriptName = "C-PM-1.002.py"

import time
import os
import sys
import math
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner

#******************************************************************************
# This section automates the connection to the Trima device.
#******************************************************************************

test_machine = int(raw_input("\nEnter the Trima machine ID:\n\n"));		
machine_base = int(test_machine/256)	
machine_remainder = int(test_machine-(machine_base*256))	
machine_base = str(machine_base)	
machine_remainder = str(machine_remainder)
address = "172.21."+machine_base+"."+machine_remainder

#instantiate a Runner.Commander
runner = Runner.Commander(address)

#******************************************************************************
# Command cache and execution
#------------------------------------------------------------------------------
# This section uses the runner.py module to interact with the spoofer.
# Commands are cached in the runner object, then executed in the order in 
# which they are cached without pause.
#******************************************************************************

# This script spoofs the centrifuge pressure high (to 1801mmHg) during the first 3 PrimeChannel substates

# Does this test goes through PrimeChannel2 or PrimeChannel2A?
print("Does this procedure go through substate PrimeChannel2 or PrimeChannel2A?\n")
option = raw_input("Enter 2 or 2A: ")

# PART 1 - PrimeChannel
print('\nWaiting for PrimeChannel')
runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
runner.AddCommand('spoof_main "-id Spoof_CPS_mmHg -enable -timeFromActive 2 -var1 1801 -var2 0 -beginSubstate PrimeChannel"\n','->',False)
runner.Run()
runner.ResetCommands()

# PART 2 - PrimeChannel2 or 2a
os.system('cls')
if option == '2A':
    print('Waiting for PrimeChannel2A')
    runner.AddCommand('spoof_main "-id Spoof_CPS_mmHg -enable -timeFromActive 2 -var1 1801 -var2 0 -beginSubstate PrimeChannel2A"\n','->',False)
else:
    print('Waiting for PrimeChannel2')
    runner.AddCommand('spoof_main "-id Spoof_CPS_mmHg -enable -timeFromActive 2 -var1 1801 -var2 0 -beginSubstate PrimeChannel2"\n','->',False)
runner.Run()
runner.ResetCommands()

# PART 3 - PrimeChannel3
os.system('cls')
print('Waiting for PrimeChannel3')
runner.AddCommand('spoof_main "-id Spoof_CPS_mmHg -enable -timeFromActive 2 -var1 1801 -var2 0 -beginSubstate PrimeChannel3"\n','->',False)
runner.Run()
runner.ResetCommands()

# Script cleanup / exit

runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

#Optional prompt to notify operator that script and all spoofs have been execute successfully.
raw_input("Press ENTER to close this window.");
