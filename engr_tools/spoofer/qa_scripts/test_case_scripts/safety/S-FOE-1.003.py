# FILENAME: S-FOE-1.003.py
# PURPOSE: Spoofs valve position to safety

spoofName = "S-FOE-1.003.py"

import os
import sys
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner
import time

# request user input for machine ID
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
runner.AddCommand('spoof_main "-object SHwStates -data rbcValve -value HW_VALVE_RETURN"\n','->',False)

# add a -enable to start the spoofs, -trace for episodic logging
runner.AddCommand("spoof_main \"-enable -trace\"\n", "->", False)
runner.Run()

# timeout
time.sleep(360)

# need to clean up between command sets. Remove all commands.
runner.ResetCommands()

# add a -disable to stop the spoofs.
runner.AddCommand("spoof_main \"-disable\"\n", "->", False)
runner.Run()

# log spoof name & exit in RDF
runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + spoofName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()
