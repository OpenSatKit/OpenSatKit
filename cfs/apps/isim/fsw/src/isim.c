/*
** Purpose: Implement the instrument simulator.
**
** Notes:
**   1. Information events are used in order to trace execution for
**      demonstrations.
**   2. I broke from convention of defining one object per file. The
**      Detetecor and SciFile were trivial enough that I define them
**      locally, similar to a child class.
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



/**********************/
/** Type Definitions **/
/**********************/

typedef enum {

   DETECTOR_READOUT_FALSE = 1,
   DETECTOR_READOUT_TRUE  = 2,
   DETECTOR_READOUT_LAST  = 3   /* Last readout in an image */

} DetectorReadout_Enum;


/**********************/
/** Global File Data **/
/**********************/

static ISIM_Class*  Isim = NULL;
/*
** Convenience Macros
*/

#define  ISIM_INSTR    (&(Isim->Instr))
#define  ISIM_SCI_FILE (&(Isim->SciFile))
#define  ISIM_DETECTOR (&(Isim->Instr.Detector))

/* 
** String lookup tables for ISIM_PwrState
*/
static char* InstrPwrStateStr[] = {
   
   "UNDEFINED",     /* 0: Invalid state  */ 
   "OFF",           /* 1: ISIM_PWR_OFF   */
   "INITIALIZING",  /* 2: ISIM_PWR_INIT  */
   "RESETTING",     /* 3: ISIM_PWR_RESET */
   "READY"          /* 4: ISIM_PWR_READY */

};

/* 
** String lookup tables for ISIM_SciState
*/
static char* InstrSciStateStr[] = {
   
   "UNDEFINED",  /* 0: Invalid state     */ 
   "OFF",        /* 1: ISIM_SCI_OFF      */
   "STARTING",   /* 2: ISIM_SCI_STARTING */
   "STOPPING",   /* 3: ISIM_SCI_STOPPING */
   "ON"          /* 4: ISIM_SCI_ENABLED  */

};


/* 
** Detector rows are strings to keep things simple for demonstration
** purposes. Science files are text.
*/
static ISIM_DetectorRow DetectorRowImage[ISIM_DETECTOR_ROWS_PER_IMAGE] = {
   { "00010203040506070809\n" }, 
   { "10111213141516171819\n" }, 
   { "20212223242526272829\n" },
   { "30313233343536373839\n" }, 
   { "40414243444546474849\n" },
   { "50515253545556575859\n" },
   { "60616263646566676869\n" },
   { "70717273747576777879\n" },
   { "80818283848586878889\n" },
   { "90919293949596979899\n" }
};

static char DetectorRowInitState[ISIM_DETECTOR_ROW_LEN] = { ',','I','n','i','t','i','a','l',' ','S','t','a','t','e','\n' };

/* Only one character is substituted per sample when a fault is present */ 
static char FaultCorruptedChar[ISIM_DETECTOR_ROWS_PER_IMAGE] = { 70, 65, 85, 76, 84, 65, 76, 69, 82, 84 };


/*******************************/
/** Local Function Prototypes **/
/*******************************/

/*
** Detector Object
*/

static void Detector_Init(ISIM_Detector* Detector, boolean ClearImageCnt);
static DetectorReadout_Enum Detector_Readout(ISIM_Detector* Detector);


/*
** Science File Object
*/

static void    SciFile_Init(ISIM_SciFile* SciFile);
static boolean SciFile_Create(ISIM_SciFile* SciFile, ISIMTBL_SciFile* SciFileTbl, uint16 ImageId);
static void    SciFile_Close(ISIM_SciFile* SciFile);
static boolean SciFile_WriteDetectorRow(ISIM_SciFile* SciFile, ISIM_DetectorRow* DetectorRow);
static void    SciFile_CreateCntFilename(ISIM_SciFile* SciFile, ISIMTBL_SciFile* SciFileTbl, uint16 ImageId);


/******************************************************************************
** Function: ISIM_Constructor
**
*/
void ISIM_Constructor(ISIM_Class*  IsimPtr)
{
 
   Isim = IsimPtr;

   CFE_PSP_MemSet((void*)Isim, 0, sizeof(ISIM_Class));

   Isim->Instr.PwrState = ISIM_PWR_OFF;
   Isim->Instr.SciState = ISIM_SCI_OFF;
   
   Detector_Init(ISIM_DETECTOR, TRUE);
   SciFile_Init(ISIM_SCI_FILE);

} /* End ISIM_Constructor() */


/******************************************************************************
** Function: ISIM_Execute
**
** Execute instrument simulation cycle. This method manages the behavior for
** the power and science state machines. Command functions can set state flags
** but the resulting behavior will be implemented in this method. 
**
*/
void ISIM_Execute(void)
{

   boolean CriticalError = FALSE;
   DetectorReadout_Enum DetectorReadout;
   static boolean ResumeSci = FALSE; 
           
   switch (Isim->Instr.PwrState) {
   
      case ISIM_PWR_OFF:
         
         if (Isim->Instr.SciState != ISIM_SCI_OFF) {
            
            Isim->Instr.SciState = ISIM_SCI_OFF;
            Detector_Init(ISIM_DETECTOR, TRUE);
            SciFile_Close(ISIM_SCI_FILE);

         }
         break;
   
      case ISIM_PWR_INIT:
         
         if (++Isim->Instr.PwrInitCycleCnt >= Isim->Tbl.SciInstr.PwrInitCycles) {
            
            Isim->Instr.PwrState = ISIM_PWR_READY;
            
            CFE_EVS_SendEvent (ISIM_INIT_COMPLETE_EID, CFE_EVS_INFORMATION,
                               "ISIM completed initialization after power on in %d cycles.",Isim->Instr.PwrInitCycleCnt);
            
            Isim->Instr.PwrInitCycleCnt = 0;
         
         } /* End if init cycle complete */
         break;
         
      /* Reset clears faults and will resume science if it was in progress when the reset occurred */ 
      case ISIM_PWR_RESET:
                 
         if (Isim->Instr.PwrResetCycleCnt == 0) {
         
            ResumeSci = (Isim->Instr.SciState == ISIM_SCI_ON);
            Isim->Instr.SciState = ISIM_SCI_OFF;
            Detector_Init(ISIM_DETECTOR, FALSE);
            SciFile_Close(ISIM_SCI_FILE);
         
         }
         
         if (++Isim->Instr.PwrResetCycleCnt >= Isim->Tbl.SciInstr.PwrResetCycles) {
            
            Isim->Instr.PwrState = ISIM_PWR_READY;
            if (ResumeSci) Isim->Instr.SciState = ISIM_SCI_STARTING;
            Isim->Instr.Detector.FaultPresent = FALSE;
            
            CFE_EVS_SendEvent (ISIM_RESET_COMPLETE_EID, CFE_EVS_INFORMATION,
                               "ISIM completed initialization after power reset in %d cycles. Cleared fault and restored science state to %s",
                               Isim->Instr.PwrResetCycleCnt, InstrSciStateStr[Isim->Instr.SciState]);
            
            Isim->Instr.PwrResetCycleCnt = 0;
            ResumeSci = FALSE;
            
         } /* End if reset cycle complete */    
         break;

      case ISIM_PWR_READY:
         
         switch (Isim->Instr.SciState) {
      
            case ISIM_SCI_OFF:
               break;
         
            case ISIM_SCI_STARTING:
            
               SciFile_Create(ISIM_SCI_FILE, &(Isim->Tbl.SciFile), Isim->Instr.Detector.ImageCnt);
               Isim->Instr.CurrFileImageCnt = 0;
               Isim->Instr.SciState = ISIM_SCI_ON;
               break;
            
            case ISIM_SCI_STOPPING:
            
               SciFile_Close(ISIM_SCI_FILE);
               Isim->Instr.SciState = ISIM_SCI_OFF;
               break;
         
            case ISIM_SCI_ON:

               DetectorReadout = Detector_Readout(ISIM_DETECTOR);
               
               if (DetectorReadout == DETECTOR_READOUT_TRUE || DetectorReadout == DETECTOR_READOUT_LAST) {
               
                  SciFile_WriteDetectorRow(ISIM_SCI_FILE, &(Isim->Instr.Detector.Row));
                  
                  if (DetectorReadout == DETECTOR_READOUT_LAST) {

                     ++Isim->Instr.CurrFileImageCnt;
                     if (Isim->Instr.CurrFileImageCnt >= Isim->Tbl.SciFile.ImagesPerFile) {
                  
                        SciFile_Close(ISIM_SCI_FILE);
                        SciFile_Create(ISIM_SCI_FILE, &(Isim->Tbl.SciFile), Isim->Instr.Detector.ImageCnt);
                        Isim->Instr.CurrFileImageCnt = 0;
                     
                     }
                  }
               }
               break;
            
            default:
         
               CFE_EVS_SendEvent (ISIM_INVALID_STATE_EID, CFE_EVS_CRITICAL,
                                  "Invalid ISIM science data state %d. Powering off instrument.",Isim->Instr.SciState);
               CriticalError = TRUE;

         } /* End sci data state switch */
         
         break;
         
      default:
         
         CFE_EVS_SendEvent (ISIM_INVALID_STATE_EID, CFE_EVS_CRITICAL,
                            "Invalid ISIM power state %d. Powering off instrument.",Isim->Instr.PwrState);
         
         CriticalError = TRUE;
      
   } /* End instrument state switch */
      
   if (CriticalError) {
      
      Isim->Instr.PwrState = ISIM_PWR_OFF;
      Isim->Instr.SciState = ISIM_SCI_OFF;

      SciFile_Close(ISIM_SCI_FILE);
      
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
      
	  case ISIMTBL_OBJ_SCI_INSTR:
         CFE_PSP_MemCpy(&(Isim->Tbl.SciInstr), ObjData, sizeof(ISIMTBL_SciInstr));
         break;
		 
      case ISIMTBL_OBJ_SCI_FILE:
         CFE_PSP_MemCpy(&(Isim->Tbl.SciFile), ObjData, sizeof(ISIMTBL_SciFile));
         break;
	  
      default:
	     RetStatus = FALSE;
   
   } /* End ObjId switch */
   
   CFE_EVS_SendEvent(ISIM_LOAD_TBL_OBJ_EID, CFE_EVS_INFORMATION, "ISIM table JSON object %d loaded.", ObjId);
   
   return RetStatus;

} /* End ISIM_LoadTblEntry() */


/******************************************************************************
** Functions: ISIM_PwrOnInstrCmd
**
** Power on the science instrument.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_PwrOnInstrCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;
   
   if (Isim->Instr.PwrState == ISIM_PWR_OFF) {
      
      Isim->Instr.PwrState = ISIM_PWR_INIT;
      Isim->Instr.PwrInitCycleCnt = 0;
      CFE_EVS_SendEvent (ISIM_PWR_ON_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument powered on. Entered power initializing state.");
      RetStatus = TRUE;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_PWR_ON_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Power on instrument cmd rejected. Instrument must be in OFF state and it's in the %s state.",
                         InstrPwrStateStr[Isim->Instr.PwrState]);
   
   }
   
   return RetStatus;

} /* End ISIM_PwrOnInstrCmd() */


/******************************************************************************
** Functions: ISIM_PwrOffInstrCmd
**
** Power off science instrument regardless of current state. The science state
** is unmodified and the ISIM_Execute() function takes care of any science
** data cleanup activities.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_PwrOffInstrCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (ISIM_PWR_OFF_CMD_EID, CFE_EVS_INFORMATION,        
                      "Instrument powered off from previous state %s",InstrPwrStateStr[Isim->Instr.PwrState]);
   
   Isim->Instr.PwrState = ISIM_PWR_OFF;
   
   /* See function prologue */
      
   return TRUE;

} /* End ISIM_PwrOffInstrCmd() */


/******************************************************************************
** Functions: ISIM_PwrResetInstrCmd
**
** Initiate a power reset. The ISIM_Execute() method defines how the simple
** responds to a reset.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_PwrResetInstrCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;

   if (Isim->Instr.PwrState == ISIM_PWR_READY) {
      
      Isim->Instr.PwrState = ISIM_PWR_RESET;
      Isim->Instr.PwrResetCycleCnt = 0;
      CFE_EVS_SendEvent (ISIM_PWR_RESET_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument reset initiated.");
      RetStatus = TRUE;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_PWR_RESET_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Power reset instrument cmd rejected. Instrument must be in READY state and it's in the %s state.",
                         InstrPwrStateStr[Isim->Instr.PwrState]);
   
   }
   
   return RetStatus;

} /* End ISIM_PwrResetInstrCmd() */


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
   
   if (Isim->Instr.PwrState == ISIM_PWR_READY) {
      
      Isim->Instr.SciState = ISIM_SCI_STARTING;
      CFE_EVS_SendEvent (ISIM_START_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument data started.");
      
      RetStatus = TRUE;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_START_SCI_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Start science instrument data cmd rejected. Instrument must be in READY state and it's in the %s state.",
                         InstrPwrStateStr[Isim->Instr.PwrState]);
   
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
   
   if (Isim->Instr.SciState == ISIM_SCI_STARTING ||
       Isim->Instr.SciState == ISIM_SCI_ON) {
      
      CFE_EVS_SendEvent (ISIM_STOP_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Science instrument data stopped.");
      
      Isim->Instr.SciState = ISIM_SCI_STOPPING;
   
   }  
   else { 
   
      CFE_EVS_SendEvent (ISIM_STOP_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                         "Stop science instrument data cmd ignored. Science data already stopped.");

   }
      
   return TRUE;

} /* End ISIM_StopSciCmd() */



/******************************************************************************
** Functions: ISIM_SetFaultCmd
**
** Set instrument fault state to TRUE.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_SetFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   Isim->Instr.Detector.FaultPresent = TRUE;

   CFE_EVS_SendEvent (ISIM_SET_FAULT_CMD_EID, CFE_EVS_INFORMATION, 
                      "Science instrument fault set to TRUE.");
               
   return TRUE;

} /* End ISIM_SetFaultCmd() */


/******************************************************************************
** Functions: ISIM_ClearFaultCmd
**
** Set instrument fault state to FALSE.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_ClearFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   Isim->Instr.Detector.FaultPresent = FALSE;

   CFE_EVS_SendEvent (ISIM_SET_FAULT_CMD_EID, CFE_EVS_INFORMATION, 
                      "Science instrument fault set to FALSE.");
               
   return TRUE;

} /* End ISIM_ClearFaultCmd() */



/*******************************/
/*******************************/
/****                       ****/
/****    DETECTOR OBJECT    ****/
/****                       ****/
/*******************************/
/*******************************/


/******************************************************************************
** Functions: Detector_Init
**
** Initialize the detector to a know state.
**
** Notes:
**   None
*/
static void Detector_Init(ISIM_Detector* Detector, boolean ClearImageCnt)
{

   Detector->ReadoutRow  = 0;
   Detector->FaultPresent = 0;
   if (ClearImageCnt) Detector->ImageCnt = 0;
   
   strncpy (Detector->Row.Data, DetectorRowInitState, ISIM_DETECTOR_ROW_LEN);

} /* End Detector_Init() */


/******************************************************************************
** Functions: Detector_Readout
**
** Load the next instrument detector readout row.
**
** Return Values:
**   DETECTOR_READOUT_FALSE: Detector->Row.Data[] not loaded
**   DETECTOR_READOUT_TRUE:  Detector->Row.Data[] loaded & its not the last row in an image
**   DETECTOR_READOUT_LAST:  Detector->Row.Data[] loaded & it is the last row in an image
**
*/
static DetectorReadout_Enum Detector_Readout(ISIM_Detector* Detector)
{

   DetectorReadout_Enum DetectorReadout = DETECTOR_READOUT_FALSE;
   
   if (Detector->ReadoutRow < ISIM_DETECTOR_ROWS_PER_IMAGE) {
   
      strncpy (Detector->Row.Data, DetectorRowImage[Detector->ReadoutRow].Data, ISIM_DETECTOR_ROW_LEN);
      
      /* Corrupt firt column of data when a fault is present */
      if (Detector->FaultPresent) {
         Detector->Row.Data[0] = FaultCorruptedChar[Detector->ReadoutRow];
      }
         
      Detector->ReadoutRow++;

      if (Detector->ReadoutRow < ISIM_DETECTOR_ROWS_PER_IMAGE) {
      
         DetectorReadout = DETECTOR_READOUT_TRUE;
      
      }
      else {
       
         Detector->ReadoutRow = 0;
         Detector->ImageCnt++;
         
         DetectorReadout = DETECTOR_READOUT_LAST;
      
      }
      
   } /* If valid Detector->ReadoutRow */
   else {
      

      CFE_EVS_SendEvent (ISIM_DETECTOR_ERR_EID, CFE_EVS_CRITICAL, 
                         "Invalid detector row %d index exceeds fixed maximum row index of %d.",Detector->ReadoutRow, (ISIM_DETECTOR_ROWS_PER_IMAGE-1));

      Detector->ReadoutRow = 0;
   
   }
   
   return DetectorReadout;
   
} /* Detector_Readout() */




/*******************************/
/*******************************/
/****                       ****/
/****    SCI FILE OBJECT    ****/
/****                       ****/
/*******************************/
/*******************************/


/******************************************************************************
** Functions: SciFile_Init
**
** Initialize the SciFile object to a known state.
**
** Notess:
**   None
*/
static void SciFile_Init(ISIM_SciFile* SciFile)
{
   
   SciFile->Handle = 0;
   SciFile->IsOpen = FALSE;
   strcpy(Isim->SciFile.Name, ISIM_NULL_FILENAME);
   
} /* End SciFile_Init() */


/******************************************************************************
** Functions: SciFile_Create
**
** Create a new science file using the ImageId in the filename
**
** Notes:
**   None
*/
static boolean SciFile_Create(ISIM_SciFile* SciFile, ISIMTBL_SciFile* SciFileTbl, uint16 ImageId)
{

   boolean RetStatus = FALSE;
   
   if (SciFile->IsOpen) {
      
      CFE_EVS_SendEvent (ISIM_SCI_FILE_CREATE_ERR_EID, CFE_EVS_ERROR, 
                         "Create science file failed due to a file already being open: %s",SciFile->Name);         
   
   }
   else {
   
      SciFile_CreateCntFilename(SciFile, SciFileTbl, ImageId);
      
      SciFile->Handle = OS_creat(SciFile->Name, OS_WRITE_ONLY);

      if (SciFile->Handle >= OS_FS_SUCCESS) {
      
         RetStatus = TRUE;
         SciFile->IsOpen = TRUE;
         CFE_EVS_SendEvent (ISIM_SCI_FILE_CREATE_EID, CFE_EVS_INFORMATION, 
                            "New science file created: %s",SciFile->Name);         

      }
      else {
         
         CFE_EVS_SendEvent (ISIM_SCI_FILE_CREATE_ERR_EID, CFE_EVS_ERROR, 
                            "Error creating new science file %s. Return status = 0x%4X",
                            SciFile->Name, SciFile->Handle);         
      
      }
   } /* End if no file currently open */
            
   return RetStatus;
   
} /* End SciFile_Create() */


/******************************************************************************
** Functions: SciFile_Close
**
** Close the current science file.
**
** Notes:
**   None
*/
static void SciFile_Close(ISIM_SciFile* SciFile)
{
 
   if (SciFile->IsOpen) {
      
      OS_close(SciFile->Handle);
      
      CFE_EVS_SendEvent (ISIM_SCI_FILE_CLOSE_EID, CFE_EVS_INFORMATION, 
                         "Closed science file %s",SciFile->Name);         
      
      SciFile->IsOpen = FALSE;
      strcpy(SciFile->Name, ISIM_NULL_FILENAME);

   }

} /* End SciFile_Close() */


/******************************************************************************
** Functions: SciFile_WriteDetectorRow
**
** Write a detector row to the current science file
**
** Notes:
**   None
*/
static boolean SciFile_WriteDetectorRow(ISIM_SciFile* SciFile, ISIM_DetectorRow* DetectorRow)
{
   
   int32   WriteStatus = 0;
   boolean RetStatus = FALSE;
   
   if (SciFile->IsOpen) {
     
      WriteStatus = OS_write(SciFile->Handle, DetectorRow, strlen(DetectorRow->Data));
      
      RetStatus = (WriteStatus > 0);
        
   } /* End file open */

   if (RetStatus == FALSE) {
   
      CFE_EVS_SendEvent (ISIM_SCI_FILE_WRITE_ERR_EID, CFE_EVS_ERROR, 
                         "Error writing to science file %s. IsOpen=%d, WriteStatus=%d",
                         SciFile->Name, SciFile->IsOpen, WriteStatus);

   }
   
   return RetStatus;
   
} /* End SciFile_WriteDetectorRow() */


/******************************************************************************
** Functions: SciFile_CreateCntFilename
**
** Create a filename using the table-defined base path/filename, current image
** ID, and the table-defined extension. 
**
** Notes:
**   1. No string buffer error checking performed
*/
static void SciFile_CreateCntFilename(ISIM_SciFile* SciFile, ISIMTBL_SciFile* SciFileTbl, uint16 ImageId)
{
   
   int i;
   
   char ImageIdStr[64];

   sprintf(ImageIdStr,"%03d",ImageId);

   strcpy (SciFile->Name, SciFileTbl->PathBaseFilename);

   i = strlen(SciFile->Name);  /* Starting position for image ID */
   strcat (&(SciFile->Name[i]), ImageIdStr);
   
   i = strlen(SciFile->Name);  /* Starting position for extension */
   strcat (&(SciFile->Name[i]), SciFileTbl->FileExtension);
   

} /* End SciFile_CreateCntFilename() */


/******************************************************************************
** Functions: SciFile_CreateTimeFilename
**
** Create a filename using the tabled-defined base path/filename, current time,
** and the table-defined extension. 
**
** Notes:
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

static void SciFile_CreateTimeFilename(char* Filename, ISIMTBL_SciFile* SciFileTbl)
{

   uint16  i;
   char    TimeStr[64];
   CFE_TIME_SysTime_t SysTime;

   strcpy (Filename,SciFileTbl->PathBaseFilename);
   
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
      
   strcat (&Filename[i], SciFileTbl->FileExtension);

} /* End SciFile_CreateTimeFilename() */

