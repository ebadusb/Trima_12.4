
#include <iostream>
#include <cstdlib>
#include "CSVparser.h"
 

CSVParser::CSVParser()
{
   
}

 
void 
CSVParser::parseALine(char* line)
{
   int cnt = 0;
   char * pch;
   pch = strtok (line,",");
   while ((pch != NULL) && (cnt < 7))   {
       cnt++;
      //printf ("%s\n",pch);
      tokens.push_back(pch);
      pch = strtok (NULL, ",");
   }
}


char* 
CSVParser::getColm(int colmN)
{
    char* err = "Error";

    if (!(colmN > tokens.size()))
    {
        
       return tokens[colmN];

    }
    return err;
}


