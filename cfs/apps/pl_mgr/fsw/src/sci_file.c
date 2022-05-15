/*
** Purpose: Implement the science file management object 
**
** Notes:
**   1. Information events are used in order to trace execution for
**      demonstrations.
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
#include "sci_file.h"


/**********************/
/** Global File Data **/
/**********************/

static SCI_FILE_Class_t*  SciFile = NULL;


/* 
** String lookup tables for SCI_FILE_State
*/

static char* SciStateStr[] =
{
   
   "UNDEFINED",  /* 0: Invalid state     */ 
   "OFF",        /* 1: SCI_FILE_OFF      */
   "STARTING",   /* 2: SCI_FILE_STARTING */
   "STOPPING",   /* 3: SCI_FILE_STOPPING */
   "ON"          /* 4: SCI_FILE_ON       */

};


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void    InitFileState(void);
static boolean CreateFile(uint16 ImageId);
static void    CloseFile(void);
static boolean WriteDetectorRow(PL_SIM_LIB_DetectorRow_t *DetectorRow);
static void    CreateCntFilename(uint16 ImageId);


/******************************************************************************
** Function: SCI_FILE_Constructor
**
*/
void SCI_FILE_Constructor(SCI_FILE_Class_t *SciFilePtr, INITBL_Class *IniTbl)
{
 
   SciFile = SciFilePtr;

   CFE_PSP_MemSet((void*)SciFile, 0, sizeof(SCI_FILE_Class_t));
    
   /* Load initialization configurations */
   
   SciFile->Config.ImagesPerFile = INITBL_GetIntConfig(IniTbl, CFG_SCI_FILE_IMAGE_CNT);
   strncpy(SciFile->Config.PathBaseFilename,
           INITBL_GetStrConfig(IniTbl, CFG_SCI_FILE_PATH_BASE),
           OS_MAX_PATH_LEN);
   strncpy(SciFile->Config.FileExtension,
           INITBL_GetStrConfig(IniTbl, CFG_SCI_FILE_EXTENSION),
           SCI_FILE_EXT_MAX_CHAR);

   /* Initialize to a known state. Call after config parameters in case they're used*/
   InitFileState();

} /* End SCI_FILE_Constructor() */


/******************************************************************************
** Function:  SCI_FILE_ResetStatus
**
*/
void SCI_FILE_ResetStatus(void)
{

   /* No state data should be changed */
   
} /* End SCI_FILE_ResetStatus() */


/******************************************************************************
** Functions: SCI_FILE_ConfigCmd
**
** Set configuration parameters for managing science files 
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. TODO: Add error checks
**  3. TODO: PathBaseFilename max len must be less than OS_MAX_PATH_LEN
**           rest of filename and extension.
**
*/
boolean SCI_FILE_ConfigCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SCI_FILE_ConfigCmdMsg_t *ConfigCmd = (SCI_FILE_ConfigCmdMsg_t *)MsgPtr;
   
   SciFile->Config.ImagesPerFile = ConfigCmd->Config.ImagesPerFile;
   
   strncpy(SciFile->Config.PathBaseFilename, ConfigCmd->Config.PathBaseFilename, OS_MAX_PATH_LEN);
   SciFile->Config.PathBaseFilename[OS_MAX_PATH_LEN-1] = '\0';
   
   strncpy(SciFile->Config.FileExtension, ConfigCmd->Config.FileExtension, SCI_FILE_EXT_MAX_CHAR);
   SciFile->Config.FileExtension[SCI_FILE_EXT_MAX_CHAR-1] = '\0';

} /* End SCI_FILE_ConfigCmd() */


/******************************************************************************
** Functions: SCI_FILE_Start
**
** Start the science data collection process 
**
** Notes:
**   1. See SCI_FILE_WriteDetectorData() prologue for file creation and
**      detector image synchronization.
**
*/
boolean SCI_FILE_Start(void)
{
   
   SciFile->State = SCI_FILE_ENABLED;
   SciFile->CreateNewFile = TRUE;
   
   return TRUE;

} /* End SciFile_Start() */


/******************************************************************************
** Functions: SCI_FILE_Stop
**
** Stop the science data collection process 
**
** Notes:
**   1. Always return true. Receiving the command even if sceince not in
**      progress allows automated cleanup procedures to execute without
**      generating errors.
**
*/
boolean SCI_FILE_Stop(char *EventStr, uint16 MaxStrLen)
{
  
   if (SciFile->State == SCI_FILE_DISABLED)
   {
   
      strncpy(EventStr, "Stop science received with no science data collection in progress", MaxStrLen);
   
   }
   else
   {
      
      CloseFile();
      strncpy(EventStr, "Sucessfully stopped science", MaxStrLen);
   
   } /* End if science enabled */
      
   InitFileState();
         
   return TRUE;
   
} /* End SciFile_Stop() */


/******************************************************************************
** Functions: SCI_FILE_WriteDetectorData
**
** Manage writing detector data to files
**
** Notes:
**   1. The Control input allows the caller to force a shutdown regardless
**      of the science data file processing state. The Detector data is
**      ignored and the science data file is closed. 
**   2. There is no synchronization between the start of a detector read out 
**      and the first file created after the initial start of writing science
**      data to a file. Therefore the first file after science collection is
**      started could have partial data. 
**
*/
void SCI_FILE_WriteDetectorData(PL_SIM_LIB_Detector_t *Detector, SCI_FILE_Control_t Control)
{

   boolean SaveDetectorRow = TRUE; 

   if (Control == SCI_FILE_SHUTDOWN)
   {
      CloseFile();
      InitFileState();
   }
   else
   {
      if (SciFile->State == SCI_FILE_ENABLED)     
      {

         if (SciFile->CreateNewFile)
         {
         
            /* Wait for first row before creating a file */
            if (Control == SCI_FILE_FIRST_ROW)
            {
               CreateFile(Detector->ImageCnt);
               SciFile->CreateNewFile = FALSE;
            }
            else
            {
               SaveDetectorRow = FALSE;
            }
         }
         
         if (SaveDetectorRow) WriteDetectorRow(&Detector->Row);         
         
         if (Control == SCI_FILE_LAST_ROW)
         {
            SciFile->ImageCnt++;
            if (SciFile->ImageCnt >= SciFile->Config.ImagesPerFile)
            {
               CloseFile();
               SciFile->CreateNewFile = TRUE;
            }
            
         } /* End if SCI_FILE_SAVE_LAST_ROW */
      } /* End if SCI_FILE_ENABLED */
   }
   
} /* End SciFile_WriteDetectorData() */


/******************************************************************************
** Functions: InitFileState
**
** Initialize the SciFile object to a known state.
**
** Notess:
**   None
*/
static void InitFileState(void)
{
   
   SciFile->CreateNewFile = FALSE;
   SciFile->State    = SCI_FILE_DISABLED;
   SciFile->Handle   = 0;
   SciFile->IsOpen   = FALSE;
   SciFile->ImageCnt = 0;
   strcpy(SciFile->Name, SCI_FILE_UNDEF_FILE);
   
} /* End InitFileState() */


/******************************************************************************
** Functions: CreateFile
**
** Create a new science file using the ImageId in the filename
**
** Notes:
**   None
*/
static boolean CreateFile(uint16 ImageId)
{

   boolean RetStatus = FALSE;
   
   if (SciFile->IsOpen)
   {
      
      CFE_EVS_SendEvent (SCI_FILE_CREATE_ERR_EID, CFE_EVS_ERROR, 
                         "Create science file failed due to a file already being open: %s", SciFile->Name);         
   
   }
   else
   {
   
      CreateCntFilename(ImageId);
      
      SciFile->Handle = OS_creat(SciFile->Name, OS_WRITE_ONLY);
      
      if (SciFile->Handle >= OS_FS_SUCCESS)
      {
      
         RetStatus = TRUE;
         SciFile->ImageCnt = 0;
         SciFile->IsOpen = TRUE;
         CFE_EVS_SendEvent (SCI_FILE_CREATE_EID, CFE_EVS_INFORMATION, 
                            "New science file created: %s",SciFile->Name);         

      }
      else
      {
         
         CFE_EVS_SendEvent (SCI_FILE_CREATE_ERR_EID, CFE_EVS_ERROR, 
                            "Error creating new science file %s. Return status = 0x%4X",
                            SciFile->Name, SciFile->Handle);         
      
      }
   } /* End if no file currently open */
            
   return RetStatus;
   
} /* End CreateFile() */


/******************************************************************************
** Functions: CloseFile
**
** Close the current science file.
**
** Notes:
**   None
*/
static void CloseFile(void)
{
 
   if (SciFile->IsOpen)
   {
      
      OS_close(SciFile->Handle);
      
      CFE_EVS_SendEvent (SCI_FILE_CLOSE_EID, CFE_EVS_INFORMATION, 
                         "Closed science file %s",SciFile->Name);         
      
      SciFile->IsOpen = FALSE;
      strcpy(SciFile->Name, SCI_FILE_UNDEF_FILE);

   }

} /* End SciFile_Close() */


/******************************************************************************
** Functions: CreateCntFilename
**
** Create a filename using the table-defined base path/filename, current image
** ID, and the table-defined extension. 
**
** Notes:
**   1. No string buffer error checking performed
*/
static void CreateCntFilename(uint16 ImageId)
{
   
   int i;
   
   char ImageIdStr[64];

   sprintf(ImageIdStr,"%03d",ImageId);

   strcpy (SciFile->Name, SciFile->Config.PathBaseFilename);

   i = strlen(SciFile->Name);  /* Starting position for image ID */
   strcat (&(SciFile->Name[i]), ImageIdStr);
   
   i = strlen(SciFile->Name);  /* Starting position for extension */
   strcat (&(SciFile->Name[i]), SciFile->Config.FileExtension);
   
} /* End CreateCntFilename() */


/******************************************************************************
** Functions: CreateTimeFilename
**
** Create a filename using the config-defined base path/filename, current time,
** and the config-defined extension. 
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
** SCI_FILE time string has format: "YYYYDDDHHMMSS"...
*/
#define SCI_FILE_YYYY_OFFSET  0
#define SCI_FILE_DDD_OFFSET   4
#define SCI_FILE_HH_OFFSET    7
#define SCI_FILE_MM_OFFSET    9
#define SCI_FILE_SS_OFFSET   11
#define SCI_FILE_TERM_OFFSET 13

static void CreateTimeFilename(char* Filename)
{

   uint16  i;
   char    TimeStr[64];
   CFE_TIME_SysTime_t SysTime;

   strcpy (Filename, SciFile->Config.PathBaseFilename);
   
   i = strlen(Filename);  /* Starting position for the time stamp */
   
   SysTime = CFE_TIME_GetTime();
   CFE_TIME_Print(TimeStr, SysTime);

   Filename[i + SCI_FILE_YYYY_OFFSET]     = TimeStr[CFE_YYYY_INDEX];
   Filename[i + SCI_FILE_YYYY_OFFSET + 1] = TimeStr[CFE_YYYY_INDEX + 1];
   Filename[i + SCI_FILE_YYYY_OFFSET + 2] = TimeStr[CFE_YYYY_INDEX + 2];
   Filename[i + SCI_FILE_YYYY_OFFSET + 3] = TimeStr[CFE_YYYY_INDEX + 3];

   Filename[i + SCI_FILE_DDD_OFFSET]      = TimeStr[CFE_DDD_INDEX];
   Filename[i + SCI_FILE_DDD_OFFSET + 1]  = TimeStr[CFE_DDD_INDEX + 1];
   Filename[i + SCI_FILE_DDD_OFFSET + 2]  = TimeStr[CFE_DDD_INDEX + 2];

   Filename[i + SCI_FILE_HH_OFFSET]       = TimeStr[CFE_HH_INDEX];
   Filename[i + SCI_FILE_HH_OFFSET + 1]   = TimeStr[CFE_HH_INDEX + 1];

   Filename[i + SCI_FILE_MM_OFFSET]       = TimeStr[CFE_MM_INDEX];
   Filename[i + SCI_FILE_MM_OFFSET + 1]   = TimeStr[CFE_MM_INDEX + 1];

   Filename[i + SCI_FILE_SS_OFFSET]       = TimeStr[CFE_SS_INDEX];
   Filename[i + SCI_FILE_SS_OFFSET + 1]   = TimeStr[CFE_SS_INDEX + 1];

   i += SCI_FILE_TERM_OFFSET;
      
   strcat (&Filename[i], SciFile->Config.FileExtension);

} /* End CreateTimeFilename() */

/******************************************************************************
** Functions: WriteDetectorRow
**
** Write a detector row to the current science file
**
** Notes:
**   None
*/
static boolean WriteDetectorRow(PL_SIM_LIB_DetectorRow_t *DetectorRow)
{
   
   int32   WriteStatus = 0;
   boolean RetStatus = FALSE;
   
   if (SciFile->IsOpen)
   {
     
      WriteStatus = OS_write(SciFile->Handle, DetectorRow->Data, strlen(DetectorRow->Data));
      
      RetStatus = (WriteStatus > 0);
        
   } /* End file open */

   if (RetStatus == FALSE)
   {
   
      CFE_EVS_SendEvent (SCI_FILE_WRITE_ERR_EID, CFE_EVS_ERROR, 
                         "Error writing to science file %s. IsOpen=%d, WriteStatus=%d",
                         SciFile->Name, SciFile->IsOpen, WriteStatus);

   }
   
   return RetStatus;
   
} /* End WriteDetectorRow() */


