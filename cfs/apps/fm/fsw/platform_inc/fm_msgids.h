/*
** $Id: fm_msgids.h 1.2.1.2 2017/01/23 21:53:14EST sstrege Exp  $
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
** Title: File Manager (FM) Message ID Header File
**
** Purpose: Specification for the CFS FM application software bus
**          message identifiers
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_msgids_h_
#define _fm_msgids_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/* FM command packet message ID's */
#define FM_CMD_MID                   0x188C      /** < \brief FM ground commands */
#define FM_SEND_HK_MID               0x188D      /** < \brief FM send housekeeping */

/* FM telemetry packet message ID's */
#define FM_HK_TLM_MID                0x088A      /** < \brief FM housekeeping */
#define FM_FILE_INFO_TLM_MID         0x088B      /** < \brief FM get file info */
#define FM_DIR_LIST_TLM_MID          0x088C      /** < \brief FM get dir list */
#define FM_OPEN_FILES_TLM_MID        0x088D      /** < \brief FM get open files */
#define FM_FREE_SPACE_TLM_MID        0x088E      /** < \brief FM get free space */

#endif /* _fm_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
