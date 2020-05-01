/*
** Purpose: Define command message class
**
** Notes:
**   1. This does not define the command message content. That is defined by
**      the cFE. This class defines a functional interface for processing
**      command messages.
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

#ifndef _cmd_msg_
#define _cmd_msg_

/*
** Includes
*/

extern "C" {
   #include "cfe.h"
}
#include "OskCppFwCfg.h"


namespace oskfw {


// Define some common command types for convenience 

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CmdMsg_Param0;
#define CMD_MSG_PARAM0_DATA_LEN  (sizeof(CmdMsg_Param0) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16  Param;

} CmdMsg_Param16;
#define CMD_MSG_PARAM16_DATA_LEN  (sizeof(CmdMsg_Param16) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint32  Param;

} CmdMsg_Param32;
#define CMD_MSG_PARAM32_DATA_LEN  (sizeof(CmdMsg_Param32) - CFE_SB_CMD_HDR_SIZE)


class CmdMsg {
public:
   
   CmdMsg();
   CmdMsg(uint16 data_len);
   ~CmdMsg();
   
   virtual boolean Process(const CFE_SB_MsgPtr_t msg_ptr) = 0;
   
   boolean ValidDataLen(uint16 data_len) { return data_len == m_data_len; }   
   uint16  data_len()                    { return m_data_len; }
   void    set_data_len(uint16 data_len) { m_data_len = data_len; }
   
private:

   uint16   m_data_len;   // User data length in bytes (not the packet length)
      
}; // End class CmdMsg


} // End namespace oskfw

#endif // _cmd_msg_
