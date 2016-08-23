from spoof.run import Runner 

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
	
    address = "172.21."+machine_base+"."+machine_remainder
	
    runner = Runner.Commander(address)

    runner.AddCommand(raw_input(""),'->',False)
    runner.Run()