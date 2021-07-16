/*
** Purpose: Implement the coreJSON adapter
**
** Notes:
**   1. This provides a wrapper for apps to use coreJSON for their tables.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
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
#include "cjson.h"


/***********************/
/** Macro Definitions **/
/***********************/


/**********************/
/** Type Definitions **/
/**********************/


/**********************/
/** Global File Data **/
/**********************/


/* JSONStatus_t - String lookup table */

static const char* JsonStatusStr[] = {
  
  "ValidButPartial",    /* JSONPartial          */
  "Valid",              /* JSONSuccess          */
  "Invalid-Malformed",  /* JSONIllegalDocument  */
  "MaxDepthExceeded",   /* JSONMaxDepthExceeded */
  "QueryKeyNotFound",   /* JSONNotFound         */
  "QueryNullPointer",   /* JSONNullParameter    */
  "QueryKeyInvalid",    /* JSONBadParameter     */
  
};

/* JSONTypes_t -  String lookup table */

static const char* JsonTypeStr[] = {
  
  "Invalid",  /* JSONInvalid */
  "String",   /* JSONString  */
  "Number",   /* JSONNumber  */
  "True",     /* JSONTrue    */
  "False",    /* JSONFalse   */
  "Null",     /* JSONNull    */
  "Object",   /* JSONObject  */
  "Array",    /* SONArray    */
  
};


/*
** Local File Function Prototypes
*/


/******************************************************************************
** Function: CJSON_ObjConstructor
**
** Notes:
**    1. This is used to construct individual CJSON_Obj structures. This 
**       constructor is not needed if the user creates a static CJSON_Obj array
**       with default values.
*/
void CJSON_ObjConstructor(CJSON_Obj* Obj, const char* QueryKey, 
                          JSONTypes_t JsonType, void* TblData, size_t TblDataLen)
{

   Obj->Updated    = FALSE;
   Obj->TblData    = TblData;
   Obj->TblDataLen = TblDataLen;   
   Obj->Type       = JsonType;
   
   if (strlen(QueryKey) <= CJSON_MAX_KEY_LEN) {
      
      strncpy (Obj->Query.Key, QueryKey, CJSON_MAX_KEY_LEN);
      Obj->Query.KeyLen = strlen(Obj->Query.Key);
   }
   else {
   
      CFE_EVS_SendEvent(CJSON_OBJ_CONSTRUCT_ERR_EID, CFE_EVS_ERROR,
                        "Error constructing table. Query key %s exceeds maximum key length %d.",
                        QueryKey, CJSON_MAX_KEY_LEN);
   
   }
      
} /* End CJSON_ObjConstructor() */


/******************************************************************************
** Function: CJSON_LoadObjArray
**
** Notes:
**    1. See CJSON_LoadObj() for supported JSON types
**
*/
size_t CJSON_LoadObjArray(CJSON_Obj* Obj, size_t ObjCnt, char* Buf, size_t BufLen)
{
   
   int     i;
   size_t  ObjLoadCnt = 0;
   
   for (i=0; i < ObjCnt; i++) {
   
      if (CJSON_LoadObj(&Obj[i], Buf, BufLen)) ObjLoadCnt++;
      
   } /* End object loop */
   
   return ObjLoadCnt;
      
} /* End CJSON_LoadObjArray() */


/******************************************************************************
** Function: CJSON_ProcessFile
**
** Notes:
**  1. Entire JSON file is read into memory
**  2. User callback function LoadJsonData()calls CJSON_LoadObjArray() to load
**     the JSON data into the user's table. This gives the caller control over 
**     table-specific validation criteria.
*/
boolean CJSON_ProcessFile(const char* Filename, char* JsonBuf, 
                          size_t MaxJsonFileChar, CJSON_LoadJsonData LoadJsonData)
{

   int          FileHandle;
   int32        ReadStatus;
   JSONStatus_t JsonStatus;

   boolean  RetStatus = FALSE;
   
   FileHandle = OS_open(Filename, OS_READ_ONLY, 0);
   
   /*
   ** Read entire JSON table into buffer. Logic kept very simple and JSON
   ** validate will catch if entire file wasn't read.
   */
   if (FileHandle >= 0) {

      ReadStatus = OS_read(FileHandle, JsonBuf, MaxJsonFileChar);

      if (ReadStatus == OS_FS_ERROR) {

         CFE_EVS_SendEvent(CJSON_PROCESS_FILE_ERR_EID, CFE_EVS_ERROR, 
                           "CJSON error reading file %s. Status = 0x%08X",
                           Filename, ReadStatus);

      }
      else {
     
         JsonStatus = JSON_Validate(JsonBuf, ReadStatus);
         
         if (JsonStatus == JSONSuccess) { 
         
            RetStatus = LoadJsonData(ReadStatus);
         
         }
         else {
         
            CFE_EVS_SendEvent(CJSON_PROCESS_FILE_ERR_EID, CFE_EVS_ERROR, 
                              "CJSON error reading file %s. Status = 0x%08X",
                              Filename, ReadStatus);

         }
         
      } /* End if valid read */
   }/* End if valid open */
   else {

      CFE_EVS_SendEvent(CJSON_PROCESS_FILE_ERR_EID, CFE_EVS_ERROR, "CJSON error opening file %s. Status = 0x%08X", 
                        Filename, FileHandle);
      
   }
   
   return RetStatus;
   
} /* End CJSON_ProcessFile() */


/******************************************************************************
** Function: CJSON_LoadObj
**
** Notes:
**    1. See switch statement for supported JSON types
**
*/
boolean CJSON_LoadObj(CJSON_Obj* Obj, const char* Buf, size_t BufLen)
{
   
   boolean      RetStatus = FALSE;
   JSONStatus_t JsonStatus;
   const char   *Value;
   size_t       ValueLen;
   JSONTypes_t  ValueType;
   char         *ErrCheck;
   char         NumberBuf[20], StrBuf[256];
   int          IntValue;
   
   Obj->Updated = FALSE;
   
   JsonStatus = JSON_SearchConst(Buf, BufLen, 
                                 Obj->Query.Key, Obj->Query.KeyLen,
                                 &Value, &ValueLen, &ValueType);
                                 
   if (JsonStatus == JSONSuccess) { 
   
      switch (ValueType) {
         
         case JSONString:
         
            if (ValueLen <= Obj->TblDataLen) {
            
               CFE_EVS_SendEvent(CJSON_LOAD_OBJ_EID, CFE_EVS_DEBUG, "JSON string %s, len = %d", Value, ValueLen);

               strncpy(StrBuf,Value,ValueLen);
               StrBuf[ValueLen] = '\0';
               
               memcpy(Obj->TblData,StrBuf,ValueLen+1);
               Obj->Updated = TRUE;
               RetStatus = TRUE;
            
            }
            else {
               
               CFE_EVS_SendEvent(CJSON_LOAD_OBJ_ERR_EID, CFE_EVS_ERROR, "JSON string length %d exceeds %s's max length %d", 
                                 ValueLen, Obj->Query.Key, Obj->TblDataLen);
            
            }
            break;
   
         case JSONNumber:
         
            CFE_EVS_SendEvent(CJSON_LOAD_OBJ_EID, CFE_EVS_DEBUG, "JSON number %s, len = %d", Value, ValueLen);

            strncpy(NumberBuf,Value,ValueLen);
            NumberBuf[ValueLen] = '\0';
            IntValue = (int)strtol(NumberBuf, &ErrCheck, 10);
			   if (ErrCheck != NumberBuf) {
               memcpy(Obj->TblData,&IntValue,sizeof(int));
               Obj->Updated = TRUE;
               RetStatus = TRUE;
            }
            else {
               CFE_EVS_SendEvent(CJSON_LOAD_OBJ_ERR_EID, CFE_EVS_ERROR,
                                 "CJSON number conversion error for value %s",
                                 NumberBuf);
            }
            
            break;

         default:
         
            CFE_EVS_SendEvent(CJSON_LOAD_OBJ_ERR_EID, CFE_EVS_ERROR, "Unsupported JSON type %s returned for query %s", 
                              JsonTypeStr[ValueType], Obj->Query.Key);
      
      } /* End ValueType switch */
      
   }/* End if successful search */
   else {
   
      CFE_EVS_SendEvent(CJSON_LOAD_OBJ_EID, CFE_EVS_INFORMATION, "JSON search error for query %s. Status = %s.", 
                        Obj->Query.Key, JsonStatusStr[JsonStatus]);

   }
         
   return RetStatus;
   
} /* End CJSON_LoadObj() */


