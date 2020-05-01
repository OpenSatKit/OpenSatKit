/* 
** File:
**   $Id: $
**
** Purpose: Network interface abstractions file transfer application. 
**
** Notes
** References:
**   1. Core Flight Executive Application Developers Guide.
**   2. OpenSat Object-based Programmer's Guide
**
**
** $Date: $
** $Revision: $
** $Log: $
*/

/*
** Include Files:
*/

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include "ft_netif.h"
#include "cfe.h"
#include "tf_platform_cfg.h"
#include "app_cfg.h"


/*
** Global File Data
*/
 
#define TFAPP_DEF_IP_ADDR_STR  "192.168.199.213"  /* cpu talking to */

static char    IpAddrStr[16]; /* IPv4 only */
static int16   ServerPort;
static int16   ClientPort;
static int     InSocketId;		  /* Input socket descriptor  */
static int     OutSocketId;		  /* Output socket descriptor */
static int     XferSocketId;		/* Transfer data socket descriptor */
static struct  sockaddr_in  InSocketAddr;   /* Constructor()     */
static struct  sockaddr_in  OutSocketAddr;  /* CreateOutSocket() */
static struct  sockaddr_in  XferSocketAddr;
static boolean SocketsCreated;
static boolean OutSocketAddrCreated;
/*
** Local Function Prototypes
*/

static void FlushSocket(int SocketId);


/******************************************************************************
** Function: CreateSockets
**
*/

static boolean CreateSockets() {
  
   strcpy(IpAddrStr, TFAPP_DEF_IP_ADDR_STR);  


   OS_printf("TFAPP: TFTP about to create input socket\n");

   if ( (InSocketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {
      ServerPort = TFAPP_IN_PORT;
      InSocketAddr.sin_family      = AF_INET;
      InSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      InSocketAddr.sin_port        = htons(ServerPort);

      if ( (bind(InSocketId, (struct sockaddr *) &(InSocketAddr), sizeof(InSocketAddr)) >= 0) ) {

         OS_printf("TFAPP: Connected to port %d\n", TFAPP_IN_PORT);
         /* Set the socket to non-blocking. Not available in vxWorks, so conditionally compiled. */
         #ifdef _HAVE_FCNTL_
            fcntl(InSocketId, F_SETFL, O_NONBLOCK);
         #endif

         FlushSocket(InSocketId);

         OS_printf("TFAPP: TFTP about to create output socket\n");
	 
         if ( (OutSocketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
         {
            CFE_EVS_SendEvent(TFTP_NET_IF_CREATE_ERR_EID, CFE_EVS_ERROR, 
	                      "Output socket creation failed. SocketId = %d, errno = %d\n", OutSocketId, errno);
         }
         else {
            OS_printf("TFAPP: Created OutSocket %d\n", OutSocketId);
         }

      }/* End if successful bind */
      else {

        CFE_EVS_SendEvent(TFTP_NET_IF_ERRNO_EID, CFE_EVS_ERROR, "Input socket bind failed. errno = %d\n", errno);
      }

    } /* End if successful socket creation */
    else {

         CFE_EVS_SendEvent(TFTP_NET_IF_CREATE_ERR_EID, CFE_EVS_ERROR, 
	                   "Input socket creation failed. SocketId = %d, errno = %d\n", InSocketId, errno);
    }

    return TRUE;

}/* End CreateSockets() */

/******************************************************************************
** Function: CreateOutSocketAddr
**
*/

static void CreateOutSocketAddr(int16 client_port) {

   CFE_PSP_MemSet((void*)&OutSocketAddr, 0, sizeof(OutSocketAddr));

   OutSocketAddr.sin_family      = AF_INET;
   OutSocketAddr.sin_addr.s_addr = inet_addr(IpAddrStr);
   OutSocketAddr.sin_port        = client_port;

   /* OutSocketAddrCreated = TRUE; DCM */

} /* End CreateOutSocketAddr() */


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
** Function: SendToNetIF
**
** Create a TFTP message according to the state information maintained
** in the TFTP data structure and the parameters provided. Since all
** TFTP message are very similar, we use a single function to encode
** all message types.
*/
int32 SendToNetIF (const uint8 NetIFid, const uint8 *BufPtr, uint16 len)
 {
   int     Status = 0;

   CFE_EVS_SendEvent(999,CFE_EVS_DEBUG,"SendToNetIF on port %d",ntohs(OutSocketAddr.sin_port));

   Status = sendto(OutSocketId, BufPtr, len, 0, (struct sockaddr *) &OutSocketAddr, sizeof(OutSocketAddr) );

   if (Status < 0) {
     
     CFE_EVS_SendEvent(TFTP_SEND_PKT_SOCKET_ERR_EID,CFE_EVS_ERROR, "Error sending packet on socket %d, port %d, return status %d, errno %d",
                        OutSocketId, ntohs(OutSocketAddr.sin_port), Status, errno);

   } /* End if sent packet */
   else {
     
      CFE_EVS_SendEvent(TFTP_SEND_PKT_SOCKET_ERR_EID,CFE_EVS_DEBUG, "Sent packet on socket %d, port %d, return status %d, errno %d",
                        OutSocketId, ntohs(OutSocketAddr.sin_port), Status, errno);
   }
     
 	 
  return (Status);
  		   
} /* End SendToNetIF() */


/******************************************************************************
** Function: RcvFromNetIF
**
** Each message processed as it is received so no need for more than one buffer.
*/
int32 RcvFromNetIF(const uint8 NetIFid, void *BufPtr, const uint16 BufSize, boolean ServerListen) {

  int AddrLen = sizeof(InSocketAddr);
  int Status = 0;

  if (NetIFid == 0)
  {
     CFE_PSP_MemSet( &InSocketAddr, 0, sizeof(InSocketAddr));
      
     if (ServerListen) 
     {
       
       Status = recvfrom(InSocketId, BufPtr, TFTP_RECV_BUFF_LEN, MSG_DONTWAIT,
                        (struct sockaddr *) &(InSocketAddr), (socklen_t *)&AddrLen);
     }
     else
     {
       Status = recvfrom(InSocketId, BufPtr, TFTP_RECV_BUFF_LEN, MSG_DONTWAIT,
                        (struct sockaddr *) &(XferSocketAddr), (socklen_t *)&AddrLen);
       CFE_EVS_SendEvent(999,CFE_EVS_DEBUG,"Recvfrom XferSocketAddr on port %d with Status %d, eerno %d",ntohs(XferSocketAddr.sin_port),Status,errno);
     
     }
     if (Status > 0)
     {
        CFE_EVS_SendEvent(999,CFE_EVS_DEBUG,"RcvFromNetIF() on port %d, Status = %d",ntohs(InSocketAddr.sin_port),Status);
        if (!OutSocketAddrCreated)   /* DCM - Quick hack to test TCTP Client. Need to fully design app to be either client or server */
        {
           CFE_EVS_SendEvent(999,CFE_EVS_DEBUG,"CreateOutSocketAddr on port %d",ntohs(InSocketAddr.sin_port));
           /* CreateOutSocketAddr(InSocketAddr.sin_port); */
           OutSocketAddr = InSocketAddr;
           OutSocketAddrCreated = TRUE; 
        }
     } /* End if Status > 0 */
     if ( (Status < 0) && (errno != EWOULDBLOCK) )
     {
	      CFE_EVS_SendEvent(TFTP_NET_IF_ERRNO_EID,CFE_EVS_ERROR,
               		        "TF error on RcvFrom NetIF %d, errno: %d", NetIFid, errno );
     }
       
  } /* End if NetIFid == 0 */
  else
     OS_printf("RcvFromNetIF NetIFid %d not implemented\n", NetIFid);
      
  return Status;

} /* RcvFromNetIF */

void  CreateNetIF()
  {
      SocketsCreated = CreateSockets();/*input socket/listening (i think)*/
}


void CreatePeerNetIF()     /* All Peers on the local network */
{
   CreateOutSocketAddr(htons(TFAPP_OUT_PORT)); /*destination (i think)*/
}

void ClearClientNetIF()
{
   OutSocketAddrCreated = FALSE;
}

/******************************************************************************
** Function: DistroyNetIF
**
** This function is called when the app is terminated. This should
** never occur but if it does this will close the network socketS.
*/
void DistroyNetIF(void) {

   OS_printf("DistroyNetIF Closing Network socket.\n");

      close(InSocketId);
      close(OutSocketId);

} /* End DistroyNetIF() */

