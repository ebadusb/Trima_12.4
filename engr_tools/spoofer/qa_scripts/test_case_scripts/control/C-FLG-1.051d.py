# FILENAME: C-FLG-1.051d.py
# PURPOSE: spoof red/green and integrated yield values to control flagging during a platelet-only procedure

scriptName = "C-FLG-1.051d.py"

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

# PART 1 - spoof red/green values to cause RBC detector calibration to fail
runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
runner.AddCommand("Log_Test_Info(\"Running Part 1\")\n", "->", False)
runner.AddCommand('spoof_main "-object CHwStates -data red -value 900"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 1000"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# PART 2 - spoof red/green values to avoid unwanted flags
raw_input("Press ENTER to run Part 2.");
os.system('cls')
runner.AddCommand("Log_Test_Info(\"Running Part 2\")\n", "->", False)
runner.AddCommand('spoof_main "-object CHwStates -data red -value 995"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 950"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# PART 3 - spoof the integrated yield to cause the correct flags (IF calibration hadn't failed earlier).
# The value of the spoofed integrated yield below is 50% of (Yp-i)e11.
# This is below the 54% threshold to cause flagging; however, it will not
#   trigger a flag until the accumulated predicted yield equals Yp-i (which depends on the Trima version and/or region)
raw_input("Press ENTER to run Part 3.");
os.system('cls')
reply = raw_input("Enter the Yp-i value (e.g., 1.0 or 1.5): ")
Yp = float(reply) * (10 ** 11) * 0.5

runner.AddCommand('spoof_main "-object ProcRun_IntegratedPlateletYield -data data -value ' + str(Yp) + '"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

# Script cleanup / exit
runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)

runner.Run()
runner.ResetCommands()

#Optional prompt to notify operator that script and all spoofs have executed.
raw_input("Press ENTER to close this window.");
