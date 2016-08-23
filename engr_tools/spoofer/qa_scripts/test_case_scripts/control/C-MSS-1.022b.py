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
#Spoof green value to < 400 in order to establish green average of less than 400 during R/G baseline.
runner.AddCommand('spoof_main "-enable -object CHwStates -data green -value 395"\n','->',False)


#Execute cached commands.
#Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
raw_input("After MSSConnect, at transition to MSSPLTPrime2, press ENTER to spoof green value to 395.");
runner.Run()
runner.ResetCommands()


#Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
raw_input("At transition to MSSPltAdditionPause, press ENTER to unspoof green value.");


#Disable spoofer upon completion of requisite spoofs.
runner.AddCommand('spoof_main "-disable"\n', "->", False)
runner.Run()
runner.ResetCommands()

#Terminate telnet session and close window when completely done running all spoofs.
raw_input("Spoofing complete! Press ENTER to EXIT.");
