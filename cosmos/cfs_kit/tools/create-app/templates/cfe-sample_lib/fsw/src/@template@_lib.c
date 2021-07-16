/*************************************************************************
** File:
**   $Id: @template@_lib.c $
**
** Purpose: 
**   @Template@ CFS library
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "@template@_lib.h"
#include "@template@_lib_version.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/


/*************************************************************************
** Private Function Prototypes
*************************************************************************/
int32 @TEMPLATE@_LibInit(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 @TEMPLATE@_LibInit(void)
{
    
    OS_printf ("@TEMPLATE@ Lib Initialized.  Version %d.%d.%d.%d\n",
                @TEMPLATE@_LIB_MAJOR_VERSION,
                @TEMPLATE@_LIB_MINOR_VERSION, 
                @TEMPLATE@_LIB_REVISION, 
                @TEMPLATE@_LIB_MISSION_REV);
                
    return CFE_SUCCESS;
 
}/* End @TEMPLATE@_LibInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @Template@ Lib function                                             */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 @TEMPLATE@_Function( void ) 
{
   OS_printf ("@TEMPLATE@_Function called\n");

   return(CFE_SUCCESS);
   
} /* End @TEMPLATE@_Function */

/************************/
/*  End of File Comment */
/************************/
