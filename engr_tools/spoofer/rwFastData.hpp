
#ifndef MSC_SPOOFEDMSG_FASTDATA_HPP
#define MSC_SPOOFEDMSG_FASTDATA_HPP

#include "spoofedobject.hpp"

#include <message.h>				// Trima Message System
#include <spoofermessagesystem.h>

#include <fastdata.h>               // Used to access FASTDATA

class RW_FastData : public SpoofedObject {

	public :
		RW_FastData( ):
			SpoofedObject( "RW_FastData" ){ 
			
			_msg.init( Callback< RW_FastData >( this, &RW_FastData::notify ),
				   MessageBase::SNDRCV_GLOBAL );

			((SpooferMessageSystem*)MessageSystem::MsgSystem())->spoofMessage( _msg, 
				Callback< RW_FastData >( this, &RW_FastData::spoof ) );
		
			}
		virtual ~RW_FastData( ){ }

		// blocking callback for spoofing an unspoofed msg
		virtual void spoof( ){ 
			FASTDATA sData = _msg.getData( );

			// Poison the Data here in sData.raw
			memset( sData.raw, 0, FASTDATALENGTH * sizeof(short) );
			sData.raw[0] = 1000;

			// Pass (send()) on the poison data
			_msg.send( sData );
		} 

/*{{{*/
        // blocking callback for receiving spoofed msg
		virtual void notify( )
		{  
			//DataLog (log_level_datalog_info) << "In function: notify" << endmsg;
			//assert ( 0 ) ;  
		}

		// sends a spoofed msg, with fields set as specified
        virtual void send( )
		{  
			//DataLog (log_level_datalog_info) << "In function: send" << endmsg;
			assert ( 0 ) ;  
		}

		// returns ptr to spoofed data to send
        virtual char* spoofedBuf()
		{  
			//DataLog (log_level_datalog_info) << "In function: spoofedBuf" << endmsg;
			assert ( 0 ) ; 
			return ( NULL ); 
		}
        // returns ptr to original data
		virtual char* originalBuf() 
		{
			//DataLog (log_level_datalog_info) << "In function: originalBuf" << endmsg;
			assert ( 0 ) ; 
			return ( NULL ); 
		} 
/*}}}*/

	private :
		Message< FASTDATA >		_msg;
};

#endif // MSC_SPOOFEDMSG_FASTDATA_HPP

