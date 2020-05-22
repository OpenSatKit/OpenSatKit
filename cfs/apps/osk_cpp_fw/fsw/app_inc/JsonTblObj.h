/*
** Purpose: Define a JSON Table Object class
**
** Notes:
**   1. A JSON object corresponds to a 'table entry'. This is a virtual class
**      that should be subclassed for each table entry. The subclass has 
**      knowledge of the JSON keywords/structure and parses the JSON object. 
**       
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

#ifndef _json_tbl_obj_
#define _json_tbl_obj_

/*
** Includes
*/

#include <string> 
#include "Tbl.h"

namespace oskfw {

class Json; // Forward declaration

class JsonTblObj {
public:
   
   JsonTblObj(const std::string& p_name);

   virtual bool ProcessToken(Json* p_json, int token_idx) = 0;
   
   std::string name() { return m_name; }
   void set_name(std::string& p_name) { m_name = p_name; }
      
   bool modified() { return m_modified; }
   void set_modified(bool new_state) { m_modified = new_state; }
   
protected:

   bool         m_modified;
   std::string  m_name;
   
}; // End class JsonTblObj

} // End namespace oskfw

#endif // _json_tbl_obj_
