/*
** Purpose: Define a coreJSON adapter for JSON table management
**
** Notes:
**   1. Provide an interface to the FreeRTOS coreJSON library that simplifies
**      app JSON table management and makes the usage consistent.
**   2. The JSON table is a stream of characters so string terminators are not
**      part of length definitions 
**   3. This is designed as a functional library rather than an object-based
**      service for a couple of reasons. 
**      - Users can write custom code for table loads. A general purpose load
**        could be provided with a table validation callback but when I looked
**        into the user table-to-CJSON interface it got more complex. The
**        amount of code a user needs to write with the current design is 
**        trivial and it allows more control expecially for partial table load
**        situations.
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
#ifndef _cjson_
#define _cjson_

/*
** Includes
*/

#include "osk_c_fw_cfg.h"
#include "core_json.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define CJSON_MAX_KEY_LEN  32  /* Number of characters in key, does not include a string terminator */ 

/*
** Event Message IDs
*/

#define CJSON_OBJ_CONSTRUCT_ERR_EID  (CJSON_BASE_EID + 0)
#define CJSON_PROCESS_FILE_ERR_EID   (CJSON_BASE_EID + 1)
#define CJSON_LOAD_OBJ_EID           (CJSON_BASE_EID + 2)
#define CJSON_LOAD_OBJ_ERR_EID       (CJSON_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/

/* TODO - Consider refactor into 2 structures so one can be passed as a const */

typedef struct {

   char     Key[CJSON_MAX_KEY_LEN];
   size_t   KeyLen;

} CJSON_Query;

typedef struct {

   void*        TblData;
   size_t       TblDataLen;
   boolean      Updated;
   JSONTypes_t  Type;
   CJSON_Query  Query;

} CJSON_Obj;


/* User callback function to load table data */
typedef boolean (*CJSON_LoadJsonData)(size_t JsonFileLen);


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: CJSON_ObjConstructor
**
** Initialize a JSON table object
**
** Notes:
**   1. See file prologue for supported JSON types.
**
*/
void CJSON_ObjConstructor(CJSON_Obj* Obj, const char* QueryKey, 
                          JSONTypes_t JsonType, void* TblData, size_t TblDataLen);


/******************************************************************************
** Function: CJSON_LoadObj
**
** Notes:
**   1. See file prologue for supported JSON types.
**
*/
boolean CJSON_LoadObj(CJSON_Obj* Obj, const char* Buf, size_t BufLen);


/******************************************************************************
** Function: CJSON_LoadObjArray
**
** Notes:
**   1. See file prologue for supported JSON types.
**
*/
size_t CJSON_LoadObjArray(CJSON_Obj* Obj, size_t ObjCnt, char* Buf, size_t BufLen);


/******************************************************************************
** Function: CJSON_ProcessFile
**
** Notes:
**  1. Takes care of all generic table processing and validation. User's 
**     callback function performs table-specific data processing
*/
boolean CJSON_ProcessFile(const char* Filename, char* JsonBuf, 
                          size_t MaxJsonFileChar, CJSON_LoadJsonData LoadJsonData);


#endif /* _cjson_ */



