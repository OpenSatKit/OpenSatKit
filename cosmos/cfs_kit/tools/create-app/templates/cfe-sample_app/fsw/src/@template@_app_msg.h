/*******************************************************************************
** File:
**   @template@_app_msg.h 
**
** Purpose: 
**  Define @TEMPLATE@ App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _@template@_app_msg_h_
#define _@template@_app_msg_h_

/*
** @TEMPLATE@ App command codes
*/
#define @TEMPLATE@_APP_NOOP_CC                 0
#define @TEMPLATE@_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} @TEMPLATE@_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (@TEMPLATE@ App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             @template@_command_count;
    uint16             @template@_command_error_count;

}   OS_PACK @template@_hk_tlm_t  ;

#define @TEMPLATE@_HK_TLM_LNGTH   sizeof ( @template@_hk_tlm_t )

#endif /* _@template@_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
