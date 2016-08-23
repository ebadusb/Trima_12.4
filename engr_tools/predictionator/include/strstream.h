#ifndef __STRSTREAM_H_INC
#define __STRSTREAM_H_INC

class string
{
public:
   string() {};
   virtual ~string() {};
   const char *c_str() const { return 0; };
   const char *operator =( char * lpsz ) { return 0; };
   const char *operator =( const char * lpsz ){ return 0; };
};
#endif