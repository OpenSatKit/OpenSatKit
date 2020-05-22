/*
** Purpose: Define Table 
**
** Notes:
**   1. This is an abstract base class that provides table file management
**      and load/dump command interfaces. The derived class provides table
**      content processing.
**   2. A partial table load command is supported but it is up to the derived
**      class to define how the replace table option behaves when all of the
**      table elements are not defined.
**   3. Enumerations have storage specifications for cmd/tlm definitions
**   4. C++ std::string is used to store filenames so max file length is not 
**      checked until the framework utilities are invoked. However the filename
**      is updated in this class. A Filename class is being considered that 
**      would serve as the flight-ground and cFE-C++ interface manager.
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

#ifndef _tbl_
#define _tbl_

/*
** Includes
*/

#include <string> 
extern "C" {
   #include "cfe.h"
}

namespace oskfw {

/******************************************************************************
** Table Class
*/
class Tbl {
public:
   
   // Table actions
   
   enum LastAction : uint8 {
      ACTION_UNDEF   = 0,
      ACTION_CREATED = 1,
      ACTION_LOAD    = 2,
      ACTION_DUMP    = 3
   };

   // Load Type options

   enum LoadOption : uint8 {
      LOAD_REPLACE = 0,   // Replace the entire table
      LOAD_UPDATE  = 1    // Update individual entries
   };
   
   // Load Type options

   enum DumpOption  : uint8 {
      DUMP_ALL = 0
   };

   Tbl();
   virtual ~Tbl();
   
   boolean Load (LoadOption load_opt, const std::string& p_filename);
   boolean Dump (DumpOption dump_opt, const std::string& p_filename);
   
   LastAction last_action()       const { return m_last_action;      }
   boolean    last_action_valid() const { return m_last_action_valid; }
   
   // Returns a pointer to a std::string that describes the last load option performed 
   const std::string LoadOptionStr(LoadOption load_opt);
   
   std::string& filename() { return m_filename; }
   
protected:

   virtual boolean LoadContent (LoadOption load_opt, const std::string& p_filename) = 0;
   virtual boolean DumpContent (const std::string& p_filename) = 0;

   LastAction m_last_action;
   boolean    m_last_action_valid;

private:

   // Tables shouldn't need to be copied
   Tbl(const Tbl &p_tbl) {}  
   Tbl& operator=(const Tbl &p_tbl) {} 
   
   boolean     m_loaded;
   std::string m_filename;  // Contains filename used for the last operation
   
}; // End class Tbl

} // End namespace oskfw

#endif // _tbl_ 
