/************************************************************************
** File:
**   $Id: utf_test_cs.c 1.5.1.1 2015/03/03 11:58:30EST sstrege Exp  $
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
** Purpose: 
**   This is a test driver used to unit test the CFS Checksum (CS)
**   Application.

** 
**   Output can be directed either to screen or to file:
**   To direct output to screen, 
**      comment in '#define UTF_USE_STDOUT' statement in the
**      utf_custom.h file.
**
**   To direct output to file, 
**      comment out '#define UTF_USE_STDOUT' statement in 
**      utf_custom.h file.
** 
**   $Log: utf_test_cs.c  $
**   Revision 1.5.1.1 2015/03/03 11:58:30EST sstrege 
**   Added copyright information
**   Revision 1.5 2012/09/14 17:20:42EDT aschoeni 
**   Updated for fix to disable error in def table command
**   Revision 1.4 2011/09/19 13:58:17EDT jmdagost 
**   CS Unit test updates
**   Revision 1.3 2010/05/03 17:23:13EDT jmdagost 
**   Major revisions to update to latest cFE and cover CS code changes since last unit test.
**   Revision 1.2 2008/09/08 10:39:39EDT njyanchik 
**   Checking in of Updated unit test
**   Revision 1.1 2008/07/23 15:28:20BST njyanchik 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/project.pj
** 
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cs_app.h"            /* Application headers */
#include "cs_events.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_app_cmds.h"
#include "cs_table_cmds.h"
#include "cs_eeprom_cmds.h"
#include "cs_memory_cmds.h"


#include "cfe_es_cds.h"        /* cFE headers         */
#include "cfe_tbl_task.h"

#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_cfe.h"
/*#include "utf_osapiarch.h"*/


#include <sys/fcntl.h>         /* System headers      */
#include <unistd.h>
#include <stdlib.h>

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

#define CS_CMD_PIPE		  1

#define CS_APP_RECORD_ID  0
#define QQ_APP_RECORD_ID  1
#define AA_APP_RECORD_ID  2
#define BB_APP_RECORD_ID  3
#define CC_APP_RECORD_ID  4
#define DD_APP_RECORD_ID  5
#define EE_APP_RECORD_ID  6
#define FF_APP_RECORD_ID  7
#define GG_APP_RECORD_ID  8
#define HH_APP_RECORD_ID  9
#define II_APP_RECORD_ID  10
#define JJ_APP_RECORD_ID  11
#define KK_APP_RECORD_ID  12
#define LL_APP_RECORD_ID  13
#define MM_APP_RECORD_ID  14
#define NN_APP_RECORD_ID  15
#define OO_APP_RECORD_ID  16
#define PP_APP_RECORD_ID  17
#define RR_APP_RECORD_ID  18
#define SS_APP_RECORD_ID  19
#define TT_APP_RECORD_ID  20
#define UU_APP_RECORD_ID  21
#define VV_APP_RECORD_ID  22
#define WW_APP_RECORD_ID  23
#define AX_APP_RECORD_ID  24

#define SIZE_OF_EEPROM_TO_TEST   512
#define SIZE_OF_MEMORY_TO_TEST   256

/*******************/
/* TBL global data */
/*******************/

extern CFE_TBL_TaskData_t CFE_TBL_TaskData;

/************************************************************************
** CS global data
*************************************************************************/
extern  CS_AppData_t CS_AppData;

/************************************************************************
 ** CS test data set
 *************************************************************************/
uint8   CS_TestDataSet[20000];


/************************************************************************
** Local function prototypes
*************************************************************************/

void    PrintHkPacket(void);

int32   CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);

int32 CFE_TBL_LoadHook(CFE_TBL_Handle_t TblHandle,
                       CFE_TBL_SrcEnum_t SrcType,
                       const void *SrcDataPtr);


void    TimeHook(void);

void CS_PrintEepromTable(void);
void CS_PrintMemoryTable(void);
void CS_PrintTablesTable(void);
void CS_PrintAppTable(void);
void CS_AppDataDump(void);
void UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[]);
void CS_PrintTableData(int argc, char *argv[]);
void CS_BackgroundCheck(int argc, char *argv[]);
void CS_ClearChildTaskFlags(int argc, char *argv[]);
void CS_ForceMiscompares(int argc, char *argv[]);
void CS_RecomputeInvalidNames(int argc, char *argv[]);
void CS_UnregisterQQTables(int argc, char *argv[]);
void CS_ReregisterQQTables(int argc, char *argv[]);
void CS_SetAppInfoTrue(int argc, char *argv[]);
void CS_SetAppInfoFalse(int argc, char *argv[]);
void CS_AddEepromMemoryResAddresses(int argc, char *argv[]);
void CS_RemoveDefTableEntry(int argc, char *argv[]);
void CS_SetEepromMemoryTblEntry11(int argc, char *argv[]);
void CS_ResetEepromMemoryTblEntry11(int argc, char *argv[]);
void CS_SetOScFEStatus(int argc, char *argv[]);
int32 CS_GetAddressHook( void **TblPtr,
                        CFE_TBL_Handle_t TblHandle );
int32 CS_GetAddressHook2( void **TblPtr,
                        CFE_TBL_Handle_t TblHandle );
void CS_AddEepromMemoryResAddressesInternal(int MemType);



CFE_TBL_Handle_t TblHandle1;
CFE_TBL_Handle_t TblHandle2;
CFE_TBL_Handle_t AATblHandle1;
CFE_TBL_Handle_t BBTblHandle1;
CFE_TBL_Handle_t CCTblHandle1;
CFE_TBL_Handle_t DDTblHandle1;
CFE_TBL_Handle_t EETblHandle1;
CFE_TBL_Handle_t FFTblHandle1;
CFE_TBL_Handle_t GGTblHandle1;
CFE_TBL_Handle_t HHTblHandle1;
CFE_TBL_Handle_t IITblHandle1;
CFE_TBL_Handle_t JJTblHandle1;
CFE_TBL_Handle_t KKTblHandle1;
CFE_TBL_Handle_t LLTblHandle1;
CFE_TBL_Handle_t MMTblHandle1;
CFE_TBL_Handle_t AATblHandle2;
CFE_TBL_Handle_t BBTblHandle2;
CFE_TBL_Handle_t CCTblHandle2;
CFE_TBL_Handle_t DDTblHandle2;
CFE_TBL_Handle_t EETblHandle2;
CFE_TBL_Handle_t FFTblHandle2;
CFE_TBL_Handle_t GGTblHandle2;
CFE_TBL_Handle_t HHTblHandle2;
CFE_TBL_Handle_t IITblHandle2;
CFE_TBL_Handle_t AXTblHandle1;
CFE_ES_AppInfo_t AppInfo;
uint32           TableTestData = 0xAABBCCDD;

/* external declaration of CS functions tested explicitly */
extern void CS_BackgroundCheckCmd (CFE_SB_MsgPtr_t MessagePtr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Program main                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{
    char                        AppName[10];
    char                        command [100];
    int32                       Status;
    CS_NoArgsCmd_t              Background;
    uint32                      Computed;
    boolean                     Done;
    CS_Res_Tables_Table_Entry_t Entry;
    int32                       TempStatus;
    
    /*
     ** Set up output file           
     */
    UTF_set_output_filename("cs_utf_test.out");

    UTF_put_text("---------------------\n\n");
    UTF_put_text("Start of CS Unit Test\n");
    UTF_put_text("---------------------\n\n");
    
    
    strcpy(AppName, "CS");
    
    /*
     ** Set up to read in script
     */
    UTF_add_input_file(CS_CMD_PIPE, "cs_utf_cmds.in");
    CS_AppData.CmdPipe = CS_CMD_PIPE;  /* Hook for application code */
    	
    UTF_add_special_command("LOAD_TABLE_FROM_GROUND", UTF_SCRIPT_LoadTableFromGround);
    UTF_add_special_command("CS_PRINT_TABLE_DATA",    CS_PrintTableData);
    UTF_add_special_command("CS_BACKGROUND_CHECK",    CS_BackgroundCheck);
    UTF_add_special_command("CS_CLEAR_CHILD_FLAGS",   CS_ClearChildTaskFlags);    
    UTF_add_special_command("SET_ES_RETURN_CODE",     UTF_SCRIPT_ES_Set_Api_Return_Code);
    UTF_add_special_command("SET_PSP_RETURN_CODE", UTF_SCRIPT_PSP_Set_Api_Return_Code);
    UTF_add_special_command("SET_OSARCH_RETURN_CODE", UTF_SCRIPT_PSP_Set_Api_Return_Code);
    UTF_add_special_command("SET_TBL_RETURN_CODE",    UTF_SCRIPT_TBL_Set_Api_Return_Code);
    UTF_add_special_command("SET_SB_RETURN_CODE",     UTF_SCRIPT_SB_Set_Api_Return_Code);
    UTF_add_special_command("CS_FORCE_MISCOMPARES",   CS_ForceMiscompares);
    UTF_add_special_command("CS_RECOMPUTE_INVALID_NAMES",   CS_RecomputeInvalidNames);    
    UTF_add_special_command("CS_UNREGISTER_QQ_TABLES",   CS_UnregisterQQTables);      
    UTF_add_special_command("CS_REREGISTER_QQ_TABLES",   CS_ReregisterQQTables);      
    UTF_add_special_command("CS_SET_APP_INFO_TRUE",   CS_SetAppInfoTrue);      
    UTF_add_special_command("CS_SET_APP_INFO_FALSE",   CS_SetAppInfoFalse);      
    UTF_add_special_command("CS_ADD_EEPROM_MEMORY_RES_ADDRESSES",   CS_AddEepromMemoryResAddresses);      
    UTF_add_special_command("CS_REMOVE_DEF_TABLE_ENTRY",   CS_RemoveDefTableEntry);      
    UTF_add_special_command("CS_SET_EEPROM_MEMORY_TBL_ENTRY_11",   CS_SetEepromMemoryTblEntry11);      
    UTF_add_special_command("CS_RESET_EEPROM_MEMORY_TBL_ENTRY_11",   CS_ResetEepromMemoryTblEntry11);      
    UTF_add_special_command("CS_SET_OS_CFE_STATUS",   CS_SetOScFEStatus);      
       

    /* Need to set return code of CFE_PSP_MemValidateRange to 'CFE_PSP_SUCCESS' so that the test can pass */
    TempStatus = UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, CFE_PSP_SUCCESS);
/*    TempStatus = UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, CFE_PSP_ERROR);
*/    
    /*
     ** Add an entry to the volume table
     */
    UTF_add_volume("/", "cf", FS_BASED, FALSE, FALSE, TRUE, "CF", "/cf", 0);
    
    /*
     ** Set up Hk packet handler           
     */
    UTF_set_packet_handler(CS_HK_TLM_MID, (utf_packet_handler)PrintHkPacket);

    UTF_put_text("\n<utf_test_cs> Unit Test Initialization\n");
    UTF_put_text("--------------------------------------\n\n");
	
    /*
     ** Initialize time data structures
     */
    UTF_init_sim_time(0.0);
    UTF_OSAPI_set_function_hook(OS_GETLOCALTIME_HOOK, TimeHook);
    
    /*
     ** Register app CS with executive services.                         
     */

    UTF_put_text("\n<utf_test_cs> Registering Apps...\n");
    UTF_put_text("---------------------------------\n\n");
    
    UTF_ES_InitAppRecords();
    UTF_ES_AddAppRecord(AppName,CS_APP_RECORD_ID);  

    /* Register additional apps so CS can verify unique app names in app table */
    UTF_ES_AddAppRecord("QQ", QQ_APP_RECORD_ID);  /* QQ is used for other tests, too */
    UTF_ES_AddAppRecord("AA",AA_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("BB",BB_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("CC",CC_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("DD",DD_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("EE",EE_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("FF",FF_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("GG",GG_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("HH",HH_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("II",II_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("JJ",JJ_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("KK",KK_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("LL",LL_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("MM",MM_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("NN",NN_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("OO",OO_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("PP",PP_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("RR",RR_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("SS",SS_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("TT",TT_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("UU",UU_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("VV",VV_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("WW",WW_APP_RECORD_ID);  
    UTF_ES_AddAppRecord("AX",AX_APP_RECORD_ID);  

    UTF_put_text("\n<utf_test_cs> Done registering Apps\n\n");
    
    /*
     ** Initialize table services data structures, though we
     ** don't use any tables for these tests
     */
    CFE_ES_CDS_EarlyInit();
    CFE_TBL_EarlyInit();

    UTF_put_text("\n<utf_test_cs> Load non-CS tables\n");
    UTF_put_text("--------------------------------\n\n");
    
    /* Load tables that don't belong to CS so CS can verify unique table names */

    /* Start with two tables for QQ */
    
    UTF_CFE_ES_SetAppID(QQ_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register another table to hold a table handle */
    Status = CFE_TBL_Register( &TblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( TblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(TblHandle2);
    
    /* Register one table to be checksummed */
    Status = CFE_TBL_Register( &TblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( TblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(TblHandle1);
    

    /* Two Tables for AA */
    
    UTF_CFE_ES_SetAppID(AA_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &AATblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( AATblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(AATblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &AATblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( AATblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(AATblHandle2);
    
    
    /* Two Tables for BB */
    
    UTF_CFE_ES_SetAppID(BB_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &BBTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( BBTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(BBTblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &BBTblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( BBTblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(BBTblHandle2);
    
    
    /* Two Tables for CC */
    
    UTF_CFE_ES_SetAppID(CC_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &CCTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( CCTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(CCTblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &CCTblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( CCTblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(CCTblHandle2);
    
    
    /* Two Tables for DD */
    
    UTF_CFE_ES_SetAppID(DD_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &DDTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( DDTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(DDTblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &DDTblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( DDTblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(DDTblHandle2);
    
    
    /* Two Tables for EE */
    
    UTF_CFE_ES_SetAppID(EE_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &EETblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( EETblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(EETblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &EETblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( EETblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(EETblHandle2);
    
    
    /* Two Tables for FF */
    
    UTF_CFE_ES_SetAppID(FF_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &FFTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( FFTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(FFTblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &FFTblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( FFTblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(FFTblHandle2);
    
    
    /* Two Tables for GG */
    
    UTF_CFE_ES_SetAppID(GG_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &GGTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( GGTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(GGTblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &GGTblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( GGTblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(GGTblHandle2);
    
    
    /* Two Tables for HH */
    
    UTF_CFE_ES_SetAppID(HH_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &HHTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( HHTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(HHTblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &HHTblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( HHTblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(HHTblHandle2);
    
    
    /* Two Tables for II */
    
    UTF_CFE_ES_SetAppID(II_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &IITblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( IITblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(IITblHandle1);
    
    /* Register table 2 to be checksummed */
    Status = CFE_TBL_Register( &IITblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( IITblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(IITblHandle2);
    
    
    /* One Table for JJ */
    
    UTF_CFE_ES_SetAppID(JJ_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table to be checksummed */
    Status = CFE_TBL_Register( &JJTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( JJTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(JJTblHandle1);
    
    
    /* One Table for KK */
    
    UTF_CFE_ES_SetAppID(KK_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table to be checksummed */
    Status = CFE_TBL_Register( &KKTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( KKTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(KKTblHandle1);
    
    
    /* One Table for LL */
    
    UTF_CFE_ES_SetAppID(LL_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table to be checksummed */
    Status = CFE_TBL_Register( &LLTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( LLTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(LLTblHandle1);
    
    
    /* One Table for MM */
    
    UTF_CFE_ES_SetAppID(MM_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table to be checksummed */
    Status = CFE_TBL_Register( &MMTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( MMTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(MMTblHandle1);

    /* One Tables for AX */
    
    UTF_CFE_ES_SetAppID(AX_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /* Register table 1 to be checksummed */
    Status = CFE_TBL_Register( &AXTblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( AXTblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(AXTblHandle1);

    
    /* Done loading tables */    

    UTF_put_text("\n<utf_test_cs> Done loading non-CS tables\n");
    
    UTF_CFE_ES_SetAppID(CS_APP_RECORD_ID);
    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    
    /* Set up the AppInfo properly */
    CS_SetAppInfoTrue(0, 0);
    
    UTF_put_text("\n<utf_test_cs> End Setup and Initialization\n");
    UTF_put_text("------------------------------------------\n");
    
    
    strcpy(command ,"cp -v cf/apps/DefaultTable/*.tbl cf/apps/");
    system(command);
    
    UTF_put_text("\n<utf_test_cs> Call #1 to CS_AppMain() with default tables \n");
    UTF_put_text("----------------------------------------------------------\n\n");
    
    CS_AppMain();

    UTF_put_text("\n<utf_test_cs> Exited #1 CS_AppMain()\n\n");

    /* Manually fire off child tasks */
    
    UTF_put_text("\n<utf_test_cs> Manually fire off child task (CFE Core)\n");
    UTF_put_text("-----------------------------------------------------\n\n");
       
    CS_AppData.ChildTaskTable = 0;
    CS_AppData.ChildTaskEntryID = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.CfeCoreCodeSeg;
    CS_RecomputeEepromMemoryChildTask();
    
    /* Test the condition of CFE_ES_ChildTaskRegister failing */
    UTF_put_text("\n<utf_test_cs> Test condition of memory child task registration failing\n");
    UTF_put_text("----------------------------------------------------------------------\n\n");   
    
    UTF_CFE_ES_Set_Api_Return_Code(15,-1);
    CS_RecomputeEepromMemoryChildTask();    
    UTF_CFE_ES_Set_Api_Return_Code(15,0); 
    
    /* Test each of the Recomputing  using a child task functions */
    UTF_put_text("\n<utf_test_cs> Test recompute of OS (child task)\n");
    UTF_put_text("-----------------------------------------------\n\n");
    
    CS_AppData.ChildTaskTable = 1;
    CS_AppData.ChildTaskEntryID = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.OSCodeSeg;
    CS_RecomputeEepromMemoryChildTask();
    
    UTF_put_text("\n<utf_test_cs> Test recompute of EEPROM (child task)\n");
    UTF_put_text("---------------------------------------------------\n\n");
    
    CS_AppData.ChildTaskTable = 2;
    CS_AppData.ChildTaskEntryID = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr = CS_AppData.ResEepromTblPtr;
    CS_RecomputeEepromMemoryChildTask();
    

    UTF_put_text("\n<utf_test_cs> Test recompute of memory (child task)\n");
    UTF_put_text("---------------------------------------------------\n\n");
    
    CS_AppData.ChildTaskTable = 3;
    CS_AppData.ChildTaskEntryID = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr = CS_AppData.ResMemoryTblPtr;

    CS_RecomputeEepromMemoryChildTask();
   
    UTF_put_text("\n<utf_test_cs> Test recompute of Apps (child task)\n");
    UTF_put_text("-------------------------------------------------\n\n");
    
    CS_AppData.RecomputeAppEntryPtr = CS_AppData.ResAppTblPtr;
    CS_RecomputeAppChildTask();   
    
    /* Test the condition of CFE_ES_ChildTaskRegister failing */
    
    UTF_put_text("\n<utf_test_cs> Test condition of App child task registration failing\n");
    UTF_put_text("-------------------------------------------------------------------\n\n");
    
    UTF_CFE_ES_Set_Api_Return_Code(15,-1);
    CS_RecomputeAppChildTask();  
    UTF_CFE_ES_Set_Api_Return_Code(15,0); 

    UTF_put_text("\n<utf_test_cs> Test recompute of Tables (child task)\n");
    UTF_put_text("---------------------------------------------------\n\n");
    
    CS_AppData.RecomputeTablesEntryPtr = CS_AppData.ResTablesTblPtr;
    CS_RecomputeTablesChildTask();   
    
    /* Test the condition of CFE_ES_ChildTaskRegister failing */
    
    UTF_put_text("\n<utf_test_cs> Test condition of Tables child task registration failing\n");
    UTF_put_text("----------------------------------------------------------------------\n\n");
    
    UTF_CFE_ES_Set_Api_Return_Code(15,-1);
    CS_RecomputeTablesChildTask();   
    UTF_CFE_ES_Set_Api_Return_Code(15,0); 
    
    UTF_put_text("\n<utf_test_cs> Test one-shot (child task)\n");
    UTF_put_text("----------------------------------------\n\n");
        
    CS_AppData.LastOneShotSize = 200;
/*    CS_AppData.LastOneShotAddress = 0x1F1B4; <--- this value causes segfault */
    CS_AppData.LastOneShotAddress = (uint32)(&CS_TestDataSet[0]);
    CS_OneShotChildTask();

    /* Test the condition of CFE_ES_ChildTaskRegister failing */
    UTF_put_text("\n<utf_test_cs> Test condition of one-shot child task registration failing\n");
    UTF_put_text("------------------------------------------------------------------------\n\n");
    
    UTF_CFE_ES_Set_Api_Return_Code(15, -1);
    CS_OneShotChildTask();
    UTF_CFE_ES_Set_Api_Return_Code(15, 0);

    
    /* Reset TBL for a fresh start */
    
    CFE_TBL_EarlyInit();
    
    strcpy(command ,"cp -v cf/apps/CorrFullTable/*.tbl cf/apps/");
    system(command);
    
    UTF_put_text("\n<utf_test_cs> Call #2 to CS_AppMain()\n");
    UTF_put_text("-------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #2 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");
    
    strcpy(command ,"rm -v cf/apps/*.tbl");
    system(command);
    
   
    /* Test startup errors */
    UTF_put_text("\n<utf_test_cs> Test the Startup Errors\n");
    UTF_put_text("-------------------------------------\n\n");
    
    
    /* Get the three Subscribes to fail */
    UTF_put_text("\n<utf_test_cs> Get the three subscribes to fail\n");
    UTF_put_text("----------------------------------------------\n\n");
    
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK,
                             (void *)&CFE_SB_SubscribeHook);
    UTF_put_text("\n<utf_test_cs> Call #3 to CS_AppMain() - subscribe error #1\n");
    UTF_put_text("----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #3 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");

    
    UTF_put_text("\n<utf_test_cs> Call #4 to CS_AppMain() - subscribe error #2\n");
    UTF_put_text("----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #4 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");

    
    UTF_put_text("\n<utf_test_cs> Call #5 to CS_AppMain() - subscribe error #3\n");
    UTF_put_text("----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #5 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");
    
    /* Test four failures of TBL Init */

    UTF_put_text("\n<utf_test_cs> Test four failures of table init\n");
    UTF_put_text("----------------------------------------------\n\n");
    
    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK,
                              (void *)&CFE_TBL_LoadHook);
    
    CFE_TBL_EarlyInit(); 
    strcpy(command ,"cp -v cf/apps/CorrFullTable/*.tbl cf/apps/");
    system(command);
    
    UTF_put_text("\n<utf_test_cs> Call #6 to CS_AppMain() - Table init error #1\n");
    UTF_put_text("-----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #6 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");

    
    UTF_put_text("\n<utf_test_cs> Call #7 to CS_AppMain() - Table init error #2\n");
    UTF_put_text("-----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #7 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");

    
    UTF_put_text("\n<utf_test_cs> Call #8 to CS_AppMain() - Table init error #3\n");
    UTF_put_text("-----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #8 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");

    
    UTF_put_text("\n<utf_test_cs> Call #9 to CS_AppMain() - Table init error #4\n");
    UTF_put_text("-----------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #9 CS_AppMain()\n");
    UTF_put_text("------------------------------------\n\n");
    
    
    strcpy(command ,"rm -v cf/apps/*.tbl");
    system(command);

    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, NULL);

    
    /* Make CreatePipe's fail */
    UTF_put_text("\n<utf_test_cs> Test CreatePipe failures\n");
    UTF_put_text("--------------------------------------\n\n");
    
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, -1);
    
    UTF_put_text("\n<utf_test_cs> Call #10 to CS_AppMain() - Pipe creation error\n");
    UTF_put_text("------------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #10 CS_AppMain()\n");
    UTF_put_text("-------------------------------------\n\n");
    
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, CFE_SUCCESS);

    
    /* Now restart the main app with the tables in the correct area
       Initialize table services data structures again for a fresh start
       Also test the path of the Kernel Text segment not getting set
     */
    
    
    /*UTF_OSBSP_Set_Api_Return_Code(OS_BSPGETKERNELTEXTSEGMENTINFO_PROC, OS_ERR_NOT_IMPLEMENTED);*/
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC, OS_ERR_NOT_IMPLEMENTED);
    CFE_TBL_EarlyInit(); 
    
    strcpy(command ,"rm -v cf/apps/*.tbl");
    system(command);
    
    
    UTF_put_text("\n<utf_test_cs> Call #11 to CS_AppMain() with memory-loaded tables - Kernel Text segment not set\n");
    UTF_put_text("---------------------------------------------------------------------------------------------\n\n");
    
    CS_AppMain();
    
    UTF_put_text("\n<utf_test_cs> Exited #11 CS_AppMain()\n");
    UTF_put_text("-------------------------------------\n\n");
    
/*
    UTF_OSBSP_Set_Api_Return_Code(OS_BSPGETKERNELTEXTSEGMENTINFO_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
*/
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);

    
    /* Need to catch the case of having a bad checksum table, in case memory gets corrupted */
    UTF_put_text("\n<utf_test_cs> Test CS_BackgroundCheckCmd -- bad checksum table\n");
    UTF_put_text("--------------------------------------------------------------\n\n");
    
    
    CFE_SB_InitMsg(&Background, CS_BACKGROUND_CYCLE_MID, sizeof(CS_NoArgsCmd_t), TRUE);
    CS_AppData.ChecksumState = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable =24;
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)& Background); 
    
    Entry.State = CS_STATE_ENABLED;
    Entry.TblHandle = 3;
    strcpy( Entry.Name, "CS.DefEepromTbl");

    
    
    /* Test Compute Tables Errors */
    
    UTF_put_text("\n<utf_test_cs> Test CS_ComputeTables -- CFE Table never loaded error\n");
    UTF_put_text("-------------------------------------------------------------------\n\n");
    
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, CFE_TBL_ERR_NEVER_LOADED);
    Status = CS_ComputeTables( &(Entry), &Computed, &Done);
    UTF_put_text("<utf_test_cs> CS_ComputeTables returned status = %d\n", Status);
    
    Entry.State = CS_STATE_ENABLED;
    Entry.TblHandle = 3;
    strcpy( Entry.Name, "CS.DefEepromTbl");
    
    UTF_put_text("\n<utf_test_cs> Test CS_ComputeTables -- CFE Table not registered error\n");
    UTF_put_text("---------------------------------------------------------------------\n\n");
    
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, CFE_TBL_ERR_UNREGISTERED);
    
    Status = CS_ComputeTables( &(Entry), &Computed, &Done);
    UTF_put_text("<utf_test_cs> CS_ComputeTables returned status = %d\n", Status);
        
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    
    Entry.State = CS_STATE_ENABLED;
    Entry.TblHandle = 3;
    strcpy( Entry.Name, "CS.ResEepromTbl");
        
    /* Get the second GetAddress Error */
    UTF_put_text("\n<utf_test_cs> Test CS_ComputeTables -- second GetAddress error\n");
    UTF_put_text("--------------------------------------------------------------\n\n");
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *) &CS_GetAddressHook);
    
    Status = CS_ComputeTables( &Entry, &Computed, &Done);
    UTF_put_text("<utf_test_cs> CS_ComputeTables returned status = %d\n", Status);

    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, NULL);
    
    /* Get the ReleaseAddress Error */
    UTF_put_text("\n<utf_test_cs> Test CS_ComputeTables -- ReleaseAddress error\n");
    UTF_put_text("--------------------------------------------------------------\n\n");
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *) &CS_GetAddressHook2); /* Hook function sets release address status */
    
    Status = CS_ComputeTables( &Entry, &Computed, &Done);
    UTF_put_text("<utf_test_cs> CS_ComputeTables returned status = %d\n", Status);
    
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_RELEASEADDRESS_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);  /* Reset to default */    
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, NULL);
    
    
    /* More path coverage */
    
    /* Get correct, non-full app table and run CS_AppMain once to load it */
    /* We need the non-full table so that we have one empty entry in the first slot of the App table */
    
    CFE_ES_CDS_EarlyInit();
    CFE_TBL_EarlyInit();
    strcpy(command ,"cp -v cf/apps/CorrNonFullTable/cs_apptbl.tbl cf/apps/");
    system(command);
    CS_AppData.RunStatus = CFE_ES_APP_ERROR;
    CS_AppMain();
    CS_AddEepromMemoryResAddressesInternal(0);    
    CS_AddEepromMemoryResAddressesInternal(1);    

    /* Manually fire off child task */
    
    UTF_put_text("\n<utf_test_cs> Test recompute of Apps (child task) for path coverage\n");
    UTF_put_text("-------------------------------------------------------------------\n\n");
    
    CS_AppData.RecomputeAppEntryPtr = CS_AppData.ResAppTblPtr;
    CS_RecomputeAppChildTask();   
    
         
    UTF_put_text("\n------------------------------\n");
    UTF_put_text("<utf_test_cs> End CS Unit Test\n");
    UTF_put_text("------------------------------\n\n");
    
    
    return 0;
   
} /* end main */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_SB_Subscribe that will return an error    */
/* on every call except the first one                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId) 
{
    static uint32 Count = 0;

    if (Count == 0 || Count == 2 || Count == 5)
    {   
        UTF_put_text("\n<utf_test_cs:CFE_SB_SubscribeHook> Count = %d, CFE_SB_MAX_MSGS_MET \n",Count);
        UTF_put_text("----------------------------------------------\n\n");
    
        Count++;
        return (CFE_SB_MAX_MSGS_MET);
    }
    else
    {
        UTF_put_text("\n<utf_test_cs:CFE_SB_SubscribeHook> Count = %d, CFE_SUCCESS \n",Count);
        UTF_put_text("----------------------------------------------\n\n");
        
        Count++;
        return CFE_SUCCESS;
    }
}/* end CFE_SB_SubscribeHook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Load that will return an error        */
/* on the calls necessary to test                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_LoadHook(CFE_TBL_Handle_t TblHandle,
                       CFE_TBL_SrcEnum_t SrcType,
                       const void *SrcDataPtr)
{
    static uint32 Count2 = 0;
	int32                       Status;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    
    /* The following count tests are set this way because the CS_TableInit function
       calls CFE_TBL_Load twice.  The first call is to load the table file.  If it fails,
       CS_TableInit calls CFE_TBL_Load again to load the default the table from memory.
       In order to test the table initialization failure paths (the reason we set up this
       hook function), we must fail the table load call two consecutive times.  However,
       for the _successful_ calls, we only need to succeed once (the second table load is
       only called if the first call fails).  In order to test successive tables (EEPROM, 
       memory, tables, apps), we use the following sequence:
       
       0 fail, 1 fail (eeprom table fails, CS app exits)
       2 pass, 3 fail 4 fail (eeprom tbl passes, memory tbl fails, CS exits)
       5 pass, 6 pass, 7 fail, 8 fail (eeprom/memory tbls pass, tables tbl fails, CS exits)
       9 pass, 10 pass, 11 pass, 12 fail, 13 fail (eeprom/memory/tables tbls pass, app tbl fails, CS exits)
     */
    
    if (Count2 == 0 || Count2 == 1 || Count2 == 3 || Count2 == 4 ||
        Count2 == 7 || Count2 == 8 || Count2 == 12 || Count2 == 13)
    {   
        UTF_put_text("\n<utf_test_cs:CFE_TBL_LoadHook> Count2 = %d, return -2 \n",Count2);
        UTF_put_text("------------------------------------------------------\n\n");
    
        Count2++;
        return (-2);
    }
    else
    {
        UTF_put_text("\n<utf_test_cs:CFE_TBL_LoadHook> Count2 = %d, CFE_SUCCESS \n",Count2);
        
        /* The following code is copied from UTF_CFE_TBL_Load in order for the table
           processing to continue successfully in the CS app.  Otherwise, other table
           function calls will fail */
        
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_TaskData.Handles[TblHandle];
        RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
    
        /* Load Working Buffer, and set proper notify bits */
        Status = CFE_TBL_LoadWorkingBuffer(TblHandle, SrcType, SrcDataPtr);
    
        if (Status == CFE_SUCCESS)
        {
            Status = CFE_TBL_Manage(TblHandle);
            if (Status == CFE_TBL_INFO_UPDATED)
            {
                Status = CFE_SUCCESS;
            }
            else
            {
                UTF_put_text("\n<utf_test_cs:CFE_TBL_LoadHook> Call to CFE_TBL_Manage failed! \n");
            }

        }
        else
        {
            UTF_put_text("\n<utf_test_cs:CFE_TBL_LoadHook> Call to CFE_TBL_LoadWorkingBuffer failed! \n");
        }
        
        Count2++;
        UTF_put_text("------------------------------------------------------\n\n");
        return Status;
    }

    
}/* end CFE_TBL_LoadHook */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to simulate time incrementing                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TimeHook(void)
{
    UTF_set_sim_time(UTF_get_sim_time() + 1.0);

    UTF_put_text("\n(<utf_test_cs:TimeHook> Sim time incremented) \n\n");
    
} /* end TimeHook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values in the CS housekeeping packet     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintHkPacket (void) 
{
    UTF_put_text("\n<utf_test_cs:PrintHkPacket> Housekeeping Packet Contents \n");
    UTF_put_text("---------------------------------------------------------\n\n");

    UTF_put_text("*****************************************************\n");
    UTF_put_text("CmdCounter         : 0x%02X\n",CS_AppData.HkPacket.CmdCounter);
    UTF_put_text("CmdErrCounter      : 0x%02X\n",CS_AppData.HkPacket.CmdErrCounter);
    UTF_put_text("\n");
    UTF_put_text("ChecksumState      : 0x%02X\n",CS_AppData.HkPacket.ChecksumState);
    UTF_put_text("Eeprom CS State    : 0x%02X\n",CS_AppData.HkPacket.EepromCSState);
    UTF_put_text("Memory CS State    : 0x%02X\n",CS_AppData.HkPacket.MemoryCSState);
    UTF_put_text("Apps   CS State    : 0x%02X\n",CS_AppData.HkPacket.AppCSState);
    UTF_put_text("Tables CS State    : 0x%02X\n",CS_AppData.HkPacket.TablesCSState);
    UTF_put_text("OS Seg CS State    : 0x%02X\n",CS_AppData.HkPacket.OSCSState);
    UTF_put_text("CfeCre CS State    : 0x%02X\n",CS_AppData.HkPacket.CfeCoreCSState);
    UTF_put_text("\n");
    UTF_put_text("Child Task In Use  : 0x%02X\n",CS_AppData.HkPacket.ChildTaskInUse);
    UTF_put_text("OneShot Task In Use: 0x%02X\n",CS_AppData.HkPacket.OneShotTaskInUse);
    UTF_put_text("\n");
    UTF_put_text("Current Table      : 0x%04X\n",CS_AppData.HkPacket.CurrentCSTable);
    UTF_put_text("CurrEntry in Table : 0x%04X\n",CS_AppData.HkPacket.CurrentEntryInTable);
    UTF_put_text("\n");
    UTF_put_text("Eeprom Baseline     : 0x%08X\n",CS_AppData.HkPacket.EepromBaseline);
    UTF_put_text("OS Baseline         : 0x%08X\n",CS_AppData.HkPacket.OSBaseline);
    UTF_put_text("cFE Core Baseline   : 0x%08X\n",CS_AppData.HkPacket.CfeCoreBaseline);
    UTF_put_text("Last OneShot Addr   : 0x%08X\n",CS_AppData.HkPacket.LastOneShotAddress);
    UTF_put_text("Last OneShot Size   : 0x%08X\n",CS_AppData.HkPacket.LastOneShotSize);
    UTF_put_text("Last OneShot Chksm  : 0x%08X\n",CS_AppData.HkPacket.LastOneShotChecksum);
    UTF_put_text("Pass Counter        : 0x%08X\n",CS_AppData.HkPacket.PassCounter);
    UTF_put_text("\n");

 }


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS AppDataDump                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
                 
void CS_AppDataDump(void)
{
    UTF_put_text("\n<utf_test_cs:CS_AppDataDump> App Data Dump \n");
    UTF_put_text("-------------------------------------------\n\n");
                 
    OS_printf("*****************************************************\n");
    OS_printf("ChecksumState       : 0x%02X\n",CS_AppData.ChecksumState);
    OS_printf("Eeprom CS State     : 0x%02X\n",CS_AppData.EepromCSState);
    OS_printf("Memory CS State     : 0x%02X\n",CS_AppData.MemoryCSState);
    OS_printf("Apps   CS State     : 0x%02X\n",CS_AppData.AppCSState);
    OS_printf("Tables CS State     : 0x%02X\n",CS_AppData.TablesCSState);
    OS_printf("OS Seg CS State     : 0x%02X\n",CS_AppData.OSCSState);
    OS_printf("CfeCre CS State     : 0x%02X\n",CS_AppData.CfeCoreCSState);
    OS_printf("\n");
    OS_printf("Child Task In Use   : 0x%02X\n",CS_AppData.ChildTaskInUse);
    OS_printf("OneShot Task In Use : 0x%02X\n",CS_AppData.OneShotTaskInUse);
    OS_printf("\n");
    OS_printf("Current Table       : 0x%04X\n",CS_AppData.CurrentCSTable);
    OS_printf("CurrEntry in Table  : 0x%04X\n",CS_AppData.CurrentEntryInTable);
    OS_printf("\n");
    OS_printf("Eeprom Baseline     : 0x%08X\n",CS_AppData.EepromBaseline);
    OS_printf("OS Baseline         : 0x%08X\n",CS_AppData.OSBaseline);
    OS_printf("cFE Core Baseline   : 0x%08X\n",CS_AppData.CfeCoreBaseline);
    OS_printf("Last OneShot Addr   : 0x%08X\n",CS_AppData.HkPacket.LastOneShotAddress);
    OS_printf("Last OneShot Size   : 0x%08X\n",CS_AppData.HkPacket.LastOneShotSize);
    OS_printf("Last OneShot Chksm  : 0x%08X\n",CS_AppData.HkPacket.LastOneShotChecksum);
    OS_printf("Pass Counter        : 0x%08X\n",CS_AppData.PassCounter);
    OS_printf("\n");
    
    return;
}/* end CS_AppDataDump */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS print Core table                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_PrintCoreTable(void)
{
    UTF_put_text("\n<utf_test_cs:CS_PrintCoreTable> Printing core table \n");
    UTF_put_text("-----------------------------------------------------\n\n");
    
    OS_printf("cFECore\n");
    OS_printf("OS\n");
    OS_printf("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue\n");
    
    OS_printf("0x%02X ", 0);
    OS_printf("0x%02X ", CS_AppData.CfeCoreCodeSeg.State);              
    OS_printf("0x%02X ", CS_AppData.CfeCoreCodeSeg.ComputedYet);   
    OS_printf("0x%08X ", CS_AppData.CfeCoreCodeSeg.StartAddress);
    OS_printf("0x%08X ", CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum); 
    OS_printf("0x%08X ", CS_AppData.CfeCoreCodeSeg.ComparisonValue);    
    OS_printf("0x%08X ", CS_AppData.CfeCoreCodeSeg.ByteOffset);  
    OS_printf("0x%08X ", CS_AppData.CfeCoreCodeSeg.TempChecksumValue);  
    OS_printf("\n");
    OS_printf("0x%02X ", 1);
    OS_printf("0x%02X ", CS_AppData.OSCodeSeg.State);              
    OS_printf("0x%02X ", CS_AppData.OSCodeSeg.ComputedYet);        
    OS_printf("0x%08X ", CS_AppData.OSCodeSeg.StartAddress);
    OS_printf("0x%08X ", CS_AppData.OSCodeSeg.NumBytesToChecksum); 
    OS_printf("0x%08X ", CS_AppData.OSCodeSeg.ComparisonValue);    
    OS_printf("0x%08X ", CS_AppData.OSCodeSeg.ByteOffset);  
    OS_printf("0x%08X ", CS_AppData.OSCodeSeg.TempChecksumValue);  
    OS_printf("\n");
    
    return;
}/* end CS_PrintCoreTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS OS_print EepromTable                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_PrintEepromTable(void)
{
    uint16                                      Loop;

    UTF_put_text("\n<utf_test_cs:CS_PrintEepromTable> Printing EEPROM table \n");
    UTF_put_text("--------------------------------------------------------\n\n");

    OS_printf("Eeprom\n");
    OS_printf("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue\n");
    
    for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {        
        OS_printf("0x%02X ", Loop);              
        OS_printf("0x%02X ", CS_AppData.ResEepromTblPtr[Loop].State);              
        OS_printf("0x%02X ", CS_AppData.ResEepromTblPtr[Loop].ComputedYet);        
        OS_printf("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].StartAddress);
        OS_printf("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].NumBytesToChecksum); 
        OS_printf("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].ComparisonValue);    
        OS_printf("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].ByteOffset);  
        OS_printf("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].TempChecksumValue);  
        OS_printf("\n");
    }
    return;
}/* end CS_PrintEepromTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS OS_print MemoryTable                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_PrintMemoryTable(void)
{
    uint16                                  Loop;
    
    UTF_put_text("\n<utf_test_cs:CS_PrintMemoryTable> Printing Memory table \n");
    UTF_put_text("--------------------------------------------------------\n\n");

    OS_printf("Memory\n");
    OS_printf("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue\n");
    for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {
        OS_printf("0x%02X ", Loop);
        OS_printf("0x%02X ", CS_AppData.ResMemoryTblPtr[Loop].State);              
        OS_printf("0x%02X ", CS_AppData.ResMemoryTblPtr[Loop].ComputedYet);       
        OS_printf("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].StartAddress);
        OS_printf("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].NumBytesToChecksum); 
        OS_printf("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].ComparisonValue);    
        OS_printf("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].ByteOffset);  
        OS_printf("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].TempChecksumValue);  
        OS_printf("\n");
    }
    return;
}/* end CS_PrintMemoryTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS OS_print TablesTable                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_PrintTablesTable(void)
{
    uint16                                  Loop;
    
    UTF_put_text("\n<utf_test_cs:CS_PrintTablesTable> Printing Tables table \n");
    UTF_put_text("--------------------------------------------------------\n\n");

    OS_printf("Tables\n");
    UTF_put_text("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue  TblHandle  CSOwner    Name\n");
    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        OS_printf("0x%02X ", Loop);
        OS_printf("0x%02X ", CS_AppData.ResTablesTblPtr[Loop].State);              
        OS_printf("0x%02X ", CS_AppData.ResTablesTblPtr[Loop].ComputedYet);        
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].StartAddress);
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].NumBytesToChecksum); 
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].ComparisonValue);    
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].ByteOffset);  
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].TempChecksumValue);  
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].TblHandle); 
        OS_printf("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].IsCSOwner);
        OS_printf("%s ", CS_AppData.ResTablesTblPtr[Loop].Name);
        OS_printf("\n");
    }
    return;
}/* end CS_PrintTablesTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS OS_print AppTable                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_PrintAppTable(void)
{
    uint16                                  Loop;
    
    UTF_put_text("\n<utf_test_cs:CS_PrintAppTable> Printing App table \n");
    UTF_put_text("--------------------------------------------------\n\n");

    OS_printf("Apps\n");
    OS_printf("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue  Name\n");
    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {
        OS_printf("0x%02X ", Loop);
        OS_printf("0x%02X ", CS_AppData.ResAppTblPtr[Loop].State);              
        OS_printf("0x%02X ", CS_AppData.ResAppTblPtr[Loop].ComputedYet);        
        OS_printf("0x%08X ", CS_AppData.ResAppTblPtr[Loop].StartAddress);
        OS_printf("0x%08X ", CS_AppData.ResAppTblPtr[Loop].NumBytesToChecksum); 
        OS_printf("0x%08X ", CS_AppData.ResAppTblPtr[Loop].ComparisonValue);    
        OS_printf("0x%08X ", CS_AppData.ResAppTblPtr[Loop].ByteOffset);  
        OS_printf("0x%08X ", CS_AppData.ResAppTblPtr[Loop].TempChecksumValue);  
        OS_printf("%s ", CS_AppData.ResAppTblPtr[Loop].Name);
        OS_printf("\n");
    }
    return;
}/* end CS_PrintAppTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load a table from the ground                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[])
{
	char Table_Name[38], File_Name[50];
    
    UTF_put_text("\n<utf_test_cs:UTF_SCRIPT_LoadTableFromGround> Loading table...\n");
	
    if (argc != 3)
	{
        UTF_error("Error: Read %d args w/script cmd LOAD_TABLE_FROM_GROUND. Expected 2.\n\n",
                  argc -1 );
        UTF_exit();
	}
    
	strcpy(Table_Name,argv[1]);
	strcpy(File_Name,argv[2]);
    
    UTF_put_text("\n<utf_test_cs:UTF_SCRIPT_LoadTableFromGround> Table_Name is %s\n",Table_Name);
    UTF_put_text("<utf_test_cs:UTF_SCRIPT_LoadTableFromGround> File_Name is %s\n", File_Name);
    UTF_put_text("----------------------------------------------------------------------------------\n\n");
    
	UTF_TBL_LoadTableFromGround(Table_Name, File_Name);
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* View Table Data                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_PrintTableData(int argc, char *argv[])
{
    uint16 Loop;

    UTF_put_text("\n<utf_test_cs:CS_PrintTableData> Printing all results table data \n");
    UTF_put_text("----------------------------------------------------------------\n\n");
    
    UTF_put_text("cFECore\n");
    UTF_put_text("OS\n");
    UTF_put_text("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue\n");
    
    UTF_put_text("0x%02X ", 0);
    UTF_put_text("0x%02X ", CS_AppData.CfeCoreCodeSeg.State);              
    UTF_put_text("0x%02X ", CS_AppData.CfeCoreCodeSeg.ComputedYet);   
    UTF_put_text("0x%08X ", CS_AppData.CfeCoreCodeSeg.StartAddress);
    UTF_put_text("0x%08X ", CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum); 
    UTF_put_text("0x%08X ", CS_AppData.CfeCoreCodeSeg.ComparisonValue);    
    UTF_put_text("0x%08X ", CS_AppData.CfeCoreCodeSeg.ByteOffset);  
    UTF_put_text("0x%08X ", CS_AppData.CfeCoreCodeSeg.TempChecksumValue);  
    UTF_put_text("\n");
    UTF_put_text("0x%02X ", 1);
    UTF_put_text("0x%02X ", CS_AppData.OSCodeSeg.State);              
    UTF_put_text("0x%02X ", CS_AppData.OSCodeSeg.ComputedYet);        
    UTF_put_text("0x%08X ", CS_AppData.OSCodeSeg.StartAddress);
    UTF_put_text("0x%08X ", CS_AppData.OSCodeSeg.NumBytesToChecksum); 
    UTF_put_text("0x%08X ", CS_AppData.OSCodeSeg.ComparisonValue);    
    UTF_put_text("0x%08X ", CS_AppData.OSCodeSeg.ByteOffset);  
    UTF_put_text("0x%08X ", CS_AppData.OSCodeSeg.TempChecksumValue);  
    UTF_put_text("\n");
    UTF_put_text("Eeprom\n");
    UTF_put_text("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue\n");
    
    for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {        
        UTF_put_text("0x%02X ", Loop);              
        UTF_put_text("0x%02X ", CS_AppData.ResEepromTblPtr[Loop].State);              
        UTF_put_text("0x%02X ", CS_AppData.ResEepromTblPtr[Loop].ComputedYet);        
        UTF_put_text("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].StartAddress);
        UTF_put_text("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].NumBytesToChecksum); 
        UTF_put_text("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].ComparisonValue);    
        UTF_put_text("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].ByteOffset);  
        UTF_put_text("0x%08X ", CS_AppData.ResEepromTblPtr[Loop].TempChecksumValue);  
        UTF_put_text("\n");
    }
    
    UTF_put_text("Memory\n");
    UTF_put_text("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue\n");
    for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {
        UTF_put_text("0x%02X ", Loop);
        UTF_put_text("0x%02X ", CS_AppData.ResMemoryTblPtr[Loop].State);              
        UTF_put_text("0x%02X ", CS_AppData.ResMemoryTblPtr[Loop].ComputedYet);       
        UTF_put_text("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].StartAddress);
        UTF_put_text("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].NumBytesToChecksum); 
        UTF_put_text("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].ComparisonValue);    
        UTF_put_text("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].ByteOffset);  
        UTF_put_text("0x%08X ", CS_AppData.ResMemoryTblPtr[Loop].TempChecksumValue);  
        UTF_put_text("\n");
    }
    UTF_put_text("Tables\n");
    UTF_put_text("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue  TblHandle  CSOwner    Name\n");
    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        UTF_put_text("0x%02X ", Loop);
        UTF_put_text("0x%02X ", CS_AppData.ResTablesTblPtr[Loop].State);              
        UTF_put_text("0x%02X ", CS_AppData.ResTablesTblPtr[Loop].ComputedYet);        
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].StartAddress);
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].NumBytesToChecksum); 
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].ComparisonValue);    
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].ByteOffset);  
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].TempChecksumValue);  
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].TblHandle); 
        UTF_put_text("0x%08X ", CS_AppData.ResTablesTblPtr[Loop].IsCSOwner); 
        UTF_put_text("%s ", CS_AppData.ResTablesTblPtr[Loop].Name);
        
        UTF_put_text("\n");
    }
    UTF_put_text("Apps\n");
    UTF_put_text("ID   St   CY   Addr       NumBytes   CS         BytesOffs  TempValue  Name\n");
    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {
        UTF_put_text("0x%02X ", Loop);
        UTF_put_text("0x%02X ", CS_AppData.ResAppTblPtr[Loop].State);              
        UTF_put_text("0x%02X ", CS_AppData.ResAppTblPtr[Loop].ComputedYet);        
        UTF_put_text("0x%08X ", CS_AppData.ResAppTblPtr[Loop].StartAddress);
        UTF_put_text("0x%08X ", CS_AppData.ResAppTblPtr[Loop].NumBytesToChecksum); 
        UTF_put_text("0x%08X ", CS_AppData.ResAppTblPtr[Loop].ComparisonValue);    
        UTF_put_text("0x%08X ", CS_AppData.ResAppTblPtr[Loop].ByteOffset);  
        UTF_put_text("0x%08X ", CS_AppData.ResAppTblPtr[Loop].TempChecksumValue);  
        UTF_put_text("%s ", CS_AppData.ResAppTblPtr[Loop].Name);
        UTF_put_text("\n");
    }
    
    UTF_put_text("\n<utf_test_cs:CS_PrintTableData> Complete \n\n");

    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Send a bunch of background commands                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_BackgroundCheck(int argc, char *argv[])
{
 
    uint16 Loop;
    uint16 Max;
    CS_NoArgsCmd_t Arg;

    UTF_put_text("\n<utf_test_cs:CS_BackgroundCheck> Sending background commands \n");
    UTF_put_text("-------------------------------------------------------------\n\n");
    
    CFE_SB_InitMsg (& Arg,
                    CS_BACKGROUND_CYCLE_MID, 
                    sizeof (CS_NoArgsCmd_t),
                    TRUE);
    
    Max = 200;
    
    for (Loop = 0; Loop < Max; Loop ++)
    {
        CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)&Arg);
    }
    
    UTF_put_text("\n<utf_test_cs:CS_BackgroundCheck> Sent %d background commands \n", Loop);
    UTF_put_text("-----------------------------------------------------------------\n\n");
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Clear the flags that the recompute child tasks should clear      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ClearChildTaskFlags(int argc, char *argv[])
{
    UTF_put_text("\n<utf_test_cs:CS_ClearChildTaskFlags> Clearing child task flags \n");
    UTF_put_text("---------------------------------------------------------------\n\n");
    
    CS_AppData.ChildTaskInUse   = FALSE;
    CS_AppData.OneShotTaskInUse = FALSE;
    CS_AppData.ChildTaskID      = 0;
    CS_AppData.OSCodeSeg.ComputedYet = FALSE;
    CS_AppData.CfeCoreCodeSeg.ComputedYet = FALSE;
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Force Miscompares for an entry in each table                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ForceMiscompares(int argc, char *argv[])
{ 
    UTF_put_text("\n<utf_test_cs:CS_ForceMiscompares> Forcing miscompares \n");
    UTF_put_text("------------------------------------------------------\n\n");
    
    CS_AppData.CfeCoreCodeSeg.ComparisonValue =1;
    CS_AppData.OSCodeSeg.ComparisonValue =1;
    CS_AppData.ResTablesTblPtr[0].ComparisonValue = 1;
    CS_AppData.ResAppTblPtr[0].ComparisonValue = 1;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 1;
    CS_AppData.ResMemoryTblPtr[0].ComparisonValue = 1;
    CS_AppData.OSCodeSeg.ComputedYet = TRUE;
    CS_AppData.CfeCoreCodeSeg.ComputedYet = TRUE;
    CS_AppData.ResTablesTblPtr[0].ComputedYet = TRUE;
    CS_AppData.ResAppTblPtr[0].ComputedYet = TRUE;
    CS_AppData.ResEepromTblPtr[0].ComputedYet = TRUE;
    CS_AppData.ResMemoryTblPtr[0].ComputedYet = TRUE;
    CS_AppData.ResAppTblPtr[1].ComparisonValue = 1;
    CS_AppData.ResAppTblPtr[1].ComputedYet = TRUE;
    
    return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Do recomputes after invalid names have been loaded              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeInvalidNames(int argc, char *argv[])
{
    UTF_put_text("\n<utf_test_cs:CS_RecomputeInvalidNames> Calling CS_RecomputeAppChildTask() #1 \n");
    UTF_put_text("-----------------------------------------------------------------------------\n\n");
    
    CS_AppData.RecomputeAppEntryPtr = & CS_AppData.ResAppTblPtr[0];
    CS_RecomputeAppChildTask();   
    
    UTF_put_text("\n<utf_test_cs:CS_RecomputeInvalidNames> Calling CS_RecomputeTablesChildTask() #2 \n");
    UTF_put_text("--------------------------------------------------------------------------------\n\n");
    
    CS_AppData.RecomputeTablesEntryPtr = &CS_AppData.ResTablesTblPtr[3];
    CS_RecomputeTablesChildTask();  
    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unregister the QQ app tables and reregster them again           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_UnregisterQQTables(int argc, char *argv[])
{
    int32            Status;

    UTF_put_text("\n<utf_test_cs:CS_UnregisterQQTables> Unregistering QQ App tables and re-registering in opposite order \n");
    UTF_put_text("-----------------------------------------------------------------------------------------------------\n\n");
    
    /* Unregister both QQ APP tables and then reregister them
     in opposite order */
    
    UTF_CFE_ES_SetAppID(QQ_APP_RECORD_ID);

    CFE_TBL_Unregister(TblHandle1);
    CFE_TBL_Unregister(TblHandle2);

    
    /* Register another table to hold a table handle */
    Status = CFE_TBL_Register( &TblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( TblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(TblHandle2);

    
    /* Register one table to be checksummed */
    Status = CFE_TBL_Register( &TblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( TblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(TblHandle1);
    
    UTF_CFE_ES_SetAppID(CS_APP_RECORD_ID);
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reregister the QQ app tables and reregster them again           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReregisterQQTables(int argc, char *argv[])
{
    int32            Status;
    UTF_CFE_ES_SetAppID(QQ_APP_RECORD_ID);

    UTF_put_text("\n<utf_test_cs:CS_ReregisterQQTables> Re-registering QQ App tables \n");
    UTF_put_text("-----------------------------------------------------------------\n\n");
    
    /* Register another table to hold a table handle */
    Status = CFE_TBL_Register( &TblHandle2, "Table2", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( TblHandle2, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(TblHandle2);
     
      
    /* Register one table to be checksummed */
    Status = CFE_TBL_Register( &TblHandle1, "Table1", sizeof(uint32), 0, 0  );
    Status = CFE_TBL_Load( TblHandle1, CFE_TBL_SRC_ADDRESS, &TableTestData);
    Status = CFE_TBL_Manage(TblHandle1);
    UTF_CFE_ES_SetAppID(CS_APP_RECORD_ID);

    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* When Getting the App Info from ES, set it to a good value       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_SetAppInfoTrue(int argc, char *argv[])
{
    UTF_put_text("\n<utf_test_cs:CS_SetAppInfoTrue> Set up App Info to be a good value \n");
    UTF_put_text("-------------------------------------------------------------------\n\n");
    
    /* Set the App Info  up to be a good value */
    CFE_ES_GetAppInfo(&AppInfo, 0);
    AppInfo.AddressesAreValid = TRUE;
    AppInfo.CodeSize = 20000;
    AppInfo.CodeAddress = (uint32)(&CS_TestDataSet[0]);
    UTF_CFE_ES_SetDefaultAppInfo(AppInfo);
    
    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* When Getting the App Info from ES, set it to a bad value        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_SetAppInfoFalse(int argc, char *argv[])
{
    UTF_put_text("\n<utf_test_cs:CS_SetAppInfoFalse> Set up App Info to be a bad value \n");
    UTF_put_text("-------------------------------------------------------------------\n\n");
    
    /* Set the App Info  up to be a bad value */
    CFE_ES_GetAppInfo(&AppInfo, 0);
    AppInfo.AddressesAreValid = FALSE;
    AppInfo.CodeSize = 20000;
    AppInfo.CodeAddress = (uint32)(&CS_TestDataSet[0]);
    UTF_CFE_ES_SetDefaultAppInfo(AppInfo);

    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Need a function hook to test the 2nd call to TBL_GetAddress     */
/* in CS_ComputeTables                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_GetAddressHook( void **TblPtr,
                        CFE_TBL_Handle_t TblHandle )
{
    int32 Status; 
    static uint32 CS_GetAddressCount = 0;
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, NULL);
    
    CFE_TBL_GetAddress(TblPtr, TblHandle);
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *) &CS_GetAddressHook);
    
    
    if (CS_GetAddressCount == 1)
    {
        UTF_put_text("\n<utf_test_cs:CS_GetAddressHook> CS_GetAddressCount = %d, CFE_TBL_ERR_NEVER_LOADED \n", CS_GetAddressCount);
        UTF_put_text("----------------------------------------------------------------------------------\n\n");
        
        Status = CFE_TBL_ERR_NEVER_LOADED;
    }
    
    else
    {
        UTF_put_text("\n<utf_test_cs:CS_GetAddressHook> CS_GetAddressCount = %d, CFE_TBL_ERR_UNREGISTERED \n", CS_GetAddressCount);
        UTF_put_text("----------------------------------------------------------------------------------\n\n");
        
        Status =  CFE_TBL_ERR_UNREGISTERED;
    }
    
    CS_GetAddressCount++;
    
    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Need a function hook to set ReleaseAddress return code          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_GetAddressHook2( void **TblPtr,
                        CFE_TBL_Handle_t TblHandle )
{
    int32 Status; 
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, NULL);
    
    CFE_TBL_GetAddress(TblPtr, TblHandle);
    
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *) &CS_GetAddressHook);

    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_RELEASEADDRESS_PROC, CFE_TBL_ERR_UNREGISTERED);  /* Anything != CFE_SUCCESS */    
    
    Status = CFE_SUCCESS;
    
    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to allow us to populate the EEEPROM or Memory Results  */
/* table with valid addresses (overwrite the ones in their         */
/* definition tables)                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_AddEepromMemoryResAddresses(int argc, char *argv[])
{
    uint32 Loop;
    uint32 TestIndex;
    
    TestIndex = 0;
    
    if (strncmp(argv[1],"0",1) == 0)
    {
        UTF_put_text("\n<utf_test_cs:CS_AddEepromMemoryResAddresses> Put good addresses in EEPROM Results table\n");
        UTF_put_text("----------------------------------------------------------------------------------\n\n");
        
        for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
        {        
            CS_AppData.ResEepromTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.ResEepromTblPtr[Loop].NumBytesToChecksum = SIZE_OF_EEPROM_TO_TEST;
            
            /* Need to update Definitions table, too, to make sure table comparisons still work in cs_compute.c */
            
            CS_AppData.DefEepromTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.DefEepromTblPtr[Loop].NumBytesToChecksum = SIZE_OF_EEPROM_TO_TEST;
            
            TestIndex += SIZE_OF_EEPROM_TO_TEST;
        }
    }
    else
    {
        UTF_put_text("\n<utf_test_cs:CS_AddEepromMemoryResAddresses> Put good addresses in Memory Results table\n");
        UTF_put_text("----------------------------------------------------------------------------------\n\n");
        
        for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
        {        
            CS_AppData.ResMemoryTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.ResMemoryTblPtr[Loop].NumBytesToChecksum = SIZE_OF_MEMORY_TO_TEST;
            
            /* Need to update Definitions table, too, to make sure table comparisons still work in cs_compute.c */
            
            CS_AppData.DefMemoryTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.DefMemoryTblPtr[Loop].NumBytesToChecksum = SIZE_OF_MEMORY_TO_TEST;
            
            TestIndex += SIZE_OF_MEMORY_TO_TEST;
        }
    }
    
    return;
}


void CS_ResetEepromMemoryTblEntry11(int argc, char *argv[])
{
    /* Set index 10 of each table to 0x00034020 */
    CS_AppData.ResMemoryTblPtr[11].StartAddress = (uint32)(&CS_TestDataSet[0]);
    CS_AppData.DefMemoryTblPtr[11].StartAddress = (uint32)(&CS_TestDataSet[0]);
    CS_AppData.ResEepromTblPtr[11].StartAddress = (uint32)(&CS_TestDataSet[0]);
    CS_AppData.DefEepromTblPtr[11].StartAddress = (uint32)(&CS_TestDataSet[0]);
    
    UTF_put_text("\n<utf_test_cs:CS_SetEepromMemoryTblEntry11> Entry 11 set to TestData for eeprom and memory\n");
    UTF_put_text("----------------------------------------------------------------------------------------------\n\n");
    
    return;
}


void CS_SetEepromMemoryTblEntry11(int argc, char *argv[])
{
    /* Set index 10 of each table to 0x00034020 */
    CS_AppData.ResMemoryTblPtr[11].StartAddress = 0x00034020;
    CS_AppData.DefMemoryTblPtr[11].StartAddress = 0x00034020;
    CS_AppData.ResEepromTblPtr[11].StartAddress = 0x00034020;
    CS_AppData.DefEepromTblPtr[11].StartAddress = 0x00034020;
    
    UTF_put_text("\n<utf_test_cs:CS_SetEepromMemoryTblEntry11> Entry 11 set to 0x00034020 for eeprom and memory\n");
    UTF_put_text("----------------------------------------------------------------------------------------------\n\n");
    
    return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to allow us to remove an entry from a definition table */
/* This is needed to test failures of definition table updates.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RemoveDefTableEntry(int argc, char *argv[])
{
    uint32 Loop;
    boolean AppFound = FALSE;
    
    /* Check if it's a request to change the apps table */
    if (strncmp(argv[1],"APP",3) == 0)
    {
        UTF_put_text("\n<utf_test_cs:CS_RemoveDefTableEntry> Modifying the apps definition table\n");
        UTF_put_text("------------------------------------------------------------------------\n\n");
        
        for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
        {        
            if (strncmp(CS_AppData.DefAppTblPtr[Loop].Name,"BB",6) == 0)
            {
                strncpy(CS_AppData.DefAppTblPtr[Loop].Name, "BX",6);
                AppFound = TRUE;
            }
        }
        
        if (AppFound != TRUE)
        {
            UTF_put_text("\n<utf_test_cs:CS_RemoveDefTableEntry> BB not found!\n");
            UTF_put_text("------------------------------------------------------------------------\n\n");
        }
    }
    else
    {
        if (strncmp(argv[1],"TABLES",6) == 0)
        {
            UTF_put_text("\n<utf_test_cs:CS_RemoveDefTableEntry> Modifying the tables definition table\n");
            UTF_put_text("------------------------------------------------------------------------\n\n");
            
            for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
            {        
                if (strncmp(CS_AppData.DefTablesTblPtr[Loop].Name,"AA.Table1",13) == 0)
                {
                    strncpy(CS_AppData.DefTablesTblPtr[Loop].Name, "AX.Table1",13);
                    AppFound = TRUE;
                }
            }
            
            if (AppFound != TRUE)
            {
                UTF_put_text("\n<utf_test_cs:CS_RemoveDefTableEntry> AA.Table1 not found!\n");
                UTF_put_text("------------------------------------------------------------------------\n\n");
            }
        }
        else 
        {
            if (strncmp(argv[1],"EEPROM",6) == 0)
            {
                CS_AppData.DefEepromTblPtr[0].State = CS_STATE_EMPTY;  
            }
            else /* Assume MEMORY */
            {
                CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_EMPTY;   
            }
        }
    }
    
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to enable/disable OS and cFE status to simulate        */
/* recompute-in-progress.                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_SetOScFEStatus(int argc, char *argv[])
{
    
    if (strncmp(argv[1],"0",1) == 0)  /* Disable */
    {
        UTF_put_text("\n<utf_test_cs:CS_OScFEStatus> Set OS and cFE Core to DISABLED\n");
        UTF_put_text("--------------------------------------------------------------\n\n");

        CS_AppData.CfeCoreCodeSeg.State = CS_STATE_DISABLED;
        CS_AppData.OSCodeSeg.State = CS_STATE_DISABLED;
        
        CS_AppData.CfeCoreCSState = CS_STATE_ENABLED;
        CS_AppData.OSCSState = CS_STATE_ENABLED;
        
    }
    else
    {
        UTF_put_text("\n<utf_test_cs:CS_OScFEStatus> Set OS and cFE Core to ENABLED\n");
        UTF_put_text("--------------------------------------------------------------\n\n");
        
        CS_AppData.CfeCoreCodeSeg.State = CS_STATE_ENABLED;
        CS_AppData.OSCodeSeg.State = CS_STATE_ENABLED;
        
    }
    
    return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to allow us to populate the EEEPROM or Memory Results  */
/* table with valid addresses (overwrite the ones in their         */
/* definition tables) -- Called Internally                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_AddEepromMemoryResAddressesInternal(int MemType)
{
    uint32 Loop;
    uint32 TestIndex;
    
    TestIndex = 0;
    
    if (MemType == 0)
    {
        UTF_put_text("\n<utf_test_cs:CS_AddEepromMemoryResAddressesInternal> Put good addresses in EEPROM Results table\n");
        UTF_put_text("------------------------------------------------------------------------------------------------\n\n");
        
        for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
        {        
            CS_AppData.ResEepromTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.ResEepromTblPtr[Loop].NumBytesToChecksum = SIZE_OF_EEPROM_TO_TEST;
            
            /* Need to update Definitions table, too, to make sure table comparisons still work in cs_compute.c */
            
            CS_AppData.DefEepromTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.DefEepromTblPtr[Loop].NumBytesToChecksum = SIZE_OF_EEPROM_TO_TEST;
            
            TestIndex += SIZE_OF_EEPROM_TO_TEST;
        }
    }
    else
    {
        UTF_put_text("\n<utf_test_cs:CS_AddEepromMemoryResAddressesInternal> Put good addresses in Memory Results table\n");
        UTF_put_text("----------------------------------------------------------------------------------------------\n\n");
        
        for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
        {        
            CS_AppData.ResMemoryTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.ResMemoryTblPtr[Loop].NumBytesToChecksum = SIZE_OF_MEMORY_TO_TEST;
            
            /* Need to update Definitions table, too, to make sure table comparisons still work in cs_compute.c */
            
            CS_AppData.DefMemoryTblPtr[Loop].StartAddress = (uint32)(&CS_TestDataSet[TestIndex]);
            CS_AppData.DefMemoryTblPtr[Loop].NumBytesToChecksum = SIZE_OF_MEMORY_TO_TEST;
            
            TestIndex += SIZE_OF_MEMORY_TO_TEST;
        }
    }
    
    return;
}

