/*************************************************************************
** File: mqtt_lib.h
**
** Purpose: 
**   Specification for the sample library functions.
**
*************************************************************************/
#ifndef _mqtt_lib_h_
#define _mqtt_lib_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/* include the Paho Embedded C MQTT library header */
#include "MQTTClient.h"

/* include the JSMN JSON tokenizer */
#include "jsmn.h"

/************************************************************************
** Type Definitions
*************************************************************************/

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief MQTT Lib Function 
**  
**  \par Description
**        This is a sample function
**
**  \par Assumptions, External Events, and Notes:
**        None
**       
**  \returns
**  \retstmt Returns #CFE_SUCCESS \endcode
**  \endreturns
** 
*************************************************************************/
int32 MQTT_LibFunction( void ); 

/*
** Types of functions needed
*/

/*
** I'm inclined to just use the Paho embedded library right now
** Use and get working, then maybe abstract later.
** Just need to add the include path to CMAKE
** Or copy headers to public inc??
*/

/* Network Init 
** Call the NetworkInit and NetworkConnect functions 
** Need to pass in the port and IP addr 
** This seems easy enough..
**
*/

/*
** Abstracted params needed:
**  IP Addr,
**  Port
**  read and write buffer sizes? ( or buffers themselves?) 
** 
**  Options:
**    Client ID, Uname/password, etc
** 
**  Subscribe ( topic, QOS, callback function )
**
**  Yield function (timeout)
**
**  Disconnect MQTT Client call
**  
**  Network disconnect call
**   
*/

/*
** MQTT Client init 
** Needs a MQTT client structure
** passes in read and write buffers and sizes 
*/


/* 
** Set options
** MQTT Version
** Client ID
** Username and password
** Keep alive interval
** Will flag
**
*/

/*
** MQTT Client Connect
**
** with init client and data(options)
*/

/*
** Subscribe to topics, passing in the 
** Topic, QOS, and pointer to message arrive callback 
**/

/*
** Need a yield call
*/

/*
** MQTT Disconnect call ( with client pointer )
*/

/*
** Network Disconnect call ( with network option )
*/


/*
** It might make sense just to use the actual calls and data types?
** 
** MQTTLinux.h has:
**  Network object
**  NetworkInit function
**  NetworkConnect function
**  Network disconnect function
** ( also timer functions )

**
**  MQTTClient.h has:
**  MQTTClient structure
**  MQTTClientInit function
**  MQTTConnect function(s)
**  MQTTPublish function
**  MQTTSetMessageHandler ( for topics)
**  MQTTSubscribe/Unscubscribe
**  MQTTDisconnect
**  MQTTYield
**  MQTTIsConnected
**
**  If MQTT_TASK is defined 
**   define MQTTStartTask
**   this starts a task, so the Yield would not be called
**
**
**  Should I try to  define an abstracted interface 
**   Or just expose the MQTT client interface ?
**   It would depend on this particular implementation
**  OTOH, I could define a minimal interface that could be used with 
**  a different abstraction.  
*/  


#endif /* _mqtt_lib_h_ */

/************************/
/*  End of File Comment */
/************************/
