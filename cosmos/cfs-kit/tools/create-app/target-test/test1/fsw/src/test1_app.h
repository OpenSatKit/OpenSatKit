/*******************************************************************************
** File: test1_app.h
**
** Purpose:
**   This file is main hdr file for the TEST1 application.
**
**
*******************************************************************************/

#ifndef _test1_app_h_
#define _test1_app_h_

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

#define TEST1_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (TEST1_AppMain), these
**       functions are not called from any other source module.
*/
void TEST1_AppMain(void);
void TEST1_AppInit(void);
void TEST1_ProcessCommandPacket(void);
void TEST1_ProcessGroundCommand(void);
void TEST1_ReportHousekeeping(void);
void TEST1_ResetCounters(void);

boolean TEST1_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _test1_app_h_ */
