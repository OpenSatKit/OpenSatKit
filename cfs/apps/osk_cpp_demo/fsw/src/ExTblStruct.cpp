/*
** Purpose: Implement example table.
**
** Notes:
**   None
**      
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

#include <string>
#include "ExTblStruct.h"
using std::string;


/******************************************************************************
**
*/

ExTblStruct::ExTblStruct() {

   Reset();
   
}


void ExTblStruct::Reset() {

   memset((void *)&m_param,0,sizeof(ExTblParam));
   
   for (int entry=0; entry < AppCfg::EXTBL_MAX_ENTRY_ID; entry++) m_modified[entry] = false;
   
}


/******************************************************************************
** Get/Set
**
** Notes:
**   1. Since this is a data structure let caller send event for erroneous entry_id. 
*/
bool ExTblStruct::get_entry(int id, ExTblEntry* entry) {
   
   bool ret_status = false;
    
   if (ValidEntryId(id)) {      
   
      memcpy(entry,&m_param.entry[id],sizeof(ExTblEntry));      
      ret_status = true;      
   
   }      
      
   return ret_status;
      
} // get_entry()


bool ExTblStruct::set_entry(int id, const ExTblEntry* entry) {
   
   bool ret_status = false;
    
   if (ValidEntryId(id)) {
      
      memcpy(&m_param.entry[id],entry,sizeof(ExTblEntry));      
      m_modified[id]    = true;   
      ret_status = true;
       
   }      
      
   return ret_status;
      
} // set_entry()


bool ExTblStruct::set_entry(int id, uint16 data1, uint16 data2, uint16 data3) {

   bool ret_status = false;
    
   if (ValidEntryId(id)) {
      
      m_param.entry[id].data1 = data1;
      m_param.entry[id].data2 = data2;
      m_param.entry[id].data3 = data3;
      m_modified[id] = true;
      
      ret_status = true;
      
   }      
      
   return ret_status;

} // End set_entry()


void ExTblStruct::set_param(const ExTblParam* param) {

   memcpy(&m_param,param,sizeof(ExTblParam));

   for (int entry=0; entry < AppCfg::EXTBL_MAX_ENTRY_ID; entry++) m_modified[entry] = true;
   
} // End set_param()


void ExTblStruct::get_modified_entries(ExTblParam* param) {

   for (int entry=0; entry < AppCfg::EXTBL_MAX_ENTRY_ID; entry++) {

      if (m_modified[entry]) {
      
         CFE_EVS_SendEvent(AppCfg::EX_APP_DEBUG, CFE_EVS_DEBUG, "ExTblStruct: Replacing table entry %d",entry);
         param->entry[entry] = m_param.entry[entry];
      
      }
   } // End entry loop

} // End get_modfied_entries()
