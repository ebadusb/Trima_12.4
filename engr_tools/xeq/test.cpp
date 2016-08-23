#include <stdio.h>

#ifdef __cplusplus 
extern "C" void test ( int argc, char** argv); 
#else 
void test ( int argc, char** argv); 
#endif

void test( int argc, char** argv)
{
    for( int j=0; j<argc; j++ )
    {
        printf( "argv[%d] = \"%s\"\n", j, argv[j] );
    }

}
