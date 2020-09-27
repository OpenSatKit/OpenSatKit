/*
** Purpose: Define application configurations for the File Manager (FILEMGR)
**          application
**
** Notes:
**   1. The enumeration macro design is from 
**      https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c/202511 
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
#include "app_cfg.h"
#include "inilib.h"

/*
** Table Structure Objects 
*/

#define  INITBL_OBJ_CONFIG    0
#define  INITBL_OBJ_CNT       1

#define  INITBL_OBJ_CONFIG_NAME  "config"

/******************************************************************************
** Init File declarations create:
**
**  typedef enum {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigEnum;
**    
**  typedef struct {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigStruct;
**
**   const char *GetConfigStr(value);
**   ConfigEnum GetConfigVal(const char *str);
**
** XX(name,type)
*/

#define CFG_CMD_PIPE_DEPTH  CMD_PIPE_DEPTH
#define CFG_CMD_PIPE_NAME   CMD_PIPE_NAME
 
#define APP_CONFIG(XX) \
    XX(CMD_PIPE_DEPTH,uint16) \
    XX(CMD_PIPE_NAME,char*) \


DECLARE_ENUM(Config,APP_CONFIG)

typedef struct {
   boolean Initialized;
   char    Buf[OS_MAX_PATH_LEN];
} INITBL_ConfigStr;

typedef struct {

   uint16 StrIndex;

   INITBL_ConfigStruct Item;
   INITBL_ConfigStr    Str[FILEMGR_INI_MAX_STRINGS];
   
} INITBL_Config;

typedef struct {

   uint16   AttrErrCnt;
   uint16   JsonVarCnt;
   
   INITBL_Config Config;

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
boolean INITBL_Constructor(INITBL_Class* IniTblPtr);

#endif /* _ini_tbl_ */
