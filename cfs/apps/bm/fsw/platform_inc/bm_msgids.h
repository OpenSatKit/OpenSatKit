/*=======================================================================================
** File Name:  bm_msgids.h
**
** Title:  Message ID Header File for BM Application
**
** $Author:    Ian Wilmoth
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  This header file contains declartions and definitions of all BM's 
**           Message IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-06-16 | Ian Wilmoth | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _BM_MSGIDS_H_
#define _BM_MSGIDS_H_

/***** TODO:  These Message ID values are default and may need to be changed by the developer  *****/
#define BM_CMD_MID            	 0x19D2
#define BM_SEND_HK_MID        	 0x19D3
#define BM_WAKEUP_MID        	 0x19D4
#define BM_OUT_DATA_MID          0x19D5
#define BM_CREATE_CHILD_TASK_MID 0x19D6
#define BM_UNLOCK_PARAMS_MID	 0x19D7

#define BM_HK_TLM_MID		0x09CA
#define BM_TST_MID			0x09CB
#define BM_CHLD_MID			0x09CC

    


#endif /* _BM_MSGIDS_H_ */

/*=======================================================================================
** End of file bm_msgids.h
**=====================================================================================*/
    