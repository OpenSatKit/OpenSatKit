/*
** Purpose: Define the base application class
**
** Notes:
**   1. This is an abstract base class and pure virtual functions are used to force the
**      compiler to generated the internal vtable and typeinfo data structures. If non
**      pure-virtual are used the compiler can make assumptions and the symbols may be
**      ommited.
**   2. Init() must be called prior to Execute() or as part of Execute's infinite run
**      loop. See Developer's Guide. Constructors shouldn't call a virtual functions
**      which is why it's not automatically called.  
**   3. See AppFwVer.h for version numbering converntion.
**   4. App Services
**      - By default all apps have a CmdMgr, TblMgr and a JSON parser.  
**      - An app is not required to have a table
**      - An app may have 0..N JSON parsers. The app is responsible
**        for setting the Json pointer to the instance that should be
**        manipulated by App Framework commands.
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

#ifndef _app_
#define _app_

/*
** Includes
*/

#include "OskCppFwCfg.h"
#include "CmdMgr.h"
#include "TblMgr.h"
#include "Json.h"

namespace oskfw {
   

// Numeric ID for each application framework component. Specify size for external interfaces

enum AppCompId : uint8 {

   CMD_MGR  = 1,
   TBL_MGR  = 2,
   JSON     = 3
   
};

// Commands

// Typically debug one aspect of an app at a time so enabling/disabling one 
// app framework component at a time as opposed to something like a bitmask
// to configure multiple components is not necessary
 
typedef struct {
   
   uint8      cmd_header[CFE_SB_CMD_HDR_SIZE]; // cFE Software Bus Command Message Header
   AppCompId  app_comp_id;
   boolean    enabled;
   
} APP_DebugCmdMsg;
#define APP_DEBUG_CMD_DATA_LEN  (sizeof(APP_DebugCmdMsg) - CFE_SB_CMD_HDR_SIZE)


class App {
public:

   App(int major_ver, int minor_ver, int local_rev);
   virtual ~App();
   
   virtual int32   Init() = 0;
   virtual void Execute() = 0;

protected:
   
   virtual boolean ResetState() = 0;

   class NoopCmd : public CmdMsg {
   public:
   
      NoopCmd(App& app);
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);
   private:
      App& m_app;
   };

   class ResetCmd : public CmdMsg {
   public:
   
      ResetCmd(App& app);
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);
   private:
      App& m_app;
   };
   
   class DebugCmd : public CmdMsg {
   public:
   
      DebugCmd(App& app, uint16 data_len);
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);
   private:
      App& m_app;
   };

   int m_major_ver;
   int m_minor_ver;
   int m_local_rev;
   
   // App Commands
   
   NoopCmd   m_noop_cmd;
   ResetCmd  m_reset_cmd;
   DebugCmd  m_debug_cmd;
   
   // App Services
  
   CmdMgr  m_cmd_mgr;
   TblMgr  m_tbl_mgr;
   Json*   m_json;     
   
   CFE_SB_PipeId_t m_cmd_pipe;

}; // End class App

} // End namespace oskfw

#endif // _app_ 
