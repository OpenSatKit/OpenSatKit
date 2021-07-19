/*
** Purpose: Provide a table for identifying onboard file systems and for
**          managing commands that obtain information about the file systems. 
**
** Notes:
**   1. Refactored from NASA's FM FreeSpace table. I renamed to FileSys 
**      because "free space" is an attrbute of a file system volume.
**   2. The original design doesn't have concepts such as File and Dir
**      objects but it did separate table from non-table functions. This
**      design includes file system functions like "OpenFileListToPkt"
**      because it is not operating on a File object. 
**   3. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      Note the cFE's buffers are used to store the actual data itself.
**      This is a table-specific file so it doesn't need to be re-entrant.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _filesys_
#define _filesys_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/


#define FILESYS_TBL_ENTRY_DISABLED     0
#define FILESYS_TBL_ENTRY_ENABLED      1
#define FILESYS_TBL_ENTRY_UNUSED       2


#define FILESYS_TBL_REGISTER_ERR_EID         (FILESYS_BASE_EID + 0)
#define FILESYS_TBL_VERIFY_ERR_EID           (FILESYS_BASE_EID + 1)
#define FILESYS_TBL_VERIFIED_EID             (FILESYS_BASE_EID + 2)
#define FILESYS_SEND_PKT_ERR_EID             (FILESYS_BASE_EID + 3)
#define FILESYS_SEND_PKT_CMD_EID             (FILESYS_BASE_EID + 4)
#define FILESYS_SET_TBL_STATE_LOAD_ERR_EID   (FILESYS_BASE_EID + 5)
#define FILESYS_SET_TBL_STATE_ARG_ERR_EID    (FILESYS_BASE_EID + 5)
#define FILESYS_SET_TBL_STATE_UNUSED_ERR_EID (FILESYS_BASE_EID + 6)
#define FILESYS_SET_TBL_STATE_CMD_EID        (FILESYS_BASE_EID + 7)
#define FILESYS_SEND_OPEN_FILES_CMD_EID      (FILESYS_BASE_EID + 8)


/**********************/
/** Type Definitions **/
/**********************/



/******************************************************************************
** Command Packets
*/

typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} FILESYS_SendTblPktCmdMsg;
#define FILESYS_SEND_TBL_PKT_CMD_DATA_LEN  (sizeof(FILESYS_SendTblPktCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32  TblVolumeIndex;
    uint32  TblVolumeState;

} FILESYS_SetTblStateCmdMsg;
#define FILESYS_SET_TBL_STATE_CMD_DATA_LEN  (sizeof(FILESYS_SetTblStateCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} FILESYS_SendOpenFilesPktCmdMsg;
#define FILESYS_SEND_OPEN_FILES_PKT_CMD_DATA_LEN  (sizeof(FILESYS_SendOpenFilesPktCmdMsg) - CFE_SB_CMD_HDR_SIZE)



/******************************************************************************
** Telemetry Packets
*/


/* Telemetry packet sent in response to FILESYS_SendTblPktCmd */

typedef struct {

    uint32  FreeSpace_A;             /* First 32 bit portion of a 64 bit value  */
    uint32  FreeSpace_B;             /* Second 32 bit portion of a 64 bit value */
    char    Name[OS_MAX_PATH_LEN];

} FILESYS_PktVolume;


typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];

    FILESYS_PktVolume  Volume[FILEMGR_FILESYS_TBL_VOL_CNT];
                                                 
} FILESYS_TblPkt;


/*
** Packet sent in response to FILESYS_SendOpenFilesPktCmd 
** - Little risky basing telemetry packet structure on a utility's
**   structure definition but regression testing should catch it if the
**   structure changes. 
*/

typedef struct {

   uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];

   FileUtil_OpenFileList  List;
   
} FILESYS_OpenFilesPkt;


/******************************************************************************
** Table Struture
*/

typedef struct {
   
   uint32  State; 
   char    Name[OS_MAX_PATH_LEN];

} FILESYS_Volume;


typedef struct {
   
   FILESYS_Volume Volume[FILEMGR_FILESYS_TBL_VOL_CNT];

} FILESYS_TblData;


/******************************************************************************
** Class Struture
*/

typedef struct {

   boolean           Registered;
   int32             Status;        /* Status of last cFE Table service call */
   CFE_TBL_Handle_t  Handle;
   FILESYS_TblData*  DataPtr;

} FILESYS_CfeTbl;

typedef struct {

   /*
   ** App Framework
   */
   
   INITBL_Class*  IniTbl;

   /*
   ** Tables
   */
   
   FILESYS_CfeTbl  CfeTbl;

   /*
   ** Telemetry
   */
   
   FILESYS_TblPkt        TblPkt;
   FILESYS_OpenFilesPkt  OpenFilesPkt;

   /*
   ** Class State Data
   */

   const char* CfeTblName;
   FileUtil_OpenFileList OpenFileList;
   

} FILESYS_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FILESYS_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   None
*/
void FILESYS_Constructor(FILESYS_Class* FileSysPtr, INITBL_Class* IniTbl);


/******************************************************************************
** Function: FILESYS_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void FILESYS_ResetStatus(void);


/******************************************************************************
** Function: FILESYS_ManageTbl
**
** Manage the cFE table interface for table loads and validation. 
*/
void FILESYS_ManageTbl(void);


/******************************************************************************
** Function: FILESYS_SendOpenFilesPktCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean FILESYS_SendOpenFilesPktCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILESYS_SendTblPktCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean FILESYS_SendTblPktCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILESYS_SetTblStateCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean FILESYS_SetTblStateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _filesys_ */
