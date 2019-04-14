/*************************************************************************
** File:
**  $Id: lc_def_adt.c $
**
** Purpose:
**   Limit Checker (LC) Actionpoint Definition Table (ADT)
**
** Notes:
**   This table is computer generated and is NOT to be hand modified.
**   Generated April 15, 2014, 1:08 pm
**
**
**   $Log: lc_def_adt.c  $
**   2014/04/15 13:19:00EDT acalder
**   GPM FSSE CCR029 Update to AP 82 persistence
**   2014/11/18 M. Lambertson, Columbus Technologies and Services Inc.
**                            CCR-035: Added AP 157 for GMI SMA Undertemperature.
**   2015/05/18 M. Lambertson, Columbus Technologies and Services Inc.
**                            CCR-052: Set AP-082 to "LC_ACTION_NOT_USED".
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
    "LC_ADT", "LC.LC_ADT", "LC actionpoint definition table",
    "lc_def_adt.tbl", (sizeof(LC_ADTEntry_t) * LC_MAX_ACTIONPOINTS)
};
/*
** Actionpoint Definition Table (ADT) data
*/
LC_ADTEntry_t LC_ADT[LC_MAX_ACTIONPOINTS] =
{
    /** DefaultState,   MaxPassiveEvents,   MaxPassFailEvents,   MaxFailPassEvents,   RTSId,   MaxFailsBeforeRTS,   RPNEquation,   EventType,   EventID,   EventText **/

