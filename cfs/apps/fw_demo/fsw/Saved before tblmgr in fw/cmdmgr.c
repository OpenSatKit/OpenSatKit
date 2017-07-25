/* 
** File:
**   $Id: $
**
** Purpose: Manage command dispatching for the application.
**
** Notes
**   1. This file should not contain any application specific
**      references (i.e. it's reusable).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**
**
** $Date: $
** $Revision: $
** $Log: $
*/

/*
** Include Files:
*/

#include "cmdmgr.h"


/*
** File Function Prototypes
*/

static boolean UnusedFuncCode(const CFE_SB_MsgPtr_t MsgPtr);

/*
** Global Data
*/

CMDMGR_Class* CmdMgr2;

/******************************************************************************
** Function: CMDMGR_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being called
**       because the "singleton" design pattern is being used. Only one command
**       manager should be needed for an application.
**
*/
void CMDMGR_Constructor2(CMDMGR_Class* CmdMgrPtr)
{

   int i;
   CmdMgr2 = CmdMgrPtr;

   CFE_PSP_MemSet(CmdMgrPtr, 0, sizeof(CMDMGR_Class));
   for (i=0; i < CMDMGR_CMD_FUNC_TOTAL; i++)
   {
      CmdMgr2->Cmd[i].FuncPtr = UnusedFuncCode;
   }

} /* End CMDMGR_Constructor() */

/******************************************************************************
** Function: CMDMGR_RegisterFunc
**
*/
void CMDMGR_RegisterFunc2(uint16 FuncCode, CMDMGR_CmdFuncPtr FuncPtr, uint16 UserDataLen)
{

   if (FuncCode < CMDMGR_CMD_FUNC_TOTAL)
   {

      CmdMgr2->Cmd[FuncCode].FuncPtr = FuncPtr;
      CmdMgr2->Cmd[FuncCode].UserDataLen = UserDataLen;

   }
   else
   {
      CFE_EVS_SendEvent (CMDMGR_REG_INVALID_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,
      "Attempt to register function code %d which is greater than max %d",
      FuncCode,CMDMGR_CMD_FUNC_TOTAL);
   }

} /* End CMDMGR_RegisterFunc() */

/******************************************************************************
** Function: CMDMGR_ResetStatus
**
*/
void CMDMGR_ResetStatus2(void)
{

   CmdMgr2->ValidCmdCnt = 0;
   CmdMgr2->InvalidCmdCnt = 0;

} /* End CMDMGR_ResetStatus() */

/******************************************************************************
** Function: CMDMGR_DispatchFunc
**
*/
boolean CMDMGR_DispatchFunc2(const CFE_SB_MsgPtr_t  MsgPtr)
{

   boolean  ValidCmd = FALSE;

   uint16 UserDataLen = CFE_SB_GetUserDataLength(MsgPtr);
   uint16 FuncCode    = CFE_SB_GetCmdCode(MsgPtr);
   uint32 Checksum    = CFE_SB_GetChecksum(MsgPtr);

   if (FuncCode < CMDMGR_CMD_FUNC_TOTAL)
   {

      if (UserDataLen == CmdMgr2->Cmd[FuncCode].UserDataLen)
      {

         if (CFE_SB_ValidateChecksum(MsgPtr) == TRUE)
         {

            ValidCmd = (CmdMgr2->Cmd[FuncCode].FuncPtr)(MsgPtr);

         } /* End if valid checksum */
         else
         {

            CFE_EVS_SendEvent (CMDMGR_DISPATCH_INVALID_CHECKSUM_ERR_EID, CFE_EVS_ERROR,
                               "Invalid command checksum %d", Checksum);
         }

      } /* End if valid length */
      else
      {

         CFE_EVS_SendEvent (CMDMGR_DISPATCH_INVALID_LEN_ERR_EID, CFE_EVS_ERROR,
                            "Invalid command user data length %d, expected %d for function code %d",
                            UserDataLen, CmdMgr2->Cmd[FuncCode].UserDataLen,FuncCode);

      }

   } /* End if valid function code */
   else
   {
      CFE_EVS_SendEvent (CMDMGR_DISPATCH_INVALID_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,
                         "Invalid command function code %d is greater than max %d",
                         FuncCode, CMDMGR_CMD_FUNC_TOTAL);

   } /* End if invalid function code */

   ValidCmd ? CmdMgr2->ValidCmdCnt++ : CmdMgr2->InvalidCmdCnt++;
   
   return ValidCmd;

} /* End CMDMGR_DispatchFunc() */


/******************************************************************************
** Function: UnusedFuncCode
**
*/
static boolean UnusedFuncCode(const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (CMDMGR_DISPATCH_UNUSED_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,
                      "Unused command function code %d received",CFE_SB_GetCmdCode(MsgPtr));

   return FALSE;

} /* End UnusedFuncCode() */

/* end of file */
