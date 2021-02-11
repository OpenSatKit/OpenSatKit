/*
** Purpose: Provide an interface to the JSMN JSON tokenizer. It is not a parser
**          but provides an API for apps to process JOSN tables
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   2. This example illustrates how to integrate a user-defined text parser
**      into the opensat app framework. A more complex parser is needed
**      for non-homogeneous data files.
**   3. One of the goals of the object-based cFS application effort is to 
**      explore using text-based tables that are managed by the application.
**      Once a 'standard' text format is agreed upon the common utilities 
**      could be provided as libraries to the apps.
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
#ifndef _json_
#define _json_

/*
** Includes
*/

#include <stddef.h>
#include "jsmn.h"
#include "osk_c_fw_cfg.h"


#define JSON_MAX_FILE_TOKENS       4096
#define JSON_MAX_FILE_LINE_CHAR     512
#define JSON_MAX_FILE_CHAR        49152      /* 32768 - kit_sch message table broke this limit */
#define JSON_MAX_CONTAINER_TOKENS   256
#define JSON_MAX_STR_LEN             32
#define JSON_MAX_OBJ_NAME_CHAR       32

#define JSON_UNDEF_STATUS_STR  "Undefined"   /* Status used in ground reporting */
#define JSON_UNDEF_VAL_STR     "undefined"   /* JSON property value */

/*
** Event Message IDs
*/

#define JSON_REG_CALLBACK_ERR_EID    (JSON_BASE_EID +  0)
#define JSON_INVLD_TYPE_ERR_EID      (JSON_BASE_EID +  1)
#define JSON_INVLD_FILE_EID          (JSON_BASE_EID +  2)
#define JSON_INVLD_BOOL_VAL_ERR_EID  (JSON_BASE_EID +  3)
#define JSON_INVLD_BOOL_TYPE_ERR_EID (JSON_BASE_EID +  4)
#define JSON_INVLD_INT_TYPE_ERR_EID  (JSON_BASE_EID +  5)
#define JSON_INT_CONV_ERR_EID        (JSON_BASE_EID +  6)
#define JSON_INVLD_STR_TYPE_ERR_EID  (JSON_BASE_EID +  7)
#define JSON_INVLD_FLT_TYPE_ERR_EID  (JSON_BASE_EID +  8)
#define JSON_FLT_CONV_ERR_EID        (JSON_BASE_EID +  9)
#define JSON_DBG_OPEN_FILE_EID       (JSON_BASE_EID + 10)
#define JSON_DBG_REG_CALLBACK_EID    (JSON_BASE_EID + 11)
#define JSON_DBG_PROC_TOKENS_EID     (JSON_BASE_EID + 12)

/*
** Type Definitions
*/


typedef enum {
  JSON_FILE_UNDEF         = 0,  /* Code assumes this status is 0 */
  JSON_FILE_OPEN_ERR      = 1,
  JSON_FILE_LINE_CHAR_ERR = 2,  /* Exceeded maximum characters in a line */ 
  JSON_FILE_CHAR_ERR      = 3,  /* Exceeded maximum characters in a file */
  JSON_FILE_JSMN_ERR      = 4,  /* JSMN reported a parsing error */
  JSON_FILE_VALID         = 5 
} JSON_FILE_STATUS;



/*
** Application's function that processes JSON table container objects. 
*/
typedef boolean (*JSON_ContainerFuncPtr) (void* UserData, int TokenIdx);

typedef struct {

  char   Name[JSON_MAX_STR_LEN];
  void*  UserData;
  JSON_ContainerFuncPtr FuncPtr;
  
} JSON_ContainerCallBack;


typedef struct {

   JSON_FILE_STATUS FileStatus;
   char*       FileBuf;

   int         ContainerDepth;  /* Recursive token container call depth */
   int         FileObjTokens;   /* Number of tokens in the file level object */
   int         JsmnStatus;
   jsmntok_t*  FileTokens;
   jsmn_parser JsmnParser;

   int         CallBackIdx;
   JSON_ContainerCallBack ContainerCallBack[JSON_MAX_CONTAINER_TOKENS];

} JSON_Class;

/*
** Structure used to manage each JSON object used for table
** loads and dumps.
** 
*/

typedef struct
{

   char                   Name[JSON_MAX_OBJ_NAME_CHAR];
   boolean                Modified;
   JSON_ContainerFuncPtr  Callback;
   void*                  UserData;
   
} JSON_Obj;

/*
** Exported Functions
*/


/******************************************************************************
** Function: JSON_ObjConstructor
**
** Notes:
**    1. This must be called prior to any other functions using the JSON_OBJ
**    2. The object name must be identical (case sensitive) to the name in the
**       JSON file. 
**    3. CallerData is a pointer to data that the user wants access to while
**       processing the JSON object.
**
*/
void JSON_ObjConstructor(JSON_Obj*              Obj,
                         char*                  Name,
                         JSON_ContainerFuncPtr  Callback,
                         void*                  UserData);


/******************************************************************************
** Function: JSON_ObjArrayReset
**
** Notes:
**    None
**
*/
void JSON_ObjArrayReset(JSON_Obj* ObjArray,
                        uint16    ObjCnt);


/******************************************************************************
** Function: JSON_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void JSON_Constructor(JSON_Class* Json, char* FileBuf, jsmntok_t* FileTokens);


/******************************************************************************
** Function: JSON_OpenFile
**
** Notes:
**    
**
*/
boolean JSON_OpenFile(JSON_Class* Json, const char* FileName);


/******************************************************************************
** Function: JSON_ProcessTokens
**
** Notes:
**    
**   1. This processing deviates from the current standard and requires the
**      JSON file start with an object or array. Originally all JSON files had
**      to start with an object or an array (RFC-4627). RFC-7159 changed the
**      original spec and added number, string, 'null', 'true', and 'false' to
**      the valid items that can start a file.
** 
*/
void JSON_ProcessTokens(JSON_Class* Json);


/******************************************************************************
** Function: JSON_ProcessTokens
**
** Notes:
**    
*/
void JSON_ProcessTokens(JSON_Class* Json);


/******************************************************************************
** Function: JSON_TokenStrEq
**
** Notes:
**    
*/
boolean JSON_TokenStrEq(char *js, jsmntok_t *t, const char *s);


/******************************************************************************
** Function: JSON_TokenToStr
**
** Notes:
**    
*/
char* JSON_TokenToStr(char *js, jsmntok_t *t);


/******************************************************************************
** Function: JSON_PrintTokens
**
** Notes:
**    
*/
void JSON_PrintTokens(JSON_Class* Json, int TokenCnt);


/******************************************************************************
** Function: JSON_RegContainerCallback
**
** Notes:
**    
*/
void JSON_RegContainerCallback(JSON_Class* Json, const JSON_Obj* JsonObj);


/******************************************************************************
** Function: JSON_GetContainerSize
**
** Notes:
** 
*/
int JSON_GetContainerSize(JSON_Class* Json, int ContainTokenIdx);


/******************************************************************************
** Function: JSON_GetValBool
**
** Notes:
**    
*/
boolean JSON_GetValBool(JSON_Class* Json, int ContainTokenIdx, const char* Key, boolean* BoolVal);


/******************************************************************************
** Function: JSON_GetValShortInt
**
** Notes:
**    
*/
boolean JSON_GetValShortInt(JSON_Class* Json, int ContainTokenIdx, const char* Key, int* IntVal);


/******************************************************************************
** Function: JSON_GetValUint32
**
** Notes:
**    
*/
boolean JSON_GetValUint32(JSON_Class* Json, int ContainTokenIdx, const char* Key, uint32* Uint32Val);


/******************************************************************************
** Function: JSON_GetValStr
**
** Notes:
**    
*/
boolean JSON_GetValStr(JSON_Class* Json, int ContainTokenIdx, const char* Key, char* StrVal);


/******************************************************************************
** Function: JSON_GetValDouble
**
** Notes:
**    
*/
boolean JSON_GetValDouble(JSON_Class* Json, int ContainTokenIdx, const char* Key, double* DoubleVal);


/******************************************************************************
** Function: JSON_GetFileStatusStr
**
** Notes:
**
*/
char* JSON_GetFileStatusStr(int Enum);


/******************************************************************************
** Function: JSON_GetBoolStr
**
** Notes:
**   1. Input uint16 because it is a common JSOSN parser integer size
**   2. Returns lowercase 'true' or 'false' by convention. Returns 'undef'
**      all non TRUE/FALSE values  
*/
char* JSON_GetBoolStr (uint16 State);
   
/******************************************************************************
** Function: JSON_GetJsmnErrStr
**
** Notes:
**
*/
char* JSON_GetJsmnErrStr(int Enum);


/******************************************************************************
** Function: JSON_GetJsmnTypeStr
**
** Notes:
**
*/
char* JSON_GetJsmnTypeStr(int Enum);

#endif /* _json_ */
