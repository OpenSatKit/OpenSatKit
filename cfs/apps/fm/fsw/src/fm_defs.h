/*
** $Id: fm_defs.h 1.2.1.2 2017/01/23 21:53:09EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Macro Definitions File
**
** Purpose: Value definitions
**
** Author: Scott Walling (Microtel)
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_defs_h_
#define _fm_defs_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task semaphore does not exist                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_CHILD_SEM_INVALID        0xFFFFFFFF


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM argument to not calculate CRC during Get File Info command   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_IGNORE_CRC               0


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM directory entry definitions                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_THIS_DIRECTORY           "."
#define FM_PARENT_DIRECTORY         ".."


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM filename status definitions                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_NAME_IS_INVALID          0
#define FM_NAME_IS_NOT_IN_USE       1
#define FM_NAME_IS_FILE_OPEN        2
#define FM_NAME_IS_FILE_CLOSED      3
#define FM_NAME_IS_DIRECTORY        4


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM free space table entry state definitions                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define FM_TABLE_ENTRY_DISABLED     0
#define FM_TABLE_ENTRY_ENABLED      1
#define FM_TABLE_ENTRY_UNUSED       2


#endif /* _fm_defs_h_ */

/************************/
/*  End of File Comment */
/************************/

