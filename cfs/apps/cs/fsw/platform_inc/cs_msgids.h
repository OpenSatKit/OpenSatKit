/*************************************************************************
 ** File:
 **   $Id: cs_msgids.h 1.2.1.1 2015/03/03 11:58:23EST sstrege Exp  $
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
 **   Specification for the CFS Checksum constants for message IDs
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 ** Notes:
 **
 **   $Log: cs_msgids.h  $
 **   Revision 1.2.1.1 2015/03/03 11:58:23EST sstrege 
 **   Added copyright information
 **   Revision 1.2 2008/10/17 08:35:18EDT njyanchik 
 **   Changed the telemtry ID for HK to the correct value
 **   Revision 1.1 2008/07/23 15:26:48BST njyanchik 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/platform_inc/project.pj

 ** 
 *************************************************************************/
#ifndef _cs_msgids_
#define _cs_msgids_

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

/**
 ** \name CS Command Message IDs */ 
/** \{ */
#define CS_CMD_MID                          (0x189F)      /**< \brief Msg ID for cmds to CS   */
#define CS_SEND_HK_MID                      (0x18A0)      /**< \brief Msg ID to request CS HK */
#define CS_BACKGROUND_CYCLE_MID             (0x18A1)      /**< \brief Msg ID for background checking */
/** \} */

/**
 ** \name CS Telemetery Message Number */ 
/** \{ */
#define CS_HK_TLM_MID                       (0x08A4)
/** \} */

#endif /*_cs_msgids_*/

/************************/
/*  End of File Comment */
/************************/

