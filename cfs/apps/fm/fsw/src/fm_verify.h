/*
** $Id: fm_verify.h 1.6.1.2 2017/01/23 21:52:54EST sstrege Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: File Manager (FM) Configuration Parameter Verification Header File
**
** Purpose:
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_verify_h_
#define _fm_verify_h_

/* Need definition of OS_MAX_NUM_OPEN_FILES */
#include "cfe.h"

#include "fm_platform_cfg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - application definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Application name */
#ifndef FM_APP_NAME
    #error FM_APP_NAME must be defined!
#endif


/* Input command pipe */
#ifndef FM_APP_PIPE_NAME
    #error FM_APP_PIPE_NAME must be defined!
#endif


/* Mission specific portion of application revision number */
#ifndef FM_MISSION_REV
    #error FM_MISSION_REV must be defined!
#elif (FM_MISSION_REV < 0)
    #error FM_MISSION_REV must be greater than or equal to zero!
#endif 


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - output file definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Default directory listing output filename */
#ifndef FM_DIR_LIST_FILE_DEFNAME
    #error FM_DIR_LIST_FILE_DEFNAME must be defined!
#endif


/* Max number of entries written to a directory listing file */
#ifndef FM_DIR_LIST_FILE_ENTRIES
    #error FM_DIR_LIST_FILE_ENTRIES must be defined!
#elif FM_DIR_LIST_FILE_ENTRIES < 100
    #error FM_DIR_LIST_FILE_ENTRIES cannot be less than 100
#elif FM_DIR_LIST_FILE_ENTRIES > 10000
    #error FM_DIR_LIST_FILE_ENTRIES cannot be greater than 10000 files
#endif


/* cFE file header sub-type for directory list files */
#ifndef FM_DIR_LIST_FILE_SUBTYPE
    #error FM_DIR_LIST_FILE_SUBTYPE must be defined!
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - TLM packet definitions   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Number of entries in get dir list telemetry packet */
#ifndef FM_DIR_LIST_PKT_ENTRIES
    #error FM_DIR_LIST_PKT_ENTRIES must be defined!
#elif FM_DIR_LIST_PKT_ENTRIES < 10
    #error FM_DIR_LIST_PKT_ENTRIES cannot be less than 10
#elif FM_DIR_LIST_PKT_ENTRIES > 100
    #error FM_DIR_LIST_PKT_ENTRIES cannot be greater than 100
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - child task definitions   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Size of each child task file data read or write */
#ifndef FM_CHILD_FILE_BLOCK_SIZE
    #error FM_CHILD_FILE_BLOCK_SIZE must be defined!
#elif FM_CHILD_FILE_BLOCK_SIZE < 256
    #error FM_CHILD_FILE_BLOCK_SIZE cannot be less than 256
#elif FM_CHILD_FILE_BLOCK_SIZE > 32768
    #error FM_CHILD_FILE_BLOCK_SIZE cannot be greater than 32K
#endif

/* Number of file blocks between task delays */
#ifndef FM_CHILD_FILE_LOOP_COUNT
    #error FM_CHILD_FILE_LOOP_COUNT must be defined!
#elif FM_CHILD_FILE_LOOP_COUNT < 1
    #error FM_CHILD_FILE_LOOP_COUNT cannot be less than 1
#endif


/* Length of time for each task delay */
#ifndef FM_CHILD_FILE_SLEEP_MS
    #error FM_CHILD_FILE_SLEEP_MS must be defined!
#elif FM_CHILD_FILE_SLEEP_MS < 0
    #error FM_CHILD_FILE_SLEEP_MS cannot be less than zero
#elif FM_CHILD_FILE_SLEEP_MS > 100
    #error FM_CHILD_FILE_SLEEP_MS cannot be greater than 100
#endif


/* Number of entries in the child task command queue */
#ifndef FM_CHILD_QUEUE_DEPTH
    #error FM_CHILD_QUEUE_DEPTH must be defined!
#elif FM_CHILD_QUEUE_DEPTH < 1
    #error FM_CHILD_QUEUE_DEPTH cannot be less than 1
#elif FM_CHILD_QUEUE_DEPTH > 10
    #error FM_CHILD_QUEUE_DEPTH cannot be greater than 10
#endif


/* Child task name */
#ifndef FM_CHILD_TASK_NAME
    #error FM_CHILD_TASK_NAME must be defined!
#endif


/* Child task stack size */
#ifndef FM_CHILD_TASK_STACK_SIZE
    #error FM_CHILD_TASK_STACK_SIZE must be defined!
#elif FM_CHILD_TASK_STACK_SIZE < 2048
    #error FM_CHILD_TASK_STACK_SIZE cannot be less than 2048
#elif FM_CHILD_TASK_STACK_SIZE > 20480
    #error FM_CHILD_TASK_STACK_SIZE cannot be greater than 20480
#endif


/* Child task priority */
#ifndef FM_CHILD_TASK_PRIORITY
    #error FM_CHILD_TASK_PRIORITY must be defined!
#elif FM_CHILD_TASK_PRIORITY < 1
    #error FM_CHILD_TASK_PRIORITY must be greater than 0
#elif FM_CHILD_TASK_PRIORITY > 255
    #error FM_CHILD_TASK_PRIORITY cannot be greater than 255
#endif


/* Child task semaphore name */
#ifndef FM_CHILD_SEM_NAME
    #error FM_CHILD_SEM_NAME must be defined!
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - table definitions        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* cFE object name for free space table - not filename */
#ifndef FM_TABLE_CFE_NAME
    #error FM_TABLE_CFE_NAME must be defined!
#endif


/* Free space table filename - with path */
#ifndef FM_TABLE_DEF_NAME
    #error FM_TABLE_DEF_NAME must be defined!
#endif


/* Free space table filename - without path */
#ifndef FM_TABLE_FILENAME
    #error FM_TABLE_FILENAME must be defined!
#endif


/* Default description text for free space table */
#ifndef FM_TABLE_DEF_DESC
    #error FM_TABLE_DEF_DESC must be defined!
#endif


/* Number of free space table entries */
#ifndef FM_TABLE_ENTRY_COUNT
    #error FM_TABLE_ENTRY_COUNT must be defined!
#elif FM_TABLE_ENTRY_COUNT < 1
    #error FM_TABLE_ENTRY_COUNT cannot be less than 1
#elif FM_TABLE_ENTRY_COUNT > 32
    #error FM_TABLE_ENTRY_COUNT cannot be greater than 32
#endif


/* Table validation error code */
#ifndef FM_TABLE_VALIDATION_ERR
    #error FM_TABLE_VALIDATION_ERR must be defined!
#endif


#endif /* _fm_verify_h_ */

/************************/
/*  End of File Comment */
/************************/
