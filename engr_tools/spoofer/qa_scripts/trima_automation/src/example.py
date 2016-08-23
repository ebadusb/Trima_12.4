# FILENAME: example.py
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

# instantiate a Runner.Commander
runner = Runner.Commander(address)

runner.AddCommand('spoof_main "-object CHwOrders -data acRPM -value 0.33 -enable"\n','->',False)
# Add a -enable to start the spoofs.
runner.AddCommand("spoof_main \"-enable\"\n", "->", False)
runner.Run()
time.sleep(10)
# Need to clean up between command sets. Remove all commands.
runner.ResetCommands()
# Add a -disable to start the spoofs.
runner.AddCommand("spoof_main \"-disable\"\n", "->", False)
runner.Run()

# if entered in command line: spoof_main "-object CHwOrders -data acRPM -value 1 -timeout 20 -enable"
