# FILENAME: C-FLG-1.052.py
# PURPOSE: spoof red/green values to control flagging during a platelet-only procedure

scriptName = "C-FLG-1.052.py"

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

# PART 1 - spoof red/green values during prime substates to avoid unwanted flags
runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
runner.AddCommand("Log_Test_Info(\"Running Part 1\")\n", "->", False)
runner.AddCommand('spoof_main "-object CHwStates -data red -value 995"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 950"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# PART 2 - spoof the integrated yield to avoid unwanted flags
# the value below = 55% of the 1.1e11 target yield for this test (keeps us from triggering a separate flag
#  which occurs if the % drops below 54%)
raw_input("Press ENTER to run Part 2.");
os.system('cls')
print('Running Part 2')

runner.AddCommand('spoof_main "-object ProcRun_IntegratedPlateletYield -data data -value 60500000000"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# Script cleanup / exit

runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

#Optional prompt to notify operator that script and all spoofs have been execute successfully.
raw_input("Press ENTER to close this window.");
