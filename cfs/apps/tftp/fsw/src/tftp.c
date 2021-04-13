/* 
** Purpose: Implement the TFTP protocol.
**
** Notes
**   1. This code is based on the following file: http://cnds.eecs.jacobs-university.de/courses/np-2012/src/tftp/tftp.c
**      In it's header it states "A TFTP (RFC 1350) over IPv4/IPv6 client. This code is intended to
**      demonstrate (i) how to write encoding/decoding functions, (ii) how to implement a simple state
**   2. A general design decision is that low level encode and decode functions would send event messages for
**      errors. Higher logic functions may still send an event message such as an aborted transaction and the low level
**      message would provide details.
**   3. Only one transfer at a time
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
#include "netif.h"
#include "tftp.h"

/*
** Global File Data
*/

static TFTP_Class* Tftp = NULL;
static uint16   OpCode;

/*
** Local Function Prototypes
*/

static void DestructorCallback(void);

static boolean ProcessMsg(uint8* Buf, uint16 BufLen);

static void CompleteFileTransfer(void);

static int16 EncodePkt(const uint16 Opcode, uint16 BlockNum,
                       uint8 *DataPtr, uint16 DataLen);
					   
static boolean SendPkt(const uint16 Opcode, uint16 BlockNum,
                       uint8 *DataPtr, uint16 DataLen);

static boolean DecodeOpcode(uint8 *Buf, uint16 BufLen, uint16 *Opcode);

static boolean DecodeBlockNum(uint8 *Buf, uint16 BufLen, uint16 *BlockNum);

static boolean DecodeData(uint8 *Buf, uint16 BufLen, uint8 **Data, uint16 *DataLen);

static boolean DecodeErrorMsg(uint8 *Buf, uint16 BufLen, uint16 *ErrCode, char **ErrMsg);

static boolean DecodeRequestStrings(uint8 *Buf, uint16 BufLen, char  *Filename, char *Mode);

static void SendDumpBufEvent(void);

/******************************************************************************
** Function: TFTP_Constructor
**
*/
void TFTP_Constructor(TFTP_Class*  TftpPtr) {

   Tftp = TftpPtr;

   /* Assumes some default/undefined states are 0 */
   
   CFE_PSP_MemSet((void*)TftpPtr, 0, sizeof(TFTP_Class));
   
   Tftp->State   = TFTP_STATE_IDLE;
   Tftp->Timer = 0;
   Tftp->NetIFid = 0;
   
   strcpy(Tftp->SrcFilename, "Undefined");
   strcpy(Tftp->DestFilename, "Undefined");

   /* Set up the network interfaces */
   NETIF_Constructor(&(Tftp->NetIf), TFTP_IP_ADDR_STR, TFTP_IN_PORT);
   
   OS_TaskInstallDeleteHandler((void *)(&DestructorCallback)); /* Call when application terminates */

   CFE_SB_InitMsg(&Tftp->TransferReqPkt, TFTP_TRANSFER_REQ_MID, TFTP_TRANSFER_REQ_PKT_LEN, TRUE);

} /* End TFTP_Constructor() */


/******************************************************************************
** Function:  TFTP_ResetStatus
**
*/
void TFTP_ResetStatus() {

   Tftp->GetFileCnt = 0;
   Tftp->PutFileCnt = 0;

   Tftp->MsgCnt = 0;
   Tftp->MsgErrCnt = 0;

   Tftp->State = TFTP_STATE_IDLE;
   Tftp->Timer = 0;
   Tftp->BlockNum = 0;

} /* End TFTP_ResetStatus() */


/******************************************************************************
** Function: TFTP_Read
**
** Each message processed as it is received so no need for more than one buffer.
*/
uint16 TFTP_Read(uint16 MaxMsgRead) {

   int i = 0;
   int Status;
   boolean ServerListen = FALSE;
  
   if (Tftp->State != TFTP_STATE_IDLE) {
      Tftp->Timer++;
   }
   
   /* TODO use timer to reset state machines */
   if ( (Tftp->Timer > TFTP_STATE_TIMEOUT) && 
        (Tftp->State  != TFTP_STATE_IDLE) ) {
           
	   CFE_EVS_SendEvent(TFTP_STATE_TIMEOUT_EID, CFE_EVS_INFORMATION, 
                        "TFTP timed out on peer response State = %d, Timer = %d", Tftp->State, Tftp->Timer);
	   CompleteFileTransfer();
	}


   for (i = 0; i < MaxMsgRead; i++) {
      
      ServerListen = (Tftp->State == TFTP_STATE_IDLE);
      Status = NETIF_RcvFrom(Tftp->NetIFid, Tftp->MsgBuf, sizeof(Tftp->MsgBuf), ServerListen );
        
      if (Status <= 0)
         break; /* no (more) messages or an error has occurred*/
      else {
            
	      if (Status <= TFTP_MAX_MSGSIZE) {
            Tftp->MsgCnt++;
            ProcessMsg(Tftp->MsgBuf,Status);
         }
	      else {
            /* TODO -- Also had a message buffer overflow so bigger issues if this occurs */
	         Tftp->MsgErrCnt++;
            CFE_EVS_SendEvent(TFTP_RECV_LEN_ERR_EID,CFE_EVS_ERROR,
	                           "TFTP message dropped (too long). Header: 0x%0lx 0x%0lx",
			                     *(long *)Tftp->MsgBuf,  *(long *)(&Tftp->MsgBuf[4]) );
         }
      } /* End if valid read */

   } /* End receive loop */

   if (Status < 0)
      return(Status); /* return the error number */
   else
      return i;

} /* End TFTP_Read() */


/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the app is terminated. This should
** never occur but if it does this will close the network interfaces.
*/
static void DestructorCallback(void) {
 
   NETIF_Destructor();
   
} /* End DestructorCallback() */


/******************************************************************************
** Function: TFTP_PutFileCmd
**
** Send a put file TRANSFER_REQ_PKT to the ground so it can initiate a file
** transfer. The FSW is always the TFTP server to avoid having the OSK
** installation require a TFTP server installation. 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean TFTP_PutFileCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const TFTP_PutFileCmdMsg *PutFileCmd = (const TFTP_PutFileCmdMsg *) MsgPtr;
   boolean  RetStatus = TRUE;

   Tftp->TransferReqPkt.CmdCode = TFTP_PUT_CMD_CODE;

   strncpy(Tftp->TransferReqPkt.SrcFilename, PutFileCmd->SrcFilename, TFTP_FILENAME_LEN);
   Tftp->TransferReqPkt.SrcFilename[TFTP_FILENAME_LEN-1] = '\0';

   strncpy(Tftp->TransferReqPkt.DestFilename, PutFileCmd->DestFilename, TFTP_FILENAME_LEN);
   Tftp->TransferReqPkt.DestFilename[TFTP_FILENAME_LEN-1] = '\0';

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Tftp->TransferReqPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Tftp->TransferReqPkt);

   return RetStatus;

} /* End TFTP_PutFileCmd() */


/******************************************************************************
** Function: TFTP_GetFileCmd
**
** Send a put file TRANSFER_REQ_PKT to the ground so it can initiate a file
** transfer. The FSW is always the TFTP server to avoid having the OSK
** installation require a TFTP server installation. 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean TFTP_GetFileCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const TFTP_GetFileCmdMsg *GetFileCmd = (const TFTP_GetFileCmdMsg *) MsgPtr;
   boolean  RetStatus = TRUE;

   Tftp->TransferReqPkt.CmdCode = TFTP_GET_CMD_CODE;

   strncpy(Tftp->TransferReqPkt.SrcFilename, GetFileCmd->SrcFilename, TFTP_FILENAME_LEN);
   Tftp->TransferReqPkt.SrcFilename[TFTP_FILENAME_LEN-1] = '\0';

   strncpy(Tftp->TransferReqPkt.DestFilename, GetFileCmd->DestFilename, TFTP_FILENAME_LEN);
   Tftp->TransferReqPkt.DestFilename[TFTP_FILENAME_LEN-1] = '\0';

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Tftp->TransferReqPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Tftp->TransferReqPkt);

   return RetStatus;

} /* End TFTP_GetFileCmd() */


/******************************************************************************
** Function: CompleteFileTransfer
**
** Clean up after a transfer even if it is aborted.
*/
static void CompleteFileTransfer(void) {
    
   if (Tftp->FileHandle >= 0) {
      OS_close(Tftp->FileHandle);
      Tftp->FileHandle = -1;
   }
   Tftp->State = TFTP_STATE_IDLE;
   Tftp->Timer = 0;

   NETIF_ClearClient();
   
} /* End CompleteFileTransfer() */


/******************************************************************************
** Function: SendPkt
**
** Create a TFTP message according to the state information maintained
** in the TFTP data structure and the parameters provided. Since all
** TFTP message are very similar, we use a single function to encode
** all message types.
*/

static boolean SendPkt(const uint16 Opcode, uint16 BlockNum,
		                   uint8 *DataPtr, uint16 DataLen) {

   boolean PktSent = FALSE;
   uint16  SendMsgLen;
   int32   Status = 0;
 
   Tftp->Timer = 0;
   if ( (SendMsgLen = EncodePkt(Opcode, BlockNum, DataPtr, DataLen)) > 0) {
   
      Tftp->SendMsgLen = SendMsgLen;
    
      CFE_EVS_SendEvent(TFTP_SEND_PKT_DBG_EID, CFE_EVS_DEBUG, 
                        "TFTP sending data on Net IF %d, block number %d, Length %d\n", 
                        Tftp->NetIFid, Tftp->BlockNum, Tftp->SendMsgLen);
	  
      Status = NETIF_SendTo(Tftp->NetIFid, Tftp->SendMsgBuf, SendMsgLen);

      if (Status < 0) {

         CFE_EVS_SendEvent(TFTP_SEND_PKT_DBG_EID, CFE_EVS_ERROR, 
	                        "Error sending packet on NET IF %d, return status %d",
                           Tftp->NetIFid, Status);
      } /* End if sent packet */
      else {
         PktSent = TRUE;
      }
   
   } /* End if encoded packet */
   else {
      CFE_EVS_SendEvent(TFTP_SEND_PKT_ENCODE_ERR_EID,CFE_EVS_ERROR, 
                        "Error encoding packet. Opcode %d, BlockNum %d, DataLen %d", Opcode, BlockNum, DataLen);
   } /* End if encode packet failed */
   
   return PktSent;
					   
} /* End SendPkt() */


/******************************************************************************
** Function: EncodePkt
**
** Create a TFTP message according to the state information maintained
** in the TFTP data structure and the parameters provided. Since all
** TFTP message are very similar, we use a single function to encode
** all message types.
**
** DataPtr and len are only used when sending file data. The Tftp structure
** is used for all other input data. Tftp updates are managed by the caller.
**
** RRQ/WRQ only encoded when FSW is initiating a file transfer from a
** get/put command.  The source and destination file names are relative
** to the sender of those commands:
**
** Command   FSW Response  Filename
**   Put        RRQ        Put source file is read from
**   Get        WRQ        Get destination file is written to
*/

static int16 EncodePkt(const uint16 Opcode, uint16 BlockNum,
		               uint8 *DataPtr, uint16 DataLen) {

   uint8 *MsgPtr = Tftp->SendMsgBuf;
   uint16 FilenameLen, ModeLen, ErrStrLen, MsgLen;

   *MsgPtr = ((Opcode >> 8) & 0xff); MsgPtr++;
   *MsgPtr = (Opcode & 0xff);        MsgPtr++;

   switch (Opcode) {
   

      case TFTP_OPCODE_RRQ:

         FilenameLen = strlen(Tftp->SrcFilename) + 1;
         ModeLen     = strlen(Tftp->Mode) + 1;
        
         if ( (FilenameLen + ModeLen + 4) > TFTP_MAX_MSGSIZE) {
 		   
            CFE_EVS_SendEvent(TFTP_ENCODE_RRQ_ERR_EID, CFE_EVS_ERROR,
 		    	                  "TFTP encoding error: RRQ filename too long. File name len %d, Mode len %d\n",
                              FilenameLen, ModeLen);
            return -1;
 	      }

 	      memcpy(MsgPtr, Tftp->SrcFilename, FilenameLen);
 	      MsgPtr += FilenameLen;
         memcpy(MsgPtr, Tftp->Mode, ModeLen);
         MsgPtr += ModeLen;
         break;

      case TFTP_OPCODE_WRQ:

         FilenameLen = strlen(Tftp->DestFilename) + 1;
         ModeLen     = strlen(Tftp->Mode) + 1;
         if ( (FilenameLen + ModeLen + 4) > TFTP_MAX_MSGSIZE) {
            CFE_EVS_SendEvent(TFTP_ENCODE_WRQ_ERR_EID, CFE_EVS_ERROR,
                              "TFTP encoding error: WRQ filename too long. File name len %d, Mode len %d\n",
                              FilenameLen, ModeLen);
            return -1;
         }

         memcpy(MsgPtr, Tftp->DestFilename, FilenameLen);
         MsgPtr += FilenameLen;
         memcpy(MsgPtr, Tftp->Mode, ModeLen);
         MsgPtr += ModeLen;
         break;

      case TFTP_OPCODE_DATA:
	  
         *MsgPtr = ((BlockNum >> 8) & 0xff); MsgPtr++;
         *MsgPtr = (BlockNum & 0xff); MsgPtr++;

         if ((DataLen + 4) > TFTP_MAX_MSGSIZE) {
            CFE_EVS_SendEvent(TFTP_ENCODE_DATA_ERR_EID, CFE_EVS_ERROR,
                              "TFTP encoding error: Data too big %d", DataLen);
            return -1;
         }
         memcpy(MsgPtr, DataPtr, DataLen);
         MsgPtr += DataLen;
         break;
	
      case TFTP_OPCODE_ACK:
	
         *MsgPtr = ((BlockNum >> 8) & 0xff); MsgPtr++;
         *MsgPtr = (BlockNum & 0xff);        MsgPtr++;
         break;
	
      case TFTP_OPCODE_ERROR:
	
         /*
         ** BlockNum contains an error code and data is a NUL-terminated
         ** string with an error message.
         */

         *MsgPtr = ((BlockNum >> 8) & 0xff); MsgPtr++;
         *MsgPtr = (BlockNum & 0xff);        MsgPtr++;
         
         ErrStrLen = strlen((char *) DataPtr) + 1;
         if ((ErrStrLen + 4) > TFTP_MAX_MSGSIZE) {
            CFE_EVS_SendEvent(TFTP_ENCODE_ERR_MSG_ERR_EID, CFE_EVS_ERROR,
                              "TFTP encoding error: Error message too long. Len = %d", ErrStrLen);
            return -1;
         }

         memcpy(MsgPtr, DataPtr, ErrStrLen);
         MsgPtr += ErrStrLen;
         break;
	
   } /* End Opcode switch */

   MsgLen = (MsgPtr - Tftp->SendMsgBuf);

   return MsgLen;
	
} /* End EncodePkt() */


/******************************************************************************
** Function: DecodeOpcode
**
** Utility functions to decode fields from a received TFTP message.
*/
static boolean DecodeOpcode(uint8 *Buf, uint16 BufLen, uint16 *Opcode) {

   if (BufLen < 2) {
      CFE_EVS_SendEvent(TFTP_DECODE_OPCODE_ERR_EID, CFE_EVS_ERROR, "TFTP decode opcode error. BufLen = %d",BufLen);
      return FALSE;
   }

   *Opcode = (Buf[0] << 8) + Buf[1];

   return TRUE;

} /* End DecodeOpcode() */


/******************************************************************************
** Function: DecodeBlockNum
**
** Utility functions to decode fields from a received TFTP message.
*/
static boolean DecodeBlockNum(uint8 *Buf, uint16 BufLen, uint16 *BlockNum)
{

   if (BufLen < 4) {
      CFE_EVS_SendEvent(TFTP_DECODE_BLOCK_NUM_ERR_EID, CFE_EVS_ERROR,
                        "TFTP decode block number error. BufLen = %d",BufLen);
      return FALSE;
   }

   *BlockNum = (Buf[2] << 8) + Buf[3];
   return TRUE;
	
} /* End DecodeBlockNum() */


/******************************************************************************
** Function: DecodeData
**
** Utility functions to decode fields from a received TFTP message.
*/
static boolean DecodeData(uint8 *Buf, uint16 BufLen,
                          uint8 **Data, uint16 *DataLen)
{

   if (BufLen < 4) {
      *Data = NULL;
      *DataLen = 0;
       CFE_EVS_SendEvent(TFTP_DECODE_DATA_ERR_EID, CFE_EVS_ERROR,
    	                   "TFTP decode data error. BufLen = %d",BufLen);
      return FALSE;
   }

   /* zero data indicating no more data to be sent (data was multiple of block size) */
   if (BufLen == 4) { 
      *Data = NULL;
      *DataLen = 0;  
   }
   else {
      *Data = Buf + 4;
      *DataLen = BufLen - 4;
   }
   
   return TRUE;
	
} /* End DecodeData() */


/******************************************************************************
** Function: DecodeRequestStrings
**
** Utility function to retrieve file name and mode from a RRQ/WRQ
*/
static boolean DecodeRequestStrings(uint8 *Buf, uint16 BufLen,
	                                char  *Filename, char *Mode)
{

   char     *FilenameBufPtr = (char *)&Buf[2], *ModeBufPtr;
   uint16   FilenameLen;
   boolean  DecodedStrings = FALSE;

   strncpy(Filename, FilenameBufPtr, TFTP_FILENAME_LEN-1);
   Filename[TFTP_FILENAME_LEN-1] = '\0';
   FilenameLen = strlen(Filename);

   /* Sanity length check, not a precise check */
   if (BufLen > FilenameLen) {

      ModeBufPtr = FilenameBufPtr+FilenameLen+1;  /* Remember to skip the /0 */
      strncpy(Mode, ModeBufPtr, TFTP_MAX_MODE_LEN-1);
      Mode[TFTP_MAX_MODE_LEN-1] = '\0';

      DecodedStrings = TRUE;

   }  /* End if buffer has room for mode string */
   else {

      CFE_EVS_SendEvent(TFTP_DECODE_REQ_STR_ERR_EID, CFE_EVS_ERROR,
                        "TFTP decoding error: Invalid request string message length %d", BufLen);
      SendDumpBufEvent();

   } /* End if buffer doesn't has room for mode string */

   /* TODO - Change all DBG message to DEBUG types */
   CFE_EVS_SendEvent(TFTP_DECODE_DBG_EID, CFE_EVS_ERROR,
                     "DecodeRequestString(): %s, %s",Filename,Mode);

   return DecodedStrings;

} /* End DecodeRequestStrings() */


/******************************************************************************
** Function: DecodeErrorMsg
**
** Utility functions to decode fields from a received TFTP message.
*/
static boolean DecodeErrorMsg(uint8 *Buf, uint16 BufLen,
	                          uint16 *ErrCode, char **ErrMsg)
{

   int i;

   if (BufLen < 5) {
      *ErrMsg = NULL;
      CFE_EVS_SendEvent(TFTP_DECODE_ERR_MSG_ERR_EID, CFE_EVS_ERROR,
                        "TFTP decoding error: Invalid error message length %d", BufLen);
      return FALSE;
   }

   /* 
   ** Sanity check: the error message must be nul-terminated inside
   ** of the buffer Buf, otherwise the packet is invalid 
   */
    
   for (i = 4; i < BufLen; i++) {
      if (Buf[i] == 0) break;
   }
   
   if (i == BufLen) {
      CFE_EVS_SendEvent(TFTP_DECODE_ERR_MSG_ERR_EID, CFE_EVS_ERROR,
                        "TFTP decoding error: Error message is not a null-terminated string");
      return FALSE;
   }

   *ErrCode = (Buf[2] << 8) + Buf[3];
   *ErrMsg = (char *) Buf + 4;
   return TRUE;
	
} /* End DecodeErrorMsg() */


/******************************************************************************
** Function: SendDumpBufEvent
**
** Utility function to dump the buffer in event messages
*/
static void SendDumpBufEvent()
{

   CFE_EVS_SendEvent(TFTP_DUMP_DBG_EID, CFE_EVS_ERROR, "[00:15] %04X %04X %04X %04X",*(uint16 *)(&Tftp->MsgBuf[0]),*(uint16 *)(&Tftp->MsgBuf[4]),*(uint16 *)(&Tftp->MsgBuf[8]),*(uint16 *)(&Tftp->MsgBuf[12]));
   CFE_EVS_SendEvent(TFTP_DUMP_DBG_EID, CFE_EVS_ERROR, "[00:15] %02X%02X%02X%02X %2X%2X%2X%2X %2X%2X%2X%2X %2X%2X%2X%2X",
                     Tftp->MsgBuf[0],Tftp->MsgBuf[1],Tftp->MsgBuf[2],Tftp->MsgBuf[3],
                     Tftp->MsgBuf[4],Tftp->MsgBuf[5],Tftp->MsgBuf[6],Tftp->MsgBuf[7],
                     Tftp->MsgBuf[8],Tftp->MsgBuf[9],Tftp->MsgBuf[10],Tftp->MsgBuf[11],
                     Tftp->MsgBuf[12],Tftp->MsgBuf[13],Tftp->MsgBuf[14],Tftp->MsgBuf[15]);

   CFE_EVS_SendEvent(TFTP_DUMP_DBG_EID, CFE_EVS_ERROR, "[16:31] %04X %04X %04X %04X",*(uint16 *)(&Tftp->MsgBuf[16]),*(uint16 *)(&Tftp->MsgBuf[20]),*(uint16 *)(&Tftp->MsgBuf[24]),*(uint16 *)(&Tftp->MsgBuf[28]));
   CFE_EVS_SendEvent(TFTP_DUMP_DBG_EID, CFE_EVS_ERROR, "[16:31] %2X%2X%2X%2X %2X%2X%2X%2X %2X%2X%2X%2X %2X%2X%2X%2X",
                     Tftp->MsgBuf[16],Tftp->MsgBuf[17],Tftp->MsgBuf[18],Tftp->MsgBuf[19],
                     Tftp->MsgBuf[20],Tftp->MsgBuf[21],Tftp->MsgBuf[22],Tftp->MsgBuf[23],
                     Tftp->MsgBuf[24],Tftp->MsgBuf[25],Tftp->MsgBuf[26],Tftp->MsgBuf[27],
                     Tftp->MsgBuf[28],Tftp->MsgBuf[29],Tftp->MsgBuf[30],Tftp->MsgBuf[31]);

} /* End SendDumpBufEvent() */


/******************************************************************************
** Function: ProcessMsg
**
** Implement the TFTP protocol from a data driven perspective. This function is
** called to process messages received on the TFTP port. The transfer is 
** terminated if any errors are encountered.
**
** TODO - Add note about no port or TID(?) verification 
**
** Block number is managed so it is set to the expected value to be immediately 
** used when a packet is received and processed.  
**   GND_GET: When ground ACK received block number is at expected value
**   GND_PUT: After ACK of previous data received block number is at value for next block to be sent
**
**
*/
static boolean ProcessMsg(uint8* Buf, uint16 BufLen) {

   uint8    FileBuf[TFTP_MAX_MSGSIZE];
   uint16   BlockNum, ErrCode;
   uint32   ReadStatus;
   char    *ErrMsg;
   uint8   *Data;
   uint16   DataLen;
   boolean  MsgProcessed = TRUE;


   CFE_EVS_SendEvent(TFTP_SEND_PKT_DBG_EID, CFE_EVS_DEBUG,
                     "TFTP: ProcessMsg() - State=%d, BlockNum=%d",Tftp->State,Tftp->BlockNum);
   
   if (!DecodeOpcode(Buf, BufLen, &OpCode)) {
      MsgProcessed = FALSE;
   }
   else {

      if (OpCode == TFTP_OPCODE_ERROR) {

         if (DecodeErrorMsg(Buf, BufLen, &ErrCode, &ErrMsg)) {
            CFE_EVS_SendEvent(TFTP_DECODE_ERR_MSG_EID, CFE_EVS_ERROR, "TFTP error code %d, msg = %s\n", ErrCode, ErrMsg);
         }
         MsgProcessed = FALSE;

      } /* End if Opcode error */
      else {

         switch (Tftp->State) {
         case TFTP_STATE_IDLE:

            switch (OpCode) {
	         case TFTP_OPCODE_RRQ:
               /* 
               ** Decode RRQ, Open file and send first data packet 
               ** Don't increment block number so ACK will verify block number sent 
               */
               if (DecodeRequestStrings(Buf, BufLen, Tftp->SrcFilename,Tftp->Mode)) {
                  
                  Tftp->FileHandle = OS_open(Tftp->SrcFilename, OS_READ_ONLY, 0);
                  if (Tftp->FileHandle >= 0) {
                    
                     Tftp->BlockNum = 1;
                     ReadStatus = OS_read(Tftp->FileHandle, FileBuf, TFTP_MAX_DATA_LEN);
                     SendPkt(TFTP_OPCODE_DATA, Tftp->BlockNum, FileBuf, ReadStatus);
                  }
                  else {
                     CFE_EVS_SendEvent(TFTP_RRQ_FILE_OPEN_ERR_EID, CFE_EVS_ERROR,
                                       "TFTP error opening RRQ file %s. Status = 0x%4x",
                                       Tftp->SrcFilename, Tftp->FileHandle);
                  }

                  Tftp->State = (ReadStatus == TFTP_MAX_DATA_LEN) ? TFTP_STATE_GET : TFTP_STATE_GET_FINI;

               } /* End if decoded request strings */
               break;
			 
            case TFTP_OPCODE_WRQ:
               /* Decode WRQ, Create file and send ack. Note block number is zero in this special case. */
               if (DecodeRequestStrings(Buf, BufLen, Tftp->DestFilename,Tftp->Mode)) {
                  Tftp->FileHandle = OS_creat(Tftp->DestFilename, OS_WRITE_ONLY);
                  if (Tftp->FileHandle >= 0) {
                     Tftp->BlockNum = 0;
                     CFE_EVS_SendEvent(TFTP_WRQ_DBG_EID, CFE_EVS_DEBUG, "TFTP sending WRQ ack. Block number = %d\n", Tftp->BlockNum);
                     SendPkt(TFTP_OPCODE_ACK, Tftp->BlockNum, NULL, 0);
                     CFE_EVS_SendEvent(TFTP_WRQ_DBG_EID, CFE_EVS_DEBUG, "TFTP sent WRQ ack. Block number = %d\n", Tftp->BlockNum);
                     Tftp->BlockNum++;
                  }
                  else {
                     CFE_EVS_SendEvent(TFTP_WRQ_FILE_OPEN_ERR_EID, CFE_EVS_ERROR,
                                       "TFTP error opening WRQ file %s Status = %d\n",
                                       Tftp->DestFilename, Tftp->FileHandle);
                  } /* End if fiel create error */

                  Tftp->State = TFTP_STATE_PUT;
               } /* End if decoded request strings */
               break;
            default:
               CFE_EVS_SendEvent(TFTP_UNEXP_OPCODE_ERR_EID, CFE_EVS_ERROR, "TFTP unexpected opcode in idle state %d ignored\n", OpCode);
            } /* End Opcode switch */
            break; /* End TFTP_STATE_IDLE */

         case TFTP_STATE_GET:
	    	    /* Verify Ack opcode, read data from file and send it */
            if (OpCode == TFTP_OPCODE_ACK) {
               /*  DecodeBlockNum() sends event if error */
               if (DecodeBlockNum(Buf, BufLen, &BlockNum)) {
                  if (BlockNum == Tftp->BlockNum) {
	
                     ReadStatus = OS_read(Tftp->FileHandle, FileBuf, TFTP_MAX_DATA_LEN);
                     Tftp->BlockNum++;

                     if (ReadStatus > 0) {    /* More file data */
                        SendPkt(TFTP_OPCODE_DATA, Tftp->BlockNum, FileBuf, ReadStatus);
                        if (ReadStatus < TFTP_MAX_DATA_LEN) {
                           Tftp->State = TFTP_STATE_GET_FINI;
                        }
                        else {
                           Tftp->State = TFTP_STATE_GET;
                        } /* End if max data */
                        Tftp->Timer = 0;
                     }
                     else { 
                        /* Special case where file ends on block boundries */
                        /* TODO handle read error < 0) */
                        SendPkt(TFTP_OPCODE_DATA, Tftp->BlockNum, FileBuf, 0);
                        Tftp->State = TFTP_STATE_GET_FINI;
                     }
                  } /* End if valid block num */
                  else {
                     CFE_EVS_SendEvent(TFTP_UNEXP_BLOCKNUM_ERR_EID, CFE_EVS_ERROR, "TFTP unexpected block number in ack packet. Expect %d, Received %d",Tftp->BlockNum, BlockNum);
                  } /* End if invalid block num */
               } /* End if DecodeBlockNum() */
            } /* End if Ack Opcode */
            else {
               CFE_EVS_SendEvent(TFTP_UNEXP_OPCODE_ERR_EID, CFE_EVS_ERROR, "TFTP unexpected opcode %d when expecting an Ack", OpCode);
               CompleteFileTransfer();
               MsgProcessed = FALSE;
            } /* Invalid Opcode */
            break; /* End TFTP_STATE_GET */

         case TFTP_STATE_GET_FINI:
            if (OpCode == TFTP_OPCODE_ACK) {
               Tftp->GetFileCnt++;
               CFE_EVS_SendEvent(TFTP_GND_GET_COMPLETE_EID, CFE_EVS_INFORMATION,
                                 "TFTP successfully transferred a %d block file", Tftp->BlockNum);
            }
            else {
               CFE_EVS_SendEvent(TFTP_UNEXP_OPCODE_ERR_EID, CFE_EVS_ERROR, "TFTP unexpected opcode %d when expecting an Ack", OpCode);
            }		
            CompleteFileTransfer();   
            break;
	      
         case TFTP_STATE_PUT:
	    	    /* Verify data opcode, write data to file and ack it */
            if (OpCode == TFTP_OPCODE_DATA) {
               if (DecodeBlockNum(Buf, BufLen, &BlockNum)) {
                  if (BlockNum == Tftp->BlockNum) {
                     if (DecodeData(Buf, BufLen, &Data, &DataLen)) {
                        /* TODO - Verify file write, Fix buffers */
                        if (DataLen > 0) { /* no more data */
                           OS_write(Tftp->FileHandle,Data,DataLen);
                        }
                        SendPkt(TFTP_OPCODE_ACK, Tftp->BlockNum, NULL, 0);
                        CFE_EVS_SendEvent(TFTP_WRQ_DBG_EID, CFE_EVS_DEBUG, "TFTP sending ACK for block number = %d\n", Tftp->BlockNum);
                        /* TODO - May want tlm to indicate last file transferred. Pass parameter to complete file transfer */
                        if (DataLen != TFTP_MAX_DATA_LEN) {
                           Tftp->PutFileCnt++;
                           CompleteFileTransfer();
                           CFE_EVS_SendEvent(TFTP_GND_PUT_COMPLETE_EID, CFE_EVS_INFORMATION,
                                             "TFTP successfully received a %d block file", Tftp->BlockNum);
                        }
                        else {
                           Tftp->BlockNum++;
                        }
                     } /* End if DecodeData() */
                  } /* End if valid block num */
                  else {
                     CFE_EVS_SendEvent(TFTP_UNEXP_BLOCKNUM_ERR_EID, CFE_EVS_ERROR,
                                       "TFTP unexpected block number in data packet. Expect %d, Received %d",Tftp->BlockNum, BlockNum);
                  } /* End if invalid block num */
               } /* End if DecodeBlockNum() */
            } /* End if DATA OpCode */
            else {
               CFE_EVS_SendEvent(TFTP_UNEXP_OPCODE_ERR_EID, CFE_EVS_ERROR, "TFTP unexpected opcode %d when expecting data", OpCode);
               CompleteFileTransfer();
               MsgProcessed = FALSE;
            } /* Invalid Opcode */
            break; /* End TFTP_STATE_PUT */

         default:
            CFE_EVS_SendEvent(TFTP_UNDEF_STATE_ERR_EID, CFE_EVS_ERROR, "TFTP undefined state %d while processing opcode %d", Tftp->State, OpCode);
            CompleteFileTransfer(); /* TODO - Only if not idle */
            Tftp->State = TFTP_STATE_IDLE;
            MsgProcessed = FALSE;

         } /* End State switch */
      } /* End if TFTP_OPCODE_ERROR */ 
   } /* End if DecodeOpcode() */
	
   return MsgProcessed;

} /* End ProcessMsg() */
