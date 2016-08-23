// FILENAME: spoofs.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofs.hpp 6.6 2008/08/15 22:11:09Z kgreusb Exp kgreusb $
// PURPOSE: abstract class to derive MySpoofs from 
// CHANGELOG:
//   $Log: spoofs.hpp $
//   Revision 6.6  2008/08/15 22:11:09Z  kgreusb
//   Adding run_after, etc. to time spoofing ias part of the Next generation spoofer project
//   Revision 6.5  2006/11/09 18:03:30Z  kgreusb
//   Adding run_generic to run paramterized spoofs for IT slike 7220 and 7334
//   Revision 6.4  2003/01/21 00:19:48Z  td07711
//   add help()
//   Revision 6.3  2002/11/05 17:57:14Z  td07711
//   remove Spoof* from SpoofBase
//   Revision 6.2  2002/10/30 22:58:11  td07711
//   add init() to spoof def macro
//   Revision 6.1  2002/10/24 21:24:08  td07711
//   base class for MySpoofs


#ifndef Spoofs_HPP // prevents multiple inclusion
#define Spoofs_HPP


#include <map>    // STL map container
#include <string>

#include "spoofbase.hpp"


// forward declarations
class SpooferAPI;



class Spoofs
{
    // RESPONSIBILITIES:
    //   1. manages a collection of Spoof objects.

    public:

        Spoofs( SpooferAPI* pApi );
        virtual ~Spoofs();

        virtual void init() = 0; // derived class must define to add supported spoofs

        void showAll( ); // shows all spoofs
        void show( const char* spoofName ); // shows named spoof
        void run( const char* spoofName, int timeout ); // runs named spoof
        void run_parameterized( const char* spoofName, int timeout, const char* Var1, const char* Var2 ); // runs named parameterized spoof
        void help( const char* spoofName ); // help for named spoof
        
        // Next Generation Spoofer features
        // starting after a substate transition or trigger message and/or ending 
        // with a substate transition or trigger message
        void run_after( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, int  timeFromActive ); 
        void run_parameterized_after( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, int timeFromActive, const char* Var1, const char* Var2 ); 
        void run_endby( const char* spoofName, int timeout, const char* endstate_or_msg, bool isSS ); 
        void run_parameterized_endby( const char* spoofName, int timeout, 
                        const char* endstate_or_msg, bool isSS, const char* Var1, const char* Var2 ); 
        void run_after_endby( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, int timeFromActive,const char* endstate_or_msg ); 
        void run_parameterized_after_endby( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, int timeFromActive, const char* endstate_or_msg, const char* Var1, const char* Var2 ); 


    protected:

        void add( SpoofBase* pSpoof ); // adds a spoof
        static SpooferAPI& api() { return *_pApi; };


    private:

        static SpooferAPI* _pApi;

        map< string, SpoofBase* > _allSpoofs;
        typedef map< string, SpoofBase* >::iterator SpoofIterator_t; 

        Spoofs(); // catch unauthorized use
        Spoofs(const Spoofs&); // catch unauthorized use 
        Spoofs& operator=(const Spoofs rhs); // catch unauthorized use
};



//
// Following macro is used to define your spoofs,
// see spoofer/spoof/myspoofs.hpp for usage example
// run_generic is base function so that the function does not have to be defined 
// for each spoof if it will not be used
//
#define CLASS( SPOOF )                                                  \
class SPOOF : public SpoofBase                                          \
{                                                                       \
    public:                                                             \
                                                                        \
        SPOOF()                                                         \
        : SpoofBase()                                                   \
        {                                                               \
            init();                                                     \
        };                                                              \
                                                                        \
        virtual void run();                                                     \
        virtual void init();                                                    \
	    virtual void help();                                                    \
        virtual void run_generic( const char* Var1, const char* Var2 ) { LOG_INFO ( "No parameters defined for spoof" ); run();};         \
}

// added so that spoof classes could be added which allow for parameters to
// be sent at runtime to a run_generic function instead of hardcoded in 
// in the run function. (IT 7220)
#define CLASS_WITHPARAMS( SPOOF)                                        \
class SPOOF : public SpoofBase                                          \
{                                                                       \
    public:                                                             \
                                                                        \
        SPOOF()                                                         \
        : SpoofBase()                                                   \
        {                                                               \
            init();                                                     \
        };                                                              \
                                                                        \
        virtual void run();                                                     \
        virtual void init();                                                    \
	    virtual void help();                                                    \
        virtual void run_generic( const char* Var1, const char* Var2 );         \
}

#endif // Spoofs_HPP
