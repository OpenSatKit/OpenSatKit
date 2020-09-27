/* 
** Purpose: Network interface
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
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "netif.h"


/*
** Global File Data
*/

static NETIF_Class* NetIf = NULL;

/*
** Local Function Prototypes
*/

static boolean InitSocket(const char *HostName, uint16 Port, boolean AllowBlocking);

/******************************************************************************
** Function: NETIF42_Close
**
*/
void NETIF42_Close(void) {

   if (NetIf->Connected == TRUE) {
    
      //Crashes fdclose (NetIf->StreamId);
      close(NetIf->SocketFd);
      NetIf->Connected = FALSE;

      CFE_EVS_SendEvent(NETIF_SOCKET_CLOSE_EID, CFE_EVS_INFORMATION,
                        "Successfully closed socket");

  
   } /* End if connected */
   else {
      CFE_EVS_SendEvent(NETIF_SOCKET_CLOSE_EID, CFE_EVS_INFORMATION,
                        "Attempt to close socket without a connection");
   }

} /* End NETIF42_Close() */


/******************************************************************************
** Function: NETIF42_Constructor
**
** Initialize a Network Interface object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void NETIF42_Constructor(NETIF_Class*  NetIfPtr, const char* IpAddrStr, uint16 Port) {

   NetIf = NetIfPtr;

   CFE_PSP_MemSet((void*)NetIf, 0, sizeof(NETIF_Class));
   
   NetIf->Port = Port;
   strncpy(NetIf->IpAddrStr, IpAddrStr, NETIF_IP_ADDR_STR_LEN);

   /* InitSocket reports errors */
   NetIf->Connected = InitSocket(NetIf->IpAddrStr,NetIf->Port, FALSE);
   
} /* End NETIF42_Constructor() */


/******************************************************************************
** Function: NETIF42_Connect42Cmd
**
*/
boolean NETIF42_Connect42Cmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const NETIF_Connect42CmdParam* CmdParam = (const NETIF_Connect42CmdParam *) MsgPtr;

   NETIF42_Close();

   strncpy(NetIf->IpAddrStr, CmdParam->IpAddrStr, NETIF_IP_ADDR_STR_LEN);
   NetIf->Port = CmdParam->Port;
   
   /* InitSocket reports errors */
   NetIf->Connected = InitSocket(NetIf->IpAddrStr, NetIf->Port, FALSE);

   if (NetIf->Connected) {
      CFE_EVS_SendEvent(NETIF_CONNECT_TO_42_EID, CFE_EVS_INFORMATION,
                        "Connected to 42 simulator on %s port %d", NetIf->IpAddrStr,NetIf->Port);
   }
   
   return NetIf->Connected;

} /* End NETIF42_Connect42Cmd() */


/******************************************************************************
** Function: NETIF42_Disconnect42Cmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean NETIF42_Disconnect42Cmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   NETIF42_Close();
   
   return TRUE;
   
} /* End NETIF42_Disconnect42Cmd() */


/******************************************************************************
** Function:  NETIF_ResetStatus
**
*/
void NETIF42_ResetStatus(void) {
  

} /* End NETIF_ResetStatus() */


/******************************************************************************
** Function: NETIF42_Recv
**
*/
int32 NETIF42_Recv(char* BufPtr, const uint16 BufSize) 
{

   int   TotalBytesRead = 0;
   int   BytesRemaining = (I42_SOCKET_BUF_LEN-2);
   char* InBufPtr = BufPtr;
   char* GetPtr = BufPtr;     /* Non-NULL value */
   

   if (NetIf->Connected == TRUE) {

      while (TotalBytesRead < (I42_SOCKET_BUF_LEN-2) && GetPtr != NULL) {

         if( (GetPtr = fgets(InBufPtr,BytesRemaining,NetIf->StreamId)) != NULL ) {
	
            CFE_EVS_SendEvent(NETIF_DEBUG_EID, CFE_EVS_DEBUG, "NETIF42_Recv(): Received sensor message (len=%d): %s",strlen(InBufPtr), InBufPtr);
            CFE_EVS_SendEvent(NETIF_DEBUG_EID, CFE_EVS_DEBUG, "NETIF42_Recv(): Received message (len=%d)\n",strlen(InBufPtr));
            TotalBytesRead += strlen(InBufPtr);

         }         

      } /* End while loop */
      
   } /* End if connected */
   else
      CFE_EVS_SendEvent(NETIF_RECV_ERR_EID,CFE_EVS_ERROR,
                        "Error attempting network read without being connected");

   return TotalBytesRead;
   
} /* NETIF42_Recv() */


/******************************************************************************
** Function: NETIF42_Send
**
*/
int32 NETIF42_Send (const char *BufPtr, uint16 Len) 
{
   
   int32 BytesSent = 0;

   if (NetIf->Connected)
	   BytesSent = send(NetIf->SocketFd,BufPtr,Len,0);
   else
      CFE_EVS_SendEvent(NETIF_SEND_ERR_EID,CFE_EVS_ERROR,
                        "Error attempting network send without being connected");
	   

   return BytesSent;

} /* End NETIF42_Send() */


/******************************************************************************
** Function:  InitSocket
**
*/
static boolean InitSocket(const char *HostName, uint16 Port, boolean AllowBlocking)
{

   int SocketFd, Flags;
   struct sockaddr_in  Server;
   struct hostent*     Host;


   NetIf->Connected = FALSE;
   strcpy(NetIf->IpAddrStr, HostName);  

   SocketFd = socket(AF_INET,SOCK_STREAM,0);
   if (SocketFd < 0) {
      CFE_EVS_SendEvent(NETIF_SOCKET_OPEN_ERR_EID,CFE_EVS_ERROR,
                        "Error opening socket. Socket() return code = %d", SocketFd);
      return(NetIf->Connected);
   }

   Host = gethostbyname(HostName);
   if (Host == NULL) {
     CFE_EVS_SendEvent(NETIF_HOST_ERR_EID,CFE_EVS_ERROR,
                       "Server %s not found", HostName);
     return(NetIf->Connected);
   }

   memset((char *) &Server,0,sizeof(Server));
   Server.sin_family = AF_INET;
   memcpy((char *)&Server.sin_addr.s_addr,(char *)Host->h_addr_list[0], Host->h_length);
   Server.sin_port = htons(Port);
   
   CFE_EVS_SendEvent(NETIF_DEBUG_EID, CFE_EVS_DEBUG, "***I42 NETIF***: Attempting to connect to Server %s on Port %d\n",HostName, Port);
   if (connect(SocketFd,(struct sockaddr *) &Server, sizeof(Server)) < 0) {
      CFE_EVS_SendEvent(NETIF_CONNECT_ERR_EID,CFE_EVS_ERROR,
                        "Error connecting client socket: %s", strerror(errno));
      return(NetIf->Connected);
   }
   CFE_EVS_SendEvent(NETIF_DEBUG_EID, CFE_EVS_DEBUG, "***I42 NETIF***: Successfully connected to Server %s on Port %d\n",HostName, Port);

   /* Keep read() from waiting for message to come */
   if (!AllowBlocking) {
      Flags = fcntl(SocketFd, F_GETFL, 0);
      fcntl(SocketFd,F_SETFL, Flags|O_NONBLOCK);
   }

   NetIf->SocketFd = SocketFd;
   if (SocketFd >= 0) {
      NetIf->StreamId = fdopen(SocketFd,"r+");
      if (NetIf->StreamId != NULL) {
         NetIf->Connected = TRUE;
         CFE_EVS_SendEvent(NETIF_CONNECT_EID,CFE_EVS_INFORMATION,
                          "Successfully connected to 42 server %s on Port %d\n",HostName, Port);
      }
      else {
         CFE_EVS_SendEvent(NETIF_STREAM_OPEN_ERR_EID,CFE_EVS_ERROR,
                          "Error opening socket stream: %s", strerror(errno));
      }
   } /* End if valid SocketFd */

   return NetIf->Connected;
      
} /* End InitSocket() */

