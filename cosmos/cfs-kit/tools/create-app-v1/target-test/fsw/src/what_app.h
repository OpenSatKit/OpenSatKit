/*******************************************************************************
** File: what_app.h
**
** Purpose:
**   This file is main hdr file for the WHAT application.
**
**
*******************************************************************************/

#ifndef _what_app_h_
#define _what_app_h_

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

#define WHAT_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (WHAT_AppMain), these
**       functions are not called from any other source module.
*/
void WHAT_AppMain(void);
void WHAT_AppInit(void);
void WHAT_ProcessCommandPacket(void);
void WHAT_ProcessGroundCommand(void);
void WHAT_ReportHousekeeping(void);
void WHAT_ResetCounters(void);

boolean WHAT_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _what_app_h_ */
