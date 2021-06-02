/*
** Purpose: Implement rapid JSON table.
**
** Notes:
**   1. This is for protostration purposes. More error checking would be required 
**      for FSW.
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

#include <string.h>
#include "ExRapidTbl.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include <iostream>

using std::string;
using namespace rapidjson;

// Event Message IDs

enum EID {
   
   EID_CREATE_DMP_FILE_ERR = (AppCfg::EXTBL_BASE_EID),
   EID_LOAD_CMD_DBG,
   EID_LOAD_CMD_INVALID_TYPE, 
   EID_LOAD_CMD_FILE_OPEN_ERR,
   EID_LOAD_CMD_FILE_READ_ERR,
   EID_LOAD_CMD_JSON_PARSE_ERR,
   EID_DUMP_CMD_DBG,
   EID_INVALID_ENTRY_ID,

   // JsonEntryObj   
   EID_PROCESS_TOKEN,
   EID_PROCESS_TOKEN_ENTRY,
   EID_PROCESS_TOKEN_INDEX_ERR,
   EID_PROCESS_TOKEN_ELEMENT_ERR

};

static const char JSON_OBJ_ARRAY[] = "data-array";
static const char JSON_OBJ_ENTRY[] = "entry";
static const char JSON_KEY_INDEX[] = "index";
static const char JSON_KEY_DATA1[] = "data1";
static const char JSON_KEY_DATA2[] = "data2";
static const char JSON_KEY_DATA3[] = "data3";


/******************************************************************************
** Constructors
**
** Notes:
**    1. A table can be created with or without initial values from a file.
**
*/

ExRapidTbl::ExRapidTbl() : oskfw::Tbl() {

   Init();

}

ExRapidTbl::ExRapidTbl(const string& p_filename)  : oskfw::Tbl () {

   Init();
   Load(LOAD_REPLACE,p_filename);

}


void ExRapidTbl::Init() {
   
   ResetState();
   
} 


/******************************************************************************
** Function: ResetState
**
** Notes:
**    1. Reset counters and status flags to a known reset state for telemetry.
*/
void ExRapidTbl::ResetState(void) {

   m_attr_err_cnt         = 0;
   m_data_array_entry_idx = 0;
   
   m_struct.Reset();   
 
} // End ResetState()


/******************************************************************************
** Function: LoadContent
**
** Notes:
**   1. Can assume valid table file name because base class has verified the
**      file prior to calling this virtual function.
*/
boolean ExRapidTbl::LoadContent (oskfw::Tbl::LoadOption load_opt, const std::string& filename) {

   boolean ret_status = FALSE;
   int32 file_handle;
   
   // Create File object
   CFE_EVS_SendEvent(EID_LOAD_CMD_DBG, CFE_EVS_DEBUG,"ExRapidTbl::LoadContent(%s)\n",filename.c_str());

   ResetState();  // Clear entry modified flags & attribute counters

   file_handle = OS_open(filename.c_str(), OS_READ_ONLY, 0);

   if (file_handle >= 0) {
   
      int32 read_status;
      char json_buffer[AppCfg::EXTBL_JSON_CHAR_MAX];
      Document doc;

      read_status = OS_read(file_handle, json_buffer, AppCfg::EXTBL_JSON_CHAR_MAX);
      CFE_EVS_SendEvent(EID_LOAD_CMD_DBG, CFE_EVS_DEBUG,"ExRapidTbl::LoadContent()-read_status=%d\n", read_status);

      if (read_status > 0) {

         doc.Parse(json_buffer);
   
         CFE_EVS_SendEvent(EID_LOAD_CMD_DBG, CFE_EVS_DEBUG, "ExRapidTbl::LoadContent()-Description=%s\n",doc["description"].GetString());

         const Value& data_array = doc["data-array"];
         if (data_array.IsArray()) {
          
            for (SizeType i = 0; i < data_array.Size(); i++) { // Uses SizeType instead of size_t
               for (Value::ConstMemberIterator array_itr = data_array[i].MemberBegin(); array_itr != data_array[i].MemberEnd(); ++array_itr) {
             
                  CFE_EVS_SendEvent(EID_LOAD_CMD_DBG, CFE_EVS_DEBUG,
                                    "ExRapidTbl::LoadContent()-Type of member %s is %d\n", array_itr->name.GetString(), array_itr->value.GetType());
                  
                  const rapidjson::Value& entry = array_itr->value;
                  if (entry.IsObject()) {
                  
                     const rapidjson::Value& j_index = array_itr->value[JSON_KEY_INDEX];                    
                     const rapidjson::Value& j_data1 = array_itr->value[JSON_KEY_DATA1];
                     const rapidjson::Value& j_data2 = array_itr->value[JSON_KEY_DATA2];
                     const rapidjson::Value& j_data3 = array_itr->value[JSON_KEY_DATA3];

                     if ( j_index.IsInt() && j_data1.IsInt() && j_data2.IsInt() && j_data3.IsInt()) { 
                     
                        int index = j_index.GetInt();
                        int data1 = j_data1.GetInt();
                        int data2 = j_data2.GetInt();
                        int data3 = j_data3.GetInt();
                        
                        if (index < AppCfg::EXTBL_MAX_ENTRY_ID) {
                              
                           m_struct.set_entry(index,data1, data2, data3);
                           CFE_EVS_SendEvent(EID_PROCESS_TOKEN_ENTRY,CFE_EVS_DEBUG,
                                             "ExRapidTbl::LoadContent() index, data1, data2, datat3: %d, %d, %d, %d\n",
                                             index, data1, data2, data3);
                        }
                        else {
                              
                           m_attr_err_cnt++;     
                           CFE_EVS_SendEvent(EID_LOAD_CMD_JSON_PARSE_ERR, CFE_EVS_ERROR, 
                                             "Load file data-array entry %d error. Entry index %d is from 0 to %d", 
                                             i, index, (AppCfg::EXTBL_MAX_ENTRY_ID-1));
                        }
                     }
                     else {
         
                        m_attr_err_cnt++;
                           CFE_EVS_SendEvent(EID_LOAD_CMD_JSON_PARSE_ERR, CFE_EVS_ERROR, 
                                             "Load file data-array entry %d error. Entry contains an invalid index or data definition", i);
                     
                     }
                     
                  } // End if entry object
                  else {
               
                     m_attr_err_cnt++;
                     CFE_EVS_SendEvent(EID_LOAD_CMD_JSON_PARSE_ERR, CFE_EVS_ERROR,
                                       "Load file data-array entry %d error. Entry not defined as an object", i);
                  }
               } // End entry loop
            } // End data-array loop   
         
            if (m_attr_err_cnt == 0) {
               if (load_opt == oskfw::Tbl::LOAD_REPLACE) {
               
                  CFE_EVS_SendEvent(EID_LOAD_CMD_DBG,CFE_EVS_DEBUG,"ExRapidTbl: Replacing entire table");
                  m_struct.get_param(&m_param);
                  
               } /* End if replace entire table */
               else if (load_opt == oskfw::Tbl::LOAD_UPDATE) {
               
                  // Copy with no validity checks so make valid true
                  m_last_action_valid = TRUE;
                  m_struct.get_modified_entries(&m_param);

               } /* End if update individual records */
               else {
                  CFE_EVS_SendEvent(EID_LOAD_CMD_INVALID_TYPE,CFE_EVS_ERROR,"ExRapidTbl: Invalid table command load type %d",load_opt);
               }

               ret_status = TRUE;

            } // End if attribute error
            // No need for else event message since each attribute error has an event message  
            
         } // End valid data-array type
         else {
            m_attr_err_cnt++;
            CFE_EVS_SendEvent(EID_LOAD_CMD_JSON_PARSE_ERR, CFE_EVS_ERROR, "JSON syntax error. %s is not defined as an array", JSON_OBJ_ARRAY);
         }

      } // End file read
      else {
         CFE_EVS_SendEvent(EID_LOAD_CMD_FILE_READ_ERR, CFE_EVS_ERROR, "Error reading file %s. Status=%d",
                           filename.c_str(), read_status);
      }

      OS_close (file_handle);
      
   } // End file open
   else {
      CFE_EVS_SendEvent(EID_LOAD_CMD_FILE_OPEN_ERR, CFE_EVS_ERROR, "Error opening file %s. Status=%d",
                        filename.c_str(), file_handle);
   }

   return ret_status;
    
} // End LoadContent()


/******************************************************************************
** Function: DumpContent
**
** Notes:
**   1. This is a duplicate of ExTbl's DumpContent() since it's a 
**      quick and dirty prototype.
**   2. Dump the work buffer 'm_param' and not the load buffer 'm_struct'
**
*/
boolean ExRapidTbl::DumpContent (const std::string& filename) {

   boolean  ret_status = FALSE;
   int32    file_handle;
   char     dump_record[256];
   int      i;

   CFE_EVS_SendEvent(EID_DUMP_CMD_DBG, CFE_EVS_DEBUG,
                     "ExRapidTbl: DumpContent() Entry. File %s", filename.c_str());

   file_handle = OS_creat(filename.c_str(), OS_WRITE_ONLY);

   if (file_handle >= OS_FS_SUCCESS) {


      sprintf(dump_record,"{\n\"name\": \"Example RapidJson Table\",\n");
      OS_write(file_handle,dump_record,strlen(dump_record));

      sprintf(dump_record,"\"description\": \"Example table for object-based application template.\",\n");
      OS_write(file_handle,dump_record,strlen(dump_record));

      sprintf(dump_record,"\"data-array\": [\n");
      OS_write(file_handle,dump_record,strlen(dump_record));
 
      /* 
      ** Can't end last record with a comma so start of for loop finishes 
      ** previous record's line with ",\n" so write first line before the
      ** loop starts.      
      */
      sprintf(dump_record,"{\"entry\": {\n  \"index\": %4d,\n  \"data1\": %4d,\n  \"data2\": %4d,\n  \"data3\": %4d\n}}",
              0, m_param.entry[0].data1, m_param.entry[0].data2, m_param.entry[0].data3);
      OS_write(file_handle,dump_record,strlen(dump_record));
      
      for (i=1; i < AppCfg::EXTBL_MAX_ENTRY_ID; i++) {
      
         sprintf(dump_record,",\n");
         OS_write(file_handle,dump_record,strlen(dump_record));

         sprintf(dump_record,"{\"entry\": {\n  \"index\": %4d,\n  \"data1\": %4d,\n  \"data2\": %4d,\n  \"data3\": %4d\n}}",
                 i, m_param.entry[i].data1, m_param.entry[i].data2, m_param.entry[i].data3);
         OS_write(file_handle,dump_record,strlen(dump_record));
      
      } /* End Entry loop */
 
      sprintf(dump_record,"]\n}\n");
      OS_write(file_handle,dump_record,strlen(dump_record));

      /* TODO - Add addition meta data when file dumped */
      ret_status = TRUE;

      OS_close(file_handle);

   } /* End if file create */
   else
   {
   
      CFE_EVS_SendEvent(EID_CREATE_DMP_FILE_ERR, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", filename.c_str(), file_handle);
   
   } /* End if file create error */

   return ret_status;
   
} // End DumpContent()

