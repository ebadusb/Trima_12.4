# FILENAME: C-COM-1.003a.py
# PURPOSE: Suspends any task on the control computer for a given timeout

scriptName = "C-COM-1.003a.py"

import time
import os
import sys
sys.path.append(os.environ['spfsrc_6_3'])
from spoof.run import Runner

# request user input for machine ID
test_machine = int(raw_input("\nEnter the Trima Machine ID:\n\n"));

task = str(raw_input("\nEnter task to suspend:\n\n"));

to = int(raw_input("\nEnter desired timeout in seconds:\n\n"));

# calculate the Trima IP address
machine_base = int(test_machine/256)
machine_remainder = int(test_machine-(machine_base*256))
machine_base = str(machine_base)	
machine_remainder = str(machine_remainder)
address = "172.21."+machine_base+"."+machine_remainder

# instantiate a Runner.Commander
runner = Runner.Commander(address)

# log spoof name & start in RDF
runner.AddCommand("Log_Test_Info(\"Running spoof script " + scriptName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

# add spoof commands
runner.AddCommand('ts \"' + task + '"\n', '->', False)
runner.Run()

# timeout
time.sleep(to)

# need to clean up between command sets. Remove all commands.
runner.ResetCommands()

# add a -disable to stop the spoofs.
runner.AddCommand('tr \"' + task + '"\n', '->', False)
runner.Run()

# log spoof name & exit in RDF
runner.AddCommand("Log_Test_Info(\"Exiting spoof script " + scriptName + "\")\n", "->", False)
runner.Run()
runner.ResetCommands()

raw_input("Script execution complete, press ENTER to close this window: ")