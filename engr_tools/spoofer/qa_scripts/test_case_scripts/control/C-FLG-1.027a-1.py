# FILENAME: C-FLG-1.027a-1.py
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

scriptName = "C-FLG-1.027a-1.py"

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

runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

#Cache commands to be executed.
#-enable option needs to be added either as a standalone command or as an option in another command.
runner.AddCommand('spoof_main "-object CHwStates -data red -value 995"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 950"\n','->',False)
runner.AddCommand('spoof_main "-enable"\n','->',False)

runner.Run()
runner.ResetCommands()

runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

#Optional prompt to notify operator that script and all spoofs have been execute successfully.
raw_input("Press ENTER to close this window.");
