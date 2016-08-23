DESCRIPTION OF MKS/IT TRACKING

High Level Overview - In order to link and control source changes, the Trima team requires  
that every source code change be associated with an issue tracker which provides the rationale 
and verification for the change. When checking in code through MKS, the developer will be required
to enter an IT number or numbers associated with each file that is checked-in. These IT numbers must
correspond to ITs that are (1) not "Closed", i.e., are "Under Investigation" or 
"Awaiting Confirmation"; and (2) must have a 'Target' that is associated with the current release.
This information (date, user, project, file, and IT) are logged for use by SQA in identifying code
that may need to be code reviewed.

Details -

The .pj files contain code (script) which asks the user to enter a list of ITs prior to check-in of
each file. The list of ITs is checked by the program valid.exe using a predifined list of target 
values that are contained in the .pj file. If the criteria are met for all of the ITs, then the
information is logged into a file called log.all, and the check-in is allowed to proceed. If the 
criteria are not met, then the user is informed of the nature of the problem, and the check-in is 
aborted.

At various times the program processlogs.exe is run to produce one line per file and IT in a 
comma-separated format (log.csv). This file is later used to indicate which code may need to be
reviewed, either because it is associated with safety code or because it is associated with an 
IT which is to be verified (at least in part) by code inspection.

