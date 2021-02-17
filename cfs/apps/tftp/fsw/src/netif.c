/* 
** Purpose: Network interface abstractions for TFTP. 
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

#include <unistd.h>  /* Needed for close() */

#include "netif.h"


/*
** Global File Data
*/

static NETIF_Class* NetIf = NULL;

/*
** Local Function Prototypes
*/

static void FlushSocket(int SocketId);
static boolean InitSocket(const char* DefIpAddrStr, int16 DefServerPort);

/******************************************************************************
** Function: NETIF_Constructor
**
** Create a NETIF object.
**
*/
void NETIF_Constructor(NETIF_Class*  NetIfPtr, char* DefIpAddrStr, int16 DefServerPort)
{

   NetIf = NetIfPtr;
   
   InitSocket(DefIpAddrStr,DefServerPort);


} /* NETIF_Constructor() */


/******************************************************************************
** Function: NETIF_ClearClient
**
*/
void NETIF_ClearClient()
{

   NetIf->ClientSocketAddrCreated = FALSE;

} /* NETIF_ClearClient() */


/******************************************************************************
** Function: NETIF_Destructor
**
*/
void NETIF_Destructor(void)
{

   CFE_EVS_SendEvent(NETIF_CLOSE_SOCKET_EID,CFE_EVS_INFORMATION,
                     "NETIF_Destructor: Closing Network socket");

   close(NetIf->SocketId);

} /* End NETIF_Destructor() */


/******************************************************************************
** Function: NETIF_RcvFrom
**
** Each message processed as it is received so no need for more than one buffer.
*/
int32 NETIF_RcvFrom(const uint8 NetIFid, void *BufPtr, const uint16 BufSize, boolean ServerListen)
{

   int AddrLen = sizeof(NetIf->ServerSocketAddr);
   int Status = 0;
   int16 Port;

   if (NetIFid == 0) {
     
      if (ServerListen) {
       
         Status = recvfrom(NetIf->SocketId, BufPtr, TFTP_RECV_BUFF_LEN, MSG_DONTWAIT,
                          (struct sockaddr *) &(NetIf->ServerSocketAddr), (socklen_t *)&AddrLen);
      }
      else {

         CFE_PSP_MemSet( &(NetIf->XferSocketAddr), 0, sizeof(NetIf->XferSocketAddr));  /* TODO - Is this needed? */
      
         Status = recvfrom(NetIf->SocketId, BufPtr, TFTP_RECV_BUFF_LEN, MSG_DONTWAIT,
                          (struct sockaddr *) &(NetIf->XferSocketAddr), (socklen_t *)&AddrLen);
                        
         CFE_EVS_SendEvent(NETIF_DEBUG_EID,CFE_EVS_DEBUG,"Recvfrom XferSocketAddr on port %d with Status %d, eerno %d",ntohs(NetIf->XferSocketAddr.sin_port),Status,errno);
     
      }
      if (Status > 0) {
        
         Port = ServerListen ? ntohs(NetIf->ServerSocketAddr.sin_port) : ntohs(NetIf->XferSocketAddr.sin_port);
         CFE_EVS_SendEvent(NETIF_DEBUG_EID,CFE_EVS_DEBUG,"NETIF_RcvFrom() on port %d, Status = %d",Port,Status);
        
         if (!NetIf->ClientSocketAddrCreated) {
           
            /* ServerSocketAddr contains client's ephemeral port to perform transfer */
            CFE_EVS_SendEvent(NETIF_DEBUG_EID,CFE_EVS_DEBUG,"Create ClientSocketAddr on port %d",ntohs(NetIf->ServerSocketAddr.sin_port));
            NetIf->ClientSocketAddr = NetIf->ServerSocketAddr;  
            NetIf->ClientSocketAddrCreated = TRUE; 
         }
        
      } /* End if Status > 0 */
      if ( (Status < 0) && (errno != EWOULDBLOCK) ) {
        
	      CFE_EVS_SendEvent(NETIF_RECV_ERR_EID,CFE_EVS_ERROR,
                           "TF error on RcvFrom NetIF %d, errno: %d", NetIFid, errno );
      }
       
   } /* End if NetIFid == 0 */
   else
	   CFE_EVS_SendEvent(NETIF_RECV_ERR_EID,CFE_EVS_ERROR,
                        "NETIF receive attempted and NET ID %d not implemented", NetIFid);
      
   return Status;

} /* NETIF_RcvFrom() */


/******************************************************************************
** Function: NETIF_SendTo
**
*/
int32 NETIF_SendTo (const uint8 NetIFid, const uint8 *BufPtr, uint16 len)
{

   int Status = -1;

   if (NetIf->ClientSocketAddrCreated) {
   
      CFE_EVS_SendEvent(NETIF_DEBUG_EID,CFE_EVS_DEBUG,"SendToNetIF on port %d",ntohs(NetIf->ClientSocketAddr.sin_port));

      Status = sendto(NetIf->SocketId, BufPtr, len, 0, (struct sockaddr *) &(NetIf->ClientSocketAddr), sizeof(NetIf->ClientSocketAddr) );

      if (Status < 0) {
     
         CFE_EVS_SendEvent(NETIF_SEND_ERR_EID,CFE_EVS_ERROR, "Error sending packet on socket %d, port %d, return status %d, errno %d",
                           NetIf->SocketId, ntohs(NetIf->ClientSocketAddr.sin_port), Status, errno);

      } /* End if sent packet */
      else {
     
         CFE_EVS_SendEvent(NETIF_DEBUG_EID,CFE_EVS_DEBUG, "Sent packet on socket %d, port %d, return status %d, errno %d",
                           NetIf->SocketId, ntohs(NetIf->ClientSocketAddr.sin_port), Status, errno);
      }
     
   } /* end if ClientSocketAddrCreated */
   else {
     
      CFE_EVS_SendEvent(NETIF_SEND_ERR_EID,CFE_EVS_ERROR, "Attempting to send packet to unitialized client");

   }
 	 
   return (Status);
  		   
} /* End NETIF_SendTo() */


/******************************************************************************
** Function: NETIF_InitSocketCmd
**
** Initialize the socket.
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean NETIF_InitSocketCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean  RetStatus;

   const NETIF_InitSocketCmdMsg *InitSocketCmd = (const NETIF_InitSocketCmdMsg *) MsgPtr;

   close(NetIf->SocketId);
   
   RetStatus = InitSocket(InitSocketCmd->IpAddrStr,InitSocketCmd->ServerPort);

   return RetStatus;

} /* End NETIF_InitSocketCmd() */


/******************************************************************************
** Function: FlushSocket
**
*/
static void FlushSocket(int SocketId) {

   struct sockaddr_in  s_addr;
   int    addr_len;
   int    i;
   int    status;
   char   MsgBuf[TFTP_RECV_BUFF_LEN];

   addr_len = sizeof(s_addr);
   memset ((char *) &s_addr, 0, sizeof(s_addr));

   for (i=0; i<=50; i++) {
  
      status = recvfrom(SocketId, MsgBuf, TFTP_RECV_BUFF_LEN - 1,
                        MSG_DONTWAIT,(struct sockaddr *) &s_addr, (socklen_t *) &addr_len);

      if ( (status < 0) && (errno == EWOULDBLOCK) )
         break; /* no (more) messages */

   } /* end for */

} /* End FlushSocket() */  


/******************************************************************************
** Function: InitSocket
**
** Initialize the socket.
**
*/
static boolean InitSocket(const char* DefIpAddrStr, int16 DefServerPort)
{

   boolean RetStatus = FALSE;
   
   strcpy(NetIf->IpAddrStr, DefIpAddrStr);  

   if ( (NetIf->SocketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {
     
      NetIf->ServerPort = DefServerPort;
      NetIf->ServerSocketAddr.sin_family      = AF_INET;
      NetIf->ServerSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      NetIf->ServerSocketAddr.sin_port        = htons(NetIf->ServerPort);

      if ( (bind(NetIf->SocketId, (struct sockaddr *) &(NetIf->ServerSocketAddr), 
                 sizeof(NetIf->ServerSocketAddr)) >= 0) ) {

         CFE_EVS_SendEvent(NETIF_SOCKET_PORT_BOUND_EID, CFE_EVS_INFORMATION,
                           "NETIF_Create: Bound to port %d", NetIf->ServerPort);
         
         /* Set the socket to non-blocking. Not available in vxWorks, so conditionally compiled. */
         #ifdef _HAVE_FCNTL_
            fcntl(NetIf->SocketId, F_SETFL, O_NONBLOCK);
         #endif

         FlushSocket(NetIf->SocketId);
         
		   RetStatus = TRUE;
		 
      }/* End if successful bind */
      else {

         CFE_EVS_SendEvent(NETIF_BIND_SOCKET_ERR_EID, CFE_EVS_ERROR, "Socket bind failed. errno = %d", errno);
      
      }
   } /* End if successful socket creation */
   else {

      CFE_EVS_SendEvent(NETIF_CREATE_SOCKET_ERR_EID, CFE_EVS_ERROR, 
	                     "Input socket creation failed. SocketId = %d, errno = %d", NetIf->SocketId, errno);
    
   } /* End if error creating socket */

	return RetStatus;
	
} /* InitSocket() */
