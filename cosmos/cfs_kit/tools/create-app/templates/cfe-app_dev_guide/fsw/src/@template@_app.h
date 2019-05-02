/*******************************************************************************
** File: @template@_app.h
**
** Purpose:
**   This file is main hdr file for the @TEMPLATE@ application.
**
**
*******************************************************************************/

#ifndef _@template@_app_h_
#define _@template@_app_h_

/*
**   Include Files:
*/

#include "cfe.h"
#include "@template@_tbldefs.h"


/*
** Event message ID's.
*/
#define @TEMPLATE@_INIT_INF_EID      1    /* start up message "informational" */

#define @TEMPLATE@_NOOP_INF_EID      2    /* processed command "informational" */
#define @TEMPLATE@_RESET_INF_EID     3
#define @TEMPLATE@_PROCESS_INF_EID   4
  
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
#define @TEMPLATE@_PROCESS_CC        2    /* Perform Routine Processing */

/*
** Table defines
*/
#define @TEMPLATE@_NUM_TABLES        2    /* Number of Tables */

#define @TEMPLATE@_FIRST_TBL_FILE  "/cf/@template@_tbl_1.tbl"
#define @TEMPLATE@_SECOND_TBL_FILE "/cf/@template@_tbl_2.tbl"

#define @TEMPLATE@_TBL_1_ELEMENT_OUT_OF_RANGE_ERR_CODE    1  
#define @TEMPLATE@_TBL_2_ELEMENT_OUT_OF_RANGE_ERR_CODE   -1

#define @TEMPLATE@_TBL_ELEMENT_1_MAX  10  
#define @TEMPLATE@_TBL_ELEMENT_3_MAX  30  

/*
** Software Bus defines
*/
#define @TEMPLATE@_PIPE_DEPTH        12   /* Depth of the Command Pipe for Application */
#define @TEMPLATE@_LIMIT_HK          2    /* Limit of HouseKeeping Requests on Pipe for Application */
#define @TEMPLATE@_LIMIT_CMD         4    /* Limit of Commands on pipe for Application */

/*
** @TEMPLATE@ Critical Data Store defines
*/
#define @TEMPLATE@_CDS_NAME            "@TEMPLATE@CDS"


/*
** Type definition (Critical Data Store data)
*/
typedef struct
{
  uint32  DataPtOne;    /* Values stored in my CDS */
  uint32  DataPtTwo;
  uint32  DataPtThree;
  uint32  DataPtFour;
  uint32  DataPtFive;

} @TEMPLATE@_CdsDataType_t;


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
  ** Critical Data store variables
  */
  @TEMPLATE@_CdsDataType_t      WorkingCriticalData; /* Define a copy of critical data that can be used during Application execution */
  CFE_ES_CDSHandle_t    CDSHandle;           /* Handle to CDS memory block */

  /*
  ** Initialization data (not reported in housekeeping)
  */
  char                  PipeName[16];
  uint16                PipeDepth;

  uint8                 LimitHK;
  uint8                 LimitCmd;

  CFE_EVS_BinFilter_t   EventFilters[@TEMPLATE@_EVT_COUNT];
  CFE_TBL_Handle_t      TblHandles[@TEMPLATE@_NUM_TABLES];

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

int32   @TEMPLATE@_FirstTblValidationFunc(void *TblData);
int32   @TEMPLATE@_SecondTblValidationFunc(void *TblData);


#endif /* _@template@_app_h_ */

/************************/
/*  End of File Comment */
/************************/



