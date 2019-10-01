/*******************************************************************************
** File: @template@_app.h
**
** Purpose:
**   Provide definitions for the @TEMPLATE@ application used in cFE training.
**
**
*******************************************************************************/

#ifndef _@template@_app_h_
#define _@template@_app_h_

/*
**   Include Files:
*/

#include "cfe.h"


/*
** Event message ID's.
*/
#define @TEMPLATE@_INIT_INF_EID      1    /* start up message "informational" */

#define @TEMPLATE@_NOOP_INF_EID      2    /* processed command "informational" */
#define @TEMPLATE@_RESET_INF_EID     3
//Ex#1 #define @TEMPLATE@_NEW_CMD_INF_EID   4
  
#define @TEMPLATE@_MID_ERR_EID       5    /* invalid command packet "error" */
#define @TEMPLATE@_CC1_ERR_EID       6
#define @TEMPLATE@_LEN_ERR_EID       7
#define @TEMPLATE@_PIPE_ERR_EID      8

#define @TEMPLATE@_EVT_COUNT         8    /* count of event message ID's */

/*
** Command packet command codes
*/
#define @TEMPLATE@_NOOP_CC           0    /* no-op command */
#define @TEMPLATE@_RESET_CC          1    /* reset counters */
//Ex#1 #define @TEMPLATE@_NEW_CMD_CC        2    /* Perform Routine Processing */

/*
** Software Bus defines
*/
#define @TEMPLATE@_PIPE_DEPTH        12   /* Depth of the Command Pipe for Application */
#define @TEMPLATE@_LIMIT_HK          2    /* Limit of HouseKeeping Requests on Pipe for Application */
#define @TEMPLATE@_LIMIT_CMD         4    /* Limit of Commands on pipe for Application */


/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} @TEMPLATE@_NoArgsCmd_t;

/*************************************************************************/

/*
** Type definition (@TEMPLATE@ housekeeping)
*/
typedef struct
{
  uint8                 TlmHeader[CFE_SB_TLM_HDR_SIZE];

  /*
  ** Command interface counters
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

} OS_PACK @TEMPLATE@_HkPacket_t;

/*************************************************************************/

/*
** Type definition (@TEMPLATE@ app global data)
*/
typedef struct
{
  /*
  ** Command interface counters.
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

  /*
  ** Housekeeping telemetry packet
  */
  @TEMPLATE@_HkPacket_t         HkPacket;

  /*
  ** Operational data (not reported in housekeeping).
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;
  
  /*
  ** RunStatus variable used in the main processing loop
  */
  uint32                RunStatus;


  /*
  ** Initialization data (not reported in housekeeping)
  */
  char                  PipeName[16];
  uint16                PipeDepth;

  uint8                 LimitHK;
  uint8                 LimitCmd;

  CFE_EVS_BinFilter_t   EventFilters[@TEMPLATE@_EVT_COUNT];

} @TEMPLATE@_AppData_t;

/*************************************************************************/

/*
** Local function prototypes
**
** Note: Except for the entry point (@TEMPLATE@_AppMain), these
**       functions are not called from any other source module.
*/
void    @TEMPLATE@_AppMain(void);
int32   @TEMPLATE@_AppInit(void);
void    @TEMPLATE@_AppPipe(CFE_SB_MsgPtr_t msg);

void    @TEMPLATE@_HousekeepingCmd(CFE_SB_MsgPtr_t msg);

void    @TEMPLATE@_NoopCmd(CFE_SB_MsgPtr_t msg);
void    @TEMPLATE@_ResetCmd(CFE_SB_MsgPtr_t msg);
void    @TEMPLATE@_RoutineProcessingCmd(CFE_SB_MsgPtr_t msg);

boolean @TEMPLATE@_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _@template@_app_h_ */

/************************/
/*  End of File Comment */
/************************/



