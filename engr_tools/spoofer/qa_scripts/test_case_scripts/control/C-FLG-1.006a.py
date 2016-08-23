"""
	USAGE
	1. Save a copy of this file to your computer using the test case ID as the file name (<TestCaseID>.py).
	2. Change the filename in the first line of your document to reflect the name chosen in step 1.
	3. Set the 'scriptName' variable below to be the same as the file name.
	4. Do not edit the "Automated Trima connection" section below.
	5. For each command to be sent to the spoofer, add a "runner.AddCommand" line using the format given in the examples.
	6. If multiple sets of commands need to be executed the runner command cache will need to be reset between command sets by using the runner.ResetCommands() function.
"""

scriptName = "C-FLG-1.006a.py"

import time
import os
import sys
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner


#******************************************************************************
# Display usage instructions prior to execution
#******************************************************************************

print """
--------------------------------------------------------------------
  USAGE INSTRUCTIONS
  This spoof ONLY works when performing the following run.
  
  Config:
  Plasma rinseback = On, Inlet AC ratio = 13.7
  Plt Solution addition = Off, RBC Solution addition = Off
  PLT 1: (no PAS), Yield=4.2, Conc=2100, Coll vol=200
  PLS 1 = 99 mL
  AC Infusion rate management = 6, Maximum draw flow = Fast
  Return Management = 6
  Set procedure 1 to PLT1, Plasma1, No RBC
  
  Donor Info:
  Male, 4 ft 6 in, 170 lb, 40%Hct, 350 precount
  Load a cassette capable of PLT+PLS without MSS (example: 80400).
  
  Start this spoof when you have collected exactly 100 mL plasma.
  
--------------------------------------------------------------------
"""


#******************************************************************************
# Automated Trima connection
#    *** DO NOT EDIT ***
#------------------------------------------------------------------------------
# This section automates the connection to the Trima device.
#******************************************************************************

test_machine = int(raw_input("\nEnter the Trima Machine ID# (not the IP address): "));
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

# Initialize variables
ml_of_plt_to_process_in_pca_before_dropping_red = 7.0
drop_in_redreflectance = 140.0
initial_redreflectance = 800.0
plt_flow_rate = 28.0
pls_vol_when_pca_starts = 130


# Run Qc at specific rate so we can get timing correct
runner.AddCommand('spoof_main "-object Predict_SubstateInfo -data QcD_5 -value ' + str(plt_flow_rate) + '"\n', '->', False)
runner.AddCommand('spoof_main "-object Predict_SubstateInfo -data QcR_5 -value ' + str(plt_flow_rate) + '"\n', '->', False)

# Force start of PCA substate
runner.AddCommand('spoof_main "-object Predict_SubstateInfo -data Vtarget_4 -value ' + str(pls_vol_when_pca_starts) + '"\n', '->', False)
runner.AddCommand('spoof_main "-object Predict_SubstateInfo -data VtargetType_4 -value VPBAG"\n', '->', False)
runner.AddCommand('spoof_main "-object Predict_SubstateInfo -data OrVtargetType_4 -value NOCOLLECT"\n', '->', False)

# Set the initial redreflectance value that we will drop down *from*
runner.AddCommand('spoof_main "-object CHwStates -data red -value ' + str(initial_redreflectance) + '"\n', '->', False)

raw_input("Press enter to start spoof");

# add a -enable to start the spoofs, -trace for episodic logging
runner.AddCommand("spoof_main \"-enable -trace\"\n", "->", False)
runner.Run()


# Spoof baseline for desired time
time.sleep((ml_of_plt_to_process_in_pca_before_dropping_red/plt_flow_rate)*60)
runner.Run()


# Now drop red reflection
for i in range(7):
   runner.AddCommand('spoof_main "-object CHwStates -data red -value ' + str( initial_redreflectance - drop_in_redreflectance ) + '"\n', '->', False)
   time.sleep(1)
runner.Run()


# need to clean up between command sets. Remove all commands.
#runner.ResetCommands()

# add a -disable to stop the spoofs.
runner.AddCommand("spoof_main \"-disable\"\n", "->", False)
#runner.Run()

# exit
runner.Run()
runner.ResetCommands()

# Prompt to notify operator that script and all spoofs have been execute successfully.
raw_input("Script execution complete, press ENTER to close this window.");

