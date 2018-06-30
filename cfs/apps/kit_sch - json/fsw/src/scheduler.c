/* 
** Purpose: Implement the scheduler. Uses scheduler & message tables.
**
** Notes:
**   1. Adopted from the GPM MCP750 scheduler application
**   2. This design intentionally decouples the scheduler table from 
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

/*
** Include Files:
*/

#include "scheduler.h"
#include "cfe_time_msg.h"


#if (OS_SUCCESS != CFE_SUCCESS)
   #error Code assumes OS_SUCCESS equals CFE_SUCCESS
#endif


/*
** File Function Prototypes
*/

static void   MajorFrameCallback(void);
static void   MinorFrameCallback(uint32 TimerId);
static uint32 GetCurrentSlotNumber(void);
static uint32 GetMETSlotNumber(void);
static int32  ProcessNextSlot(void);


/*
** Global File Data
*/

static SCHEDULER_Class*  Scheduler = NULL;

/*
** Exported Functions
*/


/******************************************************************************
** Function: SCHEDULER_Constructor
**
*/
void SCHEDULER_Constructor(SCHEDULER_Class* ObjPtr)
{

   int32 Status = CFE_SUCCESS;

   Scheduler = ObjPtr;

   Scheduler->SlotsProcessedCount = 0;
   Scheduler->SkippedSlotsCount   = 0;
   Scheduler->MultipleSlotsCount  = 0;
   Scheduler->SameSlotCount       = 0;
   Scheduler->ScheduleActivitySuccessCount = 0;
   Scheduler->ScheduleActivityFailureCount = 0;

   /*
   ** Start off assuming Major Frame synch is normal
   ** and should be coming at any moment
   */
   Scheduler->SendNoisyMajorFrameMsg = TRUE;
   Scheduler->IgnoreMajorFrame       = FALSE;
   Scheduler->UnexpectedMajorFrame   = FALSE;
   Scheduler->SyncToMET              = SCHEDULER_SYNCH_FALSE;
   Scheduler->MajorFrameSource       = SCHEDULER_MF_SRC_NONE;
   Scheduler->NextSlotNumber         = 0;
   Scheduler->MinorFramesSinceTone   = SCHEDULER_TIME_SYNC_SLOT;
   Scheduler->LastSyncMETSlot        = 0;
   Scheduler->SyncAttemptsLeft       = 0;
   Scheduler->UnexpectedMajorFrameCount   = 0;
   Scheduler->MissedMajorFrameCount       = 0;
   Scheduler->ValidMajorFrameCount        = 0;
   Scheduler->WorstCaseSlotsPerMinorFrame = 1;

   /*
   ** Configure Major Frame and Minor Frame sources
   */
   Scheduler->ClockAccuracy = SCHEDULER_WORST_CLOCK_ACCURACY;

   /*
   ** Create an OSAL timer to drive the Minor Frames
   */
   Status = OS_TimerCreate(&Scheduler->TimerId,
                           SCHEDULER_TIMER_NAME,
                           &Scheduler->ClockAccuracy,
                           MinorFrameCallback);

    if (Status != OS_SUCCESS) {

        CFE_EVS_SendEvent(SCHEDULER_MINOR_FRAME_TIMER_CREATE_ERR_EID, CFE_EVS_ERROR,
                          "Error creating Minor Frame Timer (RC=0x%08X)",
                          Status);
    }
    else {

       /*
       ** Determine if the timer has an acceptable clock accuracy
       */
       if (Scheduler->ClockAccuracy > SCHEDULER_WORST_CLOCK_ACCURACY) {

          CFE_EVS_SendEvent(SCHEDULER_MINOR_FRAME_TIMER_ACC_WARN_EID, CFE_EVS_INFORMATION,
                          "OS Timer Accuracy (%d > reqd %d usec) requires Minor Frame MET sync",
                          Scheduler->ClockAccuracy, SCHEDULER_WORST_CLOCK_ACCURACY);

           /* Synchronize Minor Frame Timing with Mission Elapsed Time to keep from losing slots */
           Scheduler->SyncToMET = SCHEDULER_SYNCH_TO_MINOR;

           /* Calculate how many slots we may have to routinely process on each Minor Frame Wakeup */
           Scheduler->WorstCaseSlotsPerMinorFrame = ((Scheduler->ClockAccuracy * 2) / SCHEDULER_NORMAL_SLOT_PERIOD) + 1;

       } /* End if bad accuracy */

       /*
       ** Create main task semaphore (given by MajorFrameCallback and MinorFrameCallback)
       */

       Status = OS_BinSemCreate(&Scheduler->TimeSemaphore, SCHEDULER_SEM_NAME, SCHEDULER_SEM_VALUE, SCHEDULER_SEM_OPTIONS);

       if (Status != CFE_SUCCESS) {

          CFE_EVS_SendEvent(SCHEDULER_SEM_CREATE_ERR_EID, CFE_EVS_ERROR,
                          "Error creating Main Loop Timing Semaphore (RC=0x%08X)",
                          Status);

       } /* End if binary semaphore created */

    } /* End if minor frame timer created */

} /* End SCHEDULER_Constructor() */

/*******************************************************************
** Function: SCHEDULER_ResetStatus
**
*/
void SCHEDULER_ResetStatus()
{

   Scheduler->SlotsProcessedCount          = 0;
   Scheduler->SkippedSlotsCount            = 0;
   Scheduler->MultipleSlotsCount           = 0;
   Scheduler->SameSlotCount                = 0;
   Scheduler->ScheduleActivitySuccessCount = 0;
   Scheduler->ScheduleActivityFailureCount = 0;
   Scheduler->ValidMajorFrameCount         = 0;
   Scheduler->MissedMajorFrameCount        = 0;
   Scheduler->UnexpectedMajorFrameCount    = 0;
   Scheduler->TablePassCount               = 0;
   Scheduler->ConsecutiveNoisyFrameCounter = 0;
   Scheduler->IgnoreMajorFrame             = FALSE;

} /* End SCHEDULER_ResetStatus() */


/*******************************************************************
** Function: SCHEDULER_GetMsgTblPtr
**
** Return a pointer to the MsgTbl working table buffer.
*/
const MSGTBL_Struct* SCHEDULER_GetMsgTblPtr()
{

   return &(Scheduler->MsgTbl);

} /* End SCHEDULER_GetMsgTblPtr() */


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
boolean SCHEDULER_LoadMsgTbl(MSGTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(Scheduler->MsgTbl), NewTbl, sizeof(MSGTBL_Struct));       

   return TRUE;

} /* End SCHEDULER_LoadMsgTbl() */


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
boolean SCHEDULER_LoadMsgTblEntry(uint16 EntryId, MSGTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(Scheduler->MsgTbl.Entry[EntryId]),NewEntry,sizeof(MSGTBL_Entry));

   return TRUE;

} /* End SCHEDULER_LoadMsgTblEntry() */


/*******************************************************************
** Function: SCHEDULER_GetSchTblPtr
**
** Return a pointer to the SchTbl working table buffer.
*/
const SCHTBL_Struct* SCHEDULER_GetSchTblPtr()
{

   return &(Scheduler->SchTbl);

} /* End SCHEDULER_GetSchTblPtr() */


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
boolean SCHEDULER_LoadSchTbl(SCHTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(Scheduler->SchTbl), NewTbl, sizeof(SCHTBL_Struct));       

   return TRUE;

} /* End SCHEDULER_LoadSchTbl() */


/*******************************************************************
** Function: SCHEDULER_LoadSchTblEntry
**
** Load a single scheduler table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match SCHTBL_LoadTblEntry
**
*/
boolean SCHEDULER_LoadSchTblEntry(uint16 EntryId, SCHTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(Scheduler->SchTbl.Entry[EntryId]),NewEntry,sizeof(SCHTBL_Entry));

   return TRUE;

} /* End SCHEDULER_LoadSchTblEntry() */



/******************************************************************************
** Function: SCHEDULER_StartTimers
**
*/
int32 SCHEDULER_StartTimers(void)
{

   int32 Status = CFE_SUCCESS;

   /*
   ** Connect to cFE TIME's time reference marker (typically 1 Hz)
   ** to use it as the Major Frame synchronization source
   */

   Status = CFE_TIME_RegisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)&MajorFrameCallback);

   if (Status != CFE_SUCCESS) {

      CFE_EVS_SendEvent(SCHEDULER_MAJOR_FRAME_SUB_ERR_EID, CFE_EVS_ERROR,
                       "Error Subscribing to Major Frame Tone (RC=0x%08X)",
                       Status);
   }
   else {

      /*
      ** Start the Minor Frame Timer with an extended delay to allow a Major Frame Sync
      ** to start processing.  If the Major Frame Sync fails to arrive, then we will
      ** start when this timer expires and synch ourselves to the MET clock.
      */
      Status = OS_TimerSet(Scheduler->TimerId, SCHEDULER_STARTUP_PERIOD, 0);

   }

   return (Status);

} /* End SCHEDULER_StartTimers() */


/*******************************************************************
**
** SCHEDULER_Execute
**
*/
boolean SCHEDULER_Execute(void)
{
   uint32  CurrentSlot;
   uint32  ProcessCount;
   int32   Result = CFE_SUCCESS;

   /* Wait for the next slot (Major or Minor Frame) */
   Result = OS_BinSemTake(Scheduler->TimeSemaphore);

   if (Result == OS_SUCCESS)
   {

      /* TODO - OS_printf("ProcessTable::OS_BinSemTake() success");*/

      if (Scheduler->IgnoreMajorFrame)
      {
         if (Scheduler->SendNoisyMajorFrameMsg)
         {
            CFE_EVS_SendEvent(SCHEDULER_NOISY_MAJOR_FRAME_ERR_EID, CFE_EVS_ERROR,
                              "Major Frame Sync too noisy (Slot %d). Disabling synchronization.",
                              Scheduler->MinorFramesSinceTone);
            Scheduler->SendNoisyMajorFrameMsg = FALSE;
         }
      } /* End if ignore Major Frame */
      else
      {
         Scheduler->SendNoisyMajorFrameMsg = TRUE;
      }

      CurrentSlot = GetCurrentSlotNumber();

      /* Compute the number of slots we need to process (watch for rollover) */
      if (CurrentSlot < Scheduler->NextSlotNumber)
      {
         ProcessCount = SCHTBL_TOTAL_SLOTS - Scheduler->NextSlotNumber;
         ProcessCount += (CurrentSlot + 1);
      }
      else
      {
         ProcessCount = (CurrentSlot - Scheduler->NextSlotNumber) + 1;
      }

      /* CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "ProcessTable::CurrentSlot=%d, First ProcessCount=%d", CurrentSlot, ProcessCount); */

      /*
      ** Correct for the following conditions observed when minor frame driven
      ** by a clock with poor accuracy
      **
      **   1) Wake up a little too late for just 1 slot
      **      symptom = multi slots event followed by same slot event
      **
      **   2) Wake up a little too early for just 1 slot
      **      symptom = same slot event followed by multi slots event
      */
      if (ProcessCount == 2)
      {
         /*
         ** If we want to do 2 slots but last time was OK then assume we
         **    are seeing condition #1 above.  By doing just 1 slot now,
         **    there will still be 1 to do when the next wakeup occurs
         **    and we will avoid both events.  But, if we really are in
         **    a delayed state, we will process both slots when we wake
         **    up next time because then the last time will NOT be OK.
         */
         if (Scheduler->LastProcessCount == 1)
         {
            ProcessCount = 1;
         }
         Scheduler->LastProcessCount = 2;
      }
      else if (ProcessCount == SCHTBL_TOTAL_SLOTS)
      {
         /* Same as previous comment except in reverse order. */
         if (Scheduler->LastProcessCount != SCHTBL_TOTAL_SLOTS)
         {
            ProcessCount = 1;
         }
         Scheduler->LastProcessCount = SCHTBL_TOTAL_SLOTS;
      }
      else
      {
         Scheduler->LastProcessCount = ProcessCount;
      }

      /*
      ** If current slot = next slot - 1, assume current slot did not increment
      */
      if (ProcessCount == SCHTBL_TOTAL_SLOTS)
      {
         Scheduler->SameSlotCount++;

         CFE_EVS_SendEvent(SCHEDULER_SAME_SLOT_EID, CFE_EVS_DEBUG,
                           "Slot did not increment: slot = %d",
                           CurrentSlot);
         ProcessCount = 0;
      }

      /* If we are too far behind, jump forward and do just the current slot */
      if (ProcessCount > SCHEDULER_MAX_LAG_COUNT)
      {
         Scheduler->SkippedSlotsCount++;

         CFE_EVS_SendEvent(SCHEDULER_SKIPPED_SLOTS_EID, CFE_EVS_ERROR,
                           "Slots skipped: slot = %d, count = %d",
                           Scheduler->NextSlotNumber, (ProcessCount - 1));

         /*
         ** Update the pass counter if we are skipping the rollover slot
         */
         if (CurrentSlot < Scheduler->NextSlotNumber)
         {
            Scheduler->TablePassCount++;
         }

         /*
         ** Process ground commands if we are skipping the time synch slot
         ** NOTE: This assumes the Time Synch Slot is the LAST Schedule slot
         **       (see definition of SCH_TIME_SYNC_SLOT in sch_app.h)
         ** Ground commands should only be processed at the end of the schedule table
         ** so that Group Enable/Disable commands do not change the state of entries
         ** in the middle of a schedule.
         */
         if ((Scheduler->NextSlotNumber + ProcessCount) > SCHEDULER_TIME_SYNC_SLOT)
         {
             /* TODO -  Move to App level Result = SCH_ProcessCommands(); */
         }

         Scheduler->NextSlotNumber = CurrentSlot;
         ProcessCount = 1;

      } /* End if (ProcessCount > SCHEDULER_MAX_LAG_COUNT) */

      /*
      ** Don't try to catch up all at once, just do a couple
      */
      if (ProcessCount > SCHEDULER_MAX_SLOTS_PER_WAKEUP)
      {
         ProcessCount = SCHEDULER_MAX_SLOTS_PER_WAKEUP;
      }

      /* Keep track of multi-slot processing */
      if (ProcessCount > 1)
      {
         Scheduler->MultipleSlotsCount++;

         /* Generate an event message if not syncing to MET or when there is more than two being processed */
         if ((ProcessCount > Scheduler->WorstCaseSlotsPerMinorFrame) || (Scheduler->SyncToMET == SCHEDULER_SYNCH_FALSE))
         {
            CFE_EVS_SendEvent(SCHEDULER_MULTI_SLOTS_EID, CFE_EVS_INFORMATION,
                             "Multiple slots processed: slot = %d, count = %d",
                             Scheduler->NextSlotNumber, ProcessCount);
         }

      } /* End if ProcessCount > 1) */

      /* TODO - OS_printf("ProcessTable::Final ProcessCount=%d", ProcessCount);*/
      /* Process the slots (most often this will be just one) */
      while ((ProcessCount != 0) && (Result == CFE_SUCCESS))
      {
         Result = ProcessNextSlot();
         ProcessCount--;
      }

   } /* End Semaphore */

   return(Result == CFE_SUCCESS);

} /* End of SCHEDULER_ProcessTable() */


/*******************************************************************
** Function: MajorFrameCallback
**
*/
void MajorFrameCallback(void)
{
    /*
    ** Synchronize slot zero to the external tone signal
    */
    uint16 StateFlags;

    /* OS_printf("MajorFrameCallback()\n"); */
    
    /*
    ** If cFE TIME is in FLYWHEEL mode, then ignore all synchronization signals
    */
    StateFlags = CFE_TIME_GetClockInfo();

    if ((StateFlags & CFE_TIME_FLAG_FLYING) == 0)
    {
        /*
        ** Determine whether the major frame is noisy or not
        **
        ** Conditions below are as follows:
        **    If we are NOT synchronized to the MET (i.e. - the Minor Frame timer
        **    has an acceptable resolution), then the Major Frame signal should
        **    only occur in the last slot of the schedule table.
        **
        **    If we ARE synchronized to the MET (i.e. - the Minor Frame timer is
        **    not as good as we would like), then the Major Frame signal should
        **    occur within a window of slots at the end of the table.
        */
        if (((Scheduler->SyncToMET == SCHEDULER_SYNCH_FALSE) &&
             (Scheduler->MinorFramesSinceTone != SCHEDULER_TIME_SYNC_SLOT)) ||
            ((Scheduler->SyncToMET == SCHEDULER_SYNCH_TO_MINOR) &&
             (Scheduler->NextSlotNumber != 0) &&
             (Scheduler->NextSlotNumber <
              (SCHTBL_TOTAL_SLOTS - Scheduler->WorstCaseSlotsPerMinorFrame - 1))))
        {
            /*
            ** Count the number of consecutive noisy major frames and the Total number
            ** of noisy major frames.  Also, indicate in telemetry that this particular
            ** Major Frame signal is considered noisy.
            */
            Scheduler->UnexpectedMajorFrame = TRUE;
            Scheduler->UnexpectedMajorFrameCount++;

            /*
            ** If the Major Frame is not being ignored yet, then increment the consecutive noisy
            ** Major Frame counter.
            */
            if (!Scheduler->IgnoreMajorFrame)
            {
                Scheduler->ConsecutiveNoisyFrameCounter++;

                /*
                ** If the major frame is too "noisy", then send event message and ignore future signals
                */
                if (Scheduler->ConsecutiveNoisyFrameCounter >= SCHEDULER_MAX_NOISY_MF)
                {
                    Scheduler->IgnoreMajorFrame = TRUE;
                }
            }
        }
        else /* Major Frame occurred when expected */
        {
            Scheduler->UnexpectedMajorFrame = FALSE;
            Scheduler->ConsecutiveNoisyFrameCounter = 0;
        }

        /*
        ** Ignore this callback if SCH has detected a noisy Major Frame Synch signal
        */
        if (Scheduler->IgnoreMajorFrame == FALSE)
        {
            /*
            ** Stop Minor Frame Timer (which should be waiting for an unusually long
            ** time to allow the Major Frame source to resynchronize timing) and start
            ** it again with nominal Minor Frame timing
            */
            OS_TimerSet(Scheduler->TimerId, SCHEDULER_NORMAL_SLOT_PERIOD, SCHEDULER_NORMAL_SLOT_PERIOD);

            /*
            ** Increment Major Frame process counter
            */
            Scheduler->ValidMajorFrameCount++;

            /*
            ** Set current slot = zero to synchronize activities
            */
            Scheduler->MinorFramesSinceTone = 0;

            /*
            ** Major Frame Source is now from CFE TIME
            */
            Scheduler->MajorFrameSource = SCHEDULER_MF_SOURCE_CFE_TIME;

            /* Clear any Major Frame In Sync with MET flags */
            /* But keep the Minor Frame In Sync with MET flag if it is set */
            Scheduler->SyncToMET &= SCHEDULER_SYNCH_TO_MINOR;

            /*
            ** Give "wakeup SCH" semaphore
            */
            OS_BinSemGive(Scheduler->TimeSemaphore);

        } /* End if IgnoreMajorFrame == FLASE */

    } /* End if clock not fly wheeling */

    /*
    ** We should assume that the next Major Frame will be in the same MET slot as this
    */
    Scheduler->LastSyncMETSlot = GetMETSlotNumber();

    return;

} /* End MajorFrameCallback() */


/*******************************************************************
** Function: MinorFrameCallback
**
*/
void MinorFrameCallback(uint32 TimerId)
{
    uint32  CurrentSlot;


    /* OS_printf("MinorFrameCallback()\n"); */
    
    /*
    ** If this is the very first timer interrupt, then the initial
    ** Major Frame Synchronization timed out.  This can occur when
    ** either the signal is not arriving or the clock has gone into
    ** FLYWHEEL mode.  We should synchronize to the MET time instead.
    */
    if (Scheduler->MajorFrameSource == SCHEDULER_MF_SOURCE_NONE)
    {
        Scheduler->MajorFrameSource = SCHEDULER_MF_SOURCE_MINOR_FRAME_TIMER;

        /* Synchronize timing to MET */
        Scheduler->SyncToMET |= SCHEDULER_SYNCH_MAJOR_PENDING;
        Scheduler->SyncAttemptsLeft = SCHEDULER_MAX_SYNC_ATTEMPTS;
        Scheduler->LastSyncMETSlot = 0;
    }

    /* If attempting to synchronize the Major Frame with MET, then wait for zero subsecs before starting */
    if (((Scheduler->SyncToMET & SCHEDULER_SYNCH_MAJOR_PENDING) != 0) &&
        (Scheduler->MajorFrameSource == SCHEDULER_MF_SOURCE_MINOR_FRAME_TIMER))
    {
        /* Whether we have found the Major Frame Start or not, wait another slot */
        OS_TimerSet(Scheduler->TimerId, SCHEDULER_NORMAL_SLOT_PERIOD, SCHEDULER_NORMAL_SLOT_PERIOD);

        /* Determine if this was the last attempt */
        Scheduler->SyncAttemptsLeft--;

        CurrentSlot = GetMETSlotNumber();
        if ((CurrentSlot != 0) && (Scheduler->SyncAttemptsLeft > 0))
        {
            return;
        }
        else  /* Synchronization achieved (or at least, aborted) */
        {
            /* Clear the pending synchronization flag and set the "Major In Sync" flag */
            Scheduler->SyncToMET &= ~SCHEDULER_SYNCH_MAJOR_PENDING;
            Scheduler->SyncToMET |= SCHEDULER_SYNCH_TO_MAJOR;

            /* CurrentSlot should be equal to zero.  If not, this is the best estimate we can use */
            Scheduler->MinorFramesSinceTone = CurrentSlot;
            Scheduler->LastSyncMETSlot = 0;
        }
    }
    else
    {
        /*
        ** If we are already synchronized with MET or don't care to be, increment current slot
        */
        Scheduler->MinorFramesSinceTone++;
    }

    if (Scheduler->MinorFramesSinceTone >= SCHTBL_TOTAL_SLOTS)
    {
        /*
        ** If we just rolled over from the last slot to slot zero,
        ** It means that the Major Frame Callback did not cancel the
        ** "long slot" timer that was started in the last slot
        **
        ** It also means that we may now need a "short slot"
        ** timer to make up for the previous long one
        */
        OS_TimerSet(Scheduler->TimerId, SCHEDULER_SHORT_SLOT_PERIOD, SCHEDULER_NORMAL_SLOT_PERIOD);

        Scheduler->MinorFramesSinceTone = 0;

        Scheduler->MissedMajorFrameCount++;
    }

    /*
    ** Determine the timer delay value for the next slot
    */
    if (Scheduler->MinorFramesSinceTone == SCHEDULER_TIME_SYNC_SLOT)
    {
        /*
        ** Start "long slot" timer (should be stopped by Major Frame Callback)
        */
        OS_TimerSet(Scheduler->TimerId, SCHEDULER_SYNC_SLOT_PERIOD, 0);
    }

    /*
    ** Note that if this is neither the first "short" minor frame nor the
    ** last "long" minor frame, the timer is not modified.  This should
    ** provide more stable timing than introducing the dither associated
    ** with software response times to timer interrupts.
    */

    /*
    ** Give "wakeup SCH" semaphore
    */
    OS_BinSemGive(Scheduler->TimeSemaphore);

    return;

} /* End MinorFrameCallback() */


/******************************************************************************
** Function: SCHEDULER_ConfigSchEntryCmd
**
*/
boolean SCHEDULER_ConfigSchEntryCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const   SCHEDULER_ConfigSchCmd *ConfigSchEntryCmd = (const SCHEDULER_ConfigSchCmd *) MsgPtr;
   uint16  entry;
   
   if (ConfigSchEntryCmd->Slot < SCHTBL_TOTAL_SLOTS)
   {

      if (ConfigSchEntryCmd->EntryInSlot < SCHTBL_ENTRIES_PER_SLOT)
      {
         entry = SCHTBL_INDEX(ConfigSchEntryCmd->Slot, ConfigSchEntryCmd->EntryInSlot);
         Scheduler->SchTbl.Entry[entry].Enabled = ConfigSchEntryCmd->ConfigFlag;
      }
      else
      {
         CFE_EVS_SendEvent (666, CFE_EVS_ERROR, "Configure command error. Invalid entry %d greater than max %d",
                            ConfigSchEntryCmd->EntryInSlot,SCHTBL_ENTRIES_PER_SLOT);
      }

   } /* End if valid slot ID */
   else
   {
      CFE_EVS_SendEvent (666, CFE_EVS_ERROR, "Configure command error. Invalid slot %d greater than max %d",
                         ConfigSchEntryCmd->Slot,SCHTBL_TOTAL_SLOTS);

   } /* End if invalid slot ID */

   return TRUE;

} /* End TBLMGR_ConfigSchEntryCmd() */


/*******************************************************************
**
** Configure (Enable/Disable) a single entry
**
*/
void SCHTBL_ConfigureTableEntry(uint16 SlotId, uint16 SlotEntry, boolean EnableFlag)
{

   uint16 i = SCHTBL_INDEX(SlotId,SlotEntry);

   Scheduler->SchTbl.Entry[i].Enabled = EnableFlag;

} /* End SCHTBL_ConfigureTableEntry() */


/*******************************************************************
**
** GetCurrentSlotNumber
**
*/
static uint32 GetCurrentSlotNumber(void)
{
    uint32  CurrentSlot;

    if (Scheduler->SyncToMET != SCHEDULER_SYNCH_FALSE)
    {
        CurrentSlot = GetMETSlotNumber();

        /*
        ** If we are only concerned with synchronizing the minor frames to an MET,
        ** then we need to adjust the current slot by whatever MET time is prevalent
        ** when the Major Frame Signal is received.
        ** If we are synchronizing the Major Frame, then, by definition, LastSyncMETSlot
        ** would be a zero and the current slot would be appropriate.
        */
        if (CurrentSlot < Scheduler->LastSyncMETSlot)
        {
            CurrentSlot = CurrentSlot + SCHTBL_TOTAL_SLOTS - Scheduler->LastSyncMETSlot;
        }
        else
        {
            CurrentSlot = CurrentSlot - Scheduler->LastSyncMETSlot;
        }
    }
    else
    {
        CurrentSlot = Scheduler->MinorFramesSinceTone;
    }

    return CurrentSlot;

} /* End GetCurrentSlotNumber() */

/*******************************************************************
**
** GetMETSlotNumber
**
*/
static uint32 GetMETSlotNumber(void)
{
    uint32 SubSeconds = 0;
    uint32 MicroSeconds;
    uint32 Remainder;
    uint32 METSlot;

    /*
    ** Use MET rather than current time to avoid time changes
    */
    SubSeconds = CFE_TIME_GetMETsubsecs();

    /*
    ** Convert sub-seconds to micro-seconds
    */
    MicroSeconds = CFE_TIME_Sub2MicroSecs(SubSeconds);

    /*
    ** Calculate schedule table slot number
    */
    METSlot = (MicroSeconds / SCHEDULER_NORMAL_SLOT_PERIOD);

    /*
    ** Check to see if close enough to round up to next slot
    */
    Remainder = MicroSeconds - (METSlot * SCHEDULER_NORMAL_SLOT_PERIOD);

    /*
    ** Add one more microsecond and see if it is sufficient to add another slot
    */
    Remainder += 1;
    METSlot += (Remainder / SCHEDULER_NORMAL_SLOT_PERIOD);

    /*
    ** Check to see if the Current Slot number needs to roll over
    */
    if (METSlot == SCHTBL_TOTAL_SLOTS)
    {
        METSlot = 0;
    }

    return METSlot;

} /* end GetMETSlotNumber() */


/*******************************************************************
**
** ProcessNextSlot
**
*/
static int32 ProcessNextSlot(void)
{
    
   int32  Result = CFE_SUCCESS; /* TODO - Fix after resolve ground command processing */
   int32  EntryNumber;
   int32  SlotIndex;
   uint32 Remainder;
   SCHTBL_Entry *NextEntry;
   uint16 *MsgPtr;
   int32  MsgSendStatus;

   SlotIndex = Scheduler->NextSlotNumber * SCHTBL_ENTRIES_PER_SLOT;
   NextEntry = &Scheduler->SchTbl.Entry[SlotIndex];

   /* Process each enabled entry in the schedule table slot */
   for (EntryNumber = 0; EntryNumber < SCHTBL_ENTRIES_PER_SLOT; EntryNumber++) {
      
      if (NextEntry->Enabled == TRUE) {

         Remainder = Scheduler->TablePassCount % NextEntry->Frequency;

         if (Remainder == NextEntry->Offset) {

            /* CFE_EVS_SendEvent(SCHEDULER_PACKET_SEND_ERR_EID, CFE_EVS_INFORMATION,"Processed scheduler table slot %d, entry %d, msgid %d", Scheduler->NextSlotNumber, EntryNumber, NextEntry->MsgTblEntryId); */
             
            MsgSendStatus = CFE_SB_NO_MESSAGE;  /* use any non-success error code */
            if (MSGTBL_GetMsgPtr(NextEntry->MsgTblEntryId, &MsgPtr)) {
               
               MsgSendStatus = CFE_SB_SendMsg((CFE_SB_Msg_t *)MsgPtr);
               /* 
               OS_printf("MSGTBL_SendMsg() - %d\n", EntryId);
               CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION,"MSGTBL Send: EntryId = %d, Buffer[0] = 0x%04x, SB_SendMsg Status = 0x%08X", EntryId, MsgTbl->Table.Entry[EntryId].Buffer[0], MsgSendStatus);
               CFE_EVS_SendEvent(MSGTBL_SB_SEND_ERR_EID, CFE_EVS_ERROR,
                                 "MSGTBL Send Error: EntryId = %d, MsgId = 0x%04x, SB_SendMsg Status = 0x%08X",
                                 EntryId, MsgTbl->Tbl.Entry[EntryId].Buffer[0], MsgSendStatus);
               */
            } /* End if got msg ptr */ 

            if (MsgSendStatus == CFE_SUCCESS)
               
               Scheduler->ScheduleActivitySuccessCount++;
            
            }
            else {
                  
               NextEntry->Enabled = FALSE;
               Scheduler->ScheduleActivityFailureCount++;

               CFE_EVS_SendEvent(SCHEDULER_PACKET_SEND_ERR_EID, CFE_EVS_ERROR,
                                 "Activity error: slot = %d, entry = %d, err = 0x%08X",
                                 Scheduler->NextSlotNumber, EntryNumber, MsgSendStatus);
            
            } /* End if msg send error */
         
         } /* End if offset met */

      } /* End if entry is enabled */

      NextEntry++;

   } /* Entries per slot loop */

   /*
   ** Process ground commands in the slot reserved for time synch
   ** Ground commands should only be processed at the end of the schedule table
   ** so that Group Enable/Disable commands do not change the state of entries
   ** in the middle of a schedule.
   */
   if (Scheduler->NextSlotNumber == SCHEDULER_TIME_SYNC_SLOT) {     
      /* TODO - Move to app level Result = SCH_ProcessCommands(); */
   }

   Scheduler->NextSlotNumber++;

   if (Scheduler->NextSlotNumber == SCHTBL_TOTAL_SLOTS) {
       
      Scheduler->NextSlotNumber = 0;
      Scheduler->TablePassCount++;
   }

   Scheduler->SlotsProcessedCount++;

   return(Result);

} /* End ProcessNextSlot() */


/* end of file */
