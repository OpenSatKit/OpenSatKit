/*
** Purpose: Provide an interface to the JSMN JSON tokenizer. JSMN is not a parser
**          as advertized but provides an API for apps to process JSON tables
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   2. This example illustrates how to integrate a user-defined text parser
**      into the opensat app framework. A more complex parser is needed
**      for non-homogeneous data files.
**   3. One of the goals of the object-based cFS application effort is to 
**      explore using text-based tables that are managed by the application.
**      Once a 'standard' text format is agreed upon the common utilities 
**      could be provided as libraries to the apps.
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
#ifndef _json_
#define _json_

/*
** Includes
*/

#include <stddef.h>
extern "C" {
   #include "Jsmn.h"
}
#include "OskCppFwCfg.h"
#include "JsonTblObj.h"

namespace oskfw {

/******************************************************************************
** Type Definitions
*/


typedef enum {

  JSON_FILE_UNDEF         = 0,  /* Code assumes this status is 0 */
  JSON_FILE_OPEN_ERR      = 1,
  JSON_FILE_LINE_CHAR_ERR = 2,  /* Exceeded maximum characters in a line */ 
  JSON_FILE_CHAR_ERR      = 3,  /* Exceeded maximum characters in a file */
  JSON_FILE_JSMN_ERR      = 4,  /* JSMN reported a parsing error */
  JSON_FILE_VALID         = 5 

} JsonFileStatus;


class Json {
public:

   static const int MAX_FILE_TOKENS     =  2048;
   static const int MAX_FILE_LINE_CHAR  =   512;
   static const int MAX_FILE_CHAR       = 32768;
   static const int MAX_TBL_OBJECTS     =   256;
   static const int MAX_STR_LEN         =    32;
   static const int MAX_OBJ_NAME_CHAR   =    32;
   
   static const std::string UNDEF_STATUS_STR; // Mixed case Status used in ground reporting
   static const std::string UNDEF_VAL_STR;    // Lowercase JSON property value

   Json();
   
   void ResetState();
   
   void RegisterTblObj(JsonTblObj* p_json_tbl_obj);
   
   boolean OpenFile(const std::string filename);

   void ProcessTokens();
   void PrintTokens(int token_cnt);
   int ProcessContainerToken(int token_id);
   int GetContainerSize(int contain_token_idx);

   boolean GetValBool(int contain_token_idx, const char* key, boolean* bool_val);
   boolean GetValShortInt(int contain_token_idx, const char* key, int* int_val);
   boolean GetValStr(int contain_token_idx, const char* key, char* str_val);
   boolean GetValDouble(int contain_token_idx, const char* key, double* double_val);

   boolean TokenStrEq(const char *js, jsmntok_t *t, const char *s);
   char* TokenToStr(char *js, jsmntok_t *t);
   const char* GetFileStatusStr(int status);
   const char* GetJsmnErrStr(int error);
   const char* GetJsmnTypeStr(int type);
   const std::string GetBoolStr (uint16 state);
   size_t StrLen(const char *Str, size_t max_len);

   JsonFileStatus file_status()        { return m_file_status; }
   unsigned int   file_obj_token_cnt() { return m_file_obj_tokens; }
   jsmnerr_t      jsmn_status()      { return m_jsmn_status; }

   boolean debug_enabled() { return m_debug_enabled; }
   void set_debug_enabled(boolean debug_enabled) { m_debug_enabled = debug_enabled; }
   
private:

   JsonFileStatus m_file_status;
   unsigned int   m_file_obj_tokens;

   unsigned int   m_container_depth;
   
   jsmnerr_t      m_jsmn_status;
   jsmn_parser    m_jsmn_parser;

   char           m_file_buf[MAX_FILE_CHAR];
   jsmntok_t      m_file_tokens[MAX_FILE_TOKENS];

   unsigned int   m_tbl_obj_idx;
   JsonTblObj*    m_tbl_obj[MAX_TBL_OBJECTS];

   boolean        m_debug_enabled;

};

} // End namespace oskfw

#endif /* _json_ */
