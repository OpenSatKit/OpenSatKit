/*
** Purpose: Define platform configurations for the TFTP application
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL). 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _tftp_platform_cfg_
#define _tftp_platform_cfg_

/*
** Includes
*/

#include "tftp_mission_cfg.h"
#include "tftp_msgids.h"
#include "tftp_perfids.h"

/******************************************************************************
** TFTP Application Macros
*/

#define  TFTP_APP_RUNLOOP_DELAY       500  /* Delay in milliseconds for main loop        */
#define  TFTP_APP_RUNLOOP_PKTS          8  /* Max number of packets to read per run loop */
#define  TFTP_APP_HK_TLM_PERIOD      4000  /* Time between HK pkts in milliseconds       */
#define  TFTP_APP_HK_RUNLOOP_PERIOD (TFTP_APP_HK_TLM_PERIOD/TFTP_APP_RUNLOOP_DELAY)
 

/******************************************************************************
** TFTP Object Macros
**
** - FSW is connectionless server and ground system  is client
**
*/

#define  TFTP_IP_ADDR_STR    "192.168.199.213"  /* cpu talking to */
#define  TFTP_IN_PORT          69
#define  TFTP_RECV_BUFF_LEN  1024

#endif /* _tftp_platform_cfg_ */
