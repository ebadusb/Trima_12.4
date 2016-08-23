This tools is use to analyze message delays between proc and safety.

To use this tool:

1- Insert the following code into the Trima source code and rebuild the system.
File: s_exec.cpp
Function: void aControlStatusMsg_rcvr::notify()
Code:
#define DEBUGMSG 1
#if DEBUGMSG
   struct debugtime {
	   timespec proc;
	   timespec safe;
   };
   static debugtime times[100] = { 0 };
   static timespec receivedTime = { 0 };
   static int index = 0;
   clock_gettime( CLOCK_REALTIME, &receivedTime );
   times[index].proc = sendTime;
   times[index].safe = receivedTime;
   ++index;
   if (index >= 100) {
	   for (int i = 0; i < 100; ++i) {
		   DataLog(log_level_safe_exec_info) 
		       << "MSG" 
		       << " Proc: " 
			   << times[i].proc.tv_sec 
			   << ":" 
			   << times[i].proc.tv_nsec
		       << " Safe: " 
			   << times[i].safe.tv_sec 
			   << ":" 
			   << times[i].safe.tv_nsec
			   << endmsg;
	   }

	   index = 0;
   }
#endif

The code will generate logs which the tool can extract from a datalog for analysis.

The column information are the following: 
Column1: Proc Time
Column2: Safety Time
Column3: Time Offset
Column4: Delta Time
Column5: Time Drift
Column6: Latency


