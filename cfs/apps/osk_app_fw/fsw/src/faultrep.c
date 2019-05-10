/* 
** Purpose: Implement the fault reporter class.
**
** Note:
**   1. This code was originally acquired from the NASA cFS External Code
**      Interface (ECI) NOSA release. Several cosmetic hangs have been made
**      but it remains functionally very similar.
**   2. This code must be reentrant and no global data can be used. 
**   3. There are several (uint16) casts that are required to prevent compiler
**      warnings most are due to the compiler assuming a signed integer result
**      for integer-based math operations.
**
** License:
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include "faultrep.h"


/*
** Macro Definitions
*/

#define EVS_ID(Offset)   ((uint16)(FaultRepObj->EvsIdBase  + Offset)) 

/*
** Type Definitions
*/

typedef struct
{

   uint16  WordIndex;
   uint16  Mask;

} FaultDetBitStruct;


/* 
** File Function Prototypes
*/

static boolean GetFaultDetIdBit(FaultRep_Class*      FaultRepObj,
                                const char*          CallerStr,
                                uint16               FaultDetId,
                                FaultDetBitStruct*   FaultDetBit);
                         
/*
** Function definitions
*/


/******************************************************************************
** Function: FaultRep_ClearFaultDetCmd
**
** Notes:
**    1. Must clear both the Software Bus report packet and NewReport.
*/
boolean FaultRep_ClearFaultDetCmd(      void*  CmdObjPtr, 
                                  const void*  CmdParamPtr)
{

   FaultRep_Class* FaultRepObj = (FaultRep_Class*)CmdObjPtr;
   FaultRep_ClearFaultDetCmdParam*  CmdParam = (FaultRep_ClearFaultDetCmdParam*)CmdParamPtr;

   boolean            RetStatus = TRUE;
   FaultDetBitStruct  FaultDetBit;

   if (CmdParam->FaultDetId == FAULTREP_SELECT_ALL)
   {

      CFE_PSP_MemSet(&(FaultRepObj->FaultDet.Latched),0,sizeof(FaultRepObj->FaultDet.Latched));

      for (FaultDetBit.WordIndex=0; FaultDetBit.WordIndex < FAULTREP_BITFIELD_WORDS; FaultDetBit.WordIndex++)
      {

         FaultRepObj->NewReport.Data[FaultDetBit.WordIndex] = 0;
         FaultRepObj->SbMsg.Tlm.Data[FaultDetBit.WordIndex] = 0;

      } /* End LatchIndex loop */

   } /* End if select all */

   else 
   {

      RetStatus = GetFaultDetIdBit(FaultRepObj,
                                   "Fault Reporter Rejected Clear Detector Cmd: ",
                                   CmdParam->FaultDetId,
                                   &FaultDetBit);
      
      if (RetStatus == TRUE)
      {

         FaultDetBit.Mask = (uint16)~FaultDetBit.Mask;

         FaultRepObj->FaultDet.Latched[FaultDetBit.WordIndex] &= FaultDetBit.Mask;

         FaultRepObj->NewReport.Data[FaultDetBit.WordIndex] &= FaultDetBit.Mask;
         FaultRepObj->SbMsg.Tlm.Data[FaultDetBit.WordIndex] &= FaultDetBit.Mask;
      
      } /* End if valid ID */

   } /* End if individual ID */


   return RetStatus;


} /* End FaultRep_ClearFaultDetCmd() */



/******************************************************************************
** Function: FaultRep_ConfigFaultDetCmd
**
** Notes:
**    None
*/
boolean FaultRep_ConfigFaultDetCmd(  void*      CmdObjPtr, 
                                   const void*  CmdParamPtr)
{

   FaultRep_Class* FaultRepObj = (FaultRep_Class*)CmdObjPtr;
   FaultRep_ConfigFaultDetCmdParam*  CmdParam = (FaultRep_ConfigFaultDetCmdParam*)CmdParamPtr;

   boolean  RetStatus = TRUE;

   FaultDetBitStruct  FaultDetBit;

   if (CmdParam->Enable == TRUE || CmdParam->Enable == FALSE)
   {

      if (CmdParam->FaultDetId == FAULTREP_SELECT_ALL) 
      {
         
         if (CmdParam->Enable)
         {
            for (FaultDetBit.WordIndex=0; FaultDetBit.WordIndex < FaultRepObj->FaultDet.BitfieldWords; FaultDetBit.WordIndex++)
               FaultRepObj->FaultDet.Enabled[FaultDetBit.WordIndex] = 0xFFFF;

            if (FaultRepObj->FaultDet.BitfieldWords < FAULTREP_BITFIELD_WORDS)
               FaultRepObj->FaultDet.Enabled[FaultRepObj->FaultDet.BitfieldWords] = FaultRepObj->FaultDet.BitfieldRemMask;

         }
         else
         {
            CFE_PSP_MemSet(&(FaultRepObj->FaultDet.Enabled),0,sizeof(FaultRepObj->FaultDet.Enabled));
         }

         
      } /* End if select all */
      
      else 
      {
         
         RetStatus = GetFaultDetIdBit(FaultRepObj,
                                      "Fault Reporter Reject Config Detector Cmd:",
                                      CmdParam->FaultDetId,
                                      &FaultDetBit);
         
         if (RetStatus == TRUE)
         {
            
            if (CmdParam->Enable)
               FaultRepObj->FaultDet.Enabled[FaultDetBit.WordIndex] |= FaultDetBit.Mask;
            
            else
               FaultRepObj->FaultDet.Enabled[FaultDetBit.WordIndex] &= ~FaultDetBit.Mask;
            
         } /* End if valid ID */
         
      } /* End if individual ID */
   } /* End if valid boolean range */
   else
   {

      CFE_EVS_SendEvent (EVS_ID(FAULTREP_EVS_CONFIG_CMD_ERR),
                         CFE_EVS_ERROR,
                         "Fault Reporter Reject Config Detector Cmd: Invalid enable value %d",
                         CmdParam->Enable);

      RetStatus = FALSE;

   } /* End if invalid boolean raange */
  

   return RetStatus;


} /* End FaultRep_ConfigFaultDetCmd() */


/******************************************************************************
** Function: FaultRep_Constructor
**
** Notes:
**    None
*/
void FaultRep_Constructor(FaultRep_Class*  FaultRepObj,
                          uint16           FaultIdCnt,
                          uint16*          EvsIdBase)
{

   uint16 RemBitCnt, i;

   /*
   ** Clear entire check structure which disables all detectors and
   ** clears their latch flags.
   */
   
   CFE_PSP_MemSet(FaultRepObj,0,sizeof(FaultRep_Class));

   FaultRepObj->FaultDet.IdLimit = FaultIdCnt;
   FaultRepObj->FaultDet.BitfieldWords = (uint16)(FaultIdCnt / FAULTREP_BITS_PER_WORD);
   
   RemBitCnt = (uint16)(FaultIdCnt % FAULTREP_BITS_PER_WORD);
   for (i=0; i < RemBitCnt; i++)
   {
      FaultRepObj->FaultDet.BitfieldRemMask |= 1 << i;
   }

   FaultRepObj->TlmMode = FAULTREP_NEW_REPORT;
      
   FaultRepObj->EvsIdBase = *EvsIdBase;
   *EvsIdBase += FAULTREP_EVS_MSG_CNT;

} /* End FaultRep_Constructor() */



/******************************************************************************
** Function: FaultRep_FaultDetFailed
**
** Notes:
**    1. Errors in this functions should only occur during integration. No
**       return status is provided because the caller always expects the call
**       to be successful.
*/
void FaultRep_FaultDetFailed(FaultRep_Class*  FaultRepObj,
                             uint16           FaultDetId)
{

   boolean            ValidFaultDetId;
   FaultDetBitStruct  FaultDetBit;

      
   ValidFaultDetId = GetFaultDetIdBit(FaultRepObj,
                                      "Fault Reporter Rejected Detector Failure:",
                                      FaultDetId,
                                      &FaultDetBit);
      
   if (ValidFaultDetId == TRUE)
   {

      if (FaultRepObj->FaultDet.Enabled[FaultDetBit.WordIndex] & FaultDetBit.Mask)
      {
         
         FaultRepObj->FaultDet.Latched[FaultDetBit.WordIndex] |= FaultDetBit.Mask;
            
         FaultRepObj->NewReport.Data[FaultDetBit.WordIndex] |= FaultDetBit.Mask;
            
      } /* End if enabled */
         
   } /* End if valid detector ID */


} /* End FaultRep_FaultDetFailed() */


/******************************************************************************
** Function: FaultRep_GenTlmMsg
**
** Notes:
**    1. Logic assumes FAULTDET_REPORT_MODE has two states.
**
*/
void FaultRep_GenTlmMsg(void* TlmObjPtr, CFE_SB_MsgPtr_t TlmMsgPtr)
{

   uint16  i;
   FaultRep_Class*  FaultRepObj = (FaultRep_Class*)TlmObjPtr;
   FaultRep_TlmMsg* FaultRepMsg = (FaultRep_TlmMsg*)TlmMsgPtr;

   /*
   ** Generate the FD report packet
   ** - Merge or copy NewTlm into the telemetry packet
   ** - Clear NewTlm for the next control cycle
   */

   if (FaultRepObj->TlmMode == FAULTREP_MERGE_REPORT)
   {

      for (i=0; i < FAULTREP_BITFIELD_WORDS; i++)
         FaultRepMsg->Tlm.Data[i] |= FaultRepObj->NewReport.Data[i];

   } /* End if FAULTREP_MERGE_REPORT */
   else 
   {

      CFE_PSP_MemCpy(&(FaultRepMsg->Tlm),&(FaultRepObj->NewReport),sizeof(FaultRep_Data));

   } /* End if FAULTREP_NEW_REPORT */

   CFE_PSP_MemSet(&(FaultRepObj->NewReport),0,sizeof(FaultRep_Data));


} /* End FaultRep_GenTlmMsg() */


/******************************************************************************
** Function: FaultRep_SetTlmMode
**
** Notes:
**    None
**
*/
void FaultRep_SetTlmMode(FaultRep_Class*   FaultRepObj,
                         FaultRep_TlmMode  TlmMode)
{


   FaultRepObj->TlmMode = TlmMode;


} /* End FaultRep_SetTlmMode() */



/******************************************************************************
** Function: GetFaultDetIdBit
**
** Notes:
**    1. If the ID is invalid (too big) then an event message is sent.
**
*/
static boolean GetFaultDetIdBit(FaultRep_Class*     FaultRepObj,
                                const char*         CallerStr,
                                uint16              FaultDetId,
                                FaultDetBitStruct*  FaultDetBit)
{

   boolean  RetStatus = TRUE;


   if (FaultDetId < FaultRepObj->FaultDet.IdLimit)
   {
   
      FaultDetBit->WordIndex = (uint16)(FaultDetId/FAULTREP_BITS_PER_WORD);
      FaultDetBit->Mask = (uint16)(1 << (uint16)(FaultDetId % FAULTREP_BITS_PER_WORD));
   
   }
   else
   {

      RetStatus = FALSE;
      CFE_EVS_SendEvent (EVS_ID(FAULTREP_EVS_INV_DETECTOR_ID),
                         CFE_EVS_ERROR,
                         "%s Invalid fault ID %d (Max ID = %d)",
                         CallerStr,
                         FaultDetId,
                         FaultRepObj->FaultDet.IdLimit-1);
   }


   return RetStatus;


} /* End GetFaultIdBit() */


/* end of file */
