--------------- Smoke Test ---------------

Written By: Robert Watkins
Date:       07/25/2011
Email:      Robert.Watkins@CaridianBCT.com



== Introduction ==

This is a tool to automate the gui button presses throughout a Trima procedure. It contains modified code orginally written by Daniel Slauson (Daniel.Slauson@caridianbct.com).

Other features include:

The ability to read in the donor info from a .dat file that can be uploaded to the Trima.

A field in the .dat file to be able to specify how many times you want to run the procedure. 

Upon receiving an 'APS Too Low' or an 'APS Too High' alarm the continue button is automatically pushed an the alarm is bypassed.




== How TO Build Smoke Test ==

The Smoke Test can be built manually by issuing the following command:

c:...\engr_tools\smokeTest\> make -f Makefile.vx

 NOTE : This assumes that system variables COMDIR, CPU and RMSGDIR
	are either set globally or passed at command line while calling
	Makefile.vx

	c:....\engr_tools\smokeTest\> -f Makefile.vx COMDIR=... CPU=... > build.log

The build can be cleaned out by issuing this command:

c:...\engr_tools\smokeTest\> make -f Makefile.vx BUILD_TYPE=CLEAN

The end product, spoof_server_control.out, will be placed in 
c:...\engr_tools\smokeTest\bin.



== To Use/Install Smoke Test ==

FTP spoof_server_control.out and donorInfo.dat into /machine/tools on a Trima.

Reboot the machine.

Upon boot-up the donor info will be read from donorInfo.dat and the donor info buttons will be pushed on the gui. During the procedure the operator must open and close the appropriate clamps. The gui button presses however are automated throughout the procedure. Upon completion of the run, the Trima
will be rebooted if the option to run the procedure multiple times was specified in donorInfo.dat.

== How to Uninstall Smoke Test ==

Delete spoof_server_control.out and donorInfo.dat from /machine/tools on the Trima.


== Notes ==

Plans for future updates:

Eliminate the need for the cassette and bags to be used during a procedure. In other words add the ability to test a full procedure on a Trima without needing the cassette or bags.