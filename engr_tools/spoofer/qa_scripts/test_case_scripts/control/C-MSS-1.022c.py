# FILENAME: CMSS1.22a.py
# PURPOSE: write spoofs wwithout waiting for developers!
import time
import os
import sys

sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner



test_machine = int(raw_input("\nEnter the Trima Machine ID:\n\n"));
	
machine_base = int(test_machine/256)
	
machine_remainder = int(test_machine-(machine_base*256))
	
machine_base = str(machine_base)
	
machine_remainder = str(machine_remainder)

address = "172.21."+machine_base+"."+machine_remainder

#Instantiate a Runner.Commander
runner = Runner.Commander(address)


#Cache commands to be executed.
#Spoof green value to 10 for 25 seconds to trigger an RBC spillover.
runner.AddCommand('spoof_main "-enable -object CHwStates -data green -value 10"\n','->',False)

raw_input("At transition to MSSPltAdditionPause, press ENTER to spoof an RBC spillover.");

#Execute cached commands.
runner.Run()
time.sleep(25)
runner.ResetCommands()


#Disable spoofer upon completion of requisite spoofs.
runner.AddCommand('spoof_main "-disable"\n', "->", False)
runner.Run()
runner.ResetCommands()


#Terminate telnet session and close window when completely done running all spoofs.
raw_input("Spoofing complete! Press ENTER to EXIT.");
