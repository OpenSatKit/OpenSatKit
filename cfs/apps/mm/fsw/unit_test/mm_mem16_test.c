 /*************************************************************************
 ** File:
 **   $Id: mm_mem16_test.c 1.4.1.1 2016/11/17 16:35:29EST sstrege Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file mm_mem16.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_mem16_test.c  $
 **   Revision 1.4.1.1 2016/11/17 16:35:29EST sstrege 
 **   
 **   Revision 1.5 2016/11/17 12:51:15EST mdeschu 
 **   Remove references to temporary header file mm_test_exports.h
 **   Revision 1.4 2016/10/24 18:35:23EDT czogby 
 **   Code Walkthough Updates
 **   Revision 1.3 2016/05/04 18:59:33EDT czogby 
 **   Update MM to Compile With New UT-Assert PSP Files
 **   Revision 1.2 2016/03/04 20:00:57EST czogby 
 **   Need Temporary Header File to Import Some MM Functions Into Unit Tests
 **   Revision 1.1 2016/02/03 17:11:37EST czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "mm_mem16_test.h"
#include "mm_mem16.h"
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
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

void MM_LoadMem16FromFile_Test_Nominal (void)
{
    boolean Result;
    uint32 DestAddress = 1;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2;

    /* Set to fail condition "(ReadLength = OS_read(FileHandle, ioBuffer8, SegmentSize)) != SegmentSize" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, FileHeader.NumOfBytes, 1);

    /* Execute the function being tested */
    Result = MM_LoadMem16FromFile(FileHandle, "filename", &FileHeader, DestAddress);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_LOAD_FROM_FILE, "MM_AppData.LastAction == MM_LOAD_FROM_FILE");
    UtAssert_True (MM_AppData.MemType == MM_MEM16, "MM_AppData.MemType == MM_MEM16");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.BytesProcessed == FileHeader.NumOfBytes, "MM_AppData.BytesProcessed == FileHeader.NumOfBytes");
    UtAssert_True (strncmp(MM_AppData.FileName, "filename", OS_MAX_PATH_LEN) == 0, "MM_AppData.FileName, 'filename', OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_LoadMem16FromFile_Test_Nominal */

void MM_LoadMem16FromFile_Test_CPUHogging (void)
{
    boolean Result;
    uint32 DestAddress = 1;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2*MM_MAX_LOAD_DATA_SEG;

    /* Set to always fail condition "(ReadLength = OS_read(FileHandle, ioBuffer8, SegmentSize)) != SegmentSize" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, MM_MAX_LOAD_DATA_SEG, 1);
    Ut_OSFILEAPI_ContinueReturnCodeAfterCountZero(UT_OSFILEAPI_READ_INDEX);

    /* Execute the function being tested */
    Result = MM_LoadMem16FromFile(FileHandle, "filename", &FileHeader, DestAddress);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_LOAD_FROM_FILE, "MM_AppData.LastAction == MM_LOAD_FROM_FILE");
    UtAssert_True (MM_AppData.MemType == MM_MEM16, "MM_AppData.MemType == MM_MEM16");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.BytesProcessed == FileHeader.NumOfBytes, "MM_AppData.BytesProcessed == FileHeader.NumOfBytes");
    UtAssert_True (strncmp(MM_AppData.FileName, "filename", OS_MAX_PATH_LEN) == 0, "MM_AppData.FileName, 'filename', OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_LoadMem16FromFile_Test_CPUHogging */

void MM_LoadMem16FromFile_Test_ReadError (void)
{
    boolean Result;
    uint32 DestAddress = 1;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2;

    /* Set to generate error message MM_OS_READ_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_LoadMem16FromFile(FileHandle, "filename", &FileHeader, DestAddress);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_READ_ERR_EID, CFE_EVS_ERROR, "OS_read error received: RC = 0xFFFFFFFF Expected = 2 File = 'filename'"),
        "OS_read error received: RC = 0xFFFFFFFF Expected = 2 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMem16FromFile_Test_ReadError */

void MM_LoadMem16FromFile_Test_WriteError (void)
{
    boolean Result;
    uint32 DestAddress = 1;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2;

    /* Set to fail condition "(ReadLength = OS_read(FileHandle, ioBuffer8, SegmentSize)) != SegmentSize" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, FileHeader.NumOfBytes, 1);

    /* Set to generate error message MM_PSP_WRITE_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMWRITE16_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_LoadMem16FromFile(FileHandle, "filename", &FileHeader, DestAddress);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR, "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16"),
        "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMem16FromFile_Test_WriteError */

void MM_DumpMem16ToFile_Test_Nominal (void)
{
    boolean Result;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2;
    FileHeader.SymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_DumpMem16ToFile(FileHandle, "filename", &FileHeader);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (MM_AppData.MemType == MM_MEM16, "MM_AppData.MemType == MM_MEM16");
    UtAssert_True (MM_AppData.Address == FileHeader.SymAddress.Offset, "MM_AppData.Address == FileHeader.SymAddress.Offset");
    UtAssert_True (MM_AppData.BytesProcessed == FileHeader.NumOfBytes, "MM_AppData.BytesProcessed == FileHeader.NumOfBytes");
    UtAssert_True (strncmp(MM_AppData.FileName, "filename", OS_MAX_PATH_LEN) == 0, "MM_AppData.FileName, 'filename', OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_DumpMem16ToFile_Test_Nominal */

void MM_DumpMem16ToFile_Test_CPUHogging (void)
{
    boolean Result;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2*MM_MAX_LOAD_DATA_SEG;
    FileHeader.SymAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MM_DumpMem16ToFile(FileHandle, "filename", &FileHeader);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_DUMP_TO_FILE, "MM_AppData.LastAction == MM_DUMP_TO_FILE");
    UtAssert_True (MM_AppData.MemType == MM_MEM16, "MM_AppData.MemType == MM_MEM16");
    UtAssert_True (MM_AppData.Address == FileHeader.SymAddress.Offset, "MM_AppData.Address == FileHeader.SymAddress.Offset");
    UtAssert_True (MM_AppData.BytesProcessed == FileHeader.NumOfBytes, "MM_AppData.BytesProcessed == FileHeader.NumOfBytes");
    UtAssert_True (strncmp(MM_AppData.FileName, "filename", OS_MAX_PATH_LEN) == 0, "MM_AppData.FileName, 'filename', OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_DumpMem16ToFile_Test_CPUHogging */

void MM_DumpMem16ToFile_Test_ReadError (void)
{
    boolean Result;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;
    char ErrorMessage[125];

    FileHeader.NumOfBytes = 2;

    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD16_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_DumpMem16ToFile(FileHandle, "filename", &FileHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP read memory error: RC=0xFFFFFFFF, Src=0x%08X, Tgt=0x%08X, Type=MEM16", (unsigned int)FileHeader.SymAddress.Offset, (unsigned int)&MM_AppData.DumpBuffer[0]);
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMem16ToFile_Test_ReadError */

void MM_DumpMem16ToFile_Test_WriteError (void)
{
    boolean Result;
    uint32 FileHandle = 1;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 2;

    /* Set to generate error message MM_OS_WRITE_EXP_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_DumpMem16ToFile(FileHandle, "filename", &FileHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_WRITE_EXP_ERR_EID, CFE_EVS_ERROR, "OS_write error received: RC = 0xFFFFFFFF Expected = 2 File = 'filename'"),
        "OS_write error received: RC = 0xFFFFFFFF Expected = 2 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_DumpMem16ToFile_Test_WriteError */

void MM_FillMem16_Test_Nominal (void)
{
    MM_FillMemCmd_t CmdPacket;
    uint32 DestAddress = 1;

    CmdPacket.NumOfBytes = 2;
    CmdPacket.FillPattern = 3;

    /* Execute the function being tested */
    MM_FillMem16(DestAddress, &CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_FILL, "MM_AppData.LastAction == MM_FILL");
    UtAssert_True (MM_AppData.MemType == MM_MEM16, "MM_AppData.MemType == MM_MEM16");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue == CmdPacket.FillPattern, "MM_AppData.DataValue == CmdPacket.FillPattern");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillMem16_Test_Nominal */

void MM_FillMem16_Test_CPUHogging (void)
{
    MM_FillMemCmd_t CmdPacket;
    uint32 DestAddress = 1;

    CmdPacket.NumOfBytes = 2*MM_MAX_LOAD_DATA_SEG;
    CmdPacket.FillPattern = 3;

    /* Execute the function being tested */
    MM_FillMem16(DestAddress, &CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_FILL, "MM_AppData.LastAction == MM_FILL");
    UtAssert_True (MM_AppData.MemType == MM_MEM16, "MM_AppData.MemType == MM_MEM16");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue == CmdPacket.FillPattern, "MM_AppData.DataValue == CmdPacket.FillPattern");
    UtAssert_True (MM_AppData.BytesProcessed == CmdPacket.NumOfBytes, "MM_AppData.BytesProcessed == CmdPacket.NumOfBytes");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillMem16_Test_CPUHogging */

void MM_FillMem16_Test_WriteError (void)
{
    MM_FillMemCmd_t CmdPacket;
    uint32 DestAddress = 1;

    CmdPacket.NumOfBytes = 2;
    CmdPacket.FillPattern = 3;

    /* Set to generate error message MM_PSP_WRITE_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMWRITE16_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_FillMem16(DestAddress, &CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR, "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16"),
        "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMem16_Test_WriteError */

void MM_Mem16_Test_AddTestCases(void)
{
    UtTest_Add(MM_LoadMem16FromFile_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMem16FromFile_Test_Nominal");
    UtTest_Add(MM_LoadMem16FromFile_Test_CPUHogging, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMem16FromFile_Test_CPUHogging");
    UtTest_Add(MM_LoadMem16FromFile_Test_ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMem16FromFile_Test_ReadError");
    UtTest_Add(MM_LoadMem16FromFile_Test_WriteError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMem16FromFile_Test_WriteError");
    UtTest_Add(MM_DumpMem16ToFile_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMem16ToFile_Test_Nominal");
    UtTest_Add(MM_DumpMem16ToFile_Test_CPUHogging, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMem16ToFile_Test_CPUHogging");
    UtTest_Add(MM_DumpMem16ToFile_Test_ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMem16ToFile_Test_ReadError");
    UtTest_Add(MM_DumpMem16ToFile_Test_WriteError, MM_Test_Setup, MM_Test_TearDown, "MM_DumpMem16ToFile_Test_WriteError");
    UtTest_Add(MM_FillMem16_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_FillMem16_Test_Nominal");
    UtTest_Add(MM_FillMem16_Test_CPUHogging, MM_Test_Setup, MM_Test_TearDown, "MM_FillMem16_Test_CPUHogging");
    UtTest_Add(MM_FillMem16_Test_WriteError, MM_Test_Setup, MM_Test_TearDown, "MM_FillMem16_Test_WriteError");
} /* end MM_Mem16_Test_AddTestCases */


/************************/
/*  End of File Comment */
/************************/
