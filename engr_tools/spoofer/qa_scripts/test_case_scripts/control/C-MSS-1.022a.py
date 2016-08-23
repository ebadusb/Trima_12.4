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
#Spoof R/G to be > 4 and green value to < 400 in order to establish baseline values.
#R/G ratio (spoofed to be 4.05) will be added to the mss_spillover_trigger_ratio (4.0 from 
#cobe_config.dat mss_rbc_detector_offset_from_avg) to establish trigger threshold for a spillover condition (8.05).
runner.AddCommand('spoof_main "-enable -object CHwStates -data green -value 395"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data red -value 1600"\n','->',False)


#Should this be here or somewhere else?? Add a -enable to start the spoofs.
#runner.AddCommand("spoof_main \"-enable\"\n", "->", False)


#Execute cached commands.
#Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
raw_input("After MSSConnect, at transition to MSSPLTPrime2, press ENTER to spoof green value to 395 and red value to 1600.");
runner.Run()
runner.ResetCommands()


#Optional pause here with additional details if necessary (i.e. instructions, timing, etc.).
#time.sleep(30)
raw_input("At transition to MSSPltAdditionPause, press ENTER to trigger RBC spillover.");


#Next set of commands to be executed.
#Spoof green value to 1/16th of red value for 25 seconds to trigger an RBC spillover (R/G ratio = 16).
runner.AddCommand('spoof_main "-object CHwStates -data green -value 100"\n','->',False)
runner.Run()
time.sleep(25)
runner.ResetCommands()
runner.AddCommand('spoof_main "-unspoof -object CHwStates -data green"\n','->',False)
runner.AddCommand('spoof_main "-unspoof -object CHwStates -data red"\n','->',False)
runner.Run()


#Clean out runner command cache and disable spoofer.
runner.ResetCommands()
runner.AddCommand('spoof_main "-disable"\n', "->", False)
runner.Run()

#Terminate telnet session and close window when completely done running all spoofs.
raw_input("Spoofing complete! Press ENTER to EXIT.");


# if entered in command line: spoof_main "-object CHwOrders -data acRPM -value 1 -timeout 20 -enable"
