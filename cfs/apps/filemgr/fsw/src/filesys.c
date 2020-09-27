/*
** Purpose: Implement the File System Table
**
** Notes:
**   1. Refactored from NASA's FM FreeSpace table.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "filesys.h"
#include "cfs_utils.h"

/*
** Global File Data
*/

static FILESYS_Class*  FileSys = NULL;

/*
** Local Function Prototypes
*/

static int32 Validate(void* TblPtr);


/******************************************************************************
** Function: FILESYS_Constructor
**
*/
void FILESYS_Constructor(FILESYS_Class*  FileSysPtr)
{
 
   FileSys = FileSysPtr;

   CFE_PSP_MemSet((void*)FileSys, 0, sizeof(FILESYS_Class));
   FileSys->CfeTbl.DataPtr = (FILESYS_TblData *) NULL;

   FileSys->CfeTbl.Status = CFE_TBL_Register(&FileSys->CfeTbl.Handle, FILEMGR_INI_TBL_CFE_NAME,
                                             sizeof(FILESYS_TblData), CFE_TBL_OPT_DEFAULT, 
                                             (CFE_TBL_CallbackFuncPtr_t)Validate);
    
   FileSys->CfeTbl.Registered = (FileSys->CfeTbl.Status == CFE_SUCCESS);
   
   /* DataPtr will remain NULL if data not loaded. */
   if (FileSys->CfeTbl.Registered) {
   
      FileSys->CfeTbl.Status = CFE_TBL_Load(FileSys->CfeTbl.Handle, CFE_TBL_SRC_FILE, FILEMGR_INI_TBL_DEF_NAME);
      if (FileSys->CfeTbl.Status == CFE_SUCCESS) {
         CFE_TBL_GetAddress((void **)&(FileSys->CfeTbl.DataPtr), FileSys->CfeTbl.Handle);
      }
   }
   else {
      
      CFE_EVS_SendEvent(FILESYS_TBL_REGISTER_ERR_EID, CFE_EVS_ERROR,
                        "Error registering table %s, CFE_TBL_Register() status = 0x%08X",
                        FILEMGR_INI_TBL_DEF_NAME, FileSys->CfeTbl.Status);                        
   }

    
} /* End FILESYS_Constructor() */


/******************************************************************************
** Function:  FILESYSTBL_ResetStatus
**
*/
void FILESYS_ResetStatus()
{

   /* Nothing to do */

} /* End FILESYSTBL_ResetStatus() */



/******************************************************************************
** Function: FILESYS_ManageTbl
**
*/
void FILESYS_ManageTbl(void)
{

   if (FileSys->CfeTbl.Registered) {
   
     FileSys->CfeTbl.Status = CFE_TBL_ReleaseAddress(FileSys->CfeTbl.Handle);
      
      CFE_TBL_Manage(FileSys->CfeTbl.Handle);
      
      FileSys->CfeTbl.Status = CFE_TBL_GetAddress((void **)&(FileSys->CfeTbl.DataPtr), FileSys->CfeTbl.Handle);

      if (FileSys->CfeTbl.Status == CFE_TBL_ERR_NEVER_LOADED) {
         
         FileSys->CfeTbl.DataPtr = (FILESYS_TblData *) NULL;
      
      }
      
   } /* End if table registered */
   
} /* End FILESYS_ManageTbl() */


/******************************************************************************
** Function: Validate
**
** Callback function used by CFE Table service to allow a table to be validated
** prior to being committed.
**
** Function signature must match CFE_TBL_CallbackFuncPtr_t
**
*/
static int32 Validate(void* VoidTblPtr) 
{
   
   FILESYS_TblData* Tbl = (FILESYS_TblData *) VoidTblPtr;

   int32   RetStatus = FILEMGR_INI_TBL_ERR;
   uint16  NameLength;
   uint16  i;

   uint16 ValidEntries   = 0;
   uint16 InvalidEntries = 0;
   uint16 UnusedEntries  = 0;

   /*
   ** Verification criteria
   **
   **  1. Table volume state must be enabled or disabled or unused
   **  2. Enabled or disabled entries must have a valid file system name
   **  3. File system name for unused entries is ignored
   **
   ** CFS_IsValidFilename() checks for null string. A null string test is 
   ** done first to separate it from an invalid character error. 
   */

   for (i = 0; i < FILEMGR_FILESYS_TBL_VOL_CNT; i++) {

      /* Validate file system name if state is enabled or disabled */
      
      if ((Tbl->Volume[i].State == FILESYS_TBL_ENTRY_ENABLED) ||
          (Tbl->Volume[i].State == FILESYS_TBL_ENTRY_DISABLED)) {

         /* Search file system name buffer for a string terminator */
         
         for (NameLength = 0; NameLength < OS_MAX_PATH_LEN; NameLength++) {

            if (Tbl->Volume[i].Name[NameLength] == '\0') break;
              
         }

         /* 
         ** Valid file system names must be: non-zero length, terminated and valid characters  
         ** Only send event on first error occurence 
         */
         if (NameLength == 0) {

            ++InvalidEntries;

            if (InvalidEntries == 1) {

               CFE_EVS_SendEvent(FILESYS_TBL_VERIFY_ERR_EID, CFE_EVS_ERROR,
                                 "%s Table error: index = %d, empty name string",
                                 FILEMGR_INI_TBL_CFE_NAME, i);
            }

         }
         else if (NameLength == OS_MAX_PATH_LEN) {

            InvalidEntries++;

            if (InvalidEntries == 1) {
                    
               CFE_EVS_SendEvent(FILESYS_TBL_VERIFY_ERR_EID, CFE_EVS_ERROR,
                                 "%s table error: index = %d, non-terminated name string",
                                 FILEMGR_INI_TBL_CFE_NAME, i);
            }
            
         }
         else if (!CFS_IsValidFilename(Tbl->Volume[i].Name, NameLength)) {

            InvalidEntries++;

            if (InvalidEntries == 1) {

               CFE_EVS_SendEvent(FILESYS_TBL_VERIFY_ERR_EID, CFE_EVS_ERROR,
                                 "%s table error: index = %d, invalid name = %s",
                                 FILEMGR_INI_TBL_CFE_NAME, i, Tbl->Volume[i].Name);
                }
            }
         else {
                
            ValidEntries++;

         } /* End NameLength checks */
     
      } /* End ENABLED/DISABLED checcks */ 
      else if (Tbl->Volume[i].State == FILESYS_TBL_ENTRY_UNUSED) {

            /* Ignore (but count) unused table entries */
            ++UnusedEntries;
      
      }
      else { 
        
         /* Invalid state */
                    
         ++InvalidEntries;   
            
         if (InvalidEntries == 1) {

            CFE_EVS_SendEvent(FILESYS_TBL_VERIFY_ERR_EID, CFE_EVS_ERROR,
                              "%s table error: index = %d, invalid state = %d",
                              FILEMGR_INI_TBL_CFE_NAME, i, Tbl->Volume[i].State);
         }

      } /* End state checks **/

   } /* End volume loop */

   CFE_EVS_SendEvent(FILESYS_TBL_VERIFIED_EID, CFE_EVS_INFORMATION,
                      "%s table entry verification: valid = %d, invalid = %d, unused = %d",
                       FILEMGR_INI_TBL_CFE_NAME, ValidEntries, InvalidEntries, UnusedEntries);

   if (InvalidEntries == 0) RetStatus = CFE_SUCCESS;

   return RetStatus;

  
} /* End Validate() */


/******************************************************************************
** Function: FILESYS_SendTblPktCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/

boolean FILESYS_SendTblPktCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = TRUE;
   uint16  i;
   uint64  FreeSpace64;

   if (FileSys->CfeTbl.DataPtr == (FILESYS_TblData *) NULL) {

      CFE_EVS_SendEvent(FILESYS_SEND_PKT_ERR_EID, CFE_EVS_ERROR,
                       "Send %s table packet command error: File system free space table is not loaded",
                       FILEMGR_INI_TBL_CFE_NAME);
   
   }
   else {
      
      CFE_SB_InitMsg(&FileSys->TblPkt, FILEMGR_FILESYS_TLM_MID,
                     sizeof(FILESYS_TblPkt), TRUE);

      for (i=0; i < FILEMGR_FILESYS_TBL_VOL_CNT; i++) {
         
         if (FileSys->CfeTbl.DataPtr->Volume[i].State == FILESYS_TBL_ENTRY_ENABLED) {
            
              strcpy(FileSys->TblPkt.Volume[i].Name, FileSys->CfeTbl.DataPtr->Volume[i].Name);

              /* Get file system free space */
              FreeSpace64 = 0;
              OS_fsBytesFree(FileSys->TblPkt.Volume[i].Name, &FreeSpace64);

              /* Store as bytes to avoid boundary, endian and strict-aliasing issues */
              CFE_PSP_MemCpy(&FileSys->TblPkt.Volume[i].FreeSpace_A,
                             &FreeSpace64, sizeof(uint64));
          }
      }

      CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FileSys->TblPkt);
      CFE_SB_SendMsg((CFE_SB_Msg_t *) &FileSys->TblPkt);

      CFE_EVS_SendEvent(FILESYS_SEND_PKT_CMD_EID, CFE_EVS_DEBUG,
                       "Sent %s table telemetry packet", FILEMGR_INI_TBL_CFE_NAME);
   
   } /* End if table loaded */

   return RetStatus;


} /* End FILESYS_SendTblPktCmd() */


/******************************************************************************
** Function: FILESYS_SetTblStateCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean FILESYS_SetTblStateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   boolean RetStatus = FALSE;
   
   const FILESYS_SetTblStateCmd_t *CmdParam = (const FILESYS_SetTblStateCmd_t *) MsgPtr;

   if (FileSys->CfeTbl.DataPtr == (FILESYS_TblData *) NULL) {

      CFE_EVS_SendEvent(FILESYS_SET_TBL_STATE_LOAD_ERR_EID, CFE_EVS_ERROR,
         "Set %s Table State Command Error: File system free space table is not loaded",
         FILEMGR_INI_TBL_CFE_NAME);
   
   }
   else if (CmdParam->TblVolumeIndex >= FILEMGR_FILESYS_TBL_VOL_CNT) {
      
      CFE_EVS_SendEvent(FILESYS_SET_TBL_STATE_ARG_ERR_EID, CFE_EVS_ERROR,
         "Set %s Table State Command Error: Commanded index %d is not in valid range of 0..%d",
         FILEMGR_INI_TBL_CFE_NAME, CmdParam->TblVolumeIndex, (FILEMGR_FILESYS_TBL_VOL_CNT-1));
        
   }
   else if ((CmdParam->TblVolumeState != FILESYS_TBL_ENTRY_ENABLED) &&
            (CmdParam->TblVolumeState != FILESYS_TBL_ENTRY_DISABLED)) {
               
      CFE_EVS_SendEvent(FILESYS_SET_TBL_STATE_ARG_ERR_EID, CFE_EVS_ERROR,
         "Set %s Table State Command Error: Commanded state %d is not in (%d=Enabled, %d=Disabled)",
         FILEMGR_INI_TBL_CFE_NAME, CmdParam->TblVolumeState, FILESYS_TBL_ENTRY_ENABLED, FILESYS_TBL_ENTRY_DISABLED);

   }
   else if (FileSys->CfeTbl.DataPtr->Volume[CmdParam->TblVolumeIndex].State == FILESYS_TBL_ENTRY_UNUSED) {
      
      CFE_EVS_SendEvent(FILESYS_SET_TBL_STATE_UNUSED_ERR_EID, CFE_EVS_ERROR,
         "Set %s Table State Command Error: Attempt to change state of unused table entry at index %d",
         FILEMGR_INI_TBL_CFE_NAME, CmdParam->TblVolumeIndex);
        
   }
   else {
      
      FileSys->CfeTbl.DataPtr->Volume[CmdParam->TblVolumeIndex].State = CmdParam->TblVolumeState;

      CFE_TBL_Modified(FileSys->CfeTbl.Handle);

      CFE_EVS_SendEvent(FILESYS_SET_TBL_STATE_CMD_EID, CFE_EVS_INFORMATION,
         "Set %s Table State Command: Set table index %d state to %d (%d=Enabled,%d=Disabled)",
         FILEMGR_INI_TBL_CFE_NAME, CmdParam->TblVolumeIndex, CmdParam->TblVolumeState, FILESYS_TBL_ENTRY_ENABLED, FILESYS_TBL_ENTRY_DISABLED);
   } 

   return RetStatus;

} /* End of FILESYS_SetTableStateCmd() */
