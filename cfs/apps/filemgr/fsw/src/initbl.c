/*
** Purpose: KIT_TO Packet Table.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "initbl.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define  JSON  &(IniTbl->Json)   /* Convenience macro */


/**********************/
/** Global File Data **/
/**********************/

DEFINE_ENUM(Config,APP_CONFIG)  /* Must match DECLARE ENUM() declaration in initbl.h */

INITBL_Class *IniTbl;

/************************************/
/** Local File Function Prototypes **/
/************************************/


/******************************************************************************
** Function: LoadConfigurations
**
** Notes:
**   1. Load configurations defined in the app ini file.
*/
static boolean LoadConfigurations(void);


/******************************************************************************
** Function: ConfigCallback
**
** Notes:
**   1. A single JSON structure defines all of the initialization 
**      configurations that are processed by this function
*/
static boolean ConfigCallback (int TokenIdx);


/******************************************************************************
** Function: INITBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
boolean INITBL_Constructor(INITBL_Class* IniTblPtr)
{
   
   IniTbl = IniTblPtr;

   CFE_PSP_MemSet(IniTbl, 0, sizeof(INITBL_Class));

   JSON_Constructor(JSON, IniTbl->JsonFileBuf, IniTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(IniTbl->JsonObj[INITBL_OBJ_CONFIG]),
                       INITBL_OBJ_CONFIG_NAME,
                       ConfigCallback,
                       (void *)&(IniTbl->Config));
   
   JSON_RegContainerCallback(JSON, &(IniTbl->JsonObj[INITBL_OBJ_CONFIG]));

   return LoadConfigurations();
   
} /* End INITBL_Constructor() */


/******************************************************************************
** Function: LoadConfigurations
**
** Notes:
*/
static boolean LoadConfigurations(void)
{

   boolean RetStatus = FALSE;
   
   CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "Enter LoadConfigurations()");
   
   if (JSON_OpenFile(JSON, FILEMGR_INI_FILE_NAME)) {
  
      CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() Successfully prepared JSON file %s", FILEMGR_INI_FILE_NAME);

      JSON_ProcessTokens(JSON);

      if (IniTbl->AttrErrCnt == 0) {
      
         CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() Successfully initialized %d configurations", 
                           IniTbl->JsonVarCnt);
         RetStatus = TRUE;
         
      } /* End if no attribute errors */
      else {
         
         CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() failed with %d JSON sttribute errors",
                           IniTbl->AttrErrCnt);
         
      }
      
   } /* End if valid file */
   else {
      
      CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() Error preparing JSON file %s\n", FILEMGR_INI_FILE_NAME);

   } /* End if file processing error */

    
   return RetStatus;

} /* End of LoadConfigurations() */


/******************************************************************************
** Function: ConfigCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/

#define CFG_TBL_STRUCT(item) IniTbl->Config.Item.item

static boolean ConfigCallback (int TokenIdx)
{

   int   JsonIntData;
   char  JsonStrData[OS_MAX_PATH_LEN];
   char  *StrBuf;
   boolean RetStatus = FALSE;      

   IniTbl->JsonVarCnt = 0;
  
   if (JSON_GetValShortInt(JSON, TokenIdx, GetConfigStr(CFG_CMD_PIPE_DEPTH), &JsonIntData)) {
      ++IniTbl->JsonVarCnt; 
      CFG_TBL_STRUCT(CFG_CMD_PIPE_DEPTH) = (uint16) JsonIntData;
      CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "CMD_PIPE_DEPTH =%d", IniTbl->Config.Item.CMD_PIPE_DEPTH);
   }   
   
   if (JSON_GetValStr(JSON, TokenIdx, GetConfigStr(CFG_CMD_PIPE_NAME), JsonStrData)) { 
      ++IniTbl->JsonVarCnt;
      StrBuf = IniTbl->Config.Str[IniTbl->Config.StrIndex].Buf;
      strcpy(StrBuf,JsonStrData);
      CFG_TBL_STRUCT(CFG_CMD_PIPE_NAME) = StrBuf;
      CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "CMD_PIPE_NAME = %s at StrIndex %d", StrBuf, IniTbl->Config.StrIndex);
      ++IniTbl->Config.StrIndex;
   }
   
   CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "ConfigCallback IniTbl->JsonVarCnt=%d", IniTbl->JsonVarCnt);
   
   if (IniTbl->JsonVarCnt == 2) {
   
      RetStatus = TRUE;
         
   } /* End if valid JsonVarCnt */
   else {
	   
      ++IniTbl->AttrErrCnt;     
      CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 2.",
                        IniTbl->JsonVarCnt);
   
   } /* End if invalid JsonVarCnt */
      
   return RetStatus;

} /* ConfigCallback() */


