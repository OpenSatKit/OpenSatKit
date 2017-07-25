/*******************************************************************************
** File: @template@_app.h
**
** Purpose:
**   This file is main hdr file for the @TEMPLATE@ application.
**
**
*******************************************************************************/

#ifndef _@template@_app_h_
#define _@template@_app_h_

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

#define @TEMPLATE@_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (@TEMPLATE@_AppMain), these
**       functions are not called from any other source module.
*/
void @TEMPLATE@_AppMain(void);
void @TEMPLATE@_AppInit(void);
void @TEMPLATE@_ProcessCommandPacket(void);
void @TEMPLATE@_ProcessGroundCommand(void);
void @TEMPLATE@_ReportHousekeeping(void);
void @TEMPLATE@_ResetCounters(void);

boolean @TEMPLATE@_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _@template@_app_h_ */
