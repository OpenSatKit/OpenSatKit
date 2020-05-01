/*
** Purpose: Define the application object base class 
**
** Notes:
**   1. AppObj is an abstract base class subclassed by any object that uses app
**      framework services. It's architectural role is similar to a GUI widget.
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

#ifndef _app_obj_
#define _app_obj_

/*
** Includes
*/

#include "CmdMgr.h"
#include "TblMgr.h"


namespace oskfw {
   
class AppObj {
public:
      
   AppObj(uint16 event_id_base);
   virtual ~AppObj();
   
   virtual uint8 RegisterCmds (CmdMgr& cmd_mgr) = 0;  // Return number of commands registered
   virtual uint8 RegisterTbls (TblMgr& tbl_mgr) = 0;  // Return number of tables registered

   virtual void Execute() = 0;    // Called at application's 'execution rate'
   virtual void Monitor() = 0;    // Called at application's 'housekeeping rate'
   
   virtual void ResetState() = 0; // Reset to a known state
     
  

protected:

   uint16 m_event_id_base;

}; // End class AppObj


} // End namespace oskfw

#endif /* _app_obj_ */
