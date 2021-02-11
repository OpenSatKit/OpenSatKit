/* 
** Purpose: Manage a UDP uplink command interface.
**
** Notes:
**   None
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

/*
** Include Files:
*/

#include <errno.h>
#include <string.h>
#include <unistd.h>  /* Needed for close() */


#include "uplink.h"

/*
** Global File Data
*/

static UPLINK_Class*  Uplink = NULL;

/*
** Local Function Prototypes
*/

static void DestructorCallback(void);
static void ProcessMsgTunnelMap(void);

/******************************************************************************
** Function: UPLINK_ConfigMsgTunnelCmd
**
*/
boolean UPLINK_ConfigMsgTunnelCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const  UPLINK_ConfigMsgTunnelCmdMsg *ConfigMsgTunnel = (const UPLINK_ConfigMsgTunnelCmdMsg *) MsgPtr;

   boolean RetStatus = TRUE;
   int     Index     = ConfigMsgTunnel->Index;

   if (Index < UPLINK_MSG_TUNNEL_CNT) {

      if (ConfigMsgTunnel->Enabled == TRUE) {

         Uplink->MsgTunnel.Map[Index].Enabled = TRUE;
         Uplink->MsgTunnel.Map[Index].OrgMsgId = ConfigMsgTunnel->OrgMsgId;
         Uplink->MsgTunnel.Map[Index].NewMsgId = ConfigMsgTunnel->NewMsgId;
         Uplink->MsgTunnel.Enabled = TRUE;

         CFE_EVS_SendEvent(UPLINK_CONFIG_MSG_TUNNEL_ENA_INFO_EID, CFE_EVS_INFORMATION,
                           "Enabled message tunnel map at index %d. Message 0x%04X mapped to 0x%04X",
                           Index, Uplink->MsgTunnel.Map[Index].OrgMsgId, Uplink->MsgTunnel.Map[Index].NewMsgId);

      } /* End if enabling */
      else {

         Uplink->MsgTunnel.Map[Index].Enabled = FALSE;
         CFE_EVS_SendEvent(UPLINK_CONFIG_MSG_TUNNEL_DIS_INFO_EID, CFE_EVS_INFORMATION,
                           "Disabled message tunnel map at index %d. Message 0x%04X was mapped to 0x%04X",
                           Index, Uplink->MsgTunnel.Map[Index].OrgMsgId, Uplink->MsgTunnel.Map[Index].NewMsgId);
         Uplink->MsgTunnel.Map[Index].OrgMsgId = UPLINK_UNUSED_MSG_ID;
         Uplink->MsgTunnel.Map[Index].NewMsgId = UPLINK_UNUSED_MSG_ID;

         /* Set global flag to true if any mappings are enabled */
         for (Index=0; Index < UPLINK_MSG_TUNNEL_CNT; Index++)
      	   Uplink->MsgTunnel.Enabled |= (Uplink->MsgTunnel.Map[Index].Enabled == TRUE);


      } /* End if disabling */

   } /* End if valid index */
   else {
	   
      RetStatus = FALSE;
       
      CFE_EVS_SendEvent(UPLINK_CONFIG_MSG_TUNNEL_INDEX_ERR_EID, CFE_EVS_ERROR,
                        "Configure message tunnel index %d exceeds maximum index of %d",
                        Index, (UPLINK_MSG_TUNNEL_CNT-1));

   } /* End if invalid index */

   return RetStatus;

} /* End of UPLINK_ConfigMsgTunnelCmd() */


/******************************************************************************
** Function: UPLINK_Constructor
**
*/
void UPLINK_Constructor(UPLINK_Class*  UplinkPtr, uint16 Port)
{
 
   int i;
   Uplink = UplinkPtr;

   Uplink->Connected     = FALSE;
   Uplink->RecvMsgCnt    = 0;
   Uplink->RecvMsgErrCnt = 0;
   Uplink->RecvMsgPtr = (CFE_SB_MsgPtr_t) &(Uplink->RecvBuff[0]);

   Uplink->MsgTunnel.Enabled = FALSE;
   for (i=0; i < UPLINK_MSG_TUNNEL_CNT; i++) {
      
      Uplink->MsgTunnel.Map[i].Enabled  = FALSE;
      Uplink->MsgTunnel.Map[i].OrgMsgId = UPLINK_UNUSED_MSG_ID;
      Uplink->MsgTunnel.Map[i].NewMsgId = UPLINK_UNUSED_MSG_ID;
   }
   Uplink->MsgTunnel.MappingsPerformed    = 0;
   Uplink->MsgTunnel.LastMapping.Index    = UPLINK_MSG_TUNNEL_CNT;
   Uplink->MsgTunnel.LastMapping.OrgMsgId = UPLINK_UNUSED_MSG_ID;
   Uplink->MsgTunnel.LastMapping.NewMsgId = UPLINK_UNUSED_MSG_ID;

   CFE_PSP_MemSet(&(Uplink->SocketAddress), 0, sizeof(Uplink->SocketAddress));

   if ( (Uplink->SocketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {

      Uplink->SocketAddress.sin_family      = AF_INET;
      Uplink->SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
      Uplink->SocketAddress.sin_port        = htons(Port);

      if ( (bind(Uplink->SocketId, (struct sockaddr *) &(Uplink->SocketAddress), sizeof(Uplink->SocketAddress)) >= 0) ) {

    	   Uplink->Connected = TRUE;
         /* Set the socket to non-blocking. Not available in vxWorks, so conditionally compiled. */
         #ifdef _HAVE_FCNTL_
            fcntl(Uplink->SocketId, F_SETFL, O_NONBLOCK);
         #endif

      }/* End if successful bind */
      else {
         
         CFE_EVS_SendEvent(UPLINK_SOCKET_BIND_ERR_EID,CFE_EVS_ERROR,
                           "Socket bind failed. Status = %d", errno);
      }

   } /* End if successful socket creation */
   else {
      
      CFE_EVS_SendEvent(UPLINK_SOCKET_CREATE_ERR_EID, CFE_EVS_ERROR, "Socket creation failed. Status = %d", errno);
   
   }


   OS_TaskInstallDeleteHandler((void *)(&DestructorCallback)); /* Call when application terminates */

} /* End UPLINK_Constructor() */


/******************************************************************************
** Function: UPLINK_Read
**
*/
int UPLINK_Read(uint16 MaxMsgRead)
{

	unsigned int AddrLen = sizeof(Uplink->SocketAddress);
   int i = 0;
   int Status;

   if (Uplink->Connected == FALSE) return i;

   CFE_PSP_MemSet(&(Uplink->SocketAddress), 0, sizeof(Uplink->SocketAddress));

   for (i = 0; i < MaxMsgRead; i++) {

      Status = recvfrom(Uplink->SocketId, (char *)&(Uplink->RecvBuff[i]), sizeof(Uplink->RecvBuff), MSG_DONTWAIT,
                       (struct sockaddr *) &(Uplink->SocketAddress), &AddrLen);

      if ( (Status < 0) && (errno == EWOULDBLOCK) )
         break; /* no (more) messages */
      else {
            
         if (Status <= UPLINK_RECV_BUFF_LEN) {

            CFE_EVS_SendEvent(UPLINK_DEBUG_EID, CFE_EVS_DEBUG,
                              "UPLINK_Read: Read %d bytes from socket\n",Status);
            Uplink->RecvMsgCnt++;
            if (Uplink->MsgTunnel.Enabled) {
               
               ProcessMsgTunnelMap();

            } /* End if map enabled */
            CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &(Uplink->RecvBuff[i]));
         }
         else {
            
            Uplink->RecvMsgErrCnt++;
            CFE_EVS_SendEvent(UPLINK_RECV_LEN_ERR_EID,CFE_EVS_ERROR,
                              "Command dropped (too long). Header: 0x%02x%2x 0x%02x%2x 0x%02x%2x 0x%02x%2x",
                		         Uplink->RecvBuff[0], Uplink->RecvBuff[1], Uplink->RecvBuff[2], Uplink->RecvBuff[3],
                              Uplink->RecvBuff[4], Uplink->RecvBuff[5], Uplink->RecvBuff[6], Uplink->RecvBuff[7]);
         }
      } /* End if received a message */
   } /* End receive loop */

   return i;

} /* End UPLINK_Read() */


/******************************************************************************
** Function:  UPLINK_ResetStatus
**
*/
void UPLINK_ResetStatus()
{

   Uplink->RecvMsgCnt = 0;
   Uplink->RecvMsgErrCnt = 0;
   Uplink->MsgTunnel.MappingsPerformed = 0;
   Uplink->MsgTunnel.LastMapping.Index = UPLINK_MSG_TUNNEL_CNT;
   Uplink->MsgTunnel.LastMapping.OrgMsgId = UPLINK_UNUSED_MSG_ID;
   Uplink->MsgTunnel.LastMapping.NewMsgId = UPLINK_UNUSED_MSG_ID;

} /* End UPLINK_ResetStatus() */


/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the uplink is terminated. This should
** never occur but if it does this will close the network socket.
*/
static void DestructorCallback(void)
{

   CFE_EVS_SendEvent(UPLINK_DESTRUCTOR_INFO_EID, CFE_EVS_INFORMATION, "UPLINK deleting callback. Closing Network socket.\n");

   close(Uplink->SocketId);

} /* End DestructorCallback() */

/******************************************************************************
** Function: ProcessMsgTunnelMap
**
** This function should only be called if message tunneling is enabled in order
** to save processing time. It loops thru the message tunnel map and if the
** current input message idea is matched and the mapping is enabled then the
** message ID is replaced.
**
*/
static void ProcessMsgTunnelMap(void)
{

   int    i;
   CFE_SB_MsgId_t OrgMsgId = CFE_SB_GetMsgId(Uplink->RecvMsgPtr);

   for (i=0; i < UPLINK_MSG_TUNNEL_CNT; i++) {

      if (Uplink->MsgTunnel.Map[i].Enabled) {

    	   if (OrgMsgId == Uplink->MsgTunnel.Map[i].OrgMsgId) {
          
            CFE_SB_SetMsgId(Uplink->RecvMsgPtr,Uplink->MsgTunnel.Map[i].NewMsgId);
            CFE_SB_GenerateChecksum(Uplink->RecvMsgPtr);
            Uplink->MsgTunnel.MappingsPerformed++;
            Uplink->MsgTunnel.LastMapping.Index = i;
            Uplink->MsgTunnel.LastMapping.OrgMsgId = OrgMsgId;
            Uplink->MsgTunnel.LastMapping.NewMsgId = Uplink->MsgTunnel.Map[i].NewMsgId;
            break;
         }
      } /* End if tunnel enabled */

   } /* End map loop */

} /* End ProcessMsgTunnelMap() */
