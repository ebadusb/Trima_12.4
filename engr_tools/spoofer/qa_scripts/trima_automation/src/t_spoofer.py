'''
Created on Jan 25, 2011

@author: Jacob Davis
'''

if __name__ == '__main__':
    
    # **************************************************************
    # Trima Connection Configuration
    # **************************************************************
	
	# Set test_machine to the machine ID of the Trima machine you want to spoof
    # 
    #test_machine = '254'
	# Comment the previous line and uncomment the next line to be prompted 
	# for a machine ID
    test_machine = int(raw_input("\nEnter the Trima Machine ID:\n\n"));
	
    machine_base = int(test_machine/256)
	
    machine_remainder = int(test_machine-(machine_base*256))
	
    machine_base = str(machine_base)
	
    machine_remainder = str(machine_remainder)
	
    # Set do_run = True to automatically execute a script
    do_run = True
	# Add commands in the following format:
    # commands.append("SpoofID") where SpoofID is the name of the
    # spoof to be executed. Add as many spoofs as needed; they will
    # be executed in the order you provide below.
	#
	# Note to self: consider making -beginSubstate and other arguments configurable else == NULL
	#
	# spoofs are set up to run with a -trace
    commands = ["RED_GREEN -var1 650 -var2 625 -timeout 420","PLO_EPCA_26 -timeout 400"]
    #commands = []
    #commands.append("RED_GREEN")
    #commands.append(raw_input("\nEnter the spoof ID(s)\n\n")) 

    
    # **************************************************************
    # Actions
    # **************************************************************
    
    from spoof.run import Runner
	
    address = "172.21."+machine_base+"."+machine_remainder

    if do_run :
        runner = Runner.Commander(address)
        for cmd in commands:
            runner.AddCommand(cmd, "->", True)
        runner.Run()
    print "\nDone!\n"
