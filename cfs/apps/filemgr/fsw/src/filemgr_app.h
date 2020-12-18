/*
** Purpose: Define the File Manager application
**
** Notes:
**   1. This is a refactor of NASA's File Manager (FM) app. The refactor includes
**      adaptation to the OSK app framework and prootyping the usage of an app 
**      init JSON file. The idea is to rethink whcih configuration paarameters
**      should be compile time and which should be runtime. 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _filemgr_app_
#define _filemgr_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "childmgr.h"
#include "dir.h"
#include "file.h"
#include "filesys.h"
#include "initbl.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define FILEMGR_INIT_APP_EID    (FILEMGR_BASE_EID + 0)
#define FILEMGR_NOOP_EID        (FILEMGR_BASE_EID + 1)
#define FILEMGR_EXIT_EID        (FILEMGR_BASE_EID + 2)
#define FILEMGR_INVALID_MID_EID (FILEMGR_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];

    uint8   CommandCounter;
    uint8   CommandErrCounter;
    uint8   Spare;                          
    uint8   NumOpenFiles;                   /**< \fmtlmmnemonic \FM_NumOpen
                                                 \brief Number of open files in the system */

    uint8   ChildCmdCounter;                /**< \fmtlmmnemonic \FM_ChildCMDPC
                                                 \brief Child task command counter */
    uint8   ChildCmdErrCounter;             /**< \fmtlmmnemonic \FM_ChildCMDEC
                                                 \brief Child task command error counter */
    uint8   ChildCmdWarnCounter;            /**< \fmtlmmnemonic \FM_ChildWarnCtr
                                                 \brief Child task command warning counter */

    uint8   ChildQueueCount;                /**< \fmtlmmnemonic \FM_ChildQueueCount
                                                 \brief Number of pending commands in queue */

    uint8   ChildCurrentCC;                 /**< \fmtlmmnemonic \FM_ChildCurrCC
                                                 \brief Command code currently executing */
    uint8   ChildPreviousCC;                /**< \fmtlmmnemonic \FM_ChildPrevCC
                                                 \brief Command code previously executed */

} OS_PACK FILEMGR_HkPkt;
#define FILEMGR_TLM_HK_LEN sizeof (FILEMGR_HkPkt)


/******************************************************************************
** FILEMGR_Class
*/
typedef struct {

   /* 
   ** App Framework
   */ 
   
   INITBL_Class    IniTbl; 
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   TBLMGR_Class    TblMgr;
   
   CHILDMGR_Class  ChildMgr;
   
   FileUtil_OpenFileList OpenFileList;
   
   /*
   ** Telemetry Packets
   */
   
   FILEMGR_HkPkt  HkPkt;
   
   /*
   ** App Objects
   */ 
       
   DIR_Class      Dir;
   FILE_Class     File;
   FILESYS_Class  FileSys;
 
} FILEMGR_Class;


/*******************/
/** Exported Data **/
/*******************/

extern FILEMGR_Class  FileMgr;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FILEMGR_AppMain
**
*/
void FILEMGR_AppMain(void);


/******************************************************************************
** Function: FILEMGR_NoOpCmd
**
*/
boolean FILEMGR_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILEMGR_ResetAppCmd
**
*/
boolean FILEMGR_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILEMGR_SendHousekeepingPkt
**
*/
void FILEMGR_SendHousekeepingPkt(void);


#endif /* _filemgr_app_ */
