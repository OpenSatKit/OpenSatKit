/*
** Purpose: Define application configurations for the File Manager (FILEMGR)
**          application
**
** Notes:
**   1. The enumeration macro design is from 
**      https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c/202511 
**   2. 
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _ini_tbl_
#define _ini_tbl_

/*
** Include Files
*/

#include "osk_c_fw.h" /* Needs JSON with FW config so just include everything */


/***********************/
/** Macro Definitions **/
/***********************/

/*
** Table Structure Objects 
*/

#define INITBL_OBJ_CONFIG    0
#define INITBL_OBJ_CNT       1

#define INITBL_OBJ_CONFIG_NAME  "config"

#define INITBL_CONFIG_DEF_ERR_EID    (INILIB_BASE_EID + 0)
#define INITBL_LOAD_CONFIG_EID       (INILIB_BASE_EID + 1)
#define INITBL_LOAD_CONFIG_ERR_EID   (INILIB_BASE_EID + 2)
#define INITBL_JSON_PARSE_EID        (INILIB_BASE_EID + 3)
#define INITBL_JSON_PARSE_ERR_EID    (INILIB_BASE_EID + 4)
#define INITBL_CFG_PARAM_ERR_EID     (INILIB_BASE_EID + 5)


/**********************/
/** Type Definitions **/
/**********************/

typedef enum {
   
   INITBL_UNDEF  = 0,
   INITBL_UINT32 = 1,
   INITBL_STR    = 2
   
} INITBL_Type;


typedef struct {
   
   boolean     Initialized;
   INITBL_Type Type;
   uint32      Int;
   char        Str[OS_MAX_PATH_LEN];

} INITBL_CfgItem;


typedef struct {

   uint16   AttrErrCnt;
   uint16   JsonVarCnt;
   
   INILIB_CfgEnum  CfgEnum;
   INITBL_CfgItem  CfgItem[INITBL_MAX_CFG_ITEMS];

   JSON_Class Json;
   JSON_Obj   JsonObj[INITBL_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} INITBL_Class;
 
 
/******************************************************************************
** Function: INITBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
boolean INITBL_Constructor(INITBL_Class* IniTbl, const char* IniFile,
                           INILIB_CfgEnum* CfgEnum);


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
uint32 INITBL_GetIntConfig(INITBL_Class*  IniTbl, uint16 Param);


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
const char* INITBL_GetStrConfig(INITBL_Class*  IniTbl, uint16 Param);


#endif /* _ini_tbl_ */
