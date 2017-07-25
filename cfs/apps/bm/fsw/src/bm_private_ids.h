/*=======================================================================================
** File Name:  bm_private_ids.h
**
** Title:  ID Header File for BM Application
**
** $Author:    Ian Wilmoth
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  This header file contains declarations and definitions of BM's private IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-06-16 | Ian Wilmoth | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _BM_PRIVATE_IDS_H_
#define _BM_PRIVATE_IDS_H_

/*
** Pragmas
*/

/*
** Include Files
*/

/*
** Local Defines
*/

/* Event IDs */
#define BM_RESERVED_EID  0

#define BM_INF_EID        1
#define BM_INIT_INF_EID   2
#define BM_ILOAD_INF_EID  3
#define BM_CDS_INF_EID    4
#define BM_CMD_INF_EID    5

#define BM_ERR_EID         51
#define BM_INIT_ERR_EID    52
#define BM_ILOAD_ERR_EID   53
#define BM_CDS_ERR_EID     54
#define BM_CMD_ERR_EID     55
#define BM_PIPE_ERR_EID    56
#define BM_MSGID_ERR_EID   57
#define BM_MSGLEN_ERR_EID  58

#define BM_EVT_CNT  14

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

#endif /* _BM_PRIVATE_IDS_H_ */

/*=======================================================================================
** End of file bm_private_ids.h
**=====================================================================================*/
    