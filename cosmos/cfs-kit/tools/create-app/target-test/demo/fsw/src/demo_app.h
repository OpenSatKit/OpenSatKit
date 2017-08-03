/*******************************************************************************
** File: demo_app.h
**
** Purpose:
**   This file is main hdr file for the DEMO application.
**
**
*******************************************************************************/

#ifndef _demo_app_h_
#define _demo_app_h_

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

#define DEMO_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (DEMO_AppMain), these
**       functions are not called from any other source module.
*/
void DEMO_AppMain(void);
void DEMO_AppInit(void);
void DEMO_ProcessCommandPacket(void);
void DEMO_ProcessGroundCommand(void);
void DEMO_ReportHousekeeping(void);
void DEMO_ResetCounters(void);

boolean DEMO_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _demo_app_h_ */
