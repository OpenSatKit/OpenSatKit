/*
** Purpose: Define a Table that uses the Rapid JSON parser
**
** Notes:
**   1. The purpose of this class is to quickly evaluate the use of the rapid
**      JSON parser so it uses ExTbl.h definitions. May not be a real clean
**      setup but that's not the goal.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _ex_rapid_tbl_
#define _ex_rapid_tbl_

/*
** Includes
*/

#include "AppCfg.h"
#include "Tbl.h"
#include "ExTblStruct.h"

/******************************************************************************
** Type Definitions
*/

class ExRapidTbl: public oskfw::Tbl {
public:

   ExRapidTbl();
   ExRapidTbl(const std::string& filename);

   void ResetState();

   boolean get_entry(int id, ExTblEntry* entry) { return m_struct.get_entry(id,entry); }
   
private:

   void Init();

   boolean LoadContent (oskfw::Tbl::LoadOption load_option, const std::string& filename);
   boolean DumpContent (const std::string& filename);
   
   // Table parameters
   
   ExTblParam  m_param;   // Values used by the app
   ExTblStruct m_struct;  // Serves as the load buffer

   //~ JSON
   
   uint16   m_attr_err_cnt;
   uint16   m_data_array_entry_idx;

};


#endif /* _ex_rapid_tbl_ */

