# FILENAME: C-FLG-1.051b.py
# PURPOSE: spoof red/green and integrated yield values to control flagging during a platelet-only procedure

scriptName = "C-FLG-1.051b.py"

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

# PART 2 - spoof the integrated yield to avoid unwanted flags.
# The value below = 55% of the target yield for this test + 0.2 (keeps us from triggering a flag
#  which occurs if the % drops below 54% of the accumulated yield).
# We wait until 0.2 past Yp-i in order to show the flag is gated by the ratio in this test.
raw_input("Press ENTER to run Part 2.");
os.system('cls')
reply = raw_input("Enter the Yp-i value (e.g., 1.0 or 1.5): ")
Yp = (float(reply) + 0.2) * (10 ** 11)
print('Running Part 2')

YpAbove = 0.55 * Yp
runner.AddCommand('spoof_main "-object ProcRun_IntegratedPlateletYield -data data -value ' + str(YpAbove) + '"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# PART 3 - spoof the integrated yield to cause the correct flags.
# The value below = 53.9% of the target yield + 0.2 for this test - will cause a flag once the accumulated yield > Yp-i.
raw_input("Press ENTER to run Part 3.");
os.system('cls')
print('Running Part 3')

YpBelow = 0.539 * Yp
runner.AddCommand('spoof_main "-object ProcRun_IntegratedPlateletYield -data data -value ' + str(YpBelow) + '"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# Script cleanup / exit
runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)

runner.Run()
runner.ResetCommands()

#Optional prompt to notify operator that script and all spoofs have executed.
raw_input("Press ENTER to close this window.");
