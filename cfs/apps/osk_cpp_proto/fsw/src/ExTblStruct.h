/*
** Purpose: Define Example Table Data structure
**
** Notes:
**   1. If methods are defined they should only manipulate the data and
**      not provide any functional behavior. Even the concept of valid
**      data is left up to clients.
**   2. Use 'struct' rather than a generic term like 'data' in the type name
**      to indicate it can have memeber functions.  
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
#ifndef _ex_tbl_struct_
#define _ex_tbl_struct_

extern "C" { 
   #include "cfe.h"
}

#include <cstring>
#include "AppCfg.h"
#include "ExTblData.h"


struct ExTblStruct {
public:

   ExTblStruct();

   void Reset();

   bool get_entry(int id, ExTblEntry* entry);
   bool set_entry(int id, const ExTblEntry* entry);
   bool set_entry(int id, uint16 data1, uint16 data2, uint16 data3);

   void get_param(ExTblParam* param) { memcpy(param,&m_param,sizeof(ExTblParam)); }
   void set_param(const ExTblParam* param);
   void get_modified_entries(ExTblParam* param);
 
   bool ValidEntryId(int id) { return (id >= 0 && id < AppCfg::EXTBL_MAX_ENTRY_ID); }
      
private: 

   ExTblParam  m_param;
   bool        m_modified[AppCfg::EXTBL_MAX_ENTRY_ID];
   
};

#endif // _ex_tbl_struct_

