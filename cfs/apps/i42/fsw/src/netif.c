/*
** Purpose: Implement the 42 simulator network interface
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

/*
** Includes
*/

#include "netif.h"
#include "f42_adp.h"

#include <stdio.h>
#include <string.h>
#include <errno.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

/*
** Global File Data
*/

static NETIF_Class* NetIf = NULL;


/*
** Local Function Prototypes
*/

static boolean InitSocket(const char *HostName, int Port, int AllowBlocking);

/******************************************************************************
** Function: NETIF_Constructor
**
** Initialize a Network Interface object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void NET42IF_Constructor(NETIF_Class*  NetIfPtr) {

   NetIf = NetIfPtr;

   CFE_PSP_MemSet((void*)NetIf, 0, sizeof(NETIF_Class));
   
   NetIf->Port = NETIF_DEF_PORT;
   strncpy(NetIf->IpAddrStr, NETIF_DEF_IP_ADDR_STR, NETIF_IP_STR_LEN);
   strncpy(NetIf->IpAddrStr, NETIF_LOCAL_HOST_STR, sizeof(NETIF_LOCAL_HOST_STR));

   /* InitSocket reports errors */
   NetIf->Connected = InitSocket(NetIf->IpAddrStr,NetIf->Port,TRUE);
   
   CFE_SB_InitMsg(&(NetIf->SensorPkt), F42_SENSOR_MID,
                  F42_ADP_SENSOR_PKT_LEN, TRUE);

} /* End NETIF_Constructor() */


/******************************************************************************
** Function:  NETIF_ResetStatus
**
*/
void NETIF_ResetStatus(void) {
  
   NetIf->SensorPktCnt = 0;
   NetIf->ActuatorPktCnt = 0;

} /* End NETIF_ResetStatus() */


/******************************************************************************
** Function: NETIF_ConnectTo42Cmd
**
*/
boolean NETIF_ConnectTo42Cmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const NETIF_ConnectTo42CmdParam* CmdParam = (const NETIF_ConnectTo42CmdParam *) MsgPtr;

   NETIF_Close();

   strncpy(NetIf->IpAddrStr, CmdParam->IpAddrStr, NETIF_IP_STR_LEN);
   NetIf->Port = CmdParam->Port;
   
   /* InitSocket reports errors */
   NetIf->Connected = InitSocket(NetIf->IpAddrStr, NetIf->Port, TRUE);

   if (NetIf->Connected) {
      CFE_EVS_SendEvent(NETIF_CONNECT_TO_42_INFO_EID, CFE_EVS_INFORMATION,
                        "Connected to 42 simulator on %s port %d", NetIf->IpAddrStr,NetIf->Port);
   }
   
   return NetIf->Connected;

} /* End NETIF_ConnectTo42Cmd() */


/******************************************************************************
** Function: NETIF_Close
**
*/
void NETIF_Close(void) {


   if (NetIf->Connected == TRUE) {
    
      //Crashes fdclose (NetIf->StreamId);
      close(NetIf->SocketFd);
      NetIf->Connected = FALSE;

      CFE_EVS_SendEvent(NETIF_SOCKET_CLOSE_INFO_EID, CFE_EVS_INFORMATION,
                        "Closed 42 socket after %d sensor data cycles", NetIf->ConnectCycles);

  
   } /* End if connected */
   else {
      CFE_EVS_SendEvent(NETIF_SOCKET_CLOSE_INFO_EID, CFE_EVS_INFORMATION,
                        "I42 NETIF_Close() called without a socket connection");
   }

   NetIf->ConnectCycles = 0;
  
} /* End NETIF_Close() */



/******************************************************************************
** Function:  NETIF_ProcessSensorPkt
**
** Autodisconnect if nothing read from socket. 
*/
boolean NETIF_ProcessSensorPkt(void) {

   boolean ProcessedPkt = FALSE;
  
   if (NetIf->Connected == TRUE) {

      NetIf->ConnectCycles++;

      if( fgets(NetIf->InBuf,sizeof(NetIf->InBuf)-1,NetIf->StreamId) != NULL ) {

         //OS_printf("I42 NETIF: Received sensor message: %s",NetIf->InBuf);

         if (sscanf(NetIf->InBuf,"%ld %lf %lf %lf %ld %ld %ld %ld",
             &(NetIf->SensorPkt.Sensor.SunValid),
             &(NetIf->SensorPkt.Sensor.wbn[0]),&(NetIf->SensorPkt.Sensor.wbn[1]),&(NetIf->SensorPkt.Sensor.wbn[2]),
             &(NetIf->SensorPkt.Sensor.Css[0].Counts), &(NetIf->SensorPkt.Sensor.Css[1].Counts),
             &(NetIf->SensorPkt.Sensor.Css[2].Counts), &(NetIf->SensorPkt.Sensor.Css[3].Counts)) == 8) {

            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &NetIf->SensorPkt);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &NetIf->SensorPkt);

            NetIf->SensorPktCnt++;
            ProcessedPkt = TRUE;
         
         } /* End sscanf() */
         
         NetIf->DisconnectCycles = 0;
      
      } /* End if fgets() */
      else {
      
         if (++NetIf->DisconnectCycles >= I42_NETIF_DISCONNECT_CYCLES) {
            CFE_EVS_SendEvent(NETIF_IDLE_SOCKET_CLOSE_INFO_EID, CFE_EVS_INFORMATION,
                              "Closing 42 socket. No data received for %d attempts.", NetIf->DisconnectCycles);
            NETIF_Close();
         }

      }
   } /* End if NetIf->Connected */
   //else
   //   OS_printf("I42 NETIF: NETIF_ProcessSensorPkt() called with no connection\n");


   return ProcessedPkt;
   
} /* NETIF_ProcessSensorPkt() */
            
            
/******************************************************************************
** Function:  SendActuatorPkt
**
*/
ssize_t NETIF_SendActuatorPkt(F42_ADP_ActuatorPkt*  ActuatorPkt) {

   ssize_t BytesSent = 0;
   
   //OS_printf("I42 NETIF: WheelCmd.Torq[0] = %.6lf\n",ActuatorPkt->WheelCmd.Torq[0]);
   //OS_printf("I42 NETIF: WheelCmd.Torq[1] = %.6lf\n",ActuatorPkt->WheelCmd.Torq[1]);
   //OS_printf("I42 NETIF: WheelCmd.Torq[2] = %.6lf\n",ActuatorPkt->WheelCmd.Torq[2]);

   snprintf(NetIf->OutBuf, sizeof(NetIf->OutBuf), "%lf %lf %lf\n",
            ActuatorPkt->WheelCmd.Torq[0], ActuatorPkt->WheelCmd.Torq[1], ActuatorPkt->WheelCmd.Torq[2]);
                
   BytesSent = send(NetIf->SocketFd,NetIf->OutBuf,strlen(NetIf->OutBuf),0);

   NetIf->ActuatorPktCnt++;

   return BytesSent;
   
} /* End SendActuatorPkt() */


/******************************************************************************
** Function:  InitSocket
**
*/
boolean InitSocket(const char *HostName, int Port, int AllowBlocking)
{

   int SocketFd, Flags;
   struct sockaddr_in  Server;
   struct hostent*     Host;


   NetIf->Connected = FALSE;

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
   
   OS_printf("I42 NETIF: Attempting to connect to Server %s on Port %d\n",HostName, Port);
   if (connect(SocketFd,(struct sockaddr *) &Server, sizeof(Server)) < 0) {
      CFE_EVS_SendEvent(NETIF_CONNECT_ERR_EID,CFE_EVS_ERROR,
                        "Error connecting client socket: %s", strerror(errno));
      return(NetIf->Connected);
   }
   OS_printf("I42 NETIF: Successfully connected to Server %s on Port %d\n",HostName, Port);

   /* Keep read() from waiting for message to come */
   if (!AllowBlocking) {
      Flags = fcntl(SocketFd, F_GETFL, 0);
      fcntl(SocketFd,F_SETFL, Flags|O_NONBLOCK);
   }

   NetIf->SocketFd = SocketFd;
   if (SocketFd >= 0) {
      NetIf->StreamId = fdopen(SocketFd,"r+");
      if (NetIf->StreamId != NULL) {
         NetIf->ConnectCycles    = 0;
         NetIf->DisconnectCycles = 0;
         NetIf->Connected = TRUE;
      }
      else {
         CFE_EVS_SendEvent(NETIF_STREAM_OPEN_ERR_EID,CFE_EVS_ERROR,
                          "Error openning socket stream: %s", strerror(errno));
      }
   } /* End if valid SocketFd */

   return NetIf->Connected;
      
} /* End InitSocket() */


/* end of file */
