/*
 * File:
 *   $Id: $
 *
 * Purpose: This file contains a unit test cases for cmdmgr.c
 *
 * $Date: $
 * $Revision: $
 * $Log: $
 */

/*
 * Includes
 */

#include "common_types.h"
#include "ccsds.h"
#include "utassert.h"
#include "uttest.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "cmdmgr.h"

static CMDMGR_Class  UtCmdMgr;


/******************************************************************************
** Test Command Functions
**
*/

/* Use no parameter and parameter FCs to test as first and last FC boundary tests */

#define CMD_FC_NO_PARAM  0
#define CMD_FC_PARAM     (CMDMGR_CMD_FUNC_TOTAL-1)

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} UT_CmdNoParam;
typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   Param1;
   uint16   Param2;

} UT_CmdParam;
#define UT_CMD_PARAM_DATA_LEN  (sizeof(UtCmdParam) - CFE_SB_CMD_HDR_SIZE)

static  boolean UtCmdRetStatus = TRUE;
static  UT_CmdNoParam  UtCmdNoParam;
static  UT_CmdParam    UtCmdParam;

boolean UtNoParamCmd(const CFE_SB_MsgPtr_t MsgPtr)
{

   return UtCmdRetStatus;

} /* End UtNoParamCmd() */

boolean UtParamCmd(const CFE_SB_MsgPtr_t MsgPtr)
{

   return UtCmdRetStatus;

} /* End UtParamCmd() */


/******************************************************************************
** Event Hook
**
*/
static uint16  ExpectedEventID;
static uint16  ExpectedEventType;
static boolean DisplayEventText = FALSE;

/* Local function used to setup expected values for the callback function */
static void UtSetExpectedEvent(uint16 EventID, uint16 EventType, boolean DisplayEvent)
{

	ExpectedEventID = EventID;
	ExpectedEventType = EventType;
	DisplayEventText = DisplayEvent;

} /* End UtSetExpectedEvent() */

/* Callback function called by the UT library send event stub */
static int32 UtSendEventHook(uint16 EventID, uint16 EventType, char *EventText)
{

    UtAssert_True((strlen(EventText) < CFE_EVS_MAX_MESSAGE_LENGTH),"Event messsage too long");
    UtAssert_True(EventID == ExpectedEventID, "Event ID Error");
    UtAssert_True(EventType == ExpectedEventType, "Event Type Error");
    if (DisplayEventText)
    {
    	printf("Event %d Text: ", EventID); printf (EventText); printf("\n");
    }
    return CFE_SUCCESS;

} /* End UtSendEventHook() */

/******************************************************************************
** Packet & SB Utils and hooks
**
*/
static void UtInitCmdPkt(CFE_SB_MsgPtr_t MsgPtr, CFE_SB_MsgId_t MsgId, uint16 Length, uint8 FuncCode)
{
    CCSDS_CmdPkt_t    *CmdPktPtr = (CCSDS_CmdPkt_t *)MsgPtr;

	CCSDS_InitPkt ((CCSDS_PriHdr_t *)MsgPtr,(uint16)MsgId,Length,TRUE);
	CCSDS_WR_FC(CmdPktPtr->SecHdr,FuncCode);
	/* printf("FC = %d\n",CCSDS_RD_FC(CmdPktPtr->SecHdr)); */
    CCSDS_LoadCheckSum(CmdPktPtr);

} /* End UtInitCmdPkt() */

static boolean UtValidateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{

    CCSDS_CmdPkt_t    *CmdPktPtr = (CCSDS_CmdPkt_t *)MsgPtr;

    /* if msg type is teletry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return FALSE;
    }/* end if */

    return CCSDS_ValidCheckSum (CmdPktPtr);

} /* End UtValidateChecksum() */

static uint16 UtGetUserDataLength(CFE_SB_MsgPtr_t MsgPtr)
{

    CCSDS_CmdPkt_t  *CmdPktPtr = (CCSDS_CmdPkt_t *)MsgPtr;
	uint16 TotalMsgSize = CCSDS_RD_LEN(CmdPktPtr->PriHdr);

    return (TotalMsgSize - CFE_SB_CMD_HDR_SIZE);

} /* End UtGetUserDataLength() */

static uint16 UtGetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{

    CFE_SB_CmdHdr_t *CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    /* if msg type is telemetry or there is no secondary hdr, return 0 */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    return CCSDS_RD_FC(CmdHdrPtr->Sec);

} /* End UtGetCmdCode() */

static uint16 UtGetChecksum(CFE_SB_MsgPtr_t MsgPtr)
{

    CFE_SB_CmdHdr_t     *CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    return CCSDS_RD_CHECKSUM(CmdHdrPtr->Sec);

} /* End UtGetChecksum() */



/*
** Function Definitions
*/


/* CMDMGR_Test01  - Constructor, Unused Cmd */
void CMDMGR_Test01(void)
{

   int i;
   boolean CmdStatus;
   CMDMGR_CmdFuncPtr UnusedFuncPtr;

   /* printf("CMDMGR_Test01()\n"); */

   CFE_PSP_MemSet(&UtCmdMgr, 1, sizeof(CMDMGR_Class));

   CMDMGR_Constructor (&UtCmdMgr);

   UtAssert_True((UtCmdMgr.InvalidCmdCnt==0),"UtCmdMgr.InvalidCmdCnt==0");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt==0");

   /* Verify all set to the same function pointer and later verify it is the unused function pointer */
   UnusedFuncPtr = UtCmdMgr.Cmd[0].FuncPtr;
   for (i=0; i < CMDMGR_CMD_FUNC_TOTAL; i++)
   {
	   UtAssert_True((UtCmdMgr.Cmd[i].UserDataLen==0),"UtCmdMgr.Cmd[i].UserDataLen==0");
	   UtAssert_True((UtCmdMgr.Cmd[i].FuncPtr == UnusedFuncPtr),"UtCmdMgr.Cmd[i].FuncPtr = UnusedFuncPtr");
   }

   /*
    * Verify first & last function codes call the unused FC function
    * Call one beyond just to prove table goes to the max
    *
    */

   UtSetExpectedEvent(CMDMGR_DISPATCH_UNUSED_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,FALSE);

   UtInitCmdPkt((CFE_SB_MsgPtr_t)&UtCmdNoParam,0x1800,sizeof(UT_CmdNoParam),CMD_FC_NO_PARAM);
   /* printf("FC = %d\n",CCSDS_RD_FC(((CCSDS_CmdPkt_t*)&UtCmdNoParam)->SecHdr)); */
   CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdNoParam);
   UtAssert_True(CmdStatus==FALSE,"DispatchFunc - Unused function code");
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==1),"UtCmdMgr.InvalidCmdCnt==1");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt==0");

   UtInitCmdPkt((CFE_SB_MsgPtr_t)&UtCmdNoParam,0x1800,sizeof(UT_CmdNoParam),CMD_FC_PARAM);
   /* printf("FC = %d\n",CCSDS_RD_FC(((CCSDS_CmdPkt_t*)&UtCmdNoParam)->SecHdr)); */
   CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdNoParam);
   UtAssert_True(CmdStatus==FALSE,"DispatchFunc - Unused function code");
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==2),"UtCmdMgr.InvalidCmdCnt==2");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt==0");

   UtSetExpectedEvent(CMDMGR_DISPATCH_INVALID_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,FALSE);

   UtInitCmdPkt((CFE_SB_MsgPtr_t)&UtCmdNoParam,0x1800,sizeof(UT_CmdNoParam),CMDMGR_CMD_FUNC_TOTAL);
   /* printf("FC = %d\n",CCSDS_RD_FC(((CCSDS_CmdPkt_t*)&UtCmdNoParam)->SecHdr)); */
   CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdNoParam);
   UtAssert_True(CmdStatus==FALSE,"DispatchFunc - Unused function code");
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==3),"UtCmdMgr.InvalidCmdCnt==3");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt==0");

} /* End CMDMGR_Test01() */


/* CMDMGR_Test02  - Functional: Register, Dispatch, ResetStatus  */
void CMDMGR_Test02(void)
{

   int i;
   boolean CmdStatus;
   CMDMGR_CmdFuncPtr UnusedFuncPtr;

   printf("CMDMGR_Test02()\n");

   CMDMGR_Constructor (&UtCmdMgr);
   UnusedFuncPtr = UtCmdMgr.Cmd[0].FuncPtr;
   UtInitCmdPkt((CFE_SB_MsgPtr_t)&UtCmdNoParam,0x1800,sizeof(UT_CmdNoParam),CMD_FC_NO_PARAM);
   UtInitCmdPkt((CFE_SB_MsgPtr_t)&UtCmdParam,0x1801,sizeof(UT_CmdParam),CMD_FC_PARAM);
   UtCmdParam.Param1 = 1;
   UtCmdParam.Param2 = 2;
   CCSDS_LoadCheckSum((CCSDS_CmdPkt_t*)&UtCmdParam);  /* Clumsy but not worth rewriting the cmd init function */

   CMDMGR_RegisterFunc(CMD_FC_NO_PARAM, UtNoParamCmd, 0);
   CMDMGR_RegisterFunc(CMD_FC_PARAM, UtParamCmd,UT_CMD_PARAM_DATA_LEN);
   UtAssert_True((UtCmdMgr.Cmd[CMD_FC_NO_PARAM].FuncPtr == UtNoParamCmd), "UtCmdMgr.Cmd[CMD_FC_NO_PARAM].FuncPtr == UtNoParamCmd");
   UtAssert_True((UtCmdMgr.Cmd[CMD_FC_NO_PARAM].UserDataLen == 0), "UtCmdMgr.Cmd[CMD_FC_NO_PARAM].UserDataLen == 0");
   UtAssert_True((UtCmdMgr.Cmd[CMD_FC_PARAM].FuncPtr == UtParamCmd), "UtCmdMgr.Cmd[CMD_FC_PARAM].FuncPtr == UtParamCmd");
   UtAssert_True((UtCmdMgr.Cmd[CMD_FC_PARAM].UserDataLen == UT_CMD_PARAM_DATA_LEN), "UtCmdMgr.Cmd[CMD_FC_PARAM].UserDataLen == UT_CMD_PARAM_DATA_LEN");

   UtSetExpectedEvent(CMDMGR_REG_INVALID_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,FALSE);
   CMDMGR_RegisterFunc(CMDMGR_CMD_FUNC_TOTAL, UtNoParamCmd, 0);


   /*
   ** Verify all function code slots
   */

   for (i=0; i < CMDMGR_CMD_FUNC_TOTAL; i++)
   {
      if (i== CMD_FC_NO_PARAM)
      {
         CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdNoParam);
         UtAssert_True(CmdStatus,"DispatchFunc - No param function code");
      }
      else if (i== CMD_FC_PARAM)
      {
          CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdParam);
          UtAssert_True(CmdStatus,"DispatchFunc - Param function code");
      }
      else
      {
         /* This assumes the no param FC is 0 because it clobbers the command packet */
         UtInitCmdPkt((CFE_SB_MsgPtr_t)&UtCmdNoParam,0x1800,sizeof(UT_CmdNoParam),i);
         UtSetExpectedEvent(CMDMGR_DISPATCH_UNUSED_FUNC_CODE_ERR_EID, CFE_EVS_ERROR,FALSE);
         CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdNoParam);
         UtAssert_True(CmdStatus==FALSE,"DispatchFunc - Unused function code");
      }
   }
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==(CMDMGR_CMD_FUNC_TOTAL-2)),"UtCmdMgr.InvalidCmdCnt");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==2),"UtCmdMgr.ValidCmdCnt");

   /* Verify reset function */
   CMDMGR_ResetStatus();
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==0),"UtCmdMgr.InvalidCmdCnt after reset");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt after reset");

   /* Verify Dispatch error paths. Already did invalid FC in test case 1 so just do checksum and length errors */

   UtCmdParam.Param1 = 3; /* Make checksum invalid */
   UtSetExpectedEvent(CMDMGR_DISPATCH_INVALID_CHECKSUM_ERR_EID, CFE_EVS_ERROR,FALSE);
   CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdParam);
   UtAssert_True(CmdStatus==FALSE,"DispatchFunc - Param function code");
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==1),"UtCmdMgr.InvalidCmdCnt after reset");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt after reset");

   CCSDS_WR_LEN(((CCSDS_CmdPkt_t *)&UtCmdParam)->PriHdr,7);
   CCSDS_LoadCheckSum((CCSDS_CmdPkt_t*)&UtCmdParam);
   UtSetExpectedEvent(CMDMGR_DISPATCH_INVALID_LEN_ERR_EID, CFE_EVS_ERROR,FALSE);
   CmdStatus = CMDMGR_DispatchFunc((const CFE_SB_MsgPtr_t)&UtCmdParam);
   UtAssert_True(CmdStatus==FALSE,"DispatchFunc - Param function code");
   UtAssert_True((UtCmdMgr.InvalidCmdCnt==2),"UtCmdMgr.InvalidCmdCnt after reset");
   UtAssert_True((UtCmdMgr.ValidCmdCnt==0),"UtCmdMgr.ValidCmdCnt after reset");

} /* End CMDMGR_Test02() */


/* Initialize test environment to default state for every test */
void CMDMGR_Setup(void)
{

   Ut_CFE_EVS_SetFunctionHook(UT_CFE_EVS_SENDEVENT_INDEX,(void *)UtSendEventHook);
   Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_VALIDATECHECKSUM_INDEX,(void *)UtValidateChecksum);
   Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETCMDCODE_INDEX,(void *)UtGetCmdCode);
   Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETUSERDATALENGTH_INDEX,(void *)UtGetUserDataLength);
   Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETCHECKSUM_INDEX,(void *)UtGetChecksum);

} /* End CMDMG_Setup() */

void CMDMGR_TearDown(void)
{
    /* cleanup test environment */
}

void CMDMGR_AddTestCase(void)
{
    UtTest_Add(CMDMGR_Test01, CMDMGR_Setup, CMDMGR_TearDown, "CMDMGR_Test01 - Constructor");
    UtTest_Add(CMDMGR_Test02, CMDMGR_Setup, CMDMGR_TearDown, "CMDMGR_Test02 - TBD");
}
