/*
** $Id: $
**
** Title: CFS Heater Control (HC) Application Command and Telemetry
**        Packet Header File
**
** Purpose: Specification for the CFS HC command and telemetry messages.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $ Changed HC_SendCtrlCmd_t to include index for Component and Heater
**	   Changed HC_HousekeepingPkt_t to include average temperatures for 
**	   each heater
**	   Changed HC_HeaterControlPkt_t to reflect changes to how HSIM sends data
**	   Changed HC_Type_t to accept floats instead of integers for temperature
**	   Changed HC_Component_t to reflect changes to ComponentTable
**	   cgesterling (18.Nov.2016)
**
*/

#ifndef _hc_msg_h_
#define _hc_msg_h_

#include "cfe.h"
#include "hc_platform_cfg.h"
#include "hc_tbl.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- command packet structures                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
**  \brief Housekeeping Request command packet structure
*/

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} HC_HousekeepingCmd_t;


/*
**  \brief No-Operation command packet structure
**
**  For command details see #HC_NOOP_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} HC_NoopCmd_t;


/*
**  \brief Reset Counters command packet structure
**
**  For command details see #HC_RESET_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} HC_ResetCmd_t;


/*
**  \brief Set Status command packet structure
**
**  For command details see #HC_SET_STATUS_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

    uint8   HCType;                         /**< \brief Heater control type */

    uint8   HCStatus;                       /**< \brief New status for the heater control type */

} HC_SetStatusCmd_t;


/*
**  \brief Perform heater control command packet structure
**
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} HC_PerformHC_t;


/*
**  \brief Send heater control command packet structure 
**
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

    uint8   ComponentIndex;		    /**< \brief Component to be controlled */

    uint8   HeaterIndex;                    /**< \brief Heater to be controlled */

    boolean TurnOnFlag;                     /**< \brief Turn heater on or off */

} HC_SendCtrlCmd_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- housekeeping telemetry structure                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
** \hctlm Housekeeping telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   CommandCounter;		    /**< \hctlmmnemonic \HC_CMDPC
                                                 \brief Application command counter */
    uint8   CommandErrCounter;	            /**< \hctlmmnemonic \HC_CMDEC
                                                 \brief Application command error counter */  
    uint8   ThermalPktCount;                /**< \brief Number of thermal houskeeping packets received */

    uint8   CycleCounter;                   /**< \brief Heater control cycle counter */

    boolean TlmStale;                       /**< \brief Thermal telemetry stale flag */

    uint8   Status[HC_NUM_CTRL_TYPES];      /**< \brief Heater control status */

    float   AvgTemps[HC_MAX_COMPONENTS][HC_MAX_NUM_HEATERS];

} HC_HousekeepingPkt_t;

/**
** \hctlm Heater Control telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; 	/**< \brief cFE SB tlm hdr */

    boolean ErrorFlag;                      	/**< \brief Stale data flag */

    float   Temps[HC_MAX_COMPONENTS][HC_MAX_NUM_HEATERS][HC_MAX_NUM_THERM];	
						/**< \brief Matrix of temperature data */

} HC_HeaterControlPkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- table structures                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
** \brief Heater Control Type table entry definition
*/
typedef struct
{
    uint8   Enabled;                              /**< \brief Enable/disable flag */

    uint8   Type;                                 /**< \brief Heater control type */

    float   TempHigh;                             /**< \brief High temperature set point */

    float   TempLow;                              /**< \brief Low temperature set point */

    uint8   Index;                                /**< \brief Function index */

} HC_Type_t;

/**
**  \brief Heater Control Type table definition
**/
typedef struct
{
    HC_Type_t Types[HC_NUM_CTRL_TYPES];           /**< \brief One entry for each heater control type */
 
} HC_TypeTable_t;

/**
** \brief Components table entry definition
**/
typedef struct
{
    uint8  Type;                                		/**< \brief Heater control type */

    uint8  NumHeaters;                            		/**< \brief Number of heaters */

    uint8  Heaters[HC_MAX_NUM_HEATERS];           		/**< \brief Array of heaters */
    
} HC_Component_t;


/**
**  \brief Components table definition
**/
typedef struct
{
    HC_Component_t Components[HC_MAX_COMPONENTS]; /**< \brief One entry for each component */

} HC_ComponentTable_t;

#endif /* _hc_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
