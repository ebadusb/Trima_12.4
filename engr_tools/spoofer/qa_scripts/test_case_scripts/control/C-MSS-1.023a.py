# FILENAME: C-MSS-1.23a.py
# PURPOSE: write spoofs without waiting for developers!


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
# add spoof commands
runner.AddCommand('spoof_main "-object CHwStates -data red -value 510"\n','->',False)
runner.AddCommand('spoof_main "-object CHwStates -data green -value 100"\n','->',False)

# add a -enable to start the spoofs, -trace for episodic logging
runner.AddCommand("spoof_main \"-enable -trace\"\n", "->", False)
runner.Run()

# timeout
time.sleep(30)

# need to clean up between command sets. Remove all commands.
runner.ResetCommands()

# add a -disable to stop the spoofs.
runner.AddCommand("spoof_main \"-disable\"\n", "->", False)
runner.Run()
