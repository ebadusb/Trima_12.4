# FILENAME: S-PSS-1.002b.py
# PURPOSE: Spoofs Platelet pump Control to 60 rpm

spoofName = "S-PSS-1.002b.py"

import os
import sys
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner
import time

# request user input for machine ID
print "Thank you for requesting a scripted spoof, press 1 for English.\n\n"
test_machine = int(raw_input("\nEnter the Trima Machine ID:\n\n"));

# calculate the Trima IP address
machine_base = int(test_machine/256)
machine_remainder = int(test_machine-(machine_base*256))
machine_base = str(machine_base)	
machine_remainder = str(machine_remainder)
address = "172.21."+machine_base+"."+machine_remainder

# instantiate a Runner.Commander
runner = Runner.Commander(address)

# log spoof name & start in RDF
runner.AddCommand("Log_Test_Info(\"Running spoof script " + spoofName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

# add spoof commands
# comment/uncomment the next 5 lines to achieve different combinations of pump commands
runner.AddCommand('spoof_main "-object CHwOrders -data plateletRPM -value 60"\n','->',False)


# add a -enable to start the spoofs, -trace for episodic logging
runner.AddCommand("spoof_main \"-enable -trace\"\n", "->", False)
runner.Run()

# timeout
print "Platelet pump is being spoofed to 60 rpm for 30 seconds"
time.sleep(30)

# need to clean up between command sets. Remove all commands.
runner.ResetCommands()

# add a -disable to stop the spoofs.
runner.AddCommand("spoof_main \"-disable\"\n", "->", False)
runner.Run()

# log spoof name & exit in RDF
runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + spoofName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

# timeout
raw_input("The script is now complete, thank you for choosing to test the Pause and Stop switch.  Press any key to exit.\n\n")
