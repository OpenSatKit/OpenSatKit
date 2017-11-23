/*************************************************************************
 ** File:
 **   $Id: cs_mission_cfg.h 1.4.1.1 2015/03/03 11:57:56EST sstrege Exp  $
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
 **   Specification for the CFS Checksum macro constants that can
 **   be configured from one mission to another
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 ** Notes:
 **
 **   $Log: cs_mission_cfg.h  $
 **   Revision 1.4.1.1 2015/03/03 11:57:56EST sstrege 
 **   Added copyright information
 **   Revision 1.4 2012/07/17 18:27:35EDT sstrege 
 **   Added include for cfe_mission_cfg.h
 **   Revision 1.3 2011/06/28 14:34:00EDT jmdagost 
 **   Added a couple of carriage returns to get rid of the "missing end-of-file" warnings.
 **   Revision 1.2 2008/07/23 10:34:36EDT njyanchik 
 **   Check in of CS Unit test
 **   Revision 1.1 2008/06/13 09:03:11EDT njyanchik 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/mission_inc/project.pj
 ** 
 *************************************************************************/
#ifndef _cs_mission_cfg_
#define _cs_mission_cfg_

#include "cfe_mission_cfg.h"

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

/** \cscfg default CRC algorithm
 **  
 **  \par  Description:
 **        This parameter is the algorithm used by CS to checksum
 **        the requested data.
 **
 **  \par Limits:
 **         This parameter is limited to either #CFE_ES_DEFAULT_CRC,
 **         or  #CFE_ES_CRC_16
 */
#define CS_DEFAULT_ALGORITHM                CFE_ES_DEFAULT_CRC

#endif /*_cs_mission_cfg_*/

/************************/
/*  End of File Comment */
/************************/

