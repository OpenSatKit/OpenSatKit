/* 
** Purpose: Define a scheduler. Uses scheduler & message tables.
**
** Notes:
**   1. This design intentionally decouples the scheduler table from 
**      application specific processing such as command callback 
**      functions and file processing.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _scheduler_
#define _scheduler_

/*
** Includes
*/

#include "app_cfg.h"
#include "msgtbl.h"
#include "schtbl.h"


/*
**  Macro Definitions
*/

#define SCHEDULER_TIMER_NAME   "SCH_MINOR_TIMER"   /* Must be shorter than OS_MAX_API_NAME */
#define SCHEDULER_SEM_NAME     "SCH_TIME_SEM"      /* Must be shorter than OS_MAX_API_NAME */
#define SCHEDULER_SEM_VALUE    0
#define SCHEDULER_SEM_OPTIONS  0

/*
** Synchronized to Mission Elapsed Time States
*/
#define SCHEDULER_SYNCH_FALSE          0
#define SCHEDULER_SYNCH_TO_MINOR       1
#define SCHEDULER_SYNCH_MAJOR_PENDING  2
#define SCHEDULER_SYNCH_TO_MAJOR       4

/*
** Major Frame Signal Source Identifiers
*/
#define SCHEDULER_MF_SOURCE_NONE               0
#define SCHEDULER_MF_SOURCE_CFE_TIME           1
#define SCHEDULER_MF_SOURCE_MINOR_FRAME_TIMER  2

/*
** Minor Frame Slot Characteristics
*/

#define SCHEDULER_NORMAL_SLOT_PERIOD (SCHEDULER_MICROS_PER_MAJOR_FRAME / SCHTBL_TOTAL_SLOTS)
#define SCHEDULER_SYNC_SLOT_PERIOD   (SCHEDULER_NORMAL_SLOT_PERIOD + SCHEDULER_SYNC_SLOT_DRIFT_WINDOW)
#define SCHEDULER_SHORT_SLOT_PERIOD  (SCHEDULER_NORMAL_SLOT_PERIOD - SCHEDULER_SYNC_SLOT_DRIFT_WINDOW)

/*
** Major Frame Signal Source Identifiers
*/
#define SCHEDULER_MF_SRC_NONE               0
#define SCHEDULER_MF_SRC_CFE_TIME           1
#define SCHEDULER_MF_SRC_MINOR_FRAME_TIMER  2

#define SCHEDULER_TIME_SYNC_SLOT   (SCHTBL_TOTAL_SLOTS-1)  /* Slot processing algorithm assumes this is set to the last slot */

/*
** Maximum allowed error in minor frame timing. Worst accuracy determined to be
** the amount of drift that would cause the loss of a minor frame over one
** major frame
*/
#define SCHEDULER_WORST_CLOCK_ACCURACY  (SCHEDULER_NORMAL_SLOT_PERIOD/(SCHTBL_TOTAL_SLOTS-1))

/*
** Maximum number of minor frames to sample looking for subsecs = 0. Maximum
** number of minor frame timer expirations to allow before giving up on
** finding the slot whose MET subseconds field is zero.  It is assumed that
** three complete major frames should be sufficient for finding such a slot.
*/

#define SCHEDULER_MAX_SYNC_ATTEMPTS   (SCHTBL_TOTAL_SLOTS * 3)

/*
** Event Message IDs
*/

#define SCHEDULER_MINOR_FRAME_TIMER_CREATE_ERR_EID      (SCHEDULER_BASE_EID + 0)
#define SCHEDULER_MINOR_FRAME_TIMER_ACC_WARN_EID        (SCHEDULER_BASE_EID + 1)
#define SCHEDULER_SEM_CREATE_ERR_EID                    (SCHEDULER_BASE_EID + 2)
#define SCHEDULER_MAJOR_FRAME_SUB_ERR_EID               (SCHEDULER_BASE_EID + 3)
#define SCHEDULER_NOISY_MAJOR_FRAME_ERR_EID             (SCHEDULER_BASE_EID + 4)

#define SCHEDULER_SAME_SLOT_EID                         (SCHEDULER_BASE_EID + 5)
#define SCHEDULER_MULTI_SLOTS_EID                       (SCHEDULER_BASE_EID + 6)
#define SCHEDULER_SKIPPED_SLOTS_EID                     (SCHEDULER_BASE_EID + 7)

#define SCHEDULER_PACKET_SEND_ERR_EID                   (SCHEDULER_BASE_EID + 8)


/*
** Type Definitions
*/

typedef struct {

   MSGTBL_Struct MsgTbl;
   SCHTBL_Struct SchTbl;

   uint32  SlotsProcessedCount;           /* Total number of Schedule Slots (Minor Frames) Processed */
   uint16  SkippedSlotsCount;             /* Number of times that slot (minor frame) were skipped. NOT the number of slots that were skipped  */
   uint16  MultipleSlotsCount;            /* Number of times that multiple slots were skipped. NOT the number of slots that were skipped  */
   uint16  SameSlotCount;                 /* Number of times scheduler woke up in the same slot as last time */

   uint32  ScheduleActivitySuccessCount;  /* Number of successfully performed activities */
   uint32  ScheduleActivityFailureCount;  /* Number of unsuccessful activities attempted */

   uint32  ValidMajorFrameCount;          /* Number of valid Major Frame tones received */
   uint32  MissedMajorFrameCount;         /* Number of missing Major Frame tones */
   uint32  UnexpectedMajorFrameCount;     /* Number of unexpected Major Frame tones */

   uint32  TablePassCount;                /* Number of times Schedule Table has been processed */
   uint32  ConsecutiveNoisyFrameCounter;  /* Number of consecutive noisy Major Frames */

   uint16  MinorFramesSinceTone;          /* Number of Minor Frames since last Major Frame tone */
   uint16  NextSlotNumber;                /* Next Minor Frame to be processed */
   uint16  LastSyncMETSlot;               /* MET Slot # where Time Sync last occurred */
   uint16  SyncAttemptsLeft;              /* Timeout counter used when syncing Major Frame to MET */

   boolean SendNoisyMajorFrameMsg;        /* Flag to send noisy major frame event msg once */
   boolean IgnoreMajorFrame;              /* Major Frame too noisy to trust */
   boolean UnexpectedMajorFrame;          /* Major Frame signal was unexpected */
   uint8   SyncToMET;                     /* Slots should be aligned with subseconds */
   uint8   MajorFrameSource;              /* Major Frame Signal source identifier */

   uint32  LastProcessCount;              /* Number of Slots Processed Last Cycle */

   uint32  TimerId;                       /* OSAL assigned timer ID for minor frame timer */
   uint32  TimeSemaphore;                 /* Semaphore used by time references to control main loop */
   uint32  ClockAccuracy;                 /* Accuracy of Minor Frame Timer */
   uint32  WorstCaseSlotsPerMinorFrame;   /* When syncing to MET, worst case # of slots that may need */

} SCHEDULER_Class;

/*
** Commands
*/

typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE]; /* cFE Software Bus Command Message Header     */
   uint16   Slot;
   uint16   EntryInSlot;
   boolean  ConfigFlag;   /* 0=Disable, 1=Enable */

} OS_PACK SCHEDULER_ConfigSchCmd;
#define SCHEDULER_CFG_SCH_CMD_DATA_LEN  (sizeof(SCHEDULER_ConfigSchCmd) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/


/******************************************************************************
** Function: SCHEDULER_Constructor
**
** This method creates a new scheduler instance.
**
** Notes:
**   1. This method must be called prior to all other methods. The SchTbl
**      instance variable only needs to be passed to the constructor
**      because a reference is stored by schtbl.c.
**
*/
void SCHEDULER_Constructor(SCHEDULER_Class* ObjPtr);


/******************************************************************************
** Function: SCHEDULER_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of the scheduler
** should not be impacted. The intent is to clear counters/flags for telemetry.
**
** Notes:
**   1. See the SCHEDULER_Class definition for the effected data.
**
*/
void SCHEDULER_ResetStatus(void);


/******************************************************************************
** Function: SCHEDULER_Execute
**
** Execute the scheduler to process schduler table and dispatch messages.
**
*/
boolean SCHEDULER_Execute(void);


/******************************************************************************
** Function: SCHEDULER_StartTimers
**
*/
int32 SCHEDULER_StartTimers(void);


/*******************************************************************
** Function: SCHEDULER_GetMsgTblPtr
**
** Return a pointer to the MsgTbl working table buffer.
*/
const MSGTBL_Struct* SCHEDULER_GetMsgTblPtr();


/******************************************************************************
** Function: SCHEDULER_LoadMsgTbl
**
** Load the MsgTbl working buffer.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match MSGTBL_LoadTbl
**
*/
boolean SCHEDULER_LoadMsgTbl(MSGTBL_Struct* NewTbl);


/******************************************************************************
** Function: SCHEDULER_LoadMsgTblEntry
**
** Load a single message table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match MSGTBL_LoadTblEntry
**
*/
boolean SCHEDULER_LoadMsgTblEntry(uint16 EntryId, MSGTBL_Entry* NewEntry);


/*******************************************************************
** Function: SCHEDULER_GetSchTblPtr
**
** Return a pointer to the SchTbl working table buffer.
*/
const SCHTBL_Struct* SCHEDULER_GetSchTblPtr();


/******************************************************************************
** Function: SCHEDULER_LoadSchTbl
**
** Load the SchTbl working buffer.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match SCHTBL_LoadTbl
**
*/
boolean SCHEDULER_LoadSchTbl(SCHTBL_Struct* NewTbl);


/******************************************************************************
** Function: SCHEDULER_LoadSchTblEntry
**
** Load a single scheduler table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match SCHTBL_LoadTblEntry
**
*/
boolean SCHEDULER_LoadSchTblEntry(uint16 EntryId, SCHTBL_Entry* NewEntry);


/******************************************************************************
** Function: SCHEDULER_ConfigSchEntryCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCHEDULER_ConfigSchEntryCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

#endif /* _scheduler_ */
