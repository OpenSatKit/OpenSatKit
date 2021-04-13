/*
** Purpose: Define platform configurations for the OpenSat Kit Scheduler application
**
** Notes:
**   None
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

#ifndef _kit_sch_platform_cfg_
#define _kit_sch_platform_cfg_

/*
** Includes
*/

#include "kit_sch_mission_cfg.h"
#include "kit_sch_msgids.h"
#include "kit_sch_perfids.h"

/******************************************************************************
** Platform Deployment Configurations
*/

#define KIT_SCH_PLATFORM_REV   0
#define KIT_SCH_INI_FILENAME   "/cf/kit_sch_ini.json"


/******************************************************************************
** Scheduler Application Macros
*/

#define  KIT_SCH_STARTUP_SYNCH_TIMEOUT  10000  /* Timeout in milliseconds */

#define  KIT_SCH_DEF_MSG_TBL_FILENAME   "/cf/osk_sch_msg_tbl.json"
#define  KIT_SCH_DEF_SCH_TBL_FILENAME   "/cf/osk_sch_sch_tbl.json"

/******************************************************************************
** Scheduler Table Configurations
*/


/*
** Number of minor frame slots within each Major Frame. Must be 2 or more and less than 65536.
*/
#define SCHTBL_SLOTS      4


/*
** Maximum number of Activities per Minor Frame. Must be greater than zero.
*/
#define SCHTBL_ACTIVITIES_PER_SLOT  15


#define SCHTBL_MAX_ENTRIES (SCHTBL_SLOTS * SCHTBL_ACTIVITIES_PER_SLOT)


/******************************************************************************
** Message Table Configurations
*/

/*
** Maximum Number of Message Definitions in Message Definition Table. Must be 
** greater than zero.
*/

#define MSGTBL_MAX_ENTRIES      200
#define MSGTBL_UNUSED_MSG_ID    (CFE_SB_HIGHEST_VALID_MSGID+1)

/*
** Max message length in words.  Must be at least large enough to hold the
** smallest possible message header(see #CFE_SB_TLM_HDR_SIZE and 
** #CFE_SB_CMD_HDR_SIZE)
*/
#define MSGTBL_MAX_MSG_WORDS      8
#define MSGTBL_MAX_MSG_BYTES      (MSGTBL_MAX_MSG_WORDS*2)


/******************************************************************************
** Scheduler Configurations
*/

/*
** Number of Minor Frames that will be processed in "Catch Up"
** mode before giving up and skipping ahead.
*/
#define SCHEDULER_MAX_LAG_COUNT  (SCHTBL_SLOTS / 2)


/*
** Maximum number of slots scheduler will process when trying to
** "Catch Up" to the correct slot for the current time. Must be greater than zero.
*/
#define SCHEDULER_MAX_SLOTS_PER_WAKEUP      5


/*
** Number of microseconds in a Major Frame. Used as a "wake-up" period. Must be greater than zero.
*/
#define SCHEDULER_MICROS_PER_MAJOR_FRAME    1000000


/*
** Defines the additional time allowed in the Synchronization Slot to allow
** the Major Frame Sync signal to be received and re-synchronize processing.
** Must be less than the normal slot period.
*/
#define SCHEDULER_SYNC_SLOT_DRIFT_WINDOW   5000


/*
** Defines the timeout for the #CFE_ES_WaitForStartupSync call that scheduler
** uses to wait for all of the Applications specified in the startup script to
** finish initialization. The scheduler will wait this amount of time before
** assuming all startup script applications have been started and will then
** begin nominal schedule processing.
*/
#define SCHEDULER_STARTUP_SYNC_TIMEOUT   10000


/*
** Defines the time allowed for the first Major Frame sync signal to arrive
** before assuming it is not going to occur and switching to a free-wheeling
** mode. Must be greater than or equal to the Major Frame Period
*/
#define SCHEDULER_STARTUP_PERIOD   (5*SCHEDULER_MICROS_PER_MAJOR_FRAME)


/*
** Defines the number of consecutive "Noisy" Major Frame Signals (i.e. -
** signals that occur outside the expected window of their occurrence)
** until the Major Frame signal is automatically ignored and the Minor
** Frame Timer is used instead. This value should never be set to less
** than two because a single "noisy" Major Frame signal is likely when
** turning on or switching the 1 Hz signal on the spacecraft.
*/
#define SCHEDULER_MAX_NOISY_MF   2


#endif /* _kit_sch_platform_cfg_ */
