/*******************************************************************************
** File:
**   test1_app_msg.h 
**
** Purpose: 
**  Define TEST1 App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _test1_app_msg_h_
#define _test1_app_msg_h_

/*
** TEST1 App command codes
*/
#define TEST1_APP_NOOP_CC                 0
#define TEST1_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} TEST1_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (TEST1 App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             test1_command_count;
    uint16             test1_command_error_count;

}   OS_PACK test1_hk_tlm_t  ;

#define TEST1_APP_HK_TLM_LNGTH   sizeof ( test1_hk_tlm_t )

#endif /* _test1_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
