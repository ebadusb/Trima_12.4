-----------------------------------------------------------------------------
	Simulator instructions
-----------------------------------------------------------------------------
Building simulator
	Define environment variable SIMDIR. This is the folder definition of the build output.
	ex:, set SIMDIR=c:\SIMBUILD
	
	run createTrima.bat with CPU=SIMNT option

Simulator build creates the ff folders
	config
	machine
	source
	trima


Full simulation:
	Full simulation will generate all the screenshots. This can be accomplished by following the steps below:
	1.) Create the main folder as:
			c:\sim\trima\main
		Note that this folder location is hardcoded in trima code, so you cannot have any other folder name for simulator.
	2.) Copy all the files and folders from the build output to this main folder.
	3.) Execute the batch file c:\sim\trima\main\FullRoutine.bat

Manual simulation:
	Steps
	1.) Follow steps 1 to 2 in Full simulation.
	2.) Execute c:\sim\trima\main\startSim.bat
	3.) When the telnet prompt is shown, type the ff
		-> <host:c:\sim\trima\main\simguitelnet
	4.) Press enter

---------------------------------------------
prerequisite for running simulator
---------------------------------------------
Windows XP 
	Create network adapter and use ulip.sys from <Tornado>\host\x86-win32\bin\ and give it an ip 90.0.0.1
	

Window 7
	Use Xp mode to do it.