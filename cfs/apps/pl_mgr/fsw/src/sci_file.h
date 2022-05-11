/*
** Purpose: Define the science file management object 
**
** Notes:
**   1. An effort was made to minimize the coupling between this object
**      and the payload. Science files only need to know the data being
**      written. Logic about the payload state and whether or not science
**      files can be created is maintained by this object's owner.  
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _sci_file_
#define _sci_file_

/*
** Includes
*/

#include "app_cfg.h"
#include "pl_sim_lib.h"  /* See prologue notes */

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define SCI_FILE_CREATE_EID      (SCI_FILE_BASE_EID + 0)
#define SCI_FILE_CREATE_ERR_EID  (SCI_FILE_BASE_EID + 1)
#define SCI_FILE_WRITE_ERR_EID   (SCI_FILE_BASE_EID + 2)
#define SCI_FILE_CLOSE_EID       (SCI_FILE_BASE_EID + 3)
#define SCI_FILE_STOP_SCI_EID    (SCI_FILE_BASE_EID + 4)

/**********************/
/** Type Definitions **/
/**********************/

/*
** SCI_FILE_Control_t helps minimize sci_file's coupling to the detector
** details. SCI_FILE_WriteDetectorData() uses the control information
** for creating and closing files. 
*/
typedef enum
{

   SCI_FILE_DISABLED = 1,
   SCI_FILE_ENABLED  = 2

} SCI_FILE_State_t;


typedef enum
{

   SCI_FILE_FIRST_ROW  = 1,
   SCI_FILE_ROW        = 2,
   SCI_FILE_LAST_ROW   = 3,
   SCI_FILE_SHUTDOWN   = 4

} SCI_FILE_Control_t;

typedef struct
{
   uint16  ImagesPerFile;
   char    PathBaseFilename[OS_MAX_PATH_LEN];
   char    FileExtension[SCI_FILE_EXT_MAX_CHAR];

} SCI_FILE_Config_t;


/******************************************************************************
** Command Packets
**
*/

typedef struct
{
   uint8  CmdHeader[CFE_SB_CMD_HDR_SIZE];
   
   SCI_FILE_Config_t  Config;

} SCI_FILE_ConfigCmdMsg_t;
#define SCI_FILE_CONFIG_CMD_DATA_LEN  (sizeof(SCI_FILE_ConfigCmdMsg_t) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/



/******************************************************************************
** SCI_FILE_Class
*/

typedef struct
{

   boolean           CreateNewFile;
   int32             Handle;
   boolean           IsOpen;
   SCI_FILE_State_t  State;
   uint16            ImageCnt;
   char Name[OS_MAX_PATH_LEN];

   SCI_FILE_Config_t Config;

} SCI_FILE_Class_t;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: SCI_FILE_Constructor
**
** Initialize the intrument simulator to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void SCI_FILE_Constructor(SCI_FILE_Class_t *SciFilePtr, INITBL_Class *IniTbl);


/******************************************************************************
** Function: SCI_FILE_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void SCI_FILE_ResetStatus(void);


/******************************************************************************
** Function: SCI_FILE_WriteDetectorData
**
** Write detector data to a file
**
*/
void SCI_FILE_WriteDetectorData(PL_SIM_LIB_Detector_t *Detector, SCI_FILE_Control_t Control);


/******************************************************************************
** Functions: SCI_FILE_ConfigCmd
**
** Set configuration parameters for managing science files 
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCI_FILE_ConfigCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: SCI_FILE_Start
**
** Start the science data collection process 
**
** Notes:
**   None
**
*/
boolean SCI_FILE_Start(void);


/******************************************************************************
** Functions: SCI_FILE_Stop
**
** Stop the science data collection process 
**
** Notes:
**   None
**
*/
boolean SCI_FILE_Stop(char *EventStr, uint16 MaxStrLen);


#endif /* _sci_file_ */
