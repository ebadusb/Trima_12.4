# FILENAME: C-FLG-1.090b.py
# PURPOSE: .

"""
	USAGE
	1. Save a copy of this file to your computer using the test case ID as the file name (<TestCaseID>.py).
	2. Change the filename in the first line of your document to reflect the name chosen in step 1.
	3. Set the 'scriptName' variable below to be the same as the file name.
	4. Do not edit the "Automated Trima connection" section below.
	5. For each command to be sent to the spoofer, add a "runner.AddCommand" line using the format given in the examples.
	6. If multiple sets of commands need to be executed the runner command cache will need to be reset between command sets by using the runner.ResetCommands() function.
"""

scriptName = "C-FLG-1.090b.py"

import time
import os
import sys
import math
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner

#******************************************************************************
# Automated Trima connection
#    ***DO NOT EDIT***
#------------------------------------------------------------------------------
# This section automates the connection to the Trima device.
#******************************************************************************

test_machine = int(raw_input("\nEnter the Trima Machine ID: "));	
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

#Cache commands to be executed.
#-enable option needs to be added either as a standalone command or as an option in another command.
runner.AddCommand('spoof_main "-object CHwStates -data red -value 995"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 950"\n','->',False)
#runner.AddCommand('spoof_main "-object ProcRun_IntegratedPlateletYield -data data -value 320000000000"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)
runner.Run()
runner.ResetCommands()
raw_input("Monitor the Trima's states. Press enter to trigger spillover.");
runner.AddCommand('spoof_main "-object CHwStates -data red -value 1510"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 1000"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)
runner.Run()
runner.ResetCommands()
raw_input("Press enter before continuing from the alert.");
runner.AddCommand('spoof_main "-object CHwStates -data red -value 495"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 450"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)
runner.Run()
runner.ResetCommands()
raw_input("When recovery process finishes press enter to force a spillover .");
for num in range(0,3):  
 if (num != 0) :
  raw_input("Press enter to force a spillover.");
 runner.AddCommand('spoof_main "-object CHwStates -data red -value 1510"\n','->',False)
 runner.AddCommand('spoof_main "-object CHwStates -data green -value 1000"\n','->',False)
 runner.AddCommand('spoof_main "-enable"\n','->',False)
 runner.Run()
 runner.ResetCommands()
 raw_input("Verify there is no alert on the Trima and press enter to continue.");
 runner.AddCommand('spoof_main "-object CHwStates -data red -value 495"\n','->',False)
 runner.AddCommand('spoof_main "-object CHwStates -data green -value 450"\n','->',False)
 runner.AddCommand('spoof_main "-enable"\n','->',False)
 runner.Run()
 runner.ResetCommands()
#Optional prompt to notify operator that script and all spoofs have been execute successfully.
raw_input("Please press ENTER to exit.");

