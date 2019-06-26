/*
** Purpose: Implement a comm interface between the 42fsw and f42 objects
**
** Notes:
**   1. All of the functions in this object are related to setting up 
**      a socket and transferring the required data to/from the FSW. The
**      f42 object provides a ground interface to the 42 FSW controller. 
**   2. The current FSW controller is THREE_AXIS_FSW
**   3. Text messages are sent on sockets since optimized timing is not
**      critical and text message are easy to debug.
**   @@ TODO - If sim runs to the end, can't restart from FSW. If disconnect
**             while sim runnign it restarts fine. 
**   @@ TODO - Improve error checking & reporting
**
** References:
**   1. See 42 open source repository at 
**      https://sourceforge.net/projects/fortytwospacecraftsimulation/
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#include "42defines.h"
#include "42fsw.h"
#include "fswkit.h"
#include "iokit.h"
#include "mathkit.h"
#include "f42_comm.h"

#define MAX_LINE_CHAR 512

/*
** File Data
*/

static F42_COMM_Class F42_Comm;


/*
** Local Functions
*/

static void FlushSocket(int SocketFd);
static int  WriteSensorDataToSocket(struct FSWType *FSW,int Socket);
static bool ReadActuatorCmdsFromSocket(struct FSWType *FSW,FILE *Stream);


/******************************************************************************
** Function: F42_Comm_Init
**
** Initialize a F42_Comm object.
**
** Notes:
**   1. This must be called at the end of 42fsw's InitFSW() function. 
**
*/
void F42_Comm_Init(struct SCType *S) {
     
   /* 
   ** DT is not read in from the Inp_sim.txt file
   ** printf("DeltaTime = %0.6e\n",S->FSW.DT);
   ** F42_Comm.DeltaTime = S->FSW.DT;
   */
   
   F42_Comm.DeltaTime = 0.5;
   
   F42_Comm.WriteSensorCount = 0;
   F42_Comm.ReadActuatorCount = 0;
	 
   F42_Comm.Socket = InitSocketServer(F42_COMM_PORT,TRUE);
   F42_Comm.Stream = fdopen(F42_Comm.Socket,"r+");
      
   FlushSocket (F42_Comm.Socket);
	  
} /* End F42_Comm_Init() */


/******************************************************************************
** Function: F42_Comm_FSW
**
** Send sensor data to FSW and read commands from FSW.
**
** Notes:
**   1. Read is a blocking call so execution will be suspended until data is
**      received from the FSW 
**
*/
void F42_Comm_FSW(struct SCType *S) {

   WriteSensorDataToSocket(&(S->FSW),F42_Comm.Socket);
   ReadActuatorCmdsFromSocket(&(S->FSW),F42_Comm.Stream);

   if (F42_Comm.WriteSensorCount % 25 == 0) {
      if (F42_Comm.WriteSensorCount != F42_Comm.ReadActuatorCount)
         printf("F42_Comm:: WriteSensorCount=%ld, ReadActuatorCount=%ld\n", F42_Comm.WriteSensorCount, F42_Comm.ReadActuatorCount);
   }

} /* F42_Comm_FSW() */


/******************************************************************************
** Function: WriteSensorDataToSocket
**
** Notes:
**   1. See file prologue
**   2. These inputs to the FSW are engineering units that would normally be
**      derived from sensor inputs.
**
*/
static int WriteSensorDataToSocket(struct FSWType *FSW,int Socket) {

   int  BytesSent;
   char line[MAX_LINE_CHAR];

   //printf ("F42_comm::WriteSensorDataToSocket() - Before send\n");
   /* Must terminate with '\n' */
   //sprintf(line,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld\n",   
   sprintf(line,"%18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %ld\n",
      F42_Comm.DeltaTime,
      FSW->PosN[0],FSW->PosN[1],FSW->PosN[2],
      FSW->VelN[0],FSW->VelN[1],FSW->VelN[2],
      FSW->wbn[0],FSW->wbn[1],FSW->wbn[2],
      FSW->qbn[0],FSW->qbn[1],FSW->qbn[2],FSW->qbn[3],
      FSW->svn[0],FSW->svn[1],FSW->svn[2],
      FSW->svb[0],FSW->svb[1],FSW->svb[2],
      FSW->bvb[0],FSW->bvb[1],FSW->bvb[2],
      FSW->Hw[0],FSW->Hw[1],FSW->Hw[2],
      FSW->SunValid);
 
   //printf("Sending sensor data (len=%d): %s",strlen(line), line);
   //printf("Sending sensor data (len=%ld)\n",strlen(line));
   
   BytesSent = send(Socket,line,strlen(line),0);
   
   F42_Comm.WriteSensorCount++;

   //printf ("Bytes sent = %d\n", BytesSent);
   
   return BytesSent;
   

} /* End WriteSensorDataToSocket() */

/******************************************************************************
** Function: ReadActuatorCmdsFromSocket
**
** Notes:
**   1. See file prologue
**
*/
static bool ReadActuatorCmdsFromSocket(struct FSWType *FSW,FILE *Stream) {

   char line[MAX_LINE_CHAR];

   //printf ("F42_comm::ReadActuatorCmdsFromSocket() - Before get\n");
   fgets(line,(MAX_LINE_CHAR-1),Stream);
   
   //if (sscanf(line,"%lf %lf %lf %lf %lf %lf %lf",
   if (sscanf(line,"%le %le %le %le %le %le %le",
      &FSW->Twhlcmd[0],&FSW->Twhlcmd[1],&FSW->Twhlcmd[2],
      &FSW->Mmtbcmd[0],&FSW->Mmtbcmd[1],&FSW->Mmtbcmd[2],
      &FSW->GimCmd[0].Ang[0]) == 7) {
      //printf("Read actuator line: %s\n",line);
      F42_Comm.ReadActuatorCount++;
      return(TRUE);
      }
   else 
      return(FALSE);

} /* End ReadActuatorCmdsFromSocket() */

/******************************************************************************
** Function: FlushSocket
**
*/
static void FlushSocket(int SocketFd) {

   struct sockaddr_in  s_addr;
   int    addr_len;
   int    i;
   int    status;
   char   MsgBuf[MAX_LINE_CHAR];

   addr_len = sizeof(s_addr);
   memset ((char *) &s_addr, 0, sizeof(s_addr));
      
   printf("Flushing socket\n");

   // TODO - Fix this is it really flushes!!!   
   for (i=0; i<=2000; i++) {
      //printf("Flush socket: i=%d\n",i);
      status = recvfrom(SocketFd, MsgBuf, MAX_LINE_CHAR - 1,
                        MSG_DONTWAIT,(struct sockaddr *) &s_addr, (socklen_t *) &addr_len);

      if ( (status < 0) && (errno == EWOULDBLOCK) )
         break; /* no (more) messages */

   } /* end for */

   printf("Flushed %d packets from socket\n", i-1);
   
} /* End FlushSocket() */  
