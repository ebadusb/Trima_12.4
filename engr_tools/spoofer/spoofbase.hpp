// FILENAME: spoofbase.hpp
// $Header: H:/BCT_Development/Trima5.1x/Trima/engr_tools/spoofer/rcs/spoofbase.hpp 6.5 2003/01/21 00:19:09Z td07711 Exp kgreusb $
// PURPOSE: base stuff for a spoof
// CHANGELOG:
//   $Log: spoofbase.hpp $
//   Revision 6.5  2003/01/21 00:19:09Z  td07711
//   add help()
//   Revision 6.4  2002/11/05 17:59:26Z  td07711
//   remove Spoofs* member data
//   Revision 6.3  2002/10/30 23:00:03  td07711
//   add init()
//   Revision 6.2  2002/10/24 21:18:52  td07711
//   mod for different timeout mechanism
//   Revision 6.1  2002/10/23 20:45:54  td07711
//   add static SpooferAPI*
//   Revision 1.1  2002/10/22 23:17:13  td07711
//   Initial revision


#ifndef SpoofBase_HPP // prevents multiple inclusion
#define SpoofBase_HPP


// forward declaration
class SpooferAPI;


class SpoofBase
{
    // RESPONSIBILITIES:
    //   1. provides base stuff for a spoof
    //   2. provide static functions for managing collection of spoofs

    //
    // static data and functions
    //

    public:

        static void init( SpooferAPI* p ); // API that spoofs will use

        
    protected:

        static SpooferAPI& api() { return *_pApi; };


    private:

        static SpooferAPI* _pApi; // API in use



    //
    // non static functions and data
    //

    public:

        SpoofBase();
        virtual ~SpoofBase( );

        virtual void show() const; // shows stuff about this spoof
        
        virtual void run() = 0; // subclass must define
        virtual void init() = 0; // subclass must define
        virtual void help() = 0; // subclass must define
        virtual void run_generic(const char* Var1, const char* Var2); // subclass must define to override default and use variables


        const char* name() const { return _name; };
        const char* comment() const { return _comment; };
        int getTimeout() const { return _timeout; };
        const char* Var1() const { return _Var1; };
        const char* Var2() const { return _Var2; };
 
    protected:

        const char* _name;
        const char* _comment;
        int _timeout;
        const char* _Var1;
        const char* _Var2;


    private:

        SpoofBase(const SpoofBase&); // catch unauthorized use 
        SpoofBase& operator=(const SpoofBase& rhs); // catch unauthorized use
};


#endif // SpoofBase_HPP
