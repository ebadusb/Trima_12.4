// FILENAME: spoofs.cpp
// $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofs.cpp 6.9 2009/02/03 19:03:02Z kgreusb Exp $
// PURPOSE: SpoofedObject class implementation
// CHANGELOG:
//   $Log: spoofs.cpp $
//   Revision 6.9  2009/02/03 19:03:02Z  kgreusb
//   Change timeFrom Active from bool to int for testing Its like 7720
//   Revision 6.8  2008/08/15 22:10:08Z  kgreusb
//   Adding run_after, etc. to allow for timing spoofs as part of the Next Generation spoofer project
//   Revision 6.7  2006/11/09 18:04:23Z  kgreusb
//   Adding paramterized spoofs as run_generic for ITs such as 7220 and 7334
//   Revision 6.6  2003/07/15 17:15:56Z  td07711
//   IT6199 - use predefined datalog_levels for logging
//   Revision 6.5  2003/03/14 22:14:58Z  td07711
//   use datalogger directly instead of stderr for some msgs
//   Revision 6.4  2003/01/21 00:19:27Z  td07711
//   add help()
//   Revision 6.3  2003/01/18 01:24:13Z  td07711
//   log spoof's comment when run
//   Revision 6.2  2002/10/30 22:59:07Z  td07711
//   wait for timeout in run() instead of in each spoof
//   Revision 6.1  2002/10/24 21:22:38  td07711
//   save SpooferAPI* locally
//   removed timeout arg from run()


// libtools includes
#include "spooferlog.hpp"

// common inludes
#include "datalog_levels.h"

// spoofer includes
#include "spoofs.hpp"
#include "spooferapi.hpp"
#include "spoofbase.hpp"


// initialize statics
SpooferAPI* Spoofs::_pApi = 0;


Spoofs::Spoofs( SpooferAPI* pApi )
{
    // PURPOSE: give SpoofBase the API and save locally

    _pApi = pApi;
    SpoofBase::init( pApi );

    LOG_DEBUG("Spoofs: constructed");
}


Spoofs :: ~Spoofs()
{
    // PURPOSE: free heap

    LOG_DEBUG( "~Spoofs: n=%d", _allSpoofs.size() );

	SpoofIterator_t iter = _allSpoofs.begin();
	SpoofIterator_t end = _allSpoofs.end();

    for(; iter != end; ++iter) {
        delete iter->second; 
    }
}


void Spoofs :: add( SpoofBase* pSpoof )
{
    // PURPOSE: adds a spoof to the container

    ASSERT( pSpoof );

    if( _allSpoofs.find( pSpoof->name() ) != _allSpoofs.end() )
    {
        LOG_ERROR("add: duplicate spoof %s", pSpoof->name() );
        return;
    }
    
    _allSpoofs[ pSpoof->name() ] = pSpoof;
    
    LOG_DEBUG( "add: %s added", pSpoof->name() ); 
}


void Spoofs :: showAll()
{
    // PURPOSE: shows all Spoof objects

    LOG_INFO( "showAll: dumping all spoofs" );

    for( SpoofIterator_t i = _allSpoofs.begin();
        i != _allSpoofs.end(); i++ )
    {
        (( *i ).second)->show();
    }
}


void Spoofs :: show( const char* spoofName )
{
    // PURPOSE: shows named spoof

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("show: invalid spoof %s", spoofName );
    }
    else
    {
        _allSpoofs[ spoofName ]->show();
    }
}


void Spoofs :: run( const char* spoofName, int timeout )
{
    // PURPOSE: runs the named spoof

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    {
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
	const char* comment = _allSpoofs[ spoofName ]->comment();

        LOG_INFO( "running Spoof %s with timeout=%d", spoofName, timeout );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " with timeout=" << timeout << endmsg;

        api().setTimeout( timeout );
        _allSpoofs[ spoofName ]->run();
        api().waitForTimeout();
    }
}

// added for IT 7220 to support generic parameterized spoofs
void Spoofs :: run_parameterized( const char* spoofName, int timeout, const char* Var1, const char* Var2 )
{
    // PURPOSE: runs the generic parameterized version of the named spoof

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    {
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
        const char* comment = _allSpoofs[ spoofName ]->comment();

        LOG_INFO( "running Spoof %s with timeout=%d, var1=%s, var2=%s", spoofName, timeout, Var1, Var2 );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " with timeout=" << timeout <<" and variable1=" << Var1
            << " variable2=" << Var2 << endmsg;

        api().setTimeout( timeout );
        _allSpoofs[ spoofName ]->run_generic(Var1, Var2);
        api().waitForTimeout();
    }
}


void Spoofs :: help( const char* spoofName )
{
    // PURPOSE: runs help for the named spoof

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("help: invalid spoof %s", spoofName );
    }
    else
    {
        int timeout = _allSpoofs[ spoofName ]->getTimeout();
        const char* comment = _allSpoofs[ spoofName ]->comment();

        LOG_INFO( "Spoof %s timeout=%d", spoofName, timeout );
        LOG_INFO( "%s", comment );
        _allSpoofs[ spoofName ]->help();
    }
}


// added for Next Generation Spoofer project to give substate awareness
void Spoofs :: run_after( const char* spoofName, int timeout, 
                          const char* substate_or_msg, int delta, bool isSS, int timeFromActive )
{
    // PURPOSE: runs the named spoof after the detection of the substate transition or
    //          message trigger
    //          duration the spoof runs start "now" or after the trigger based on boolean value

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    { 
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
        const char* comment = _allSpoofs[ spoofName ]->comment();
    
        LOG_INFO( "running Spoof %s %d seconds after %s", spoofName, delta, substate_or_msg );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " with timeout=" << timeout << ", " << delta << " seconds after " << substate_or_msg << endmsg;

        int SS; 
        if (isSS)  // trigger on a substate transition
        {
            int theSS;  // enum
            int* buff = &theSS;
            int bufsize = sizeof(int);
            if (!(timeFromActive > 0)) { api().setTimeout( timeout ); }
            if (api().waitForSubstate(substate_or_msg, buff, bufsize, timeout * 1000 ) )
            {
                LOG_DEBUG( "ProcSubstateChangeMsg data is %d", theSS );
                if ( delta > 0 ) 
                {
                    api().setTimeout ( delta );
                    api().waitForTimeout();
                }
                if ((timeFromActive > 0)) {api().setTimeout( timeFromActive ); }
                _allSpoofs[ spoofName ]->run();
                api().waitForTimeout();
            }
            else {LOG_DEBUG( "timed out waiting for transition to %s to start spoof", substate_or_msg ); }
        }
        else    // trigger on a message
        {
            // for new assume will only trigger on integer messages
            if (!(timeFromActive >0)) { api().setTimeout ( timeout ); }
            if (api().getData(substate_or_msg, "data", &SS, 4, timeout * 1000) )
            {
                if ( delta > 0 ) 
                {
                    api().setTimeout ( delta );
                    api().waitForTimeout();
                }
                if ((timeFromActive > 0)) { api().setTimeout( timeFromActive ); }
                _allSpoofs[ spoofName ]->run();
                api().waitForTimeout();

            }
            else {LOG_DEBUG( "timed out without receiving trigger message to start spoof" ); }
        }

    }

}


void Spoofs :: run_parameterized_after( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, int timeFromActive, const char* Var1, const char* Var2 )
{
    // PURPOSE: runs the parameterized version of the named spoof after the detection of the substate transition or
    //          message trigger
    //          duration the spoof runs start "now" or after the trigger based on boolean value


    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    { 
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
        const char* comment = _allSpoofs[ spoofName ]->comment();

        LOG_INFO( "running Spoof %s %d seconds after %s parameters are %s and %s", 
                  spoofName, delta, substate_or_msg, Var1, Var2 );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " with timeout=" << timeout << ", " << delta << " seconds after " << substate_or_msg 
            << " with parameters " << Var1 << " and " << Var2 << endmsg;

        int SS; 
        if (isSS) //need to wait for substate/state change
        {
            int theSS;    // or enum?
            int* buff = &theSS;
            int bufsize = sizeof(int);
            if (!(timeFromActive > 0)) { api().setTimeout( timeout ); }
            if (api().waitForSubstate( substate_or_msg, buff, bufsize, timeout * 1000) )
            {
                LOG_DEBUG ("ProcSubstateChangeMsg data is %d", theSS );
                if ( delta > 0 ) 
                {
                    api().setTimeout ( delta );
                    api().waitForTimeout();
                }
                if ( (timeFromActive > 0)) { api().setTimeout( timeFromActive ); }
                _allSpoofs[ spoofName ]->run_generic(Var1, Var2);
                api().waitForTimeout();
            }
            else {LOG_DEBUG("timed out waiting for transition to %s to trigger spoof", substate_or_msg ); }
        }   
        else    // trigger on a message
        {
            // for now assume will only trigger on integer messages
            if (!(timeFromActive > 0)) { api().setTimeout ( timeout ); }
            if (api().getData(substate_or_msg, "data", &SS, 4, timeout * 1000 ) )
            {
                LOG_DEBUG( "trigger message has been detected in time");
                if ( delta > 0 ) 
                {
                    api().setTimeout ( delta );
                    api().waitForTimeout();
                }
                if ((timeFromActive > 0)) {api().setTimeout( timeFromActive ); }
                 _allSpoofs[ spoofName ]->run_generic(Var1, Var2);
                api().waitForTimeout();
            }
            else  { LOG_DEBUG( "timed out without receiving trigger message to start spoof" );}
        }
     }

}

void Spoofs :: run_endby( const char* spoofName, int timeout, const char* endstate_or_msg, bool isSS )
{
    // PURPOSE: runs the named spoof until the named substate transition or message trigger

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    { 
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
        const char* comment = _allSpoofs[ spoofName ]->comment();
    
        LOG_INFO( "running Spoof %s until %s", spoofName, endstate_or_msg );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " until " << endstate_or_msg << " or timeout=" << timeout << endmsg;

        // start spoof with timeout (the max time)
        api().setTimeout( timeout );     // do I need this part?
        _allSpoofs[ spoofName ]->run();
        
        // now wait for substate/state transition or message to signify ending the spoof
        int SS; 
        if (isSS)  // end spoofing on a substate transition
        {
            int theSS;  //for enum
            int* buff = &theSS;
            int bufsize = sizeof(int);
            if (api().waitForSubstate( endstate_or_msg, buff, bufsize, timeout * 1000 ) )
            {
                LOG_DEBUG( "waitForSubstate returned %d, ending spoof", theSS );
            }
            else
            {
                LOG_DEBUG( "waitForSubstate timed out waiting for %s to end spoof", endstate_or_msg );
            }
            
        }
        else    // trigger on a message
        {
            if (api().getData(endstate_or_msg, "data", &SS, 4, timeout * 1000) )
            {
                LOG_DEBUG( "received message %s so ending spoofing", endstate_or_msg );
            }
            else
            {
                LOG_DEBUG("timed out waiting for message %s to end spoof", endstate_or_msg );
            }
        }

    }

}

void Spoofs :: run_parameterized_endby( const char* spoofName, int timeout, 
                        const char* endstate_or_msg, bool isSS, const char* Var1, const char* Var2 )
{
    // PURPOSE: runs the generic parameterized version of the named spoof 
    //          until the named substate transition or message trigger

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    { 
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
        const char* comment = _allSpoofs[ spoofName ]->comment();
    
        LOG_INFO( "running Spoof %s until %s with parameters %s and %s", 
                  spoofName, endstate_or_msg, Var1, Var2 );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " until " << endstate_or_msg << " or timeout=" << timeout 
            << " with parameters " << Var1 << " and " << Var2 << endmsg;

        // start spoof with timeout (the max time)
        api().setTimeout ( timeout );  // do I really need this part?
       _allSpoofs[ spoofName ]->run_generic(Var1, Var2);
        
       // now wait for substate/state transition or message to signify ending the spoof
       int SS; 
       if (isSS)  // end spoofing on a substate transition
       {
           int theSS;  //for enum
           int* buff = &theSS;
           int bufsize = sizeof(int);
           if (api().waitForSubstate( endstate_or_msg, buff, bufsize, timeout * 1000 ) )
           {
               LOG_DEBUG( "waitForSubstate returned %d, ending spoof", theSS );
           }
           else
           {
               LOG_DEBUG( "waitForSubstate timed out waiting for %s to end spoof", endstate_or_msg );
           }

       }
       else    // trigger on a message
       {
           if (api().getData(endstate_or_msg, "data", &SS, 4, timeout * 1000) )
           {
               LOG_DEBUG( "received message %s so ending spoofing", endstate_or_msg );
           }
           else
           {
               LOG_DEBUG("timed out waiting for message %s to end spoof", endstate_or_msg );
           }
       }

    }

}

void Spoofs :: run_after_endby( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, 
                        int timeFromActive, const char* endstate_or_msg )
{
    // PURPOSE: runs the named spoof after the named substate transition or message trigger
    //          until the named ending substate transition or message trigger

    ASSERT( spoofName );

   if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
   {
       LOG_ERROR("run: invalid spoof %s", spoofName );
   }
   else
   { 
       if( timeout == 0 )
       {
           timeout = _allSpoofs[ spoofName ]->getTimeout();
       }
       const char* comment = _allSpoofs[ spoofName ]->comment();

       LOG_INFO( "running Spoof %s %d seconds after %s", spoofName, delta, substate_or_msg );
       LOG_INFO( "%s", comment );

       DataLog( log_level_spoofer_info ) << "running " << spoofName
           << " with timeout=" << timeout << ", " << delta << " seconds after " << substate_or_msg << endmsg;

       int SS; 
       if (isSS)  // trigger on a substate transition
       {
           int theSS;  // enum
           int* buff = &theSS;
           int bufsize = sizeof(int);
           if (!(timeFromActive > 0)) { api().setTimeout( timeout ); }
           if (api().waitForSubstate(substate_or_msg, buff, bufsize, timeout * 1000 ) )
           {
               LOG_DEBUG( "ProcSubstateChangeMsg data is %d", theSS );
               if ( delta > 0 ) 
               {
                   api().setTimeout ( delta );
                   api().waitForTimeout();
               }
               if ((timeFromActive > 0)) {api().setTimeout( timeFromActive ); }
               _allSpoofs[ spoofName ]->run();
               // now wait for the endSubstate or timeout
               if (api().waitForSubstate(endstate_or_msg, buff, bufsize, timeout * 1000) )
               {
                   LOG_DEBUG( "received end substate/state" ); 
               }
               else
               {
                   LOG_DEBUG("timed out without getting the end substate/state" );
               }
 
           }
           else {LOG_DEBUG( "timed out waiting for transition to %s to start spoof", substate_or_msg ); }
       }
       else    // trigger on a message
       {
           // for new assume will only trigger on integer messages
           if (!(timeFromActive > 0)) { api().setTimeout ( timeout ); }
           if (api().getData(substate_or_msg, "data", &SS, 4, timeout * 1000) )
           {
               if ( delta > 0 ) 
               {
                   api().setTimeout ( delta );
                   api().waitForTimeout();
               }
               if ((timeFromActive > 0)) { api().setTimeout( timeFromActive ); }  // this may not be correct
               _allSpoofs[ spoofName ]->run();
               // now wait for end message trigger or timeout
               if (api().getData(endstate_or_msg, "data", &SS, 4, timeout * 1000) )
               {
                   LOG_DEBUG( "received end message" );
               }
               else
               {
                   LOG_DEBUG( "timed out without receiving end message" );
               }
 
           }
           else {LOG_DEBUG( "timed out without receiving trigger message to start spoof" ); }
       }

   }

}

void Spoofs :: run_parameterized_after_endby( const char* spoofName, int timeout, 
                        const char* substate_or_msg, int delta, bool isSS, int timeFromActive, const char* endstate_or_msg, const char* Var1, const char* Var2 )
{
    // PURPOSE: runs the generic parameterized version of the named spoof
    //          after the named substate transition or message trigger    
    //          until the named substate transition or message trigger

    ASSERT( spoofName );

    if( _allSpoofs.find( spoofName ) == _allSpoofs.end() )
    {
        LOG_ERROR("run: invalid spoof %s", spoofName );
    }
    else
    { 
        if( timeout == 0 )
        {
            timeout = _allSpoofs[ spoofName ]->getTimeout();
        }
        const char* comment = _allSpoofs[ spoofName ]->comment();
    
        LOG_INFO( "running Spoof %s with timeout=%d after %s until %s with parameters %s and %s", 
                  spoofName, timeout, substate_or_msg, endstate_or_msg, Var1, Var2 );
        LOG_INFO( "%s", comment );

        DataLog( log_level_spoofer_info ) << "running " << spoofName
            << " with timeout=" << timeout << " after " << substate_or_msg 
            << " until " << endstate_or_msg << " with parameters " << Var1
            << " and " << Var2 << endmsg;

        int SS; 
        if (isSS)  // trigger on a substate transition
        {
           int theSS;  // enum
           int* buff = &theSS;
           int bufsize = sizeof(int);
           if (!(timeFromActive > 0)) { api().setTimeout( timeout ); }
           if (api().waitForSubstate(substate_or_msg, buff, bufsize, timeout * 1000 ) )
           {
               LOG_DEBUG( "ProcSubstateChangeMsg data is %d", theSS );
               if ( delta > 0 ) 
               {
                   api().setTimeout ( delta );
                   api().waitForTimeout();
               }
               if ((timeFromActive > 0)) {api().setTimeout( timeFromActive ); }
               _allSpoofs[ spoofName ]->run_generic(Var1, Var2);
              // now wait for the endSubstate or timeout
               if (api().waitForSubstate(endstate_or_msg, buff, bufsize, timeout * 1000) )
               {
                   LOG_DEBUG( "received end substate/state" ); 
               }
               else
               {
                   LOG_DEBUG("timed out without getting the end substate/state" );
               }
           }
           else {LOG_DEBUG( "timed out waiting for transition to %s to start spoof", substate_or_msg ); }         
        }
        else    // trigger on a message
        {
          // for new assume will only trigger on integer messages
           if (!timeFromActive) { api().setTimeout ( timeout ); }
           if (api().getData(substate_or_msg, "data", &SS, 4, timeout * 1000) )
           {
               if ( delta > 0 ) 
               {
                   api().setTimeout ( delta );
                   api().waitForTimeout();
               }
               if ((timeFromActive > 0)) api().setTimeout( timeFromActive );
               _allSpoofs[ spoofName ]->run_generic(Var1, Var2);
               // now wait for end message trigger or timeout
               if (api().getData(endstate_or_msg, "data", &SS, 4, timeout * 1000) )
               {
                   LOG_DEBUG( "received end message" );
               }
               else
               {
                   LOG_DEBUG( "timed out without receiving end message" );
               }
 
           }
           else {LOG_DEBUG( "timed out without receiving trigger message to start spoof" ); }
       }

    }

}
