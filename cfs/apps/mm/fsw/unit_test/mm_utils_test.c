 /*************************************************************************
 ** File:
 **   $Id: mm_utils_test.c 1.4.1.1 2016/11/17 16:39:10EST sstrege Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file mm_utils.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_utils_test.c  $
 **   Revision 1.4.1.1 2016/11/17 16:39:10EST sstrege 
 **   
 **   Revision 1.5 2016/11/17 12:51:16EST mdeschu 
 **   Remove references to temporary header file mm_test_exports.h
 **   Revision 1.4 2016/10/24 18:35:24EDT czogby 
 **   Code Walkthough Updates
 **   Revision 1.3 2016/05/04 19:01:14EDT czogby 
 **   Update MM to Compile With New UT-Assert PSP Files
 **   Revision 1.2 2016/03/04 20:00:58EST czogby 
 **   Need Temporary Header File to Import Some MM Functions Into Unit Tests
 **   Revision 1.1 2016/02/03 17:11:38EST czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "mm_utils_test.h"
#include "mm_utils.h"
#include "mm_msg.h"
#include "mm_msgdefs.h"
#include "mm_msgids.h"
#include "mm_events.h"
#include "mm_filedefs.h"
#include "mm_version.h"
#include "mm_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_psp_memrange_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

void MM_ResetHk_Test (void)
{
    MM_AppData.LastAction      = 1;
    MM_AppData.MemType         = 2;
    MM_AppData.Address         = 3;
    MM_AppData.DataValue       = 4;
    MM_AppData.BytesProcessed  = 5;
    MM_AppData.FileName[0]     = 6;

    /* Execute the function being tested */
    MM_ResetHk();

    /* Verify results */    
    UtAssert_True (MM_AppData.LastAction == MM_NOACTION, "MM_AppData.LastAction == MM_NOACTION");
    UtAssert_True (MM_AppData.MemType == MM_NOMEMTYPE, "MM_AppData.MemType == MM_NOMEMTYPE");
    UtAssert_True (MM_AppData.Address == MM_CLEAR_ADDR, "MM_AppData.Address == MM_CLEAR_ADDR");
    UtAssert_True (MM_AppData.DataValue == MM_CLEAR_PATTERN, "MM_AppData.DataValue == MM_CLEAR_PATTERN");
    UtAssert_True (MM_AppData.BytesProcessed == 0, "MM_AppData.BytesProcessed == 0");
    UtAssert_True (MM_AppData.FileName[0] == MM_CLEAR_FNAME, "MM_AppData.FileName[0] == MM_CLEAR_FNAME");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_ResetHk_Test */

void MM_VerifyCmdLength_Test_Nominal (void)
{
    boolean          Result;
    uint16           ExpectedLength = 80;
    MM_PeekCmd_t     CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_PEEK_CC);

    /* Execute the function being tested */
    Result = MM_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), ExpectedLength);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyCmdLength_Test_Nominal */

void MM_VerifyCmdLength_Test_HKRequestLengthError (void)
{
    boolean          Result;
    uint16           ExpectedLength = 99;
    MM_PeekCmd_t     CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_PEEK_CC);

    /* Set to satisfy condition "MessageID == MM_SEND_HK_MID" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, MM_SEND_HK_MID, 1);

    /* Execute the function being tested */
    Result = MM_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), ExpectedLength);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_HKREQ_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid HK request msg length: ID = 0x1889, CC = 2, Len = 80, Expected = 99"),
        "Invalid HK request msg length: ID = 0x1889, CC = 2, Len = 80, Expected = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyCmdLength_Test_HKRequestLengthError */

void MM_VerifyCmdLength_Test_LengthError (void)
{
    boolean          Result;
    uint16           ExpectedLength = 99;
    MM_PeekCmd_t     CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_PEEK_CC);

    /* Set to fail condition "MessageID == MM_SEND_HK_MID" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 5, 1);

    /* Execute the function being tested */
    Result = MM_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), ExpectedLength);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x0005, CC = 2, Len = 80, Expected = 99"),
        "Invalid msg length: ID = 0x0005, CC = 2, Len = 80, Expected = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyCmdLength_Test_LengthError */

void MM_VerifyPeekPokeParams_Test_ByteWidthRAM (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_RAM;
    uint8   SizeInBits = 8;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyPeekPokeParams_Test_ByteWidthRAM */

void MM_VerifyPeekPokeParams_Test_WordWidthMEM16 (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM16;
    uint8   SizeInBits = 16;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyPeekPokeParams_Test_WordWidthMEM16 */

void MM_VerifyPeekPokeParams_Test_DWordWidthMEM32 (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM32;
    uint8   SizeInBits = 32;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyPeekPokeParams_Test_DWordWidthMEM32 */

void MM_VerifyPeekPokeParams_Test_WordWidthAlignmentError (void)
{
    boolean Result;
    uint32  Address    = 1;
    uint8   MemType    = MM_MEM16;
    uint8   SizeInBits = 16;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR, "Data and address not 16 bit aligned: Addr = 0x00000001 Size = 2"),
        "Data and address not 16 bit aligned: Addr = 0x00000001 Size = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_WordWidthAlignmentError */

void MM_VerifyPeekPokeParams_Test_DWordWidthAlignmentError (void)
{
    boolean Result;
    uint32  Address    = 1;
    uint8   MemType    = MM_MEM32;
    uint8   SizeInBits = 32;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR, "Data and address not 32 bit aligned: Addr = 0x00000001 Size = 4"),
        "Data and address not 32 bit aligned: Addr = 0x00000001 Size = 4");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_DWordWidthAlignmentError */

void MM_VerifyPeekPokeParams_Test_EEPROM (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_EEPROM;
    uint8   SizeInBits = 8;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyPeekPokeParams_Test_EEPROM */

void MM_VerifyPeekPokeParams_Test_MEM8 (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM8;
    uint8   SizeInBits = 8;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyPeekPokeParams_Test_MEM8 */

void MM_VerifyPeekPokeParams_Test_RAMValidateRangeError (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_RAM;
    uint8   SizeInBits = 8;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_RAMValidateRangeError */

void MM_VerifyPeekPokeParams_Test_EEPROMValidateRangeError (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_EEPROM;
    uint8   SizeInBits = 8;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_EEPROMValidateRangeError */

void MM_VerifyPeekPokeParams_Test_MEM32ValidateRangeError (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM32;
    uint8   SizeInBits = 32;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 4 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 4 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_MEM32ValidateRangeError */

void MM_VerifyPeekPokeParams_Test_MEM16ValidateRangeError (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM16;
    uint8   SizeInBits = 16;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 2 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 2 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_MEM16ValidateRangeError */

void MM_VerifyPeekPokeParams_Test_MEM8ValidateRangeError (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM8;
    uint8   SizeInBits = 8;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_MEM8ValidateRangeError */

void MM_VerifyPeekPokeParams_Test_InvalidDataSize (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM8;
    uint8   SizeInBits = 55;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR, "Data size in bits invalid: Data Size = 55"),
        "Data size in bits invalid: Data Size = 55");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_InvalidDataSize */

void MM_VerifyPeekPokeParams_Test_MEM32InvalidDataSize (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM32;
    uint8   SizeInBits = 8;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR, "Data size in bits invalid: Data Size = 8"),
        "Data size in bits invalid: Data Size = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_MEM32InvalidDataSize */

void MM_VerifyPeekPokeParams_Test_MEM16InvalidDataSize (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM16;
    uint8   SizeInBits = 8;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR, "Data size in bits invalid: Data Size = 8"),
        "Data size in bits invalid: Data Size = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_MEM16InvalidDataSize */

void MM_VerifyPeekPokeParams_Test_MEM8InvalidDataSize (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = MM_MEM8;
    uint8   SizeInBits = 32;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR, "Data size in bits invalid: Data Size = 32"),
        "Data size in bits invalid: Data Size = 32");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_MEM8InvalidDataSize */

void MM_VerifyPeekPokeParams_Test_InvalidMemType (void)
{
    boolean Result;
    uint32  Address    = 0;
    uint8   MemType    = 99;
    uint8   SizeInBits = 8;

    /* Execute the function being tested */
    Result = MM_VerifyPeekPokeParams(Address, MemType, SizeInBits);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR, "Invalid memory type specified: MemType = 99"),
        "Invalid memory type specified: MemType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyPeekPokeParams_Test_InvalidMemType */

void MM_Utils_Test_AddTestCases(void)
{
    UtTest_Add(MM_ResetHk_Test, MM_Test_Setup, MM_Test_TearDown, "MM_ResetHk_Test");
    /* MM_SegmentBreak: Nothing to test, and code already covered by other tests */

    UtTest_Add(MM_VerifyCmdLength_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyCmdLength_Test_Nominal");
    UtTest_Add(MM_VerifyCmdLength_Test_HKRequestLengthError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyCmdLength_Test_HKRequestLengthError");
    UtTest_Add(MM_VerifyCmdLength_Test_LengthError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyCmdLength_Test_LengthError");

    UtTest_Add(MM_VerifyPeekPokeParams_Test_ByteWidthRAM, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_ByteWidthRAM");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_WordWidthMEM16, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_WordWidthMEM16");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_DWordWidthMEM32, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_DWordWidthMEM32");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_WordWidthAlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_WordWidthAlignmentError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_DWordWidthAlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_DWordWidthAlignmentError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_InvalidDataSize, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_InvalidDataSize");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_EEPROM");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM8");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_RAMValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_RAMValidateRangeError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_EEPROMValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_EEPROMValidateRangeError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM32ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM32ValidateRangeError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM32InvalidDataSize, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM32InvalidDataSize");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM16ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM16ValidateRangeError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM16InvalidDataSize, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM16InvalidDataSize");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM8ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM8ValidateRangeError");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_MEM8InvalidDataSize, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_MEM8InvalidDataSize");
    UtTest_Add(MM_VerifyPeekPokeParams_Test_InvalidMemType, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyPeekPokeParams_Test_InvalidMemType");
} /* end MM_Utils_Test_AddTestCases */


/************************/
/*  End of File Comment */
/************************/
