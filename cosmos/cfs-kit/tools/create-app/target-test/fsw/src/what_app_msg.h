/*******************************************************************************
** File:
**   what_app_msg.h 
**
** Purpose: 
**  Define WHAT App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _what_app_msg_h_
#define _what_app_msg_h_

/*
** WHAT App command codes
*/
#define WHAT_APP_NOOP_CC                 0
#define WHAT_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} WHAT_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (WHAT App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             what_command_count;
    uint16             what_command_error_count;

}   OS_PACK what_hk_tlm_t  ;

#define WHAT_APP_HK_TLM_LNGTH   sizeof ( what_hk_tlm_t )

#endif /* _what_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
