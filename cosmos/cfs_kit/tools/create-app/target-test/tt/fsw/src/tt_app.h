/*******************************************************************************
** File: tt_app.h
**
** Purpose:
**   This file is main hdr file for the TT application.
**
**
*******************************************************************************/

#ifndef _tt_app_h_
#define _tt_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define TT_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (TT_AppMain), these
**       functions are not called from any other source module.
*/
void TT_AppMain(void);
void TT_AppInit(void);
void TT_ProcessCommandPacket(void);
void TT_ProcessGroundCommand(void);
void TT_ReportHousekeeping(void);
void TT_ResetCounters(void);

boolean TT_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _tt_app_h_ */
