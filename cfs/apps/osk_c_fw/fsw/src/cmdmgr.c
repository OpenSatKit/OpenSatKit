/* 
** Purpose: Manage command dispatching for an application.
**
** Notes:
**   1. This code must be reentrant so no global data is used. 
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

/*
** Include Files:
*/

#include "cmdmgr.h"

///#define DBG_CMDMGR


/*
** File Function Prototypes
*/

static boolean UnusedFuncCode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: CMDMGR_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same cmdmgr instance.
**
*/
void CMDMGR_Constructor(CMDMGR_Class* CmdMgr)
{

   int i;

   CFE_PSP_MemSet(CmdMgr, 0, sizeof(CMDMGR_Class));
   for (i=0; i < CMDMGR_CMD_FUNC_TOTAL; i++) {
      
      CmdMgr->Cmd[i].FuncPtr = UnusedFuncCode;
   
   }

} /* End CMDMGR_Constructor() */


/******************************************************************************
** Function: CMDMGR_RegisterFunc
**
*/
boolean CMDMGR_RegisterFunc(CMDMGR_Class* CmdMgr, uint16 FuncCode, void* ObjDataPtr, 
                            CMDMGR_CmdFuncPtr ObjFuncPtr, uint16 UserDataLen)
{

   boolean RetStatus = FALSE;
   
   if (FuncCode < CMDMGR_CMD_FUNC_TOTAL) {

      if (DBG_CMDMGR) OS_printf("CMDMGR_RegisterFunc(): FuncCode %d, DataLen %d\n", FuncCode, UserDataLen);
      CmdMgr->Cmd[FuncCode].DataPtr = ObjDataPtr;
      CmdMgr->Cmd[FuncCode].FuncPtr = ObjFuncPtr;
      CmdMgr->Cmd[FuncCode].UserDataLen = UserDataLen;

      CmdMgr->Cmd[FuncCode].AltCnt.Enabled = FALSE;
      CmdMgr->Cmd[FuncCode].AltCnt.Valid   = 0;
      CmdMgr->Cmd[FuncCode].AltCnt.Invalid = 0;
      
      RetStatus = TRUE;

   }
   else {
      
      CFE_EVS_SendEvent (CMDMGR_REG_INVALID_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,
         "Attempt to register function code %d which is greater than max %d",
         FuncCode,(CMDMGR_CMD_FUNC_TOTAL-1));
   }

   return RetStatus;
   
} /* End CMDMGR_RegisterFunc() */


/******************************************************************************
** Function: CMDMGR_RegisterFuncAltCnt
**
*/
boolean CMDMGR_RegisterFuncAltCnt(CMDMGR_Class* CmdMgr, uint16 FuncCode, void* ObjDataPtr, 
                                  CMDMGR_CmdFuncPtr ObjFuncPtr, uint16 UserDataLen)
{

   boolean RetStatus = FALSE;

   if (CMDMGR_RegisterFunc(CmdMgr, FuncCode, ObjDataPtr, ObjFuncPtr, UserDataLen)) {
      
      CmdMgr->Cmd[FuncCode].AltCnt.Enabled = TRUE;      

      RetStatus = TRUE;

   }

   return RetStatus;
   
} /* End CMDMGR_RegisterFuncAltCnt() */


/******************************************************************************
** Function: CMDMGR_ResetStatus
**
** Keep logic simple and clear all alternate counters regardless of whether 
** they're enabled. 
**
*/
void CMDMGR_ResetStatus(CMDMGR_Class* CmdMgr)
{

   int i;

   CmdMgr->ValidCmdCnt   = 0;
   CmdMgr->InvalidCmdCnt = 0;

   for (i=0; i < CMDMGR_CMD_FUNC_TOTAL; i++) {
      
      CmdMgr->Cmd[i].AltCnt.Valid   = 0;
      CmdMgr->Cmd[i].AltCnt.Invalid = 0;
   
   }
   
} /* End CMDMGR_ResetStatus() */


/******************************************************************************
** Function: CMDMGR_DispatchFunc
**
** Notes:
**   1. Considered sending an event message for alternate counter commands, but
**      decided this is the client's responsibility. CmdMgr is a dispatcher and
**      if an app wants a message response then it should publish the format. 
**
*/
boolean CMDMGR_DispatchFunc(CMDMGR_Class* CmdMgr, const CFE_SB_MsgPtr_t  MsgPtr)
{

   boolean  ValidCmd = FALSE;

   uint16 UserDataLen = CFE_SB_GetUserDataLength(MsgPtr);
   uint16 FuncCode    = CFE_SB_GetCmdCode(MsgPtr);
   uint32 Checksum    = CFE_SB_GetChecksum(MsgPtr);

   if (DBG_CMDMGR) OS_printf("CMDMGR_DispatchFunc(): [0]=0x%X, [0]=0x%X, [0]=0x%X, [0]=0x%X\n",
                             ((uint16*)MsgPtr)[0],((uint16*)MsgPtr)[1],((uint16*)MsgPtr)[2],((uint16*)MsgPtr)[3]);
   if (DBG_CMDMGR) OS_printf("CMDMGR_DispatchFunc(): FuncCode %d, DataLen %d\n", FuncCode,UserDataLen);

   if (FuncCode < CMDMGR_CMD_FUNC_TOTAL) {

      if (UserDataLen == CmdMgr->Cmd[FuncCode].UserDataLen) {

         if (CFE_SB_ValidateChecksum(MsgPtr) == TRUE) {

            ValidCmd = (CmdMgr->Cmd[FuncCode].FuncPtr)(CmdMgr->Cmd[FuncCode].DataPtr, MsgPtr);

         } /* End if valid checksum */
         else {

            CFE_EVS_SendEvent (CMDMGR_DISPATCH_INVALID_CHECKSUM_ERR_EID, CFE_EVS_ERROR,
                               "Invalid command checksum %d", Checksum);
         
         }
      } /* End if valid length */
      else {

         CFE_EVS_SendEvent (CMDMGR_DISPATCH_INVALID_LEN_ERR_EID, CFE_EVS_ERROR,
                            "Invalid command user data length %d, expected %d",
                            UserDataLen, CmdMgr->Cmd[FuncCode].UserDataLen);

      }

   } /* End if valid function code */
   else {
      
      CFE_EVS_SendEvent (CMDMGR_DISPATCH_INVALID_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,
                         "Invalid command function code %d is greater than max %d",
                         FuncCode, (CMDMGR_CMD_FUNC_TOTAL-1));

   } /* End if invalid function code */

   if (CmdMgr->Cmd[FuncCode].AltCnt.Enabled) {
   
      ValidCmd ? CmdMgr->Cmd[FuncCode].AltCnt.Valid++ : CmdMgr->Cmd[FuncCode].AltCnt.Invalid++;
   
   } 
   else {
   
      ValidCmd ? CmdMgr->ValidCmdCnt++ : CmdMgr->InvalidCmdCnt++;
   
   }
   
   return ValidCmd;

} /* End CMDMGR_DispatchFunc() */


/******************************************************************************
** Function: UnusedFuncCode
**
*/
static boolean UnusedFuncCode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (CMDMGR_DISPATCH_UNUSED_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,
                      "Unused command function code %d received",CFE_SB_GetCmdCode(MsgPtr));

   return FALSE;

} /* End UnusedFuncCode() */


/******************************************************************************
** Function: CMDMGR_ValidBoolArg
**
*/
boolean CMDMGR_ValidBoolArg(uint16 BoolArg)
{
   
   return ((BoolArg == TRUE) || (BoolArg == FALSE));

} /* CMDMGR_ValidBoolArg() */


/******************************************************************************
** Function: CMDMGR_BoolStr
**
** Purpose: Return a pointer to a string describing a boolean
**
** Notes:
**   Assumes FALSE=0 and TRUE=1
*/
const char* CMDMGR_BoolStr(boolean BoolArg)
{
   
   static char* BoolStr[] = {
      "FALSE",
      "TRUE",
      "UNDEF"
   };

   uint8 i = 2;
   
   if ( BoolArg == TRUE || BoolArg == FALSE) {
   
      i = BoolArg;
   
   }
        
   return BoolStr[i];

} /* End CMDMGR_BoolStr() */

