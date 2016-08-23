#include "datalog.h"

#include <iostream>
#include <fstream>

std::ostream& DataLog_Level::operator () (const char * fileName, int lineNumber) 
{
#ifdef __DLOGSTDOUT__
	return std::cout;
#elif !defined(__PREDICTROBUSTNESS__) || defined(__LOGTOFILE__)
   
	UNUSED_ARGUMENT(fileName); UNUSED_ARGUMENT(lineNumber);
	static std::ofstream logfile;
	
	
	time_t tv_time;
	time(&tv_time);
	tm t = *localtime( &tv_time);
	
	std::ostringstream buf; 
	
	buf << "Predictionator_64_log_" 
	   << t.tm_mon+1 
	   << t.tm_mday  
	   << t.tm_year + 1900 
	   << "_"
	   << t.tm_hour 
	   << t.tm_min   
	   << "_"
	   << t.tm_sec  
	   << ".csv" 
	   << '\0';
	
	
	if(!logfile.is_open())
	{
	   p_logfile = &logfile; 
	   logfile.open( buf.str().c_str(), std::fstream::app  );
	}
	logfile.flush();
	return logfile;
#else
	   static nullbuf nb;
	   static std::ostream o(&nb);
	   return o;
	   
#endif

}
