/*=======================================================================================
** File Name:  fm_private_ids.h
**
** Title:  ID Header File for FM Application
**
** $Author:    gsfc
** $Revision: 1.1 $
** $Date:      2016-09-12
**
** Purpose:  This header file contains declarations and definitions of FM's private IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-09-12 | gsfc | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _FM_PRIVATE_IDS_H_
#define _FM_PRIVATE_IDS_H_

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
#define FM_RESERVED_EID  0

#define FM_INF_EID        1
#define FM_INIT_INF_EID   2
#define FM_ILOAD_INF_EID  3
#define FM_CDS_INF_EID    4
#define FM_CMD_INF_EID    5

#define FM_ERR_EID         51
#define FM_INIT_ERR_EID    52
#define FM_ILOAD_ERR_EID   53
#define FM_CDS_ERR_EID     54
#define FM_CMD_ERR_EID     55
#define FM_PIPE_ERR_EID    56
#define FM_MSGID_ERR_EID   57
#define FM_MSGLEN_ERR_EID  58

#define FM_EVT_CNT  14

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

#endif /* _FM_PRIVATE_IDS_H_ */

/*=======================================================================================
** End of file fm_private_ids.h
**=====================================================================================*/
    