'''
Created on Jan 25, 2011

@author: Jacob Davis
'''

if __name__ == '__main__':

    # **************************************************************
    # Build Configuration
    # **************************************************************
    
    # Set do_build = True to build. Set to False to skip build.
    do_build = True
    # Root is the path containing the create_*.bat file.
    # Warning: use forward slashes in the path.
    root = 'C:/Patrick/Local/Trima/Trima_6.0/engr_tools/'
    # Batch is the name of the batch file
    batch = "makefile.vx"
    # If scrub == True, script will delete old .OUT file before doing anything else.
    # WARNING: If scrub is set to false, the build script may not overwrite existing 
    # files!
    scrub = True  
    # By default, a log file will be created at root/create.log.
    logfile = root + "create.log"
    # By default, .OUT files are located at root/bin/.
    outdir = "C:/Patrick/Local/Trima/Trima_6.0/engr_tools/bin/"
    # The local path to the Tornado install directory. 
    tor_dir = "C:/Tornado2.2/"
    
    # **************************************************************
    # Trima Connection Configuration
    # **************************************************************
    
    # Set do_load = True if you want to auto-load the new tools build 
    # to the Trima. Set to False if you want to skip loading.
    do_load = True
    
    # Set do_reboot = True to reboot the Trima after tools are loaded.
    # WARNING: If do_load is set to False, the the Trima will not be 
    # rebooted, regardless of do_reboot's setting.
    do_reboot = True
	
	# Set test_machine to the machine ID of the Trima machine you want to load
    # with your new build
    if do_load :
        
        test_machine = int(raw_input("\nEnter the Trima Machine ID:\n\n"));
	
        machine_base = int(test_machine/256)
	
        machine_remainder = int(test_machine-(machine_base*256))
	
        machine_base = str(machine_base)
	
        machine_remainder = str(machine_remainder)
		
        address = "172.21."+machine_base+"."+machine_remainder

    
    # **************************************************************
    # Actions
    # **************************************************************
    
    from spoof.build import builder, codesyncer
    from spoof.load import tools_loader
    from spoof.run import Runner


    # Build
    if do_build :
        print "\nStarting Build...\n"
        builder.base_builder(root, batch, logfile, outdir, scrub, tor_dir).build()

    # Load the new build
    if do_load :
        loader = tools_loader.loader(address, outdir)
        print "\nTransfering files...\n"
        loader.tool_transfer()
        if do_reboot:
            print "\nRebooting Trima...\n"
            loader.reboot()

    print "\nDone!\n"
