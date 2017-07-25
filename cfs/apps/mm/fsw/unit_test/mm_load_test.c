 /*************************************************************************
 ** File:
 **   $Id: mm_load_test.c 1.5.1.1 2016/11/17 16:35:30EST sstrege Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file mm_load.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_load_test.c  $
 **   Revision 1.5.1.1 2016/11/17 16:35:30EST sstrege 
 **   
 **   Revision 1.6 2016/11/17 12:51:17EST mdeschu 
 **   Remove references to temporary header file mm_test_exports.h
 **   Revision 1.5 2016/10/31 22:09:12EDT mdeschu 
 **   Adding std=c99 to unit test makefile as per babelfish ticket #39 caused compiler error with unknown
 **   Symbol S_IFREG.  Removed setting File stat st_mode to S_IFREG in StatHook1. Has no effect on unit tests.
 **   Revision 1.4 2016/10/24 18:35:23EDT czogby 
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

#include "mm_load_test.h"
#include "mm_load.h"
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
#include "ut_cfe_psp_eeprom_stubs.h"
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

int32 UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1( uint32 *SymbolAddress, char *SymbolName )
{
    *SymbolAddress = 0;

    return OS_SUCCESS;
} /* end UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1 */

int8 UT_MM_CFE_OSFILEAPI_ReadHook1_MemType;
int32 UT_MM_CFE_OSFILEAPI_ReadHook_RunCount;
int32 UT_MM_CFE_OSFILEAPI_ReadHook1(int32  filedes, void *buffer, uint32 nbytes)
{
    ((MM_LoadDumpFileHeader_t *)(buffer))->NumOfBytes = 4;
    ((MM_LoadDumpFileHeader_t *)(buffer))->Crc = 0;
    ((MM_LoadDumpFileHeader_t *)(buffer))->MemType = UT_MM_CFE_OSFILEAPI_ReadHook1_MemType;
    ((MM_LoadDumpFileHeader_t *)(buffer))->SymAddress.Offset = 0;

    strncpy(((MM_LoadDumpFileHeader_t *)(buffer))->SymAddress.SymName, "name", OS_MAX_PATH_LEN);

    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount++;

    if (UT_MM_CFE_OSFILEAPI_ReadHook_RunCount == 1)
        return sizeof(MM_LoadDumpFileHeader_t);
    else if (UT_MM_CFE_OSFILEAPI_ReadHook_RunCount == 2)
        return 0;
    else
        return 4;

    return 0;
} /* end UT_MM_CFE_OSFILEAPI_ReadHook1 */

int32 UT_MM_CFE_OSFILEAPI_ReadHook2(int32  filedes, void *buffer, uint32 nbytes)
{
    ((MM_LoadDumpFileHeader_t *)(buffer))->NumOfBytes = 4;
    ((MM_LoadDumpFileHeader_t *)(buffer))->Crc = 99;
    ((MM_LoadDumpFileHeader_t *)(buffer))->MemType = MM_RAM;
    ((MM_LoadDumpFileHeader_t *)(buffer))->SymAddress.Offset = 0;

    strncpy(((MM_LoadDumpFileHeader_t *)(buffer))->SymAddress.SymName, "name", OS_MAX_PATH_LEN);

    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount++;

    if (UT_MM_CFE_OSFILEAPI_ReadHook_RunCount == 1)
        return sizeof(MM_LoadDumpFileHeader_t);
    else if (UT_MM_CFE_OSFILEAPI_ReadHook_RunCount == 2)
        return 0;
    else
        return 4;

    return 0;
} /* end UT_MM_CFE_OSFILEAPI_ReadHook2 */

int32 UT_MM_CFE_OSFILEAPI_ReadHook3(int32  filedes, void *buffer, uint32 nbytes)
{
    ((MM_LoadDumpFileHeader_t *)(buffer))->NumOfBytes = 4;
    ((MM_LoadDumpFileHeader_t *)(buffer))->Crc = 0;
    ((MM_LoadDumpFileHeader_t *)(buffer))->MemType = MM_RAM;
    ((MM_LoadDumpFileHeader_t *)(buffer))->SymAddress.Offset = 0;

    strncpy(((MM_LoadDumpFileHeader_t *)(buffer))->SymAddress.SymName, "name", OS_MAX_PATH_LEN);

    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount++;

    if (UT_MM_CFE_OSFILEAPI_ReadHook_RunCount == 1)
        return sizeof(MM_LoadDumpFileHeader_t);
    else if (UT_MM_CFE_OSFILEAPI_ReadHook_RunCount == 2)
        return -1;
    else
        return 4;

    return 0;
} /* end UT_MM_CFE_OSFILEAPI_ReadHook3 */

int32 UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1(const char *path, os_fstat_t  *filestats)
{
    filestats->st_size = 4 + sizeof(CFE_FS_Header_t) + sizeof(MM_LoadDumpFileHeader_t);

    return OS_FS_SUCCESS;
} /* end UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1 */

void MM_PokeCmd_Test_EEPROM (void)
{
    MM_PokeCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = 32;

    /* Execute the function being tested */
    MM_PokeCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_DWORD_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000000, Size = 32 bits, Data = 0x00000000"),
        "Poke Command: Addr = 0x00000000, Size = 32 bits, Data = 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeCmd_Test_EEPROM */

void MM_PokeCmd_Test_NonEEPROM (void)
{
    MM_PokeCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = 32;

    /* Execute the function being tested */
    MM_PokeCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_DWORD_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000000, Size = 32 bits, Data = 0x00000000"),
        "Poke Command: Addr = 0x00000000, Size = 32 bits, Data = 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeCmd_Test_NonEEPROM */

void MM_PokeCmd_Test_SymNameError (void)
{
    MM_PokeCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = 32;

    strncpy(CmdPacket.DestSymAddress.SymName, "name", OS_MAX_PATH_LEN);

    /* Set to generate error message MM_SYMNAME_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeCmd_Test_SymNameError */

void MM_PokeMem_Test_8bit (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = MM_BYTE_BIT_WIDTH;
    CmdPacket.Data = (uint8)(5);
    
    DestAddress = 1;

    /* Execute the function being tested */
    MM_PokeMem(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_BYTE_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000001, Size = 8 bits, Data = 0x05"),
        "Poke Command: Addr = 0x00000001, Size = 8 bits, Data = 0x05");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_POKE, "MM_AppData.LastAction == MM_POKE");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue  == 5, "MM_AppData.DataValue  == 5");
    UtAssert_True (MM_AppData.BytesProcessed == 1, "MM_AppData.BytesProcessed == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeMem_Test_8bit */

void MM_PokeMem_Test_8bitError (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = MM_BYTE_BIT_WIDTH;
    CmdPacket.Data = (uint8)(5);
    
    DestAddress = 1;

    /* Set to generate error message MM_PSP_WRITE_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMWRITE8_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeMem(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR, "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM8"),
        "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM8");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeMem_Test_8bitError */

void MM_PokeMem_Test_16bit (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = MM_WORD_BIT_WIDTH;
    CmdPacket.Data = (uint16)(5);
    
    DestAddress = 1;

    /* Execute the function being tested */
    MM_PokeMem(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_WORD_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000001, Size = 16 bits, Data = 0x0005"),
        "Poke Command: Addr = 0x00000001, Size = 16 bits, Data = 0x05");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_POKE, "MM_AppData.LastAction == MM_POKE");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue  == 5, "MM_AppData.DataValue  == 5");
    UtAssert_True (MM_AppData.BytesProcessed == 2, "MM_AppData.BytesProcessed == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeMem_Test_16bit */

void MM_PokeMem_Test_16bitError (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = MM_WORD_BIT_WIDTH;
    CmdPacket.Data = (uint16)(5);
    
    DestAddress = 1;

    /* Set to generate error message MM_PSP_WRITE_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMWRITE16_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeMem(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR, "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16"),
        "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM16");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeMem_Test_16bitError */

void MM_PokeMem_Test_32bit (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = MM_DWORD_BIT_WIDTH;
    CmdPacket.Data = (uint32)(5);
    
    DestAddress = 1;

    /* Execute the function being tested */
    MM_PokeMem(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_DWORD_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000001, Size = 32 bits, Data = 0x00000005"),
        "Poke Command: Addr = 0x00000001, Size = 32 bits, Data = 0x05");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_POKE, "MM_AppData.LastAction == MM_POKE");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue  == 5, "MM_AppData.DataValue  == 5");
    UtAssert_True (MM_AppData.BytesProcessed == 4, "MM_AppData.BytesProcessed == 4");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeMem_Test_32bit */

void MM_PokeMem_Test_32bitError (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.DataSize = MM_DWORD_BIT_WIDTH;
    CmdPacket.Data = (uint32)(5);
    
    DestAddress = 1;

    /* Set to generate error message MM_PSP_WRITE_ERR_EID */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMWRITE32_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeMem(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR, "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM32"),
        "PSP write memory error: RC=0xFFFFFFFF, Address=0x00000001, MemType=MEM32");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeMem_Test_32bitError */

void MM_PokeEeprom_Test_8bit (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = MM_BYTE_BIT_WIDTH;
    CmdPacket.Data = (uint8)(5);
    
    DestAddress = 1;

    /* Execute the function being tested */
    MM_PokeEeprom(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_BYTE_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000001, Size = 8 bits, Data = 0x05"),
        "Poke Command: Addr = 0x00000001, Size = 8 bits, Data = 0x05");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_POKE, "MM_AppData.LastAction == MM_POKE");
    UtAssert_True (MM_AppData.MemType == MM_EEPROM, "MM_AppData.MemType == MM_EEPROM");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue  == 5, "MM_AppData.DataValue  == 5");
    UtAssert_True (MM_AppData.BytesProcessed == 1, "MM_AppData.BytesProcessed == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeEeprom_Test_8bit */

void MM_PokeEeprom_Test_8bitError (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = MM_BYTE_BIT_WIDTH;
    CmdPacket.Data = (uint8)(5);
    
    DestAddress = 1;

    /* Set to generate error message MM_OS_EEPROMWRITE8_ERR_EID */
    Ut_CFE_PSP_EEPROM_SetReturnCode(UT_CFE_PSP_EEPROM_WRITE8_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeEeprom(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_EEPROMWRITE8_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_EepromWrite8 error received: RC = 0xFFFFFFFF, Addr = 0x00000001"),
        "CFE_PSP_EepromWrite8 error received: RC = 0xFFFFFFFF, Addr = 0x00000001");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeEeprom_Test_8bitError */

void MM_PokeEeprom_Test_16bit (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = MM_WORD_BIT_WIDTH;
    CmdPacket.Data = (uint16)(5);
    
    DestAddress = 1;

    /* Execute the function being tested */
    MM_PokeEeprom(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_WORD_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000001, Size = 16 bits, Data = 0x0005"),
        "Poke Command: Addr = 0x00000001, Size = 16 bits, Data = 0x05");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_POKE, "MM_AppData.LastAction == MM_POKE");
    UtAssert_True (MM_AppData.MemType == MM_EEPROM, "MM_AppData.MemType == MM_EEPROM");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue  == 5, "MM_AppData.DataValue  == 5");
    UtAssert_True (MM_AppData.BytesProcessed == 2, "MM_AppData.BytesProcessed == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeEeprom_Test_16bit */

void MM_PokeEeprom_Test_16bitError (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = MM_WORD_BIT_WIDTH;
    CmdPacket.Data = (uint16)(5);
    
    DestAddress = 1;

    /* Set to generate error message MM_OS_EEPROMWRITE16_ERR_EID */
    Ut_CFE_PSP_EEPROM_SetReturnCode(UT_CFE_PSP_EEPROM_WRITE16_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeEeprom(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_EEPROMWRITE16_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_EepromWrite16 error received: RC = 0xFFFFFFFF, Addr = 0x00000001"),
        "CFE_PSP_EepromWrite16 error received: RC = 0xFFFFFFFF, Addr = 0x00000001");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeEeprom_Test_16bitError */

void MM_PokeEeprom_Test_32bit (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = MM_DWORD_BIT_WIDTH;
    CmdPacket.Data = (uint32)(5);
    
    DestAddress = 1;

    /* Execute the function being tested */
    MM_PokeEeprom(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_POKE_DWORD_INF_EID, CFE_EVS_INFORMATION, "Poke Command: Addr = 0x00000001, Size = 32 bits, Data = 0x00000005"),
        "Poke Command: Addr = 0x00000001, Size = 32 bits, Data = 0x05");

    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.LastAction == MM_POKE, "MM_AppData.LastAction == MM_POKE");
    UtAssert_True (MM_AppData.MemType == MM_EEPROM, "MM_AppData.MemType == MM_EEPROM");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue  == 5, "MM_AppData.DataValue  == 5");
    UtAssert_True (MM_AppData.BytesProcessed == 4, "MM_AppData.BytesProcessed == 4");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeEeprom_Test_32bit */

void MM_PokeEeprom_Test_32bitError (void)
{
    MM_PokeCmd_t   CmdPacket;
    uint32 DestAddress;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.DataSize = MM_DWORD_BIT_WIDTH;
    CmdPacket.Data = (uint32)(5);
    
    DestAddress = 1;

    /* Set to generate error message MM_OS_EEPROMWRITE32_ERR_EID */
    Ut_CFE_PSP_EEPROM_SetReturnCode(UT_CFE_PSP_EEPROM_WRITE32_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_PokeEeprom(&CmdPacket, DestAddress);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_EEPROMWRITE32_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_EepromWrite32 error received: RC = 0xFFFFFFFF, Addr = 0x00000001"),
        "CFE_PSP_EepromWrite32 error received: RC = 0xFFFFFFFF, Addr = 0x00000001");

    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_PokeEeprom_Test_32bitError */

void MM_LoadMemWIDCmd_Test_Nominal (void)
{
    MM_LoadMemWIDCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemWIDCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_WID_CC);

    CmdPacket.DataArray[0] = 1;
    CmdPacket.DataArray[1] = 2;
    CmdPacket.NumOfBytes = 2;
    CmdPacket.Crc = 0;
    CmdPacket.DestSymAddress.Offset = 1;

    strncpy (CmdPacket.DestSymAddress.SymName, "", OS_MAX_PATH_LEN);

    /* Set to prevent segmentation fault */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LOAD_WID_INF_EID, CFE_EVS_INFORMATION, "Load Memory WID Command: Wrote 2 bytes to address: 0x00000001"),
        "Load Memory WID Command: Wrote 2 bytes to address: 0x00000001");

    UtAssert_True (MM_AppData.LastAction == MM_LOAD_WID, "MM_AppData.LastAction == MM_LOAD_WID");
    UtAssert_True (MM_AppData.Address == 1, "MM_AppData.Address == 1");
    UtAssert_True (MM_AppData.MemType == MM_RAM, "MM_AppData.MemType == MM_RAM");
    UtAssert_True (MM_AppData.BytesProcessed == 2, "MM_AppData.BytesProcessed == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemWIDCmd_Test_Nominal */

void MM_LoadMemWIDCmd_Test_CopyError (void)
{
    MM_LoadMemWIDCmd_t   CmdPacket;
    char                 Message[125];

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemWIDCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_WID_CC);

    CmdPacket.NumOfBytes = 2;
    CmdPacket.Crc = 0;
    CmdPacket.DestSymAddress.Offset = 1;
    CmdPacket.DestSymAddress.SymName[0] = '\0';

    strncpy (CmdPacket.DestSymAddress.SymName, "", OS_MAX_PATH_LEN);

    /* Set to generate error message MM_PSP_COPY_ERR_EID */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf(Message, "PSP copy memory error: RC=0xFFFFFFFF, Src=0x%08X, Tgt=0x00000001, Size=0x00000002", (unsigned int)&CmdPacket.DataArray);
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR, Message),
        Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemWIDCmd_Test_CopyError */

void MM_LoadMemWIDCmd_Test_CRCError (void)
{
    MM_LoadMemWIDCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemWIDCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_WID_CC);

    CmdPacket.DataArray[0] = 1;
    CmdPacket.DataArray[1] = 2;
    CmdPacket.NumOfBytes = 2;
    CmdPacket.Crc = 1;
    CmdPacket.DestSymAddress.Offset = 1;

    strncpy (CmdPacket.DestSymAddress.SymName, "", OS_MAX_PATH_LEN);

    /* Set to prevent CFE_PSP_MemCpy from causing any problems */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LOAD_WID_CRC_ERR_EID, CFE_EVS_ERROR, "Interrupts Disabled Load CRC failure: Expected = 0x1 Calculated = 0x0"),
        "Interrupts Disabled Load CRC failure: Expected = 0x1 Calculated = 0x0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemWIDCmd_Test_CRCError */

void MM_LoadMemWIDCmd_Test_SymNameErr (void)
{
    MM_LoadMemWIDCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemWIDCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_WID_CC);

    CmdPacket.DataArray[0] = 1;
    CmdPacket.DataArray[1] = 2;
    CmdPacket.NumOfBytes = 2;
    CmdPacket.Crc = 0;
    CmdPacket.DestSymAddress.Offset = 1;

    strncpy (CmdPacket.DestSymAddress.SymName, "name", OS_MAX_PATH_LEN);

    /* Set to prevent CFE_PSP_MemCpy from causing any problems */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Set to generate error message MM_SYMNAME_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemWIDCmd_Test_SymNameErr */

void MM_VerifyLoadWIDParams_Test_Nominal (void)
{
    uint32 Address = 0;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    MM_VerifyLoadWIDParams(Address, SizeInBytes);

    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_VerifyLoadWIDParams_Test_Nominal */

void MM_VerifyLoadWIDParams_Test_MemValidateError (void)
{
    uint32 Address = 0;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_VerifyLoadWIDParams(Address, SizeInBytes);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyLoadWIDParams_Test_MemValidateError */

void MM_VerifyLoadWIDParams_Test_DataSizeErrorTooSmall (void)
{
    uint32 Address = 0;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    MM_VerifyLoadWIDParams(Address, SizeInBytes);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyLoadWIDParams_Test_DataSizeErrorTooSmall */

void MM_VerifyLoadWIDParams_Test_DataSizeErrorTooLarge (void)
{
    uint32 Address = 0;
    uint32 SizeInBytes = MM_MAX_UNINTERRUPTABLE_DATA + 1;

    /* Execute the function being tested */
    MM_VerifyLoadWIDParams(Address, SizeInBytes);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 201"),
        "Data size in bytes invalid or exceeds limits: Data Size = 201");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyLoadWIDParams_Test_DataSizeErrorTooLarge */

void MM_LoadMemFromFileCmd_Test_RAM (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_RAM;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LD_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'"),
        "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_RAM */

void MM_LoadMemFromFileCmd_Test_EEPROM (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_EEPROM;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Set to prevent segmentation fault */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);
    Ut_CFE_PSP_MEMUTILS_ContinueReturnCodeAfterCountZero(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LD_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'"),
        "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_EEPROM */

void MM_LoadMemFromFileCmd_Test_MEM32 (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM32;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LD_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'"),
        "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_MEM32 */

void MM_LoadMemFromFileCmd_Test_MEM16 (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM16;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LD_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'"),
        "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_MEM16 */

void MM_LoadMemFromFileCmd_Test_MEM8 (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LD_MEM_FILE_INF_EID, CFE_EVS_INFORMATION, "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'"),
        "Load Memory From File Command: Loaded 4 bytes to address 0x00000000 from file 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_MEM8 */

void MM_LoadMemFromFileCmd_Test_LoadParamsError (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = 99;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR, "Invalid memory type specified: MemType = 99"),
        "Invalid memory type specified: MemType = 99");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_FILE_LOAD_PARAMS_ERR_EID, CFE_EVS_ERROR, "Load file failed parameters check: File = 'name'"),
        "Load file failed parameters check: File = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MM_LoadMemFromFileCmd_Test_LoadParamsError */

void MM_LoadMemFromFileCmd_Test_SymNameError (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Set to generate error message MM_SYMNAME_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_SymNameError */

void MM_LoadMemFromFileCmd_Test_LoadFileCRCError (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to CFS_ComputeCRCFromFile to fail, in order to generate error message MM_LOAD_FILE_CRC_ERR_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook2);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LOAD_FILE_CRC_ERR_EID, CFE_EVS_ERROR, "Load file CRC failure: Expected = 0x63 Calculated = 0x0 File = 'name'"),
        "Load file CRC failure: Expected = 0x63 Calculated = 0x0 File = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_LoadFileCRCError */

void MM_LoadMemFromFileCmd_Test_ComputeCRCError (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes condition "ComputedCRC == MMFileHeader.Crc" to fail, in order to generate error message MM_CFS_COMPUTECRCFROMFILE_ERR_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook3);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CFS_COMPUTECRCFROMFILE_ERR_EID, CFE_EVS_ERROR, "CFS_ComputeCRCFromFile error received: RC = 0xFFFFFFFF File = 'name'"),
        "CFS_ComputeCRCFromFile error received: RC = 0xFFFFFFFF File = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_ComputeCRCError */

void MM_LoadMemFromFileCmd_Test_CloseError (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Set to generate error message MM_OS_CLOSE_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CLOSE_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_CLOSE_ERR_EID, CFE_EVS_ERROR, "OS_close error received: RC = 0xFFFFFFFF File = 'name'"),
        "OS_close error received: RC = 0xFFFFFFFF File = 'name'");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MM_LoadMemFromFileCmd_Test_CloseError */

void MM_LoadMemFromFileCmd_Test_OpenError (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Set to generate error message MM_OS_OPEN_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_OPEN_ERR_EID, CFE_EVS_ERROR, "OS_open error received: RC = 0xFFFFFFFF File = 'name'"),
        "OS_open error received: RC = 0xFFFFFFFF File = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_OpenError */

void MM_LoadMemFromFileCmd_Test_InvalidFilename (void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    UT_MM_CFE_OSFILEAPI_ReadHook1_MemType = MM_MEM8;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    strncpy (CmdPacket.FileName, "***name***", OS_MAX_PATH_LEN);

    /* Causes call to MM_VerifyLoadFileSize to return TRUE, in order to satisfy the immediately following condition "Valid == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Set to satisfy condition "Valid == TRUE" immediately following call to MM_ReadFileHeaders */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Causes call to CFS_ResolveSymAddr to return a known value for DestAddress */
    Ut_OSAPI_SetFunctionHook(UT_OSAPI_SYMBOLLOOKUP_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSAPI_SymbolLookupHook1);

    /* Causes call to MM_LoadMemFromFile to return TRUE, in order to generate event message MM_LD_MEM_FILE_INF_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_MM_CFE_OSFILEAPI_ReadHook1);
    UT_MM_CFE_OSFILEAPI_ReadHook_RunCount = 0;

    /* Set to generate error message MM_CMD_FNAME_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CMD_FNAME_ERR_EID, CFE_EVS_ERROR, "Command specified filename invalid: Name = '***name***'"),
        "Command specified filename invalid: Name = '***name***'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFileCmd_Test_InvalidFilename */

void MM_LoadMemFromFile_Test_PreventCPUHogging (void)
{
    boolean Result;
    MM_LoadDumpFileHeader_t  FileHeader;
    FileHeader.MemType = MM_EEPROM;
    FileHeader.NumOfBytes = 2*MM_MAX_LOAD_DATA_SEG;

    /* Set to satisfy condition "(ReadLength = OS_read(FileHandle, ioBuffer, SegmentSize)) == SegmentSize" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, MM_MAX_LOAD_DATA_SEG, 1);
    Ut_OSFILEAPI_ContinueReturnCodeAfterCountZero(UT_OSFILEAPI_READ_INDEX);

    /* Execute the function being tested */
    Result = MM_LoadMemFromFile(0, "filename", &FileHeader, 0);

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (MM_AppData.LastAction == MM_LOAD_FROM_FILE, "MM_AppData.LastAction == MM_LOAD_FROM_FILE");
    UtAssert_True (MM_AppData.MemType == MM_EEPROM, "MM_AppData.MemType == MM_EEPROM");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.BytesProcessed == 2*MM_MAX_LOAD_DATA_SEG, "MM_AppData.BytesProcessed == 2*MM_MAX_LOAD_DATA_SEG");
    UtAssert_True (strncmp(MM_AppData.FileName, "filename", OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, 'filename', OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_LoadMemFromFile_Test_PreventCPUHogging */

void MM_LoadMemFromFile_Test_MemCpyError (void)
{
    boolean Result;
    MM_LoadDumpFileHeader_t  FileHeader;
    FileHeader.MemType = MM_EEPROM;
    FileHeader.NumOfBytes = 2;
    char ErrorMessage[125];

    /* Set to generate error message MM_PSP_COPY_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, 2, 1);

    /* Set to generate error message MM_PSP_COPY_ERR_EID */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_LoadMemFromFile(0, "filename", &FileHeader, 0);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP copy memory error: RC=0xFFFFFFFF, Src=0x%08X, Tgt=0x00000000, Size=0x00000002", (unsigned int)(&MM_AppData.LoadBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFile_Test_MemCpyError */

void MM_LoadMemFromFile_Test_ReadError (void)
{
    boolean Result;
    MM_LoadDumpFileHeader_t  FileHeader;
    FileHeader.MemType = MM_EEPROM;
    FileHeader.NumOfBytes = 2;

    /* Execute the function being tested */
    Result = MM_LoadMemFromFile(0, "filename", &FileHeader, 0);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_READ_ERR_EID, CFE_EVS_ERROR, "OS_read error received: RC = 0x00000000 Expected = 2 File = 'filename'"),
        "OS_read error received: RC = 0x00000000 Expected = 2 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LoadMemFromFile_Test_ReadError */

void MM_VerifyFileLoadParams_Test_RAMValidateRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_RAM;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_RAMValidateRangeError */

void MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_RAM;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooSmall */

void MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_RAM;
    uint32 SizeInBytes = MM_MAX_LOAD_FILE_DATA_RAM + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooLarge */

void MM_VerifyFileLoadParams_Test_EEPROMValidateRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_EEPROM;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_EEPROMValidateRangeError */

void MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_EEPROM;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooSmall */

void MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_EEPROM;
    uint32 SizeInBytes = MM_MAX_LOAD_FILE_DATA_EEPROM +1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 131073"),
        "Data size in bytes invalid or exceeds limits: Data Size = 131073");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooLarge */

void MM_VerifyFileLoadParams_Test_MEM32ValidateRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM32;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);
    
    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    /* NOTE: MemType passed into the function is not the same as "MemType" reported by error message - in error message it's always either CFE_PSP_MEM_RAM or CFE_PSP_MEM_EEPROM */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM32ValidateRangeError */

void MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM32;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooSmall */

void MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM32;
    uint32 SizeInBytes = MM_MAX_LOAD_FILE_DATA_MEM32 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooLarge */

void MM_VerifyFileLoadParams_Test_MEM32AlignmentError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM32;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR, "Data and address not 32 bit aligned: Addr = 0x00000000 Size = 1"),
        "Data and address not 32 bit aligned: Addr = 0x00000000 Size = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM32AlignmentError */

void MM_VerifyFileLoadParams_Test_MEM16ValidateRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM16;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);
    
    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    /* NOTE: MemType passed into the function is not the same as "MemType" reported by error message - in error message it's always either CFE_PSP_MEM_RAM or CFE_PSP_MEM_EEPROM */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM16ValidateRangeError */

void MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM16;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooSmall */

void MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM16;
    uint32 SizeInBytes = MM_MAX_LOAD_FILE_DATA_MEM16 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooLarge */

void MM_VerifyFileLoadParams_Test_MEM16AlignmentError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM16;
    uint32 SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR, "Data and address not 16 bit aligned: Addr = 0x00000000 Size = 1"),
        "Data and address not 16 bit aligned: Addr = 0x00000000 Size = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM16AlignmentError */

void MM_VerifyFileLoadParams_Test_MEM8ValidateRangeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM8;
    uint32 SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);
    
    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000000 Size = 1 MemType = 1");

    /* NOTE: MemType passed into the function is not the same as "MemType" reported by error message - in error message it's always either CFE_PSP_MEM_RAM or CFE_PSP_MEM_EEPROM */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM8ValidateRangeError */

void MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM8;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooSmall */

void MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = MM_MEM8;
    uint32 SizeInBytes = MM_MAX_LOAD_FILE_DATA_MEM8 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooLarge */

void MM_VerifyFileLoadParams_Test_InvalidMemTypeError (void)
{
    boolean Result;
    uint32 Address = 0;
    uint8 MemType = 99;
    uint32 SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFileLoadParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR, "Invalid memory type specified: MemType = 99"),
        "Invalid memory type specified: MemType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFileLoadParams_Test_InvalidMemTypeError */

void MM_VerifyLoadFileSize_Test_StatError (void)
{
    boolean Result;
    MM_LoadDumpFileHeader_t FileHeader;

    /* Set to generate error message MM_OS_STAT_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyLoadFileSize("filename", &FileHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_STAT_ERR_EID, CFE_EVS_ERROR, "OS_stat error received: RC = 0xFFFFFFFF File = 'filename'"),
        "OS_stat error received: RC = 0xFFFFFFFF File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyLoadFileSize_Test_StatError */

void MM_VerifyLoadFileSize_Test_SizeError (void)
{
    boolean Result;
    MM_LoadDumpFileHeader_t FileHeader;

    FileHeader.NumOfBytes = 0;

    /* Generates error message MM_LD_FILE_SIZE_ERR_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_MM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHook1);

    /* Execute the function being tested */
    Result = MM_VerifyLoadFileSize("filename", &FileHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_LD_FILE_SIZE_ERR_EID, CFE_EVS_ERROR, "Load file size error: Reported by OS = 148 Expected = 144 File = 'filename'"),
        "Load file size error: Reported by OS = 148 Expected = 144 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyLoadFileSize_Test_SizeError */

void MM_ReadFileHeaders_Test_ReadHeaderError (void)
{
    boolean Result;
    int32 FileHandle = 1;
    CFE_FS_Header_t CFEHeader;
    MM_LoadDumpFileHeader_t MMHeader;

    /* Execute the function being tested */
    Result = MM_ReadFileHeaders("filename", FileHandle, &CFEHeader, &MMHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CFE_FS_READHDR_ERR_EID, CFE_EVS_ERROR, "CFE_FS_ReadHeader error received: RC = 0x00000000 Expected = 64 File = 'filename'"),
        "CFE_FS_ReadHeader error received: RC = 0x00000000 Expected = 64 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_ReadFileHeaders_Test_ReadHeaderError */

void MM_ReadFileHeaders_Test_ReadError (void)
{
    boolean Result;
    int32 FileHandle = 1;
    CFE_FS_Header_t CFEHeader;
    MM_LoadDumpFileHeader_t MMHeader;

    /* Set to satisfy condition "OS_Status != sizeof(CFE_FS_Header_t)" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_READHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Set to generate error message MM_OS_READ_EXP_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_ReadFileHeaders("filename", FileHandle, &CFEHeader, &MMHeader);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_READ_EXP_ERR_EID, CFE_EVS_ERROR, "OS_read error received: RC = 0xFFFFFFFF Expected = 80 File = 'filename'"),
        "OS_read error received: RC = 0xFFFFFFFF Expected = 80 File = 'filename'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_ReadFileHeaders_Test_ReadError */

void MM_FillMemCmd_Test_RAM (void)
{
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_RAM;
    CmdPacket.NumOfBytes = 1;

    /* Set to prevent unintended error messages resulting from values set by call to CFE_PSP_MemCpy in MM_FillMem */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    MM_FillMemCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_FILL_INF_EID, CFE_EVS_INFORMATION, "Fill Memory Command: Filled 1 bytes at address: 0x00000000 with pattern: 0x00000000"),
        "Fill Memory Command: Filled 1 bytes at address: 0x00000000 with pattern: 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMemCmd_Test_RAM */

void MM_FillMemCmd_Test_EEPROM (void)
{
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.NumOfBytes = 1;

    /* Execute the function being tested */
    MM_FillMemCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_FILL_INF_EID, CFE_EVS_INFORMATION, "Fill Memory Command: Filled 1 bytes at address: 0x00000000 with pattern: 0x00000000"),
        "Fill Memory Command: Filled 1 bytes at address: 0x00000000 with pattern: 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMemCmd_Test_EEPROM */

void MM_FillMemCmd_Test_MEM32 (void)
{
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_MEM32;
    CmdPacket.NumOfBytes = 4;

    /* Execute the function being tested */
    MM_FillMemCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_FILL_INF_EID, CFE_EVS_INFORMATION, "Fill Memory Command: Filled 4 bytes at address: 0x00000000 with pattern: 0x00000000"),
        "Fill Memory Command: Filled 4 bytes at address: 0x00000000 with pattern: 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMemCmd_Test_MEM32 */

void MM_FillMemCmd_Test_MEM16 (void)
{
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_MEM16;
    CmdPacket.NumOfBytes = 2;

    /* Execute the function being tested */
    MM_FillMemCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_FILL_INF_EID, CFE_EVS_INFORMATION, "Fill Memory Command: Filled 2 bytes at address: 0x00000000 with pattern: 0x00000000"),
        "Fill Memory Command: Filled 2 bytes at address: 0x00000000 with pattern: 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMemCmd_Test_MEM16 */

void MM_FillMemCmd_Test_MEM8 (void)
{
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    /* Execute the function being tested */
    MM_FillMemCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_FILL_INF_EID, CFE_EVS_INFORMATION, "Fill Memory Command: Filled 1 bytes at address: 0x00000000 with pattern: 0x00000000"),
        "Fill Memory Command: Filled 1 bytes at address: 0x00000000 with pattern: 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMemCmd_Test_MEM8 */

void MM_FillMemCmd_Test_SymNameError (void)
{
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_MEM8;
    CmdPacket.NumOfBytes = 1;

    strncpy(CmdPacket.DestSymAddress.SymName, "name", OS_MAX_PATH_LEN);

    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);
    Ut_OSAPI_ContinueReturnCodeAfterCountZero(UT_OSAPI_SYMBOLLOOKUP_INDEX);

    /* Execute the function being tested */
    MM_FillMemCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMemCmd_Test_SymNameError */

void MM_FillMem_Test_Nominal (void)
{
    uint32 DestAddress = 0;
    MM_FillMemCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.NumOfBytes = 2;

    /* Execute the function being tested */
    MM_FillMem(DestAddress, &CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_FILL, "MM_AppData.LastAction == MM_FILL");
    UtAssert_True (MM_AppData.MemType == CmdPacket.MemType, "MM_AppData.MemType == CmdPacket.MemType");
    UtAssert_True (MM_AppData.Address == DestAddress, "MM_AppData.Address == DestAddress");
    UtAssert_True (MM_AppData.DataValue == CmdPacket.FillPattern, "MM_AppData.DataValue == CmdPacket.FillPattern");
    UtAssert_True (MM_AppData.BytesProcessed == 2, "MM_AppData.BytesProcessed == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_FillMem_Test_Nominal */

void MM_FillMem_Test_MemCpyError (void)
{
    uint32 DestAddress = 0;
    MM_FillMemCmd_t CmdPacket;
    char ErrorMessage[125];

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    CmdPacket.MemType = MM_EEPROM;
    CmdPacket.NumOfBytes = 2;

    /* Set to generate error message MM_PSP_COPY_ERR_EID */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_FillMem(DestAddress, &CmdPacket);

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    sprintf (ErrorMessage, "PSP copy memory error: RC=0xFFFFFFFF, Src=0x%08X, Tgt=0x00000000, Size=0x00000002", (unsigned int)(&MM_AppData.FillBuffer[0]));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR, ErrorMessage),
        ErrorMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_FillMem_Test_MemCpyError */

void MM_VerifyFillParams_Test_RAMValidateRangeError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_RAM;
    uint32  SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_RAMValidateRangeError */

void MM_VerifyFillParams_Test_RAMDataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_RAM;
    uint32  SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_RAMDataSizeErrorTooSmall */

void MM_VerifyFillParams_Test_RAMDataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_RAM;
    uint32  SizeInBytes = MM_MAX_FILL_DATA_RAM + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_RAMDataSizeErrorTooLarge */

void MM_VerifyFillParams_Test_EEPROMValidateRangeError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_EEPROM;
    uint32  SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 1 MemType = 2"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 1 MemType = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_EEPROMValidateRangeError */

void MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_EEPROM;
    uint32  SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooSmall */

void MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_EEPROM;
    uint32  SizeInBytes = MM_MAX_FILL_DATA_EEPROM + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 131073"),
        "Data size in bytes invalid or exceeds limits: Data Size = 131073");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooLarge */

void MM_VerifyFillParams_Test_MEM32ValidateRangeError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM32;
    uint32  SizeInBytes = 4;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 4 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 4 MemType = 1");

    /* NOTE: MemType passed into the function is not the same as "MemType" reported by error message - in error message it's always either CFE_PSP_MEM_RAM or CFE_PSP_MEM_EEPROM */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM32ValidateRangeError */

void MM_VerifyFillParams_Test_MEM32DataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM32;
    uint32  SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM32DataSizeErrorTooSmall */

void MM_VerifyFillParams_Test_MEM32DataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM32;
    uint32  SizeInBytes = MM_MAX_FILL_DATA_MEM32 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM32DataSizeErrorTooLarge */

void MM_VerifyFillParams_Test_MEM32AlignmentError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM32;
    uint32  SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR, "Data and address not 32 bit aligned: Addr = 0x00000001 Size = 1"),
        "Data and address not 32 bit aligned: Addr = 0x00000001 Size = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM32AlignmentError */

void MM_VerifyFillParams_Test_MEM16ValidateRangeError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM16;
    uint32  SizeInBytes = 2;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 2 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 2 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM16ValidateRangeError */

void MM_VerifyFillParams_Test_MEM16DataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM16;
    uint32  SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM16DataSizeErrorTooSmall */

void MM_VerifyFillParams_Test_MEM16DataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM16;
    uint32  SizeInBytes = MM_MAX_FILL_DATA_MEM16 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM16DataSizeErrorTooLarge */

void MM_VerifyFillParams_Test_MEM16AlignmentError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM16;
    uint32  SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR, "Data and address not 16 bit aligned: Addr = 0x00000001 Size = 1"),
        "Data and address not 16 bit aligned: Addr = 0x00000001 Size = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM16AlignmentError */

void MM_VerifyFillParams_Test_MEM8ValidateRangeError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM8;
    uint32  SizeInBytes = 1;

    /* Set to generate error message MM_OS_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 1 MemType = 1"),
        "CFE_PSP_MemValidateRange error received: RC = 0xFFFFFFFF Addr = 0x00000001 Size = 1 MemType = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM16ValidateRangeError */

void MM_VerifyFillParams_Test_MEM8DataSizeErrorTooSmall (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM8;
    uint32  SizeInBytes = 0;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 0"),
        "Data size in bytes invalid or exceeds limits: Data Size = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM8DataSizeErrorTooSmall */

void MM_VerifyFillParams_Test_MEM8DataSizeErrorTooLarge (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = MM_MEM8;
    uint32  SizeInBytes = MM_MAX_FILL_DATA_MEM8 + 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR, "Data size in bytes invalid or exceeds limits: Data Size = 1048577"),
        "Data size in bytes invalid or exceeds limits: Data Size = 1048577");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM8DataSizeErrorTooLarge */

void MM_VerifyFillParams_Test_InvalidMemTypeError (void)
{
    boolean Result;
    uint32  Address     = 1;
    uint8   MemType     = 99;
    uint32  SizeInBytes = 1;

    /* Execute the function being tested */
    Result = MM_VerifyFillParams(Address, MemType, SizeInBytes);

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR, "Invalid memory type specified: MemType = 99"),
        "Invalid memory type specified: MemType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_VerifyFillParams_Test_MEM8InvalidMemTypeError */

/* Note: Can not test the case of MM_LoadMemFromFileCmd where it gets a bad MemType, because the previous call to MM_VerifyFileLoadParams would return FALSE, causing the relevant block of code to be skipped */
/* Note: No need for MM_LoadMemFromFile_Test_Nominal, MM_VerifyFileLoadParams_Test_Nominal, MM_VerifyLoadFileSize_Test_Nominal, MM_ReadFileHeaders_Test_Nominal, MM_VerifyFillParams_Test_Nominal - already covered */

void MM_Load_Test_AddTestCases(void)
{
    UtTest_Add(MM_PokeCmd_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_PokeCmd_Test_EEPROM");
    UtTest_Add(MM_PokeCmd_Test_NonEEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_PokeCmd_Test_NonEEPROM");
    UtTest_Add(MM_PokeCmd_Test_SymNameError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeCmd_Test_SymNameError");
    UtTest_Add(MM_PokeMem_Test_8bit, MM_Test_Setup, MM_Test_TearDown, "MM_PokeMem_Test_8bit");
    UtTest_Add(MM_PokeMem_Test_8bitError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeMem_Test_8bitError");
    UtTest_Add(MM_PokeMem_Test_16bit, MM_Test_Setup, MM_Test_TearDown, "MM_PokeMem_Test_16bit");
    UtTest_Add(MM_PokeMem_Test_16bitError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeMem_Test_16bitError");
    UtTest_Add(MM_PokeMem_Test_32bit, MM_Test_Setup, MM_Test_TearDown, "MM_PokeMem_Test_32bit");
    UtTest_Add(MM_PokeMem_Test_32bitError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeMem_Test_32bitError");
    UtTest_Add(MM_PokeEeprom_Test_8bit, MM_Test_Setup, MM_Test_TearDown, "MM_PokeEeprom_Test_8bit");
    UtTest_Add(MM_PokeEeprom_Test_8bitError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeEeprom_Test_8bitError");
    UtTest_Add(MM_PokeEeprom_Test_16bit, MM_Test_Setup, MM_Test_TearDown, "MM_PokeEeprom_Test_16bit");
    UtTest_Add(MM_PokeEeprom_Test_16bitError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeEeprom_Test_16bitError");
    UtTest_Add(MM_PokeEeprom_Test_32bit, MM_Test_Setup, MM_Test_TearDown, "MM_PokeEeprom_Test_32bit");
    UtTest_Add(MM_PokeEeprom_Test_32bitError, MM_Test_Setup, MM_Test_TearDown, "MM_PokeEeprom_Test_32bitError");
    UtTest_Add(MM_LoadMemWIDCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemWIDCmd_Test_Nominal");
    UtTest_Add(MM_LoadMemWIDCmd_Test_CopyError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemWIDCmd_Test_CopyError");
    UtTest_Add(MM_LoadMemWIDCmd_Test_CRCError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemWIDCmd_Test_CRCError");
    UtTest_Add(MM_LoadMemWIDCmd_Test_SymNameErr, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemWIDCmd_Test_SymNameErr");
    UtTest_Add(MM_VerifyLoadWIDParams_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyLoadWIDParams_Test_Nominal");
    UtTest_Add(MM_VerifyLoadWIDParams_Test_MemValidateError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyLoadWIDParams_Test_MemValidateError");
    UtTest_Add(MM_VerifyLoadWIDParams_Test_DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyLoadWIDParams_Test_DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyLoadWIDParams_Test_DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyLoadWIDParams_Test_DataSizeErrorTooLarge");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_RAM, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_RAM");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_EEPROM");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_MEM32, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_MEM32");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_MEM16, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_MEM16");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_MEM8");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_LoadParamsError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_LoadParamsError");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_SymNameError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_SymNameError");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_LoadFileCRCError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_LoadFileCRCError");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_ComputeCRCError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_ComputeCRCError");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_CloseError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_CloseError");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_OpenError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_OpenError");
    UtTest_Add(MM_LoadMemFromFileCmd_Test_InvalidFilename, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFileCmd_Test_InvalidFilename");
    UtTest_Add(MM_LoadMemFromFile_Test_PreventCPUHogging, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFile_Test_PreventCPUHogging");
    UtTest_Add(MM_LoadMemFromFile_Test_MemCpyError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFile_Test_MemCpyError");
    UtTest_Add(MM_LoadMemFromFile_Test_ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_LoadMemFromFile_Test_ReadError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_RAMValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_RAMValidateRangeError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_RAMDataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFileLoadParams_Test_EEPROMValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_EEPROMValidateRangeError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_EEPROMDataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM32ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM32ValidateRangeError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM32DataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM32AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM32AlignmentError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM16ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM16ValidateRangeError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM16DataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM16AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM16AlignmentError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM8ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM8ValidateRangeError");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_MEM8DataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFileLoadParams_Test_InvalidMemTypeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFileLoadParams_Test_InvalidMemTypeError");
    UtTest_Add(MM_VerifyLoadFileSize_Test_StatError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyLoadFileSize_Test_StatError");
    UtTest_Add(MM_VerifyLoadFileSize_Test_SizeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyLoadFileSize_Test_SizeError");
    UtTest_Add(MM_ReadFileHeaders_Test_ReadHeaderError, MM_Test_Setup, MM_Test_TearDown, "MM_ReadFileHeaders_Test_ReadHeaderError");
    UtTest_Add(MM_ReadFileHeaders_Test_ReadError, MM_Test_Setup, MM_Test_TearDown, "MM_ReadFileHeaders_Test_ReadError");
    UtTest_Add(MM_FillMemCmd_Test_RAM, MM_Test_Setup, MM_Test_TearDown, "MM_FillMemCmd_Test_RAM");
    UtTest_Add(MM_FillMemCmd_Test_EEPROM, MM_Test_Setup, MM_Test_TearDown, "MM_FillMemCmd_Test_EEPROM");
    UtTest_Add(MM_FillMemCmd_Test_MEM32, MM_Test_Setup, MM_Test_TearDown, "MM_FillMemCmd_Test_MEM32");
    UtTest_Add(MM_FillMemCmd_Test_MEM16, MM_Test_Setup, MM_Test_TearDown, "MM_FillMemCmd_Test_MEM16");
    UtTest_Add(MM_FillMemCmd_Test_MEM8, MM_Test_Setup, MM_Test_TearDown, "MM_FillMemCmd_Test_MEM8");
    UtTest_Add(MM_FillMemCmd_Test_SymNameError, MM_Test_Setup, MM_Test_TearDown, "MM_FillMemCmd_Test_SymNameError");
    UtTest_Add(MM_FillMem_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_FillMem_Test_Nominal");
    UtTest_Add(MM_FillMem_Test_MemCpyError, MM_Test_Setup, MM_Test_TearDown, "MM_FillMem_Test_MemCpyError");
    UtTest_Add(MM_VerifyFillParams_Test_RAMValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_RAMValidateRangeError");
    UtTest_Add(MM_VerifyFillParams_Test_RAMDataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_RAMDataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFillParams_Test_RAMDataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_RAMDataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFillParams_Test_EEPROMValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_EEPROMValidateRangeError");
    UtTest_Add(MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_EEPROMDataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFillParams_Test_MEM32ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM32ValidateRangeError");
    UtTest_Add(MM_VerifyFillParams_Test_MEM32DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM32DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFillParams_Test_MEM32DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM32DataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFillParams_Test_MEM32AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM32AlignmentError");
    UtTest_Add(MM_VerifyFillParams_Test_MEM16ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM16ValidateRangeError");
    UtTest_Add(MM_VerifyFillParams_Test_MEM16DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM16DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFillParams_Test_MEM16DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM16DataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFillParams_Test_MEM16AlignmentError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM16AlignmentError");
    UtTest_Add(MM_VerifyFillParams_Test_MEM8ValidateRangeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM8ValidateRangeError");
    UtTest_Add(MM_VerifyFillParams_Test_MEM8DataSizeErrorTooSmall, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM8DataSizeErrorTooSmall");
    UtTest_Add(MM_VerifyFillParams_Test_MEM8DataSizeErrorTooLarge, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_MEM8DataSizeErrorTooLarge");
    UtTest_Add(MM_VerifyFillParams_Test_InvalidMemTypeError, MM_Test_Setup, MM_Test_TearDown, "MM_VerifyFillParams_Test_InvalidMemTypeError");
} /* end MM_Load_Test_AddTestCases */


/************************/
/*  End of File Comment */
/************************/
