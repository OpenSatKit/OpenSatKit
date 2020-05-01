/*
** Purpose: Implement the instrument simulator.
**
** Notes:
**   1. Information events are used in order to trace execution
**      for demonstrations.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
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
#include "isim.h"

/*
** Types
*/

typedef enum {

   CREATE_FILE = 0,
   CLOSE_FILE  = 1,
   ADD_DATA    = 2

} SciFileAction_Enum;

/*
** Global File Data
*/

static ISIM_Class*  Isim = NULL;

/* 
** String lookup tables for ISIM_STATE
*/
static char* InstrStateStr[] = {
  "OFF",           /* ISIM_OFF          = 0 */
  "INITIALIZING",  /* ISIM_INITIALIZING = 1 */
  "READY"          /* ISIM_READY        = 2 */
};

/* 
** Science data samples. 
*/
static char* DataSample[ISIM_DATA_SAMPLE_MAX] = {
  "00010203040506070809\n", 
  "10111213141516171819\n", 
  "20212223242526272829\n", 
  "30313233343536373839\n", 
  "40414243444546474849\n", 
  "50515253545556575859\n", 
  "60616263646566676869\n", 
  "70717273747576777879\n", 
  "80818283848586878889\n", 
  "90919293949596979899\n" 
};

/*
** Local Function Prototypes
*/

static boolean ManageSciFile(SciFileAction_Enum Action, ISIM_Sci* Sci, SciFile_Struct* SciTbl);
static void CreateTimeFilename(char* Filename, SciFile_Struct* SciTbl);
static void CreateCntFilename(ISIM_Sci* Sci, SciFile_Struct* SciTbl);

/******************************************************************************
** Function: ISIM_Constructor
**
*/
void ISIM_Constructor(ISIM_Class*  IsimPtr)
{
 
   Isim = IsimPtr;

   CFE_PSP_MemSet((void*)Isim, 0, sizeof(ISIM_Class));

   strcpy(Isim->Sci.Filename, ISIM_NULL_FILENAME);

} /* End ISIM_Constructor() */


/******************************************************************************
** Function: ISIM_Execute
**
** Execute instrument simulation cycle. Keep all file management logic in 
** this function so commands set flags for thsi function to act upon.
**
*/
void ISIM_Execute(void)
{

   boolean CriticalError = FALSE;
   
   switch (Isim->Instr.State) {
   
      case ISIM_OFF:
         
         if (Isim->Sci.State != ISIM_SCI_OFF) {
            
            ManageSciFile(CLOSE_FILE, &(Isim->Sci), &(Isim->Tbl.SciFile));
            Isim->Sci.State = ISIM_SCI_OFF;
            Isim->Sci.FileCnt = 0;   /* ... or leave last coudn for tlm? */
         }
         break;
   
      case ISIM_INITIALIZING:
         
         if (++Isim->Instr.InitCycleCnt >= Isim->Tbl.Instrument.PwrInitCycles) {
            
            Isim->Instr.State = ISIM_READY;
            
            CFE_EVS_SendEvent (ISIM_INIT_COMPLETE_EID, CFE_EVS_INFORMATION,
                               "ISIM completed initialization after power on in %d cycles.",Isim->Instr.InitCycleCnt);
            
            Isim->Instr.InitCycleCnt = 0;
         
         } /* End if init cycle complete */
         break;
         
      case ISIM_READY:
         
         switch (Isim->Sci.State) {
      
            case ISIM_SCI_OFF:
               break;
         
            case ISIM_SCI_START:
            
               Isim->Sci.FileCycleCnt = 0;
               Isim->Sci.State = ISIM_SCI_ON;
               ManageSciFile(CREATE_FILE, &(Isim->Sci), &(Isim->Tbl.SciFile));
               
               break;
            
            case ISIM_SCI_STOP:
            
               ManageSciFile(CLOSE_FILE, &(Isim->Sci), &(Isim->Tbl.SciFile));
               Isim->Sci.State = ISIM_SCI_OFF;
               break;
         
            case ISIM_SCI_ON:

               if (Isim->Sci.FileCycleCnt < Isim->Tbl.SciFile.CyclesPerFile) {
               
                  ManageSciFile(ADD_DATA, &(Isim->Sci), &(Isim->Tbl.SciFile));
               
               }
               else {
            
                  ManageSciFile(CLOSE_FILE, &(Isim->Sci), &(Isim->Tbl.SciFile));
                  Isim->Sci.FileCycleCnt = 0;
                  ManageSciFile(CREATE_FILE, &(Isim->Sci), &(Isim->Tbl.SciFile));
      
               }
               Isim->Sci.FileCycleCnt++;
               break;
            
            default:
         
               CFE_EVS_SendEvent (ISIM_INVALID_STATE_EID, CFE_EVS_ERROR,
                                  "Invalid ISIM science data state %d. Powering off instrument.",Isim->Instr.State);
               CriticalError = TRUE;

         } /* End sci data state switch */
         
         break;
         
      default:
         
         CFE_EVS_SendEvent (ISIM_INVALID_STATE_EID, CFE_EVS_ERROR,
                            "Invalid ISIM state %d. Powering off instrument.",Isim->Instr.State);
         
         CriticalError = TRUE;
      
   } /* End instrument state switch */
      
   if (CriticalError) {
      
      Isim->Instr.State = ISIM_OFF;
      Isim->Sci.State   = ISIM_SCI_OFF;

      /* TODO - Close file if open */
      
   } /* End if CriticalError */
   
} /* ISIM_Execute() */


/******************************************************************************
** Function:  ISIM_ResetStatus
**
*/
void ISIM_ResetStatus(void)
{

   /* No counters to be reset that wouldn't effect the simulation state */
   
} /* End ISIM_ResetStatus() */


/******************************************************************************
** Function: ISIM_GetTblPtr
**
*/
const ISIMTBL_Struct* ISIM_GetTblPtr(void)
{

   return &(Isim->Tbl);

} /* End ISIM_GetTblPtr() */


/******************************************************************************
** Function: ISIM_LoadTbl
**
*/
boolean ISIM_LoadTbl(ISIMTBL_Struct* NewTbl)
{

   boolean  RetStatus = TRUE;

   /*
   ** This is a simple table copy. More complex table loads may have pass/fail 
   ** criteria.
   */

   CFE_PSP_MemCpy(&(Isim->Tbl), NewTbl, sizeof(ISIMTBL_Struct));

   CFE_EVS_SendEvent(ISIM_LOAD_TBL_EID, CFE_EVS_INFORMATION, "ISIM table loaded.");

   return RetStatus;

} /* End ISIM_LoadTbl() */


/******************************************************************************
** Function: ISIM_LoadTblEntry
**
*/
boolean ISIM_LoadTblEntry (uint16 ObjId, void* ObjData)
{

   boolean RetStatus = TRUE;
   
   switch (ObjId) {
      
	  case ISIMTBL_OBJ_INSTRUMENT:
         CFE_PSP_MemCpy(&(Isim->Tbl.Instrument), ObjData, sizeof(Instrument_Struct));
         break;
		 
      case ISIMTBL_OBJ_SCI_FILE:
         CFE_PSP_MemCpy(&(Isim->Tbl.SciFile), ObjData, sizeof(SciFile_Struct));
         break;
	  
      default:
	     RetStatus = FALSE;
   
   } /* End ObjId switch */
   
   CFE_EVS_SendEvent(ISIM_LOAD_TBL_OBJ_EID, CFE_EVS_INFORMATION, "ISIM table JSON object %d looaded.", ObjId);
   
   return RetStatus;

} /* End ISIM_LoadTblEntry() */


/******************************************************************************
** Functions: ISIM_PwrOnSciCmd
**
** Power on the science instrument.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_PwrOnSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;
   
   if (Isim->Instr.State == ISIM_OFF) {
      
      Isim->Instr.State = ISIM_INITIALIZING;
      Isim->Instr.InitCycleCnt = 0;
      CFE_EVS_SendEvent (ISIM_PWR_ON_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument powered on. Entered initializing state.");
      RetStatus = TRUE;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_PWR_ON_SCI_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Power on science instrument cmd rejected. Instrument must be in OFF state and it's in the %s state.",
                         InstrStateStr[Isim->Instr.State]);
   
   }
   
   return RetStatus;

} /* End ISIM_PwrOnSciCmd() */


/******************************************************************************
** Functions: ISIM_PwrOffSciCmd
**
** Power off science instrument regardless of current state. The science state
** is unmodified and the ISIM_Execute() function takes care of any science
** data cleanup activities.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_PwrOffSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (ISIM_PWR_OFF_SCI_CMD_EID, CFE_EVS_INFORMATION,        
                      "Instrument powered off from previous state %s",InstrStateStr[Isim->Instr.State]);
   
   Isim->Instr.State = ISIM_OFF;
   
   /* See function prologue */
      
   return TRUE;

} /* End ISIM_PwrOffSciCmd() */


/******************************************************************************
** Functions: ISIM_StartSciCmd
**
** Start science instrument data. Instrument must be in the READY state.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_StartSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;
   
   if (Isim->Instr.State == ISIM_READY) {
      
      Isim->Sci.State = ISIM_SCI_START;
      CFE_EVS_SendEvent (ISIM_START_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument data started.");
      
      RetStatus = TRUE;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_START_SCI_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Start science instrument data cmd rejected. Instrument must be in READY state and it's in the %s state.",
                         InstrStateStr[Isim->Instr.State]);
   
   }
   
   return RetStatus;

} /* End ISIM_StartSciCmd() */



/******************************************************************************
** Functions: ISIM_StopSciCmd
**
** Stop science instrument.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_StopSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   if (Isim->Sci.State == ISIM_SCI_START ||
       Isim->Sci.State == ISIM_SCI_ON) {
      
      CFE_EVS_SendEvent (ISIM_STOP_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument data stopped.");
      
      Isim->Sci.State = ISIM_SCI_STOP;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_STOP_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Stop science instrument data cmd ignored. Science data already stopped.");

   }
      
   return TRUE;

} /* End ISIM_StopSciCmd() */



/******************************************************************************
** Functions: ISIM_CfgFaultCmd
**
** Set/clear fault state.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_CfgFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   const ISIM_CfgFaultCmdMsg* CfgFaultCmd = (const ISIM_CfgFaultCmdMsg *) MsgPtr;

   if (CfgFaultCmd->NewState == TRUE) {
      
      CFE_EVS_SendEvent (ISIM_CFG_FAULT_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument fault set to TRUE.");
            
      Isim->Fault = TRUE;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_CFG_FAULT_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument fault set to FALSE.");

      Isim->Fault = FALSE;
   
   }
   
   return TRUE;

} /* End ISIM_CfgFaultCmd() */


/******************************************************************************
** Functions: ManageSciFile
**
** Manage opening science files, adding sample data, and closing files.
**
** Note:
**   None
*/
static boolean ManageSciFile(SciFileAction_Enum Action, ISIM_Sci* Sci, SciFile_Struct* SciTbl)
{

   uint8 i=0;
   
   switch (Action) {
      
      case CREATE_FILE:

         if (Sci->FileOpen) {
            CFE_EVS_SendEvent (ISIM_CREATE_SCI_FILE_ERROR_EID, CFE_EVS_ERROR, 
                               "Create science file rejected. Issue stop science to close current file %s",Sci->Filename);         
         }
         else {
         
            CreateCntFilename(Sci,SciTbl);
            CFE_EVS_SendEvent (ISIM_CREATE_SCI_FILE_EID, CFE_EVS_INFORMATION, 
                               "Creating new science file %s",Sci->Filename);         
            Sci->FileHandle = OS_creat(Sci->Filename, OS_WRITE_ONLY);

            if (Sci->FileHandle >= OS_FS_SUCCESS) {
            
               Sci->FileOpen = TRUE;
            }
            else {
               CFE_EVS_SendEvent (ISIM_CREATE_SCI_FILE_ERROR_EID, CFE_EVS_ERROR, 
                                  "Error creating new science file %s. Return status = 0x%4X",
                                  Sci->Filename, Sci->FileHandle);         
            }
         } /* End if no file currently open */
         break;
         
      case CLOSE_FILE:
         
         if (Sci->FileOpen) {
            CFE_EVS_SendEvent (ISIM_CLOSE_SCI_FILE_EID, CFE_EVS_INFORMATION, 
                               "Closing current science file %s",Sci->Filename);         
            OS_close(Sci->FileHandle);
            Sci->FileOpen = FALSE;
            Sci->FileCycleCnt = 0;
            strcpy(Sci->Filename, ISIM_NULL_FILENAME);
         }
         break;
      
      case ADD_DATA:
      
         i = Sci->FileCycleCnt % 10;
         OS_write(Sci->FileHandle,DataSample[i],strlen(DataSample[i]));
         break;
         
   } /* End Action switch */

   return TRUE;
   
} /* End ManageSciFile() */


/******************************************************************************
** Functions: CreateCntFilename
**
** Create a filename using the tabled-defined base path/filename, current file
** counter, and the table-defined extension. 
**
** Note:
**   1. No string buffer error checking performed
*/
static void CreateCntFilename(ISIM_Sci* Sci, SciFile_Struct* SciTbl)
{
   
   int i;
   
   char FileCntStr[64];

   sprintf(FileCntStr,"%02d",Sci->FileCnt);
   Sci->FileCnt++;

   strcpy (Sci->Filename,SciTbl->PathBaseFilename);

   i = strlen(Sci->Filename);  /* Starting position for counter */
   strcat (&(Sci->Filename[i]), FileCntStr);
   
   i = strlen(Sci->Filename);  /* Starting position for extension */
   strcat (&(Sci->Filename[i]), SciTbl->FileExtension);
   

} /* End CreateCntFilename() */


/******************************************************************************
** Functions: CreateTimeFilename
**
** Create a filename using the tabled-defined base path/filename, current time,
** and the table-defined extension. 
**
** Note:
**   1. No string buffer error checking performed
*/

/*
** cFE time string has format: "YYYY-DDD-HH:MM:SS.sssss"...
*/
#define CFE_YYYY_INDEX  0
#define CFE_DDD_INDEX   5
#define CFE_HH_INDEX    9
#define CFE_MM_INDEX   12
#define CFE_SS_INDEX   15
#define CFE_ssss_INDEX 18

/*
** ISIM time string has format: "YYYYDDDHHMMSS"...
*/
#define ISIM_YYYY_OFFSET  0
#define ISIM_DDD_OFFSET   4
#define ISIM_HH_OFFSET    7
#define ISIM_MM_OFFSET    9
#define ISIM_SS_OFFSET   11
#define ISIM_TERM_OFFSET 13

static void CreateTimeFilename(char* Filename, SciFile_Struct* SciTbl)
{

   uint16  i;
   char    TimeStr[64];
   CFE_TIME_SysTime_t SysTime;

   strcpy (Filename,SciTbl->PathBaseFilename);
   
   i = strlen(Filename);  /* Starting position for the time stamp */
   
   SysTime = CFE_TIME_GetTime();
   CFE_TIME_Print(TimeStr, SysTime);

   Filename[i + ISIM_YYYY_OFFSET]     = TimeStr[CFE_YYYY_INDEX];
   Filename[i + ISIM_YYYY_OFFSET + 1] = TimeStr[CFE_YYYY_INDEX + 1];
   Filename[i + ISIM_YYYY_OFFSET + 2] = TimeStr[CFE_YYYY_INDEX + 2];
   Filename[i + ISIM_YYYY_OFFSET + 3] = TimeStr[CFE_YYYY_INDEX + 3];

   Filename[i + ISIM_DDD_OFFSET]      = TimeStr[CFE_DDD_INDEX];
   Filename[i + ISIM_DDD_OFFSET + 1]  = TimeStr[CFE_DDD_INDEX + 1];
   Filename[i + ISIM_DDD_OFFSET + 2]  = TimeStr[CFE_DDD_INDEX + 2];

   Filename[i + ISIM_HH_OFFSET]       = TimeStr[CFE_HH_INDEX];
   Filename[i + ISIM_HH_OFFSET + 1]   = TimeStr[CFE_HH_INDEX + 1];

   Filename[i + ISIM_MM_OFFSET]       = TimeStr[CFE_MM_INDEX];
   Filename[i + ISIM_MM_OFFSET + 1]   = TimeStr[CFE_MM_INDEX + 1];

   Filename[i + ISIM_SS_OFFSET]       = TimeStr[CFE_SS_INDEX];
   Filename[i + ISIM_SS_OFFSET + 1]   = TimeStr[CFE_SS_INDEX + 1];

   i += ISIM_TERM_OFFSET;
      
   strcat (&Filename[i], SciTbl->FileExtension);

} /* End CreateTimeFilename() */

