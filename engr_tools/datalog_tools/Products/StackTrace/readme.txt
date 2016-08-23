I added new optional arguments to the tool, please see the following:
StackTrace [-location buildpath] [-version (2.02 | 2.20)] [-osfile filename] [-ospath pathnames] filename

-location
This argument provides a means to specify the build location. Same as before.
Example: –location \bctquad3\home\BCT_Development\Trima5.2\trima_builds\Trima_build-7.316

-version
This is the Tornado version to use. Same as before.
Example: -version 2.02

-osfile
This is the OS binary file name. The default is vxWorks. This is needed for Trima as its sometimes necessary to specify the binary file name.
Example: -osfile vxWorks_ampro

-ospath
This is a comma delimited list of sub paths to the OS binary files. If not specified, then the OS symbols will not be loaded. Note that order matters; it must match the nodes of the system.
Example: -ospath /trima/safety/boot,/vxboot

I also added a new feature; all argument can be set as environmental variables. This is convenient if you don’t want to constantly specify the OS paths. The environmental variables names are:

STACKTRACE_location
STACKTRACE_version
STACKTRACE_osfile
STACKTRACE_ospath 

