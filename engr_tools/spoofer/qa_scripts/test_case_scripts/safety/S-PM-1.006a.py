"""
	USAGE
	1. Save a copy of this file to your computer using the test case ID as the file name (<TestCaseID>.py).
	2. Change the filename in the first line of your document to reflect the name chosen in step 1.
	3. Set the 'scriptName' variable below to be the same as the file name.
	4. Do not edit the "Automated Trima connection" section below.
	5. For each command to be sent to the spoofer, add a "runner.AddCommand" line using the format given in the examples.
	6. If multiple sets of commands need to be executed the runner command cache will need to be reset between command sets by using the runner.ResetCommands() function.
"""

scriptName = "S-PM-1.006a.py"

import time
import os
import sys
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

# instantiate a Runner.Commander
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
#Example command: runner.AddCommand('spoof_main "-enable -object CHwOrders -data acRPM -value 0.33 -enable"\n','->',False)
#Templates:
#runner.AddCommand('spoof_main "-enable"\n','->',False)
#runner.AddCommand('spoof_main "-id  -var1  -var2 "\n','->',False)
#runner.AddCommand('spoof_main "-enable -object  -data  -value "\n','->',False)
runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
runner.AddCommand('spoof_main "-object HalStatus_plasmaCmdFlow -data data -value 160"\n','->',False)
runner.AddCommand('spoof_main "-id EnableDisable -timeout 10"\n','->',False)

#Execute cached commands, then clear cached commands to get runner ready for next set of commands.
#Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
#The raw_input method will display the string then the operator will have to press ENTER to proceed.
raw_input("Press enter to spoof the commanded flow of the inlet pump to 160 ml/min:");

runner.Run()
runner.ResetCommands()


#(Optional) Pause here with additional details if necessary (i.e. instructions, timing, etc.).
#time.sleep for no operator prompt, raw_input for prompt.
#The raw_input method will display the string then the operator will have to press ENTER to proceed.
#time.sleep(30)
#raw_input("<Prompt to present to operator>, press ENTER to continue.");



#(Optional) Next set of commands to be executed.
#If more sets of commands are needed copies of this section and the "Execute" section below may be inserted after the "Execute" section below.
#If data needs to be unspoofed prior to next spoof use command below.
#runner.AddCommand('spoof_main "-unspoof -object <Spoofed Object Here> -data <Spoofed Data Here>"\n','->',False)
#Example command: runner.AddCommand('spoof_main "-enable -object CHwOrders -data acRPM -value 0.33 -enable"\n','->',False)




#(Optional) Execute next set of cached commands.
#Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
#The raw_input method will display the string then the operator will have to press ENTER to proceed.
#raw_input("<Prompt to operator>");
#runner.Run()
#runner.ResetCommands()



# Add a -disable or -unspoof commend to disable the spoofer or simply unspoof spoofed data. 
#runner.AddCommand('spoof_main "-unspoof -object <Spoofed Object Here> -data <Spoofed Data Here>"\n','->',False)

runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
runner.AddCommand('spoof_main "-disable"\n','->',False)
runner.Run()
runner.ResetCommands()


#Optional prompt to notify operator that script and all spoofs have been execute successfully.
raw_input("Script execution complete, press ENTER to close this window.");

