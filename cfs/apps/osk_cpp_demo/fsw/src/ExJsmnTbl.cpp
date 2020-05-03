/*
** Purpose: Implement example JSMN table.
**
** Notes:
**   1. The JSON parsing is done in the JsonEntryObj. A 
**      new class shodl be derived for each type of JSON table
**      object that needs to be parsed.
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
#include "ExJsmnTbl.h"
using std::string;


// Event Message IDs

enum EID {
   
   EID_CREATE_DMP_FILE_ERR = (AppCfg::EXTBL_BASE_EID),
   EID_LOAD_CMD_DBG,
   EID_LOAD_CMD_INVALID_TYPE, 
   EID_LOAD_CMD_PARSE_ERR,
   EID_LOAD_CMD_JSON_OPEN_ERR,
   EID_DUMP_CMD_DBG,
   EID_INVALID_ENTRY_ID,

   // JsonEntryObj   
   EID_PROCESS_TOKEN,
   EID_PROCESS_TOKEN_ENTRY,
   EID_PROCESS_TOKEN_INDEX_ERR,
   EID_PROCESS_TOKEN_ELEMENT_ERR

};

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

ExJsmnTbl::ExJsmnTbl() : oskfw::Tbl(), m_json_entry_obj(this, JSON_OBJ_ENTRY)  {

   Init();

}

ExJsmnTbl::ExJsmnTbl(const string& p_filename)  : oskfw::Tbl (), m_json_entry_obj(this, JSON_OBJ_ENTRY)  {

   Init();
   Load(LOAD_REPLACE,p_filename);

}


void ExJsmnTbl::Init() {
   
   ResetState();
   
   m_json.RegisterTblObj(&m_json_entry_obj);

} 


/******************************************************************************
** Function: ResetState
**
** Notes:
**    1. Reset counters and status flags to a known reset state for telemetry.
*/
void ExJsmnTbl::ResetState(void) {

   m_attr_err_cnt         = 0;
   m_data_array_entry_idx = 0;
      
   m_struct.Reset();
    
} // End ResetState()



/******************************************************************************
** Function: LoadContent
**
** Notes:
**  1. Can assume valid table file name because base class has verified the
**     file prior to calling this virtual function.
*/
boolean ExJsmnTbl::LoadContent (oskfw::Tbl::LoadOption load_opt, const std::string& filename) {

   int entry;
   
   CFE_EVS_SendEvent(EID_LOAD_CMD_DBG, CFE_EVS_DEBUG,
                     "ExJsmnTbl: LoadContent() Entry. Option %d, File %s", load_opt, filename.c_str());

   /*
   ** Set all data and flags to zero. If a table replace is commanded and
   ** all of the data is not defined the zeroes will be copied into the table. 
   ** Real flight code would validate all data is loaded for a replace.
   */
   
   ResetState();  // Clear modified flags and helps isolate errors if they occur
   
   if (m_json.OpenFile(filename)) {
  
      CFE_EVS_SendEvent(EID_LOAD_CMD_DBG,CFE_EVS_DEBUG,"ExJsmnTbl: Successfully prepared file %s", filename.c_str());
  
      m_data_array_entry_idx = 0;

      m_json.ProcessTokens();

      if (m_data_array_entry_idx > 0) {

         
		   if (load_opt == oskfw::Tbl::LOAD_REPLACE) {
         
            CFE_EVS_SendEvent(EID_LOAD_CMD_DBG,CFE_EVS_DEBUG,"ExJsmnTbl: Replacing entire table");
            m_struct.get_param(&m_param);

         } /* End if replace entire table */
         else if (load_opt == oskfw::Tbl::LOAD_UPDATE) {
         
            // Copy with no validity checks so make valid true
            m_last_action_valid = TRUE;
            m_struct.get_modified_entries(&m_param);

         } /* End if update individual records */
         else {
            CFE_EVS_SendEvent(EID_LOAD_CMD_INVALID_TYPE,CFE_EVS_ERROR,"ExJsmnTbl: Invalid table command load type %d",load_opt);
         }

      } /* End if successful parse */
      else {
         CFE_EVS_SendEvent(EID_LOAD_CMD_PARSE_ERR,CFE_EVS_ERROR,"ExJsmnTbl: Table Parsing failure for file %s",filename.c_str());
      }
      
   } /* End if valid file */
   else {
      CFE_EVS_SendEvent(EID_LOAD_CMD_JSON_OPEN_ERR,CFE_EVS_ERROR,"ExJsmnTbl: Table open failure for file %s. JSON Status = %d JSMN Status = %d",
	                    filename.c_str(), m_json.file_status(), m_json.jsmn_status());
   }
    
   return m_last_action_valid;

} // End LoadContent()


/******************************************************************************
** Function: DumpContent
**
** Notes:
**  1. Can assume valid table file name because base class has verified the
**     file prior to calling this virtual function.
**  2. File is formatted so it can be used as a load file. It does not follow
**     the cFE binary table file format. 
**  3. Creates a new dump file, overwriting anything that may have existed
**     previously
**
*/
boolean ExJsmnTbl::DumpContent (const std::string& filename) {

   boolean  ret_status = FALSE;
   int32    file_handle;
   char     dump_record[256];
   int      i;

   CFE_EVS_SendEvent(EID_DUMP_CMD_DBG, CFE_EVS_DEBUG,
                     "ExJsmnTbl: DumpContent() Entry. File %s", filename.c_str());

   file_handle = OS_creat(filename.c_str(), OS_WRITE_ONLY);

   if (file_handle >= OS_FS_SUCCESS) {


      sprintf(dump_record,"{\n\"name\": \"Example JSMN Table\",\n");
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


/******************************************************************************
**
*/
bool ExJsmnTbl::JsonEntryObj::ProcessToken(oskfw::Json* p_json, int token_idx) {

   int  entry_cnt = 0;
   int  index = AppCfg::EXTBL_MAX_ENTRY_ID;
   int  data1=0, data2=0, data3=0; 

   m_ex_tbl->m_data_array_entry_idx++;

   CFE_EVS_SendEvent(EID_PROCESS_TOKEN, CFE_EVS_DEBUG,
                     "ProcessToken() for data_array_entry_idx %d and token index %d\n",m_ex_tbl->m_data_array_entry_idx, token_idx);

   if (p_json->GetValShortInt(token_idx, JSON_KEY_INDEX, &index)) entry_cnt++;
   if (p_json->GetValShortInt(token_idx, JSON_KEY_DATA1, &data1)) entry_cnt++;
   if (p_json->GetValShortInt(token_idx, JSON_KEY_DATA2, &data2)) entry_cnt++;
   if (p_json->GetValShortInt(token_idx, JSON_KEY_DATA3, &data3)) entry_cnt++;
   
   if (entry_cnt == 4) {
      
      if (index < AppCfg::EXTBL_MAX_ENTRY_ID) {
         
         m_ex_tbl->m_struct.set_entry(index,data1, data2, data3);
         CFE_EVS_SendEvent(EID_PROCESS_TOKEN_ENTRY,CFE_EVS_DEBUG,
		                     "EntryCallBack() index, data1, data2, datat3: %d, %d, %d, %d\n",index, data1, data2, data3);
      }
      else {
         
         m_ex_tbl->m_attr_err_cnt++;     
         CFE_EVS_SendEvent(EID_PROCESS_TOKEN_INDEX_ERR, CFE_EVS_ERROR, "Load file data-array entry %d error, invalid index %d",
                           m_ex_tbl->m_data_array_entry_idx, index);
      }
      
   } /* Valid Entry */
   else
   {
      m_ex_tbl->m_attr_err_cnt++;
      CFE_EVS_SendEvent(EID_PROCESS_TOKEN_ELEMENT_ERR, CFE_EVS_ERROR, "Load file data-array entry %d error, invalid number of elements %d. Should be 4.",
                        m_ex_tbl->m_data_array_entry_idx, entry_cnt);
   } /* Invalid Entry */

   return (entry_cnt == 4);

} // JsonEntryObj::ProcessToken()
