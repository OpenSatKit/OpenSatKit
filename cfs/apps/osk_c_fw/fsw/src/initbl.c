/*
** Purpose: Application Init Configuration Table
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


/************************************/
/** Local File Function Prototypes **/
/************************************/

static boolean ConfigCallback (void* UserData, int TokenIdx);  /* Must match JSON_ContainerFuncPtr */
static const char* ConfigTypeStr(INITBL_Type Type);
static boolean LoadConfigurations(INITBL_Class* IniTbl, const char* IniFile);
static boolean ValidConfigParam(INITBL_Class* IniTbl, uint16 Param, INITBL_Type Type);


/******************************************************************************
** Function: INITBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
boolean INITBL_Constructor(INITBL_Class* IniTbl, const char* IniFile,
                           INILIB_CfgEnum* CfgEnum)
{
   
   boolean RetStatus = FALSE;
   
   CFE_PSP_MemSet(IniTbl, 0, sizeof(INITBL_Class));
   IniTbl->CfgEnum = *CfgEnum;
 
   /* Subtract 2 to account for CfgEnum 'start' and 'end' definitions */ 
   if ((IniTbl->CfgEnum.End-2) < INITBL_MAX_CFG_ITEMS) {
   
      JSON_Constructor(JSON, IniTbl->JsonFileBuf, IniTbl->JsonFileTokens);
      
      JSON_ObjConstructor(&(IniTbl->JsonObj[INITBL_OBJ_CONFIG]),
                          INITBL_OBJ_CONFIG_NAME,
                          ConfigCallback,
                          (void *)(IniTbl));
      
      JSON_RegContainerCallback(JSON, &(IniTbl->JsonObj[INITBL_OBJ_CONFIG]));

      RetStatus = LoadConfigurations(IniTbl, IniFile);
   
   }
   else {
   
      CFE_EVS_SendEvent(INITBL_CONFIG_DEF_ERR_EID, CFE_EVS_ERROR, "JSON INITBL definition error. JSON config file contains % d which is greater than frame maximum defined at %d",
                        IniTbl->CfgEnum.End, INITBL_MAX_CFG_ITEMS);
   
   }
   
   return RetStatus;
   
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
uint32 INITBL_GetIntConfig(INITBL_Class* IniTbl, uint16 Param)
{
   
   uint32 RetValue = 0;
   
   if (ValidConfigParam(IniTbl, Param, INITBL_UINT32)) {
   
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
const char* INITBL_GetStrConfig(INITBL_Class* IniTbl, uint16 Param)
{
   
   const char*  RetStrPtr = NULL;
   
   if (ValidConfigParam(IniTbl, Param, INITBL_STR)) {
   
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
static boolean LoadConfigurations(INITBL_Class* IniTbl, const char* IniFile)
{

   boolean RetStatus = FALSE;
    
   if (DBG_INITBL) OS_printf("Enter LoadConfigurations()\n");  
   
   if (JSON_OpenFile(JSON, IniFile)) {
  
      if (DBG_INITBL) OS_printf( "LoadConfigurations() Successfully prepared JSON file %s\n", IniFile);

      JSON_ProcessTokens(JSON);

      if (IniTbl->AttrErrCnt == 0) {
      
         if (DBG_INITBL) OS_printf("LoadConfigurations() Successfully initialized %d configurations\n", 
                                   IniTbl->JsonVarCnt);
         RetStatus = TRUE;
         
      } /* End if no attribute errors */
      else {
         
         CFE_EVS_SendEvent(INITBL_LOAD_CONFIG_ERR_EID, CFE_EVS_ERROR, "LoadConfigurations() failed with %d JSON attribute errors",
                           IniTbl->AttrErrCnt);
         
      }
      
   } /* End if valid file */
   else {
      
      CFE_EVS_SendEvent(INITBL_LOAD_CONFIG_ERR_EID, CFE_EVS_ERROR, "FileMgr::LoadConfigurations() Error preparing JSON file %s\n", IniFile);

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
**   2. JSON_ObjConstructor() must be called with a pointer to an INITBL_Class
**      instance. 
**   3. A single JSON structure defines all of the initialization 
**      configurations that are processed by this function
**
*/
static boolean ConfigCallback (void* UserData, int TokenIdx)
{

   INITBL_Class* IniTbl = (INITBL_Class*)UserData;
   int    Param;
   uint32 JsonIntData;
   char   JsonStrData[OS_MAX_PATH_LEN] = "\0";
   const char *CfgStrPtr;
   const char *CfgTypePtr;
   boolean RetStatus = FALSE;      

   IniTbl->JsonVarCnt = 0;
  
   for ( Param=(IniTbl->CfgEnum.Start+1); Param < IniTbl->CfgEnum.End; Param++) {
   
      CfgStrPtr  = (IniTbl->CfgEnum.GetStr)(Param);
      CfgTypePtr = (IniTbl->CfgEnum.GetType)(Param);
            
      if (!strcmp(INILIB_TYPE_INT,CfgTypePtr)) {
      
         if (JSON_GetValUint32(&(IniTbl->Json), TokenIdx, CfgStrPtr, &JsonIntData)) {
            ++IniTbl->JsonVarCnt; 
            IniTbl->CfgItem[Param].Initialized = TRUE;
            IniTbl->CfgItem[Param].Type   = INITBL_UINT32;
            IniTbl->CfgItem[Param].Int    = (uint32) JsonIntData;
            IniTbl->CfgItem[Param].Str[0] = '\0';
            if (DBG_INITBL) OS_printf("IniTbl->CfgItem[%d].Int=%u, 0x%08X\n", Param, IniTbl->CfgItem[Param].Int, IniTbl->CfgItem[Param].Int);
         }
         
      } /* End if uint32 */
      else {
      
         if (JSON_GetValStr(&(IniTbl->Json), TokenIdx, CfgStrPtr, JsonStrData)) { 
            ++IniTbl->JsonVarCnt;
            IniTbl->CfgItem[Param].Initialized = TRUE;
            IniTbl->CfgItem[Param].Type = INITBL_STR;
            IniTbl->CfgItem[Param].Int  = 0;
            strcpy(IniTbl->CfgItem[Param].Str,JsonStrData);
            if (DBG_INITBL) OS_printf("IniTbl->CfgItem[%d].Str=%s\n", Param, IniTbl->CfgItem[Param].Str);
         }

      }  /* End if char* */   
   } /* End enum loop */
   
   
   if (IniTbl->JsonVarCnt == (IniTbl->CfgEnum.End-1)) {
   
      RetStatus = TRUE;
      CFE_EVS_SendEvent(INITBL_JSON_PARSE_EID, CFE_EVS_INFORMATION, "Successfully configured %d initialization file attributes",
                        IniTbl->JsonVarCnt);
         
   } /* End if valid JsonVarCnt */
   else {
	   
      ++IniTbl->AttrErrCnt;     
      CFE_EVS_SendEvent(INITBL_JSON_PARSE_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be %d.",
                        IniTbl->JsonVarCnt, (IniTbl->CfgEnum.End-1));
   
   } /* End if invalid JsonVarCnt */
      
   return RetStatus;

} /* ConfigCallback() */


/******************************************************************************
** Function: ValidConfigParam
**
*/
static boolean ValidConfigParam(INITBL_Class* IniTbl, uint16 Param, INITBL_Type Type)
{
   
   boolean RetStatus = FALSE;
   
   if ( Param > IniTbl->CfgEnum.Start && Param < IniTbl->CfgEnum.End) {
   
      if (IniTbl->CfgItem[Param].Initialized) {
         if (IniTbl->CfgItem[Param].Type == Type) {
            RetStatus = TRUE;
         }
         else {
            CFE_EVS_SendEvent(INITBL_CFG_PARAM_ERR_EID, CFE_EVS_ERROR, "Attempt to retrieve parameter of type %s that was loaded as type %s",
                              ConfigTypeStr(Type),ConfigTypeStr(IniTbl->CfgItem[Param].Type));      
         }
      }
      else {
         CFE_EVS_SendEvent(INITBL_CFG_PARAM_ERR_EID, CFE_EVS_ERROR, "Attempt to retrieve uninitialized parameter %d", Param);
      }         
   }
   else {
      CFE_EVS_SendEvent(INITBL_CFG_PARAM_ERR_EID, CFE_EVS_ERROR, "Attempt to retrieve invalid parameter %d that is not in valid range: %d < param < %d",
                        Param, IniTbl->CfgEnum.Start, IniTbl->CfgEnum.End);
   }
   
   return RetStatus;
   
} /* ValidConfigParam() */

