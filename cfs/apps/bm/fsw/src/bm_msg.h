/*=======================================================================================
** File Name:  bm_msg.h
**
** Title:  Message Definition Header File for BM Application
**
** $Author:    Ian Wilmoth
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  To define BM's command and telemetry message defintions 
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-06-16 | Ian Wilmoth | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _BM_MSG_H_
#define _BM_MSG_H_

/*
** Pragmas
*/

/*
** Include Files
*/



/*
** Local Defines
*/

/*
** BM command codes
*/
#define BM_NOOP_CC                 0
#define BM_RESET_CC                1
#define BM_RUNALL_CC               2
#define BM_SETPARAMS_CC		   	   3

/*
** Local Structure Declarations
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              usCmdCnt;
    uint8              usCmdErrCnt;
    uint8              spare[2];
    uint32 BM_RunningBenchmarks;
	BM_Params_t		   params;
	BM_Results_t	   results;
    /* TODO:  Add declarations for additional housekeeping data here */

} BM_HkTlm_t;


#endif /* _BM_MSG_H_ */

/*=======================================================================================
** End of file bm_msg.h
**=====================================================================================*/
    