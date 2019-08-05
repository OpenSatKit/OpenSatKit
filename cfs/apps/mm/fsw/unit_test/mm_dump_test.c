 /*************************************************************************
 ** File:
 **   $Id: mm_dump_test.c 1.6.1.1 2016/11/17 16:35:29EST sstrege Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file mm_dump.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_dump_test.c  $
 **   Revision 1.6.1.1 2016/11/17 16:35:29EST sstrege 
 **   
 **   Revision 1.7 2016/11/17 12:51:16EST mdeschu 
 **   Remove references to temporary header file mm_test_exports.h
 **   Revision 1.6 2016/10/30 08:58:28EDT mdeschu 
 **   Updated Unit test MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooLarge 
 **   
 **   Babelfish Ticket #32 (DCR 145938) changes default maximum number of bytes
 **   (MM_MAX_DUMP_INEVENT_BYTES) from 16 to 15
 **   Revision 1.5 2016/10/30 01:29:15EDT mdeschu 
 **   in MM_DumpMemToFile_Test_WriteError Set expected to be SegmentSize in relation to fix in DCR 141232
 **   Revision 1.4 2016/10/24 18:35:08EDT czogby 
 **   Code Walkthough Updates
 **   Revision 1.3 2016/05/04 19:01:15EDT czogby 
 **   Update MM to Compile With New UT-Assert PSP Files
 **   Revision 1.2 2016/03/04 20:00:58EST czogby 
 **   Need Temporary Header File to Import Some MM Functions Into Unit Tests
 **   Revision 1.1 2016/02/03 17:11:36EST czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "mm_dump_test.h"
#include "mm_dump.h"
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
#include "ut_cfe_psp_ram_stubs.h"
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

void MM_PeekCmd_Test_Nominal (void)
{
    MM_PeekCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_PEEK_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = 32;

    /* Execute the function being tested */
    MM_PeekCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekCmd_Test_Nominal */

void MM_PeekCmd_Test_SymNameError (void)
{
    MM_PeekCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_PEEK_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = 32;

    strncpy(CmdPacket.SrcSymAddress.SymName, "name", OS_MAX_PATH_LEN);

    /* Set to generate error message MM_SYMNAME_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PeekCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekCmd_Test_SymNameError */

void MM_PeekMem_Test_Byte (void)
{
    MM_PeekCmd_t CmdPacket;
    uint32       SrcAddress = 1;

    CmdPacket.DataSize = MM_BYTE_BIT_WIDTH;
    CmdPacket.MemType  = MM_RAM;

    /* Execute the function being tested */
    MM_PeekMem(&CmdPacket, SrcAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PEEK_BYTE_INF_EID, CFE_EVS_INFORMATION, "Peek Command: Addr = 0x00000001 Size = 8 bits Data = 0x00"),
        "Peek Command: Addr = 0x00000001 Size = 8 bits Data = 0x00");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_PEEK, "MM_AppData.LastAction == MM_PEEK");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == 1, "MM_AppData.Address == 1");
    UtAssert_True (MM_AppData.BytesProcessed == 1, "MM_AppData.BytesProcessed == 1");
    UtAssert_True (MM_AppData.DataValue == 0, "MM_AppData.DataValue == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekMem_Test_Byte */

void MM_PeekMem_Test_ByteError (void)
{
    MM_PeekCmd_t CmdPacket;
    uint32       SrcAddress = 1;

    CmdPacket.DataSize = MM_BYTE_BIT_WIDTH;

    /* Set to generate error message MM_PSP_READ_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD8_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PeekMem(&CmdPacket, SrcAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, "PSP read memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM8"),
        "PSP read memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekMem_Test_ByteError */

void MM_PeekMem_Test_Word (void)
{
    MM_PeekCmd_t CmdPacket;
    uint32       SrcAddress = 1;

    CmdPacket.DataSize = MM_WORD_BIT_WIDTH;
    CmdPacket.MemType  = MM_RAM;

    /* Execute the function being tested */
    MM_PeekMem(&CmdPacket, SrcAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PEEK_WORD_INF_EID, CFE_EVS_INFORMATION, "Peek Command: Addr = 0x00000001 Size = 16 bits Data = 0x0000"),
        "Peek Command: Addr = 0x00000001 Size = 16 bits Data = 0x0000");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_PEEK, "MM_AppData.LastAction == MM_PEEK");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == 1, "MM_AppData.Address == 1");
    UtAssert_True (MM_AppData.BytesProcessed == 2, "MM_AppData.BytesProcessed == 2");
    UtAssert_True (MM_AppData.DataValue == 0, "MM_AppData.DataValue == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekMem_Test_Word */

void MM_PeekMem_Test_WordError (void)
{
    MM_PeekCmd_t CmdPacket;
    uint32       SrcAddress = 1;

    CmdPacket.DataSize = MM_WORD_BIT_WIDTH;

    /* Set to generate error message MM_PSP_READ_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD16_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PeekMem(&CmdPacket, SrcAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, "PSP read memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16"),
        "PSP read memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekMem_Test_WordError */

void MM_PeekMem_Test_DWord (void)
{
    MM_PeekCmd_t CmdPacket;
    uint32       SrcAddress = 1;

    CmdPacket.DataSize = MM_DWORD_BIT_WIDTH;
    CmdPacket.MemType  = MM_RAM;

    /* Execute the function being tested */
    MM_PeekMem(&CmdPacket, SrcAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PEEK_DWORD_INF_EID, CFE_EVS_INFORMATION, "Peek Command: Addr = 0x00000001 Size = 32 bits Data = 0x00000000"),
        "Peek Command: Addr = 0x00000001 Size = 32 bits Data = 0x00000000");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_PEEK, "MM_AppData.LastAction == MM_PEEK");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == 1, "MM_AppData.Address == 1");
    UtAssert_True (MM_AppData.BytesProcessed == 4, "MM_AppData.BytesProcessed == 4");
    UtAssert_True (MM_AppData.DataValue == 0, "MM_AppData.DataValue == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekMem_Test_DWord */

void MM_PeekMem_Test_DWordError (void)
{
    MM_PeekCmd_t CmdPacket;
    uint32       SrcAddress = 1;

    CmdPacket.DataSize = MM_DWORD_BIT_WIDTH;

    /* Set to generate error message MM_PSP_READ_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD32_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PeekMem(&CmdPacket, SrcAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, "PSP read memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM32"),
        "PSP read memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM32");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PeekMem_Test_DWordError */

void MM_DumpMemToFileCmd_Test_RAM (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_RAM;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_RAM */

void MM_DumpMemToFileCmd_Test_EEPROM (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_EEPROM */

void MM_DumpMemToFileCmd_Test_MEM32 (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM32;
    CmdPacket.NumOfBytes = 4;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 4 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 4 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_MEM32 */

void MM_DumpMemToFileCmd_Test_MEM16 (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM16;
    CmdPacket.NumOfBytes = 2;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 2 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 2 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_MEM16 */

void MM_DumpMemToFileCmd_Test_MEM8 (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_MEM8 */

void MM_DumpMemToFileCmd_Test_ComputeCRCError (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Set to generate error message MM_DMP_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CFS_COMPUTECRCFROMFILE_ERR_EID, CFE_EVS_ERROR, "CFS_ComputeCRCFromFile error received: RC = 0xFFFFFFFF File = 'filename'"),
        "CFS_ComputeCRCFromFile error received: RC = 0xFFFFFFFF File = 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MM_DumpMemToFileCmd_Test_ComputeCRCError */

void MM_DumpMemToFileCmd_Test_CloseError (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Set to generate error message MM_DMP_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CLOSE_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'"),
        "Dump Memory To File Command: Dumped 1 bytes from address 0x00000000 to file 'filename'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_CLOSE_ERR_EID, CFE_EVS_ERROR, "OS_close error received: RC = 0xFFFFFFFF File = 'filename'"),
        "OS_close error received: RC = 0xFFFFFFFF File = 'filename'");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MM_DumpMemToFileCmd_Test_CloseError */

void MM_DumpMemToFileCmd_Test_CreatError (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Set to generate error message MM_OS_CREAT_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_CREAT_ERR_EID, CFE_EVS_ERROR, "OS_creat error received: RC = 0xFFFFFFFF File = 'filename'"),
        "OS_creat error received: RC = 0xFFFFFFFF File = 'filename'");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_CreatError */

void MM_DumpMemToFileCmd_Test_SymNameError (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Set to generate error message MM_SYMNAME_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'SymName'"),
        "Symbolic address can't be resolved: Name = 'SymName'");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_SymNameError */

void MM_DumpMemToFileCmd_Test_FilenameError (void)
{
    MM_DumpMemToFileCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    strncpy(CmdPacket.SrcSymAddress.SymName, "SymName", OS_MAX_PATH_LEN);
    CmdPacket.SrcSymAddress.Offset = 0;

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy (CmdPacket.FileName, "***", OS_MAX_PATH_LEN);

    /* Set to satisfy 2 instances of condition "Valid == TRUE": after comment "Write the file headers" and comment "end Valid == TRUE if" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);
    Ut_CFE_FS_ContinueReturnCodeAfterCountZero(UT_CFE_FS_WRITEHDR_INDEX);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(MM_LoadDumpFileHeader_t), 1);

    /* Set to satisfy condition "Valid == TRUE" before comment "Compute CRC of dumped data" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Set to generate error message MM_CMD_FNAME_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_DumpMemToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CMD_FNAME_ERR_EID, CFE_EVS_ERROR, "Command specified filename invalid: Name = '***'"),
        "Command specified filename invalid: Name = '***'");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFileCmd_Test_FilenameError */

void MM_DumpMemToFile_Test_Nominal (void)
{
    boolean Result;

    uint32                  FileHandle = 1;
    char                    FileName[OS_MAX_PATH_LEN];
    MM_LoadDumpFileHeader_t FileHeader;

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    FileHeader.NumOfBytes = 1;
    FileHeader.SymAddress.Offset = 1;
    FileHeader.MemType = MM_RAM;

    /* Execute the function being tested */
    Result = MM_DumpMemToFile(FileHandle, FileName, &FileHeader);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (MM_AppData.MemType == FileHeader.MemType, "MM_AppData.MemType == FileHeader.MemType");
    UtAssert_True (MM_AppData.Address == FileHeader.SymAddress.Offset, "MM_AppData.Address == FileHeader.SymAddress.Offset");
    UtAssert_True (MM_AppData.BytesProcessed == 1, "MM_AppData.BytesProcessed == 1");
    UtAssert_True (strncmp(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_DumpMemToFile_Test_Nominal */

void MM_DumpMemToFile_Test_CPUHogging (void)
{
    boolean Result;

    uint32                  FileHandle = 1;
    char                    FileName[OS_MAX_PATH_LEN];
    MM_LoadDumpFileHeader_t FileHeader;

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    FileHeader.NumOfBytes = 2 * MM_MAX_DUMP_DATA_SEG;
    FileHeader.SymAddress.Offset = 1;
    FileHeader.MemType = MM_RAM;

    /* Execute the function being tested */
    Result = MM_DumpMemToFile(FileHandle, FileName, &FileHeader);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (MM_AppData.MemType == FileHeader.MemType, "MM_AppData.MemType == FileHeader.MemType");
    UtAssert_True (MM_AppData.Address == FileHeader.SymAddress.Offset, "MM_AppData.Address == FileHeader.SymAddress.Offset");
    UtAssert_True (MM_AppData.BytesProcessed == 2 * MM_MAX_DUMP_DATA_SEG, "MM_AppData.BytesProcessed == 2 * MM_MAX_DUMP_DATA_SEG");
    UtAssert_True (strncmp(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_DumpMemToFile_Test_CPUHogging */

void MM_DumpMemToFile_Test_WriteError (void)
{
    boolean Result;

    uint32                  FileHandle = 1;
    char                    FileName[OS_MAX_PATH_LEN];
    MM_LoadDumpFileHeader_t FileHeader;

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    FileHeader.NumOfBytes = 1;
    FileHeader.SymAddress.Offset = 1;
    FileHeader.MemType = MM_RAM;

    /* Set to generate error message MM_OS_WRITE_EXP_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_DumpMemToFile(FileHandle, FileName, &FileHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_WRITE_EXP_ERR_EID, CFE_EVS_ERROR, "OS_write error received: RC = 0xFFFFFFFF, Expected = 1, File = 'filename'"),
        "OS_write error received: RC = 0xFFFFFFFF, Expected = 1, File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFile_Test_WriteError */

void MM_DumpMemToFile_Test_CopyError (void)
{
    boolean Result;

    uint32                  FileHandle = 1;
    char                    FileName[OS_MAX_PATH_LEN];
    MM_LoadDumpFileHeader_t FileHeader;
    char ErrorMessage[125];

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    FileHeader.NumOfBytes = 1;
    FileHeader.SymAddress.Offset = 1;
    FileHeader.MemType = MM_RAM;

    /* Set to generate error message MM_PSP_COPY_ERR_EID */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_DumpMemToFile(FileHandle, FileName, &FileHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP copy memory error: RC=0xFFFFFFFF, Src=0x00000001, Tgt=0x%08X, Size=0x00000001", (unsigned int)(&MM_AppData.DumpBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMemToFile_Test_CopyError */

void MM_VerifyFileDumpParams_Test_RAM (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_RAM;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyFileDumpParams_Test_RAM */

void MM_VerifyFileDumpParams_Test_EEPROM (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_EEPROM;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyFileDumpParams_Test_EEPROM */

void MM_VerifyFileDumpParams_Test_MEM32 (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM32;
    uint32 SizeInBytes = 4;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyFileDumpParams_Test_MEM32 */

void MM_VerifyFileDumpParams_Test_MEM16 (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM16;
    uint32 SizeInBytes = 2;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyFileDumpParams_Test_MEM16 */

void MM_VerifyFileDumpParams_Test_MEM8 (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM8;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyFileDumpParams_Test_MEM8 */

void MM_VerifyFileDumpParams_Test_RAMRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_RAM;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_RAMRangeError */

void MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_RAM;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooSmall */

void MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_RAM;
    uint32 SizeInBytes = MM_MAX_DUMP_FILE_DATA_RAM + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooLarge */

void MM_VerifyFileDumpParams_Test_EEPROMRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_EEPROM;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_EEPROMRangeError */

void MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_EEPROM;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooSmall */

void MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_EEPROM;
    uint32 SizeInBytes = MM_MAX_DUMP_FILE_DATA_EEPROM + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 131073"),
        "Data size in bytes invalid or exceeds limits: Data Size = 131073");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooLarge */

void MM_VerifyFileDumpParams_Test_MEM32RangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM32;
    uint32 SizeInBytes = 4;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 4 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 4 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM32RangeError */

void MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM32;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooSmall */

void MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM32;
    uint32 SizeInBytes = MM_MAX_DUMP_FILE_DATA_MEM32 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooLarge */

void MM_VerifyFileDumpParams_Test_MEM32AlignmentError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM32;
    uint32 SizeInBytes = 3;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR, "Data and address not 32 bit aligned: Addr = 0x00000000 Size = 3"),
        "Data and address not 32 bit aligned: Addr = 0x00000000 Size = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM32AlignmentError */

void MM_VerifyFileDumpParams_Test_MEM16RangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM16;
    uint32 SizeInBytes = 2;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 2 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 2 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM16RangeError */

void MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM16;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooSmall */

void MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM16;
    uint32 SizeInBytes = MM_MAX_DUMP_FILE_DATA_MEM16 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooLarge */

void MM_VerifyFileDumpParams_Test_MEM16AlignmentError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM16;
    uint32 SizeInBytes = 3;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR, "Data and address not 16 bit aligned: Addr = 0x00000000 Size = 3"),
        "Data and address not 16 bit aligned: Addr = 0x00000000 Size = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM16AlignmentError */

void MM_VerifyFileDumpParams_Test_MEM8RangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM8;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM8RangeError */

void MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM8;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooSmall */

void MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = MM_MEM8;
    uint32 SizeInBytes = MM_MAX_DUMP_FILE_DATA_MEM8 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooLarge */

void MM_VerifyFileDumpParams_Test_InvalidMemoryType (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8  MemType = 99;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileDumpParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR, "Invalid memory type specified: MemType = 99"),
        "Invalid memory type specified: MemType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileDumpParams_Test_InvalidMemoryType */

void MM_WriteFileHeaders_Test_Nominal (void)
{
    boolean Result;

    char                     FileName[OS_MAX_PATH_LEN];
    int32                    FileHandle = 1;
    CFE_FS_Header_t          CFEHeader;
    MM_LoadDumpFileHeader_t  MMHeader;

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    MMHeader.NumOfBytes = 1;
    MMHeader.SymAddress.Offset = 0;
    MMHeader.MemType = MM_RAM;

    /* Set to fail condition "OS_Status != sizeof(CFE_FS_Header_t)" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Execute the function being tested */
    Result = MM_WriteFileHeaders(FileName, FileHandle, &CFEHeader, &MMHeader);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_WriteFileHeaders_Test_Nominal */

void MM_WriteFileHeaders_Test_WriteHeaderError (void)
{
    boolean Result;

    char                     FileName[OS_MAX_PATH_LEN];
    int32                    FileHandle = 1;
    CFE_FS_Header_t          CFEHeader;
    MM_LoadDumpFileHeader_t  MMHeader;

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    MMHeader.NumOfBytes = 1;
    MMHeader.SymAddress.Offset = 0;
    MMHeader.MemType = MM_RAM;

    /* Set to generate error message MM_CFE_FS_WRITEHDR_ERR_EID */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_WriteFileHeaders(FileName, FileHandle, &CFEHeader, &MMHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CFE_FS_WRITEHDR_ERR_EID, CFE_EVS_ERROR, "CFE_FS_WriteHeader error received: RC = 0xFFFFFFFF Expected = 64 File = 'filename'"),
        "CFE_FS_WriteHeader error received: RC = 0xFFFFFFFF Expected = 64 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_WriteFileHeaders_Test_WriteHeaderError */

void MM_WriteFileHeaders_Test_WriteError (void)
{
    boolean Result;

    char                     FileName[OS_MAX_PATH_LEN];
    int32                    FileHandle = 1;
    CFE_FS_Header_t          CFEHeader;
    MM_LoadDumpFileHeader_t  MMHeader;

    strncpy (FileName, "filename", OS_MAX_PATH_LEN);

    MMHeader.NumOfBytes = 1;
    MMHeader.SymAddress.Offset = 0;
    MMHeader.MemType = MM_RAM;

    /* Set to fail condition "OS_Status != sizeof(CFE_FS_Header_t)" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Set to generate error message MM_OS_WRITE_EXP_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_WriteFileHeaders(FileName, FileHandle, &CFEHeader, &MMHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_WRITE_EXP_ERR_EID, CFE_EVS_ERROR, "OS_write error received: RC = 0xFFFFFFFF Expected = 80 File = 'filename'"),
        "OS_write error received: RC = 0xFFFFFFFF Expected = 80 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_WriteFileHeaders_Test_WriteError */

void MM_DumpInEventCmd_Test_Nominal (void)
{
    MM_DumpInEventCmd_t  MessagePtr;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_RAM;
    MessagePtr.NumOfBytes = 1;
    MessagePtr.SrcSymAddress.Offset = 0;

    strncpy(MessagePtr.SrcSymAddress.SymName, "", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "Valid == TRUE" before comment "Prepare event message string header" */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_DumpInEventCmd((CFE_SB_MsgPtr_t)(&MessagePtr));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DUMP_INEVENT_INF_EID, CFE_EVS_INFORMATION, "Memory Dump: 0x00 from address: 0x00000000"),
        "Memory Dump: 0x00 from address: 0x00000000");

    UtAssert_True (MM_AppData.LastAction == MM_DUMP_INEVENT, "MM_AppData.LastAction == MM_DUMP_INEVENT");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == 1, "MM_AppData.BytesProcessed == 1");
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpInEventCmd_Test_Nominal */

void MM_DumpInEventCmd_Test_SymNameError (void)
{
    MM_DumpInEventCmd_t  MessagePtr;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_RAM;
    MessagePtr.NumOfBytes = 1;
    MessagePtr.SrcSymAddress.Offset = 0;

    strncpy(MessagePtr.SrcSymAddress.SymName, "name", OS_MAX_PATH_LEN);

    /* Set to generate error message MM_SYMNAME_ERR_EID  */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_DumpInEventCmd((CFE_SB_MsgPtr_t)(&MessagePtr));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpInEventCmd_Test_SymNameError */

void MM_VerifyDumpInEventParams_Test_RAM (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_RAM;
    uint32  SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyDumpInEventParams_Test_RAM */

void MM_VerifyDumpInEventParams_Test_EEPROM (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_EEPROM;
    uint32  SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyDumpInEventParams_Test_EEPROM */

void MM_VerifyDumpInEventParams_Test_MEM32 (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM32;
    uint32  SizeInBytes = 4;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyDumpInEventParams_Test_MEM32 */

void MM_VerifyDumpInEventParams_Test_MEM16 (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM16;
    uint32  SizeInBytes = 2;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyDumpInEventParams_Test_MEM16 */

void MM_VerifyDumpInEventParams_Test_MEM8 (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM8;
    uint32  SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyDumpInEventParams_Test_MEM8 */

void MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooSmall (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM8;
    uint32  SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooSmall */

void MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooLarge (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM8;
    uint32  SizeInBytes = MM_MAX_DUMP_INEVENT_BYTES + 1;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 16"),
        "Data size in bytes invalid or exceeds limits: Data Size = 16");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooLarge */

void MM_VerifyDumpInEventParams_Test_RAMRangeError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_RAM;
    uint32  SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_RAMRangeError */

void MM_VerifyDumpInEventParams_Test_EEPROMRangeError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_EEPROM;
    uint32  SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_EEPROMRangeError */

void MM_VerifyDumpInEventParams_Test_MEM32RangeError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM32;
    uint32  SizeInBytes = 4;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 4 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 4 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_MEM32RangeError */

void MM_VerifyDumpInEventParams_Test_MEM32AlignmentError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM32;
    uint32  SizeInBytes = 3;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR, "Data and address not 32 bit aligned: Addr = 0x00000000 Size = 3"),
        "Data and address not 32 bit aligned: Addr = 0x00000000 Size = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_MEM32AlignmentError */

void MM_VerifyDumpInEventParams_Test_MEM16RangeError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM16;
    uint32  SizeInBytes = 2;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 2 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 2 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_MEM16RangeError */

void MM_VerifyDumpInEventParams_Test_MEM16AlignmentError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM16;
    uint32  SizeInBytes = 3;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR, "Data and address not 16 bit aligned: Addr = 0x00000000 Size = 3"),
        "Data and address not 16 bit aligned: Addr = 0x00000000 Size = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_MEM16AlignmentError */

void MM_VerifyDumpInEventParams_Test_MEM8RangeError (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = MM_MEM8;
    uint32  SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_MEM32RangeError */

void MM_VerifyDumpInEventParams_Test_InvalidMemType (void)
{
    boolean Result;
    uint32  Address = 0;
    uint8   MemType = 99;
    uint32  SizeInBytes = 3;

    /* Execute the function being tested */
    Result = MM_VerifyDumpInEventParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR, "Invalid memory type specified: MemType = 99"),
        "Invalid memory type specified: MemType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyDumpInEventParams_Test_InvalidMemType */

void MM_FillDumpInEventBuffer_Test_RAM (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_RAM;
    MessagePtr.NumOfBytes = 1;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillDumpInEventBuffer_Test_RAM */

void MM_FillDumpInEventBuffer_Test_EEPROM (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_EEPROM;
    MessagePtr.NumOfBytes = 1;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillDumpInEventBuffer_Test_EEPROM */

void MM_FillDumpInEventBuffer_Test_MEM32 (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_MEM32;
    MessagePtr.NumOfBytes = 4;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillDumpInEventBuffer_Test_MEM32 */

void MM_FillDumpInEventBuffer_Test_MEM16 (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_MEM16;
    MessagePtr.NumOfBytes = 2;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillDumpInEventBuffer_Test_MEM16 */

void MM_FillDumpInEventBuffer_Test_MEM8 (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_MEM8;
    MessagePtr.NumOfBytes = 1;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillDumpInEventBuffer_Test_MEM8 */

void MM_FillDumpInEventBuffer_Test_EEPROMCopyError (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;
    char ErrorMessage[125];

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_EEPROM;
    MessagePtr.NumOfBytes = 1;
    MessagePtr.SrcSymAddress.Offset = 0;

    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP copy memory error: RC=0xFFFFFFFF, Src=0x00000000, Tgt=0x%08X, Size=0x00000001", (unsigned int)(&MM_AppData.DumpBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillDumpInEventBuffer_Test_EEPROMCopyError */

void MM_FillDumpInEventBuffer_Test_MEM32ReadError (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;
    char ErrorMessage[125];

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_MEM32;
    MessagePtr.NumOfBytes = 4;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Set to generate error message MM_PSP_READ_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD32_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP read memory error: RC=0xFFFFFFFF, Src=0x00000000, Tgt=0x%08X, Type=MEM32", (unsigned int)(&MM_AppData.DumpBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillDumpInEventBuffer_Test_MEM32ReadError */

void MM_FillDumpInEventBuffer_Test_MEM16ReadError (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;
    char ErrorMessage[125];

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_MEM16;
    MessagePtr.NumOfBytes = 4;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Set to generate error message MM_PSP_READ_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD16_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP read memory error: RC=0xFFFFFFFF, Src=0x00000000, Tgt=0x%08X, Type=MEM16", (unsigned int)(&MM_AppData.DumpBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillDumpInEventBuffer_Test_MEM16ReadError */

void MM_FillDumpInEventBuffer_Test_MEM8ReadError (void)
{
    boolean Result;
    MM_DumpInEventCmd_t  MessagePtr;
    uint32  SrcAddress = 0;
    char ErrorMessage[125];

    CFE_SB_InitMsg (&MessagePtr, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&MessagePtr, MM_DUMP_IN_EVENT_CC);

    MessagePtr.MemType = MM_MEM8;
    MessagePtr.NumOfBytes = 4;
    MessagePtr.SrcSymAddress.Offset = 0;

    /* Set to generate error message MM_PSP_READ_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD8_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_FillDumpInEventBuffer(SrcAddress, &MessagePtr, (uint8 *)(&MM_AppData.DumpBuffer[0]));

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP read memory error: RC=0xFFFFFFFF, Src=0x00000000, Tgt=0x%08X, Type=MEM8", (unsigned int)(&MM_AppData.DumpBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillDumpInEventBuffer_Test_MEM8ReadError */

void MM_Dump_Test_AddTestCases(void)
{
    UtTest_Add(MM_PeekCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_PeekCmd_Test_Nominal");
    UtTest_Add(MM_PeekCmd_Test_SymNameError, MM_Test_Setup, MM_Test_TearDown, "MM_PeekCmd_Test_SymNameError");
    UtTest_Add(MM_PeekMem_Test_Byte, MM_Test_Setup, MM_Test_TearDown, "MM_PeekMem_Test_Byte");
    UtTest_Add(MM_PeekMem_Test_ByteError, MM_Test_Setup, MM_Test_TearDown, "MM_PeekMem_Test_ByteError");
    UtTest_Add(MM_PeekMem_Test_Word, MM_Test_Setup, MM_Test_TearDown, "MM_PeekMem_Test_Word");
    UtTest_Add(MM_PeekMem_Test_WordError, MM_Test_Setup, MM_Test_TearDown, "MM_PeekMem_Test_WordError");
    UtTest_Add(MM_PeekMem_Test_DWord, MM_Test_Setup, MM_Test_TearDown, "MM_PeekMem_Test_DWord");
    UtTest_Add(MM_PeekMem_Test_DWordError, MM_Test_Setup, MM_Test_TearDown, "MM_PeekMem_Test_DWordError");

    UtTest_Add(MM_DumpMemToFileCmd_Test_RAM, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_RAM");
    UtTest_Add(MM_DumpMemToFileCmd_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_EEPROM");
    UtTest_Add(MM_DumpMemToFileCmd_Test_MEM32, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_MEM32");
    UtTest_Add(MM_DumpMemToFileCmd_Test_MEM16, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_MEM16");
    UtTest_Add(MM_DumpMemToFileCmd_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_MEM8");
    UtTest_Add(MM_DumpMemToFileCmd_Test_ComputeCRCError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_ComputeCRCError");
    UtTest_Add(MM_DumpMemToFileCmd_Test_CloseError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_CloseError");
    UtTest_Add(MM_DumpMemToFileCmd_Test_CreatError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_CreatError");
    UtTest_Add(MM_DumpMemToFileCmd_Test_SymNameError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_SymNameError");
    UtTest_Add(MM_DumpMemToFileCmd_Test_FilenameError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFileCmd_Test_FilenameError");

    UtTest_Add(MM_DumpMemToFile_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFile_Test_Nominal");
    UtTest_Add(MM_DumpMemToFile_Test_CPUHogging, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFile_Test_CPUHogging");
    UtTest_Add(MM_DumpMemToFile_Test_WriteError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFile_Test_WriteError");
    UtTest_Add(MM_DumpMemToFile_Test_CopyError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMemToFile_Test_CopyError");

    UtTest_Add(MM_VerifyFileDumpParams_Test_RAM, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_RAM");
    UtTest_Add(MM_VerifyFileDumpParams_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_EEPROM");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM32, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM32");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM16, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM16");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM8");
    UtTest_Add(MM_VerifyFileDumpParams_Test_RAMRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_RAMRangeError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooSmall");
    UtTest_Add(MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_RAMInvalidSizeTooLarge");
    UtTest_Add(MM_VerifyFileDumpParams_Test_EEPROMRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_EEPROMRangeError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooSmall");
    UtTest_Add(MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_EEPROMInvalidSizeTooLarge");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM32RangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM32RangeError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooSmall");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM32InvalidSizeTooLarge");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM32AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM32AlignmentError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM16RangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM16RangeError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooSmall");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM16InvalidSizeTooLarge");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM16AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM16AlignmentError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM8RangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM8RangeError");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooSmall");
    UtTest_Add(MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_MEM8InvalidSizeTooLarge");
    UtTest_Add(MM_VerifyFileDumpParams_Test_InvalidMemoryType, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileDumpParams_Test_InvalidMemoryType");

    UtTest_Add(MM_WriteFileHeaders_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_WriteFileHeaders_Test_Nominal");
    UtTest_Add(MM_WriteFileHeaders_Test_WriteHeaderError, MM_Test_Setup, MM_Test_TearDown, "MM_WriteFileHeaders_Test_WriteHeaderError");
    UtTest_Add(MM_WriteFileHeaders_Test_WriteError, MM_Test_Setup, MM_Test_TearDown, "MM_WriteFileHeaders_Test_WriteError");

    UtTest_Add(MM_DumpInEventCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_DumpInEventCmd_Test_Nominal");
    UtTest_Add(MM_DumpInEventCmd_Test_SymNameError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpInEventCmd_Test_SymNameError");

    UtTest_Add(MM_VerifyDumpInEventParams_Test_RAM, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_RAM");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_EEPROM");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM32, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM32");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM16, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM16");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM8");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooSmall");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_InvalidDataSizeTooLarge");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_RAMRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_RAMRangeError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_EEPROMRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_EEPROMRangeError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM32RangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM32RangeError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM32AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM32AlignmentError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM16RangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM16RangeError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM16AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM16AlignmentError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_MEM8RangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_MEM8RangeError");
    UtTest_Add(MM_VerifyDumpInEventParams_Test_InvalidMemType, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyDumpInEventParams_Test_InvalidMemType");

    UtTest_Add(MM_FillDumpInEventBuffer_Test_RAM, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_RAM");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_EEPROM");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_MEM32, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_MEM32");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_MEM16, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_MEM16");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_MEM8");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_EEPROMCopyError, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_EEPROMCopyError");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_MEM32ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_MEM32ReadError");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_MEM16ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_MEM16ReadError");
    UtTest_Add(MM_FillDumpInEventBuffer_Test_MEM8ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_FillDumpInEventBuffer_Test_MEM8ReadError");
}/* end MM_Dump_Test_AddTestCases */


/************************/
/*  End of File Comment */
/************************/
