/*******************************************************************************
** File:
**   tt_app_msg.h 
**
** Purpose: 
**  Define TT App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _tt_app_msg_h_
#define _tt_app_msg_h_

/*
** TT App command codes
*/
#define TT_APP_NOOP_CC                 0
#define TT_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} TT_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (TT App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             tt_command_count;
    uint16             tt_command_error_count;

}   OS_PACK tt_hk_tlm_t  ;

#define TT_APP_HK_TLM_LNGTH   sizeof ( tt_hk_tlm_t )

#endif /* _tt_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
