/*
** Purpose: Define Example JSMN Table
**
** Notes:
**   1. Two constructors are provided to allow an example table to be declared
**      with or without a default table.  
**   2. This example only has one JSON object. In most cases there would be 
**      multiple objects. Creating/initializing during object constructor seems
**      clumsy but I don't see a way around it. 
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
#ifndef _ex_jsmn_tbl_
#define _ex_jsmn_tbl_

/*
** Includes
*/

#include "AppCfg.h"
#include "Tbl.h"
#include "ExTblStruct.h"
#include "Json.h"
#include "JsonTblObj.h"


class ExJsmnTbl: public oskfw::Tbl {
public:

   ExJsmnTbl();
   ExJsmnTbl(const std::string& filename);

   void ResetState();

   boolean get_entry(int id, ExTblEntry* entry) { return m_struct.get_entry(id,entry); }

   oskfw::Json* json() { return &m_json; }
   
private:

   void Init();

   class JsonEntryObj : public oskfw::JsonTblObj {
   public:
      JsonEntryObj(ExJsmnTbl* ex_tbl, const std::string& name) : m_ex_tbl(ex_tbl), oskfw::JsonTblObj(name) {}
      bool ProcessToken(oskfw::Json* p_json, int token_idx);
   private:
      ExJsmnTbl* m_ex_tbl;
   };

   boolean LoadContent (oskfw::Tbl::LoadOption load_option, const std::string& p_filename);
   boolean DumpContent (const std::string& p_filename);
   
   // Table parameters
   
   ExTblParam  m_param;   // Values used by the app
   ExTblStruct m_struct;  // Serves as the load buffer

   // JSON table objects

   JsonEntryObj m_json_entry_obj;   

   // JSON table data

   oskfw::Json  m_json;
   
   uint16   m_attr_err_cnt;
   uint16   m_data_array_entry_idx;

};


#endif // _ex_jsmn_tbl_ 

