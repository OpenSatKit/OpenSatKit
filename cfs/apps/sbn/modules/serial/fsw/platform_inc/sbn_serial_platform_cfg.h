/**
 * @file
 * 
 * This file contains several user-configurable parameters
 * 
 * @author Jaclyn Beck
 * @date 2015/06/24 15:30:00
 */
#ifndef _serial_platform_cfg_h_
#define _serial_platform_cfg_h_

#define SBN_SERIAL_MAX_CHAR_NAME        32  /**< How long the device name can be in the SbnPeerData file */

#define SBN_SERIAL_CHILD_STACK_SIZE     2048 /**< Stack size that each child task gets */

#define SBN_SERIAL_CHILD_TASK_PRIORITY  70  /**< Priority of the child tasks */

#define SBN_SERIAL_PEER_HEARTBEAT 5
#define SBN_SERIAL_PEER_TIMEOUT 10
#endif
