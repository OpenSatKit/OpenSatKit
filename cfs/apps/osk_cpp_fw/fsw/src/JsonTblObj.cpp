/*
** Purpose: Implement JSON Table Object Entry class
**
** Notes:
**   None
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

/*
** Include Files:
*/

#include <string.h>
#include "JsonTblObj.h"
#include "Json.h"


namespace oskfw {

/******************************************************************************
** 
*/
JsonTblObj::JsonTblObj(const std::string& p_name)  {

   m_modified = false;
   m_name     = p_name;   
   
   if (oskfw::Cfg::JSON_DEBUG) OS_printf("JsonTblObj() p_name = %s, m_name = %s\n", p_name.c_str(), m_name.c_str());
   
}

bool JsonTblObj::ProcessToken(Json* p_json, int token_idx) { return false; }

} // End namespace oskfw
