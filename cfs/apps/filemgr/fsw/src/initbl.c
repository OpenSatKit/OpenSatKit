/*
** Purpose: FileMgr Init Configuration Table.
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

static boolean ConfigCallback (int TokenIdx);
static const char* ConfigTypeStr(INITBL_Type Type);
static boolean LoadConfigurations(const char* IniFile);
static boolean ValidConfigParam(uint16 Param, INITBL_Type Type);

/******************************************************************************
** Function: INITBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
boolean INITBL_Constructor(INITBL_Class* IniTblPtr, const char* IniFile)
{
   
   IniTbl = IniTblPtr;

   CFE_PSP_MemSet(IniTbl, 0, sizeof(INITBL_Class));

   JSON_Constructor(JSON, IniTbl->JsonFileBuf, IniTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(IniTbl->JsonObj[INITBL_OBJ_CONFIG]),
                       INITBL_OBJ_CONFIG_NAME,
                       ConfigCallback,
                       (void *)(IniTbl->CfgItem));
   
   JSON_RegContainerCallback(JSON, &(IniTbl->JsonObj[INITBL_OBJ_CONFIG]));

   return LoadConfigurations(IniFile);
   
} /* End INITBL_Constructor() */


/******************************************************************************
** Function: INITBL_GetIntConfig
**
** Notes:
**    1. This does not return a status as to whether the configuration 
**       parameter was successfully retrieved. The logic for retreiving
**       parameters should be simple and any issues should be resolved during
**       testing.
**    2. If the parameter is out of range or of the wrong type, a zero is
**       returned and an event message is sent.
**
*/
uint32 INITBL_GetIntConfig(uint16 Param)
{
   
   uint32 RetValue = 0;
   
   if (ValidConfigParam(Param,INITBL_UINT32)) {
   
      RetValue = IniTbl->CfgItem[Param].Int;
      
   }
   
   return RetValue;
   
} /* INITBL_GetIntConfig() */


/******************************************************************************
** Function: INITBL_GetStrConfig
**
** Notes:
**    1. This does not return a status as to whether the configuration 
**       parameter was successfully retrieved. The logic for retreiving
**       parameters should be simple and any issues should be resolved during
**       testing.
**    2. If the parameter is out of range or of the wrong type, a null string 
**       is returned and an event message is sent.
**
*/
const char* INITBL_GetStrConfig(uint16 Param)
{
   
   const char*  RetStrPtr = NULL;
   
   if (ValidConfigParam(Param,INITBL_STR)) {
   
      RetStrPtr = IniTbl->CfgItem[Param].Str;
      
   }
   
   return RetStrPtr;
   
} /* INITBL_GetStrConfig() */



/******************************************************************************
** Function: LoadConfigurations
**
** Purpose: Load configurations defined in the app ini file.
**
*/
static boolean LoadConfigurations(const char* IniFile)
{

   boolean RetStatus = FALSE;
   
   CFE_EVS_SendEvent(INILIB_LOAD_CONFIG_EID, CFE_EVS_INFORMATION, "Enter LoadConfigurations()");
   
   if (JSON_OpenFile(JSON, IniFile)) {
  
      CFE_EVS_SendEvent(INILIB_LOAD_CONFIG_EID, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() Successfully prepared JSON file %s", IniFile);

      JSON_ProcessTokens(JSON);

      if (IniTbl->AttrErrCnt == 0) {
      
         CFE_EVS_SendEvent(INILIB_LOAD_CONFIG_EID, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() Successfully initialized %d configurations", 
                           IniTbl->JsonVarCnt);
         RetStatus = TRUE;
         
      } /* End if no attribute errors */
      else {
         
         CFE_EVS_SendEvent(INILIB_LOAD_CONFIG_ERR_EID, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() failed with %d JSON attribute errors",
                           IniTbl->AttrErrCnt);
         
      }
      
   } /* End if valid file */
   else {
      
      CFE_EVS_SendEvent(INILIB_LOAD_CONFIG_ERR_EID, CFE_EVS_INFORMATION, "FileMgr::LoadConfigurations() Error preparing JSON file %s\n", IniFile);

   } /* End if file processing error */

    
   return RetStatus;

} /* End of LoadConfigurations() */


/******************************************************************************
** Function: ConfigTypeStr
**
** Purpose: Return a pointer to a string describing a configuration type
**
** Notes:
**   1. Can't assume type is valid INITBL_Type so perform range checking.
*/
static const char* ConfigTypeStr(INITBL_Type Type)
{
   
   static char* TypeStr[] = {
      "UNDEF",
      "UINT32",
      "STRING"
   };

   uint8 i = 0;
   
   if ( Type == INITBL_UINT32 || Type == INITBL_STR) {
   
      i = Type;
   
   }

   return TypeStr[i];

} /* End ConfigTypeStr() */


/******************************************************************************
** Function: ConfigCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. A single JSON structure defines all of the initialization 
**      configurations that are processed by this function
**
*/
#define CFG_TBL_STRUCT(item) IniTbl->Config.Item.item

static boolean ConfigCallback (int TokenIdx)
{

   int    Param;
   uint32 JsonIntData;
   char   JsonStrData[OS_MAX_PATH_LEN] = "\0";
   const char *CfgStrPtr;
   const char *CfgTypePtr;
   boolean RetStatus = FALSE;      

   IniTbl->JsonVarCnt = 0;
  
   for ( Param=(CFG_ENUM_START+1); Param < CFG_ENUM_END; Param++) {
   
      CfgStrPtr  = GetConfigStr(Param);
      CfgTypePtr = GetConfigType(Param);
            
      if (!strcmp(INILIB_TYPE_INT,CfgTypePtr)) {
      
         if (JSON_GetValUint32(JSON, TokenIdx, CfgStrPtr, &JsonIntData)) {
            ++IniTbl->JsonVarCnt; 
            IniTbl->CfgItem[Param].Initialized = TRUE;
            IniTbl->CfgItem[Param].Type   = INITBL_UINT32;
            IniTbl->CfgItem[Param].Int    = (uint32) JsonIntData;
            IniTbl->CfgItem[Param].Str[0] = '\0';
            CFE_EVS_SendEvent(INILIB_JSON_PARSE_EID, CFE_EVS_DEBUG, "IniTbl->CfgItem[%d].Int=%u, 0x%08X\n", Param, IniTbl->CfgItem[Param].Int, IniTbl->CfgItem[Param].Int);
         }
         
      } /* End if uint32 */
      else {
      
         if (JSON_GetValStr(JSON, TokenIdx, CfgStrPtr, JsonStrData)) { 
            ++IniTbl->JsonVarCnt;
            IniTbl->CfgItem[Param].Initialized = TRUE;
            IniTbl->CfgItem[Param].Type = INITBL_STR;
            IniTbl->CfgItem[Param].Int  = 0;
            strcpy(IniTbl->CfgItem[Param].Str,JsonStrData);
            CFE_EVS_SendEvent(INILIB_JSON_PARSE_EID, CFE_EVS_DEBUG, "IniTbl->CfgItem[%d].Str=%s\n", Param, IniTbl->CfgItem[Param].Str);
         }

      }  /* End if char* */   
   } /* End enum loop */
   
   
   if (IniTbl->JsonVarCnt == (CFG_ENUM_END-1)) {
   
      RetStatus = TRUE;
      CFE_EVS_SendEvent(INILIB_JSON_PARSE_EID, CFE_EVS_INFORMATION, "Successfully configured %d initialization file attributes",
                        IniTbl->JsonVarCnt);
         
   } /* End if valid JsonVarCnt */
   else {
	   
      ++IniTbl->AttrErrCnt;     
      CFE_EVS_SendEvent(INILIB_JSON_PARSE_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be %d.",
                        IniTbl->JsonVarCnt, (CFG_ENUM_END-1));
   
   } /* End if invalid JsonVarCnt */
      
   return RetStatus;

} /* ConfigCallback() */


/******************************************************************************
** Function: ValidConfigParam
**
*/
static boolean ValidConfigParam(uint16 Param, INITBL_Type Type)
{
   
   boolean RetStatus = FALSE;
   
   if ( Param > CFG_ENUM_START && Param < CFG_ENUM_END) {
   
      if (IniTbl->CfgItem[Param].Initialized) {
         if (IniTbl->CfgItem[Param].Type == Type) {
            RetStatus = TRUE;
         }
         else {
            CFE_EVS_SendEvent(INILIB_CFG_PARAM_ERR_EID, CFE_EVS_ERROR, "Attempt to retreive parameter of type %s that was loaded as type %s",
                              ConfigTypeStr(Type),ConfigTypeStr(IniTbl->CfgItem[Param].Type));      
         }
      }
      else {
         CFE_EVS_SendEvent(INILIB_CFG_PARAM_ERR_EID, CFE_EVS_ERROR, "Attempt to retreive uninitialized parameter %d", Param);
      }         
   }
   else {
      CFE_EVS_SendEvent(INILIB_CFG_PARAM_ERR_EID, CFE_EVS_ERROR, "Attempt to retreive invalid parameter %d that is not in valid range: %d < param < %d",
                        Param, CFG_ENUM_START, CFG_ENUM_END);
   }
   
   return RetStatus;
   
} /* ValidConfigParam() */

