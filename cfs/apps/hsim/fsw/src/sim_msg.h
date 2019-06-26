/*
** $Id: $
**
** Title: CFS Simulator (SIM) Application Command and Telemetry
**        Packet Header File
**
** Purpose: Specification for the CFS SIM command and telemetry messages.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $Defined SIM_MisbehaveCmd_t to be able to accept a Misbehave command
**	  Changed SIM_HeaterControlPkt_t to store temperatures as floats in
**	  a matrix
**	  Changed SIM_CmdPkt_t to accept an index for which component heater 
**	  is on
**	  cgesterling (22.Nov.2016)
**
*/

#ifndef _sim_msg_h_
#define _sim_msg_h_

#include "cfe.h"
#include "sim_platform_cfg.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM -- command packet structures                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
**  \brief Housekeeping Request command packet structure
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} SIM_HousekeepingCmd_t;


/*
**  \brief No-Operation command packet structure
**
**  For command details see #SIM_NOOP_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} SIM_NoopCmd_t;


/*
**  \brief Reset Counters command packet structure
**
**  For command details see #SIM_RESET_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} SIM_ResetCmd_t;

/*
**  \brief Heater Misbehave command packet structure
**
**  For command details see #SIM_HEATER_MISBEHAVE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

    boolean Misbehave;

} SIM_MisbehaveCmd_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM -- housekeeping telemetry structure                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
** \simtlm Housekeeping telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   CommandCounter;		    /**< \simtlmmnemonic \SIM_CMDPC
                                                 \brief Application command counter */
    uint8   CommandErrCounter;	            /**< \simtlmmnemonic \SIM_CMDEC
                                                 \brief Application command error counter */
    boolean Heaters[SIM_NUM_COMPONENTS][SIM_MAX_NUM_HEATERS];
				             /**< \brief Matrix of heater statuses */
                                          
} SIM_HousekeepingPkt_t;


/**
** \simtlm Heater Control telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    boolean StaleFlag;                      /**< \brief Stale data flag */

    float    Temps[SIM_NUM_COMPONENTS][SIM_MAX_NUM_HEATERS][SIM_MAX_NUM_THERMS];
				            /**< \brief Matrix of temperature data */

} SIM_HeaterControlPkt_t;


/**
** Command from Heater Control packet
**/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   Component;                      /**< \brief Component to be controlled */

    uint8   Heater;                         /**< \brief Heater to be controlled */

    boolean TurnOn;                         /**< \brief Turn on or off */ 
 
} SIM_CmdPkt_t;

#endif /* _sim_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
