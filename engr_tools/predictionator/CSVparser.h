#ifndef __CSVPARSE_H
#define __CSVPARSE_H

#include <string>
#include <vector>

class CSVParser {
 
   
 public:
  CSVParser();

  void parseALine(char* line);
  char* getColm(int colmN);
  void  clear();

  private:
	  std::vector <char*> tokens;
    
  
};
  
#endif