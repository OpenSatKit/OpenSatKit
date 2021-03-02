/************************************************************************
** File: md_app.h 
**
** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
** Memory Dwell Application Version 2.3.3” 
**
** Copyright © 2019 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
*
** Purpose:
**   Unit specification for the Core Flight System (CFS) 
**   Memory Dwell (MD) Application.  
**
** Notes:
**
**
*************************************************************************/

#ifndef _md_app_h_
#define _md_app_h_

/************************************************************************
** Includes
*************************************************************************/
#include "common_types.h"
#include "md_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "md_msgids.h"
#include "cfs_utils.h"
#include "md_msg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/

#define MD_DWELL_TABLE_BASENAME  "DWELL_TABLE"

/** 
**  \name   Function Return Codes for Table Validation function and related routines */
/** \{ */

#define MD_ERROR                (-1)         /**< \brief Generic error value */

#define MD_TBL_ENA_FLAG_ERROR   (0xc0000001) /**< \brief Enable flag in table load is invalid (valid values are 0 and 1) */

#define MD_ZERO_RATE_TBL_ERROR  (0xc0000002) /**< \brief Table has zero value for total delay, and at least one dwell specified */

#define MD_RESOLVE_ERROR        (0xc0000003) /**< \brief Symbolic address couldn't be resolved */ 

#define MD_INVALID_ADDR_ERROR   (0xc0000004) /**< \brief Invalid address found */

#define MD_INVALID_LEN_ERROR    (0xc0000005) /**< \brief Invalid dwell length found */

#define MD_NOT_ALIGNED_ERROR    (0xc0000006) /**< \brief Dwell address improperly aligned for specified dwell length */

#define MD_SIG_LEN_TBL_ERROR    (0xc0000007) /**< \brief Signature not null terminated in table */

/** \} */

/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief  MD enum used for representing values for enable state
*/
enum MD_Dwell_States {MD_DWELL_STREAM_DISABLED, MD_DWELL_STREAM_ENABLED};

/** 
**  \brief MD structure for specifying individual memory dwell
*/
typedef struct
{
        uint16 Length;          /**< \brief  Length of dwell  field in bytes.  0 indicates null entry. */
        uint16 Delay;           /**< \brief  Delay before following dwell sample in terms of number of task wakeup calls */
        cpuaddr ResolvedAddress; /**< \brief Dwell address in numerical form */
} MD_DwellControlEntry_t;

/** 
**  \brief MD structure for controlling dwell operations
*/
typedef struct
{
    uint16 Enabled;         /**< \brief Is table is enabled for dwell?  Valid values are #MD_DWELL_STREAM_DISABLED, #MD_DWELL_STREAM_ENABLED */
    uint16 AddrCount;       /**< \brief Number of dwell addresses to telemeter  */
    uint32 Rate;            /**< \brief Packet issuance interval in terms of number of task wakeup calls */
    uint32 Countdown;       /**< \brief Counts down from Rate to 0, then read next address */
    uint16 PktOffset;       /**< \brief Tracks where to write next data in dwell pkt */
    uint16 CurrentEntry;    /**< \brief Current entry in dwell table */
    uint16 DataSize;        /**< \brief Total number of data bytes specified in dwell table */
    uint16 Filler;          /**< \brief Preserves alignment */
    MD_DwellControlEntry_t Entry[MD_DWELL_TABLE_SIZE];  /**< \brief Array of individual memory dwell specifications */
#if MD_SIGNATURE_OPTION == 1   
    char Signature[MD_SIGNATURE_FIELD_LENGTH];          /**< \brief Signature string used for dwell table to dwell pkt traceability */
#endif
} MD_DwellPacketControl_t;


/** 
**  \brief MD global data structure
*/
typedef struct
{
    /*
    **  Command interface counters
    */
    uint8                       CmdCounter;     /**< \brief MD Application Command Counter */
    uint8                       ErrCounter;     /**< \brief MD Application Error Counter */
    
    /* 
    **  Housekeeping telemetry packet
    */
    MD_HkTlm_t                  HkPkt;          /**< \brief Housekeeping telemetry packet */
    
    /*  
    **  Operational data (not reported in housekeeping)
    */
    CFE_SB_MsgPtr_t             MsgPtr;            /**< \brief Pointer to command message    */
    CFE_SB_PipeId_t             CmdPipe;           /**< \brief Command pipe ID               */
    MD_DwellPacketControl_t     MD_DwellTables[MD_NUM_DWELL_TABLES]; /**< \brief Array of packet control structures    */
    MD_DwellPkt_t               MD_DwellPkt[MD_NUM_DWELL_TABLES];    /**< \brief Array of dwell packet  structures    */
    
    /*
    ** RunStatus variable used in the main processing loop
    */
    uint32                      RunStatus;         /**< \brief Application run status         */

    /*
    **  Initialization data (not reported in housekeeping)
    */

    char                        MD_TableName[MD_NUM_DWELL_TABLES][CFE_MISSION_TBL_MAX_NAME_LENGTH + 1]; /**< \brief Array of table names used for TBL Services */
    CFE_TBL_Handle_t            MD_TableHandle[ MD_NUM_DWELL_TABLES];  /**< \brief Array of handle ids provided by TBL Services  */

} MD_AppData_t;

/* Return values for MD_SearchCmdHndlrTbl routine used to process commands */
#define MD_BAD_CMD_CODE (-1)
#define MD_BAD_MSG_ID   (-2)

typedef enum
{
    MD_TERM_MSGTYPE=0,   /**< \brief Command Handler Table Terminator Type */
    MD_MSG_MSGTYPE,      /**< \brief Message Type (requires Message ID match) */
    MD_CMD_MSGTYPE       /**< \brief Command Type (requires Message ID and Command Code match) */
} MD_MsgType_t;

/*
** Data structure of a single record in MD_CmdHandlerTbl
*/
typedef struct 
{
    uint32        MsgId;           /**< \brief Acceptable Message ID */
    uint32        CmdCode;         /**< \brief Acceptable Command Code (if necessary) */
    uint32        ExpectedLength;  /**< \brief Expected Message Length (in bytes) including message header */
    MD_MsgType_t  MsgTypes;        /**< \brief Message Type (i.e. - with/without Cmd Code)   */
} MD_CmdHandlerTblRec_t;


/*
**
**  Memory Dwell Task Globals
**
*/

extern MD_AppData_t MD_AppData;

/************************************************************************
** Exported Functions
*************************************************************************/
/*****************************************************************************/
/**
** \brief Entry Point and main loop for the Memory Dwell task.
**
** \par Description
**          Call MD_AppInit to initialize the task. 
**          LOOP:
**            Copy any newly loaded tables
**            Pend on the Software Bus waiting to receive next message.
**            If MD_WAKEUP_MID Message is received, call MD_DwellLoop 
**               to send whatever memory values are being 'dwelled on'. 
**            If MD_CMD_MID Message is received, call MD_ExecRequest
**               for processing.
**            If MD_SEND_HK_MID Message is received, call MD_HkStatus
**               for processing.
**
** \par Assumptions, External Events, and Notes:
**          Associated with each dwell address is a 'Delay' which is the 
**          number of wake-up calls to wait before recording the next value 
**          for this Dwell Table.  The 'Rate' value associated with
**          Dwell Table is the sum of all individual delays.  
**          For a table to be dwelled on, its rate must be >=1.
**
** \retval None
******************************************************************************/
void MD_AppMain( void );

/*****************************************************************************/
/**
** \brief Memory Dwell Application Initialization
**
** \par Description
**  Initialize all data elements.
**  If this is a PROCESSOR_RESET, and saving dwell tables to CDS is indicated,
**  then restore image from CDS.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS  \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_ES_RegisterApp
** \return Any of the return values from #CFE_EVS_Register
** \return Any of the return values from #CFE_SB_CreatePipe
** \return Any of the return values from #CFE_SB_Subscribe
** \return Any of the return values from #CFE_TBL_Register
** \return Any of the return values from #CFE_TBL_Load
******************************************************************************/
int32 MD_AppInit( void );

/*****************************************************************************/
/**
** \brief Initialize local control structures
**
** \par Description
**  Initialize control structures for each of the #MD_NUM_DWELL_TABLES dwell streams.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \return None
******************************************************************************/
void MD_InitControlStructures(void);

/*****************************************************************************/
/**
** \brief Initialize Software Bus Services for the Memory Dwell Task
**
** \par Description
**  Create message pipe.
**  Subscribe to all input and output messages.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS  \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_SB_CreatePipe
** \return Any of the return values from #CFE_SB_Subscribe
******************************************************************************/
int32 MD_InitSoftwareBusServices( void );

/*****************************************************************************/
/**
** \brief Initialize Table Services for the Memory Dwell Task
**
** \par Description
**  Register Tables with Table Services.
**  Check for Recovered Tables.
**  IF Recovered Tables Found
**  THEN
**       Retrieve them.
**       Validate them.
**       IF Recovered Tables are Invalid
**          Initialize Tables
**       ENDIF
**  ELSE
**       Initialize Tables.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS  \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_TBL_Register
** \return Any of the return values from #CFE_TBL_Load
******************************************************************************/
int32 MD_InitTableServices( void );


                
                
/*****************************************************************************/
/**
** \brief Check Table Status and take appropriate actions.
**
** \par Description
**       Checks status of Tables, and takes action if
**       validation requests or update requests are pending.
** 
** \par Assumptions, External Events, and Notes:
**          Assumes that an update is pending for the specified table.
**
** \param[in] TblIndex   Dwell table identifier. 
**                       Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
**                                      
** \returns
** \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS              \endcode
**  May return any value from CFE_TBL_GetStatus, 
**  CFE_TBL_Validate, or CFE_TBL_Update.
** \endreturns

******************************************************************************/
int32 MD_ManageDwellTable (uint8 TblIndex);



/*****************************************************************************/
/**
** \brief Execute requested Memory Dwell commands
**
** \par Description
**          Processes messages obtained from the command pipe.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ExecRequest(CFE_SB_MsgPtr_t MessagePtr);



/*****************************************************************************/
/**
** \brief Send Housekeeping Status to Health & Safety task
**
** \par Description
**          For each dwell table the housekeeping data includes:
**          number of dwell addresses, number of counts for packet formation,
**          data size in bytes, current entry in data processing, current
**          offset in packet data field, countdown to next data collection.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
void MD_HkStatus( void );




/* Utility Functions */
/*****************************************************************************/
/**
** \brief Compares message with MD_CmdHandlerTbl to identify the message
**
** \par Description
**          Searches the Command Handler Table for an entry matching the
**          message ID and, if necessary, the Command Code.  If an entry
**          is not located, an error code is returned.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MessageID message ID of command message received on command pipe
**
** \param[in] CommandCode command code from command message received on command pipe
**                                      
** \returns
** \retstmt On success, a non-negative Table Index is returned.   \endcode
** \retcode #MD_BAD_CMD_CODE \copydoc MD_BAD_CMD_CODE            \endcode
** \retcode #MD_BAD_MSG_ID   \copydoc MD_BAD_MSG_ID              \endcode
** \endreturns
**
******************************************************************************/

int16 MD_SearchCmdHndlrTbl(CFE_SB_MsgId_t MessageID, uint16 CommandCode);

/******************************************************************************/

#endif /* _md_app_h_ */
/************************/
/*  End of File Comment */
/************************/
