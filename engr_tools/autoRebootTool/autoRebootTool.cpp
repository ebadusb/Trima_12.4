/*
 *
 * Copyright (c) 2010 CaridianBCT, Inc. All rights reserved.
 * Copyright (c) 2011 CaridianBCT, Inc. All rights reserved.
 *
 */

/*
 *	Temporary hack job written by Michael S. Chaffin 
 *	(Michael.Chaffin@caridianbct.com) in Jan 2011 to reboot a Trima machine 
 *	when it reaches the "Two Button Screen"; or when the 
 *	ConfirmEnableLoadSystemMsg is present.
 *
 *  Modified by Robert Watkins (Robert.Watkins@caridianbct.com) in July 2011 
 *  to also reboot upon receiving an alarm before the "Two Button Screen" 
 *  appears. A Dlog message is than printed saying what alarm was recieved.
*/

#include <taskLib.h>				// Required for taskDelay( )
#include <sysLib.h>					// Required for sysClkRateGet( )

#include <trimamessages.h>			// Used to access the message system
#include <trima_tasks.h>			/* Used to access trimaStartShutdown( ) 
										and TTE_NextProcedure  */
#include <callback.h>				// Access to CallbackBase()::CallbaseBase( )
#include <message.h>
#include <messagesystem.h>			/* The CaridianBCT Message system used in 
											Trima */

#include <datalog.h>				// Add theinterfaces for the datalog system
#include <datalog_levels.h>			// Add handles and levels for logging
#include <alarms.h>
#include <alarmEnums.h>


#ifdef __cplusplus
extern "C" int spoof_server( const char* args );
extern "C" void rebootTheTrima( );
#endif


/* Declare the Message telling us that an alarm screen has popped up. */
Message<GUI_Alarm_struct> _GUIAlarmStructResponseMessage;


/* Recieves a GUI_Alarm_struct message, prints out the name of the alarm, and 
 * then reboots the Trima. */
void handleAlarmMessage()
{
	DataLog (log_level_datalog_info ) 
		<< "handleAlarmMessage( ) : Heard the message 'GUI_Alarm_struct'. ALARM NAME: " 
		<< _GUIAlarmStructResponseMessage.getData().alarm_name 
		<< endmsg;
	trimaStartShutdown(TTE_NextProcedure);
}

/* A placeholder function used to call trimaStartShutdown */
void shutDownTrimaSafe( ){
	
	DataLog( log_level_datalog_info )
		<< "shutDownTrimaSafe( ) : Heard the message 'ConfirmEnableLoadSystemMsg'" 
		<< endmsg ;

	trimaStartShutdown(TTE_NextProcedure);
}

/* Initilizes the MessageSystem and the GUI_Alarm_struct message. Then it 
 * waits for the GUI_Alarm_struct message to be sent. */
int spoof_server(const char* args)
{

	DataLog( log_level_datalog_info )
		<< "spoof_main ( " << args << " ) \n " 
		<< " Reboot a Trima at two (2) button screen \n" 
		<< " Receive (and handle) any alarm message that happens before the \n"
		<< "   two(2) button screen and reboot the Trima." << endmsg;

	/* Message to listen when two button screen is displayed ... safe 
	 * to reboot Trima */
	ConfirmEnableLoadSystemMsg _GUIResponseMessage;

	MessageSystem _MessageSystem ;
	_MessageSystem.initBlocking( "rebootTesting", 30 );

	_GUIResponseMessage.init( 
		CallbackBase( (CallbackBase::FncPtrVoid) &shutDownTrimaSafe ),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);

	// Initialize the GUI_Alarm_struct message
	_GUIAlarmStructResponseMessage.init( 
		CallbackBase((CallbackBase::FncPtrVoid) &handleAlarmMessage),
		MessageBase::SNDRCV_RECEIVE_ONLY
		);

	_MessageSystem.dispatchMessages( );

	DataLog( log_level_datalog_info )
		<< "Normal Exit" << endmsg;

	return 0;
}


