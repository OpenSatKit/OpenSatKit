/*************************************************************************
** File:
**   $Id: mqtt_lib.c $
**
** Purpose: 
**   MQTT CFS library
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "mqtt_lib.h"
#include "mqtt_lib_version.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/


/*************************************************************************
** Private Function Prototypes
*************************************************************************/
int32 MQTT_LibInit(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 MQTT_LibInit(void)
{
    
    OS_printf ("MQTT Lib Initialized.  Version %d.%d.%d.%d\n",
                MQTT_LIB_MAJOR_VERSION,
                MQTT_LIB_MINOR_VERSION, 
                MQTT_LIB_REVISION, 
                MQTT_LIB_MISSION_REV);
                
    return CFE_SUCCESS;
 
}/* End MQTT_LibInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MQTT Lib function                                             */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 MQTT_LibFunction( void ) 
{
   OS_printf ("MQTT_LibFunction called\n");

   return(CFE_SUCCESS);
   
} /* End MQTT_Function */

/************************/
/*  End of File Comment */
/************************/
