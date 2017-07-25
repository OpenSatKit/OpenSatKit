/*******************************************************************************
** File:
**   demo_app_msg.h 
**
** Purpose: 
**  Define DEMO App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _demo_app_msg_h_
#define _demo_app_msg_h_

/*
** DEMO App command codes
*/
#define DEMO_APP_NOOP_CC                 0
#define DEMO_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} DEMO_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (DEMO App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             demo_command_count;
    uint16             demo_command_error_count;

}   OS_PACK demo_hk_tlm_t  ;

#define DEMO_APP_HK_TLM_LNGTH   sizeof ( demo_hk_tlm_t )

#endif /* _demo_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
