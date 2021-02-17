/*
** Purpose: Define configurations for the Trivial File Transfer Protocol (TFTP) application
**
** Notes:
**   1. These macros can only be build with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "tftp_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** TFTP Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release
** 1.1 - Refactored for OSK 2.2
*/

#define  TFTP_MAJOR_VER   1
#define  TFTP_MINOR_VER   1


#define  TFTP_CMD_PIPE_DEPTH    10
#define  TFTP_CMD_PIPE_NAME     "TFTP_CMD_PIPE"

/******************************************************************************
** Command Macros
*/

#define TFTP_GET_FILE_CMD_FC       (CMDMGR_APP_START_FC + 0)
#define TFTP_PUT_FILE_CMD_FC       (CMDMGR_APP_START_FC + 1)

#define NETIF_INIT_SOCKET_CMD_FC   (CMDMGR_APP_START_FC + 2)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define TO_UDP_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define PKTMGR_BASE_EID  (OSK_C_FW_APP_BASE_EID + 10)

#define TFTP_APP_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define NETIF_BASE_EID     (OSK_C_FW_APP_BASE_EID + 10)
#define TFTP_BASE_EID      (OSK_C_FW_APP_BASE_EID + 20)


#endif /* _app_cfg_ */
