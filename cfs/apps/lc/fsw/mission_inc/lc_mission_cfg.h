/*************************************************************************
** File:
**   $Id: lc_mission_cfg.h 1.3 2017/01/22 17:24:26EST sstrege Exp  $
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
** Purpose: 
**   CFS Limit Checker (LC) Application Mission Configuration Header File
**
** Notes:
**
** 
*************************************************************************/
#ifndef _lc_mission_cfg_
#define _lc_mission_cfg_

/** \lccfg RTS Request Message ID
**  
**  \par Description:
**       Message ID that will be used by the /LC_ExecuteRTS function
**       to construct the RTS request message.
**        
**       We define this here to allow the LC application to be built 
**       without including headers from the application (e.g. SC) that
**       will receive the command.  Obviously, this definition must
**       match the message ID expectyed by the receiving application.
**         
**       A mission may choose to modify the /LC_ExecuteRTS function not 
**       to use this or define it using a message ID constant that is 
**       defined numerically elsewhere.
**
**  \par Limits:
**       This parameter shouldn't be larger than the value set for 
**       #CFE_SB_HIGHEST_VALID_MSGID.
*/
#define LC_RTS_REQ_MID                    0x18A9

/** \lccfg RTS Request Command Code
**  
**  \par Description:
**       Command code that will be used by the /LC_ExecuteRTS function
**       to construct the RTS request message.
**        
**       We define this here to allow the LC application to be built 
**       without including headers from the application (e.g. SC) that
**       will receive the command.  Obviously, this definition must
**       match the command code expected by the receiving application.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 16 bit
**       integer (65535).  Of course we all know that the preceding
**       limit describes the size of the variable returned by the
**       API function #CFE_SB_GetCmdCode.  And that for as long as CFE
**       uses the existing CCSDS packet format implementation, command
**       codes are only 7 bits (unsigned) with an upper limit of 127
**       and a lower limit of zero.
*/
#define LC_RTS_REQ_CC                     4

#endif /*_lc_mission_cfg_*/

/************************/
/*  End of File Comment */
/************************/
