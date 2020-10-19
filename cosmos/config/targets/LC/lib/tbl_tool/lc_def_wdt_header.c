/*************************************************************************
** File: 
**  $Id: lc_def_wdt.c 1.19 2013/11/07 17:55:27EST sstrege Exp  $
** 
** Purpose: 
**   Limit Checker (LC) Watchpoint Definition Table (WDT)
** 
** Notes: 
**   This table is computer generated and is NOT to be hand modified. 
**   Generated November 7, 2013, 10:43 am 
** 
** 
**   $Log: lc_def_wdt.c  $ 
**   Revision 1.19 2013/11/07 17:55:27EST sstrege  
**   GPM-CCR-1133 Updates
**   2014/11/18 M. Lambertson, Columbus Technologies and Services Inc.
**                            CCR-035: Added WPs 194-196 for GMI SMA Undertemperature.
**   2015/09/29 M. Lambertson CCR-057: Modified WP 82.  
** 
*/ 
/************************************************************************* 
** Includes 
*************************************************************************/ 
#include "lc_msg.h"    
#include "lc_tbl.h"   
#include "cfe_tbl_filedef.h"  
 
/************************************************************************* 
** Exported Data 
*************************************************************************/ 
/* 
** Table file header 
*/ 
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) = 
{
    "LC_WDT", "LC.LC_WDT", "LC watchpoint definition table", 
    "lc_def_wdt.tbl", (sizeof(LC_WDTEntry_t) * LC_MAX_WATCHPOINTS) 
}; 
/* 
** Watchpoint Definition Table (WDT) data
*/ 
LC_WDTEntry_t LC_WDT[LC_MAX_WATCHPOINTS] =
{ 
    /** DataType,   OperatorID,   MessageID,   WatchpointOffset,   BitMask,   ComparisonValue,   CustomFuncArgument **/ 

