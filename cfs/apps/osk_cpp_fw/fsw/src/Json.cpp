/*
** Purpose: Provide an interface to the JSMN JSON tokenizer. It is not a parser
**          but provides an API for apps to process JOSN tables
**
** Notes:
**   1. JSON provides a front end to the JSMN library tokenizer. Although
**      advertized as a parser JSMN is really a tokenizer. This is not 
**      considered a shortcoming for FSW because JSMN was written with
**      simplicity, portability, and optimization as high priorities which
**      makes it suitable for FSW.
**   2. JSON_ProcessTokens() deviates from the current standard and requires
**      the JSON file start with an unnamed object or array. Originally all
**      JSON files had to start with an object or an array (RFC-4627). RFC-7159
**      changed the original spec and added number, string, 'null', 'true', and
**      'false' to the valid items that can start a file.
**   3. If JSMN's jsmntype_t or jsmnerr_t change the lookup string tables and
**      the corresponding functions must both change.
**   4. Since this is part of the application framework provide sufficient 
**      debug events to help user's debug their JSON table integration.
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

/*
** Include Files:
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Json.h"
#include "FileUtil.h"


// Event Message IDs

enum EID {
   
   EID_REG_TBL_OBJ = oskfw::Cfg::JSON_BASE_EID,
   EID_REG_TBL_OBJ_ERR,
   EID_OPEN_FILE,
   EID_PROC_TOKENS,
   EID_INVLD_TYPE,
   EID_INVLD_FILE,
   EID_INVLD_BOOL_VAL,
   EID_INVLD_BOOL_TYPE,
   EID_INT_CONV_ERR,
   EID_INVLD_INT_TYPE,
   EID_INVLD_STR_TYPE,
   EID_FLT_CONV_ERR,
   EID_INVLD_FLT_TYPE

};

namespace oskfw {

/******************************************************************************
** Constant Definitions
*/

const std::string Json::UNDEF_STATUS_STR = "Undefined";   
const std::string Json::UNDEF_VAL_STR    = "undefined";


/******************************************************************************
** Global File Data
*/


/* String lookup tables for JSMN jsmntype_t */
static std::string JsmnTypeStr[4] = {
  "Primitive",  /* JSMN_PRIMITIVE = 0 */
  "Object",     /* JSMN_OBJECT    = 1 */
  "Array",      /* JSMN_ARRAY     = 2 */
  "String",     /* JSMN_STRING    = 3 */
};

/* String lookup tables for JSMN jsmnerr_t */
static std::string JsmnErrStr[4] = {
  "Success",          /* JSMN_SUCCESS     =  0 */
  "Memory Error",     /* JSMN_ERROR_NOMEM = -1 */
  "Invalid Char",     /* JSMN_ERROR_INVAL = -2 */
  "Partial String",   /* JSMN_ERROR_PART  = -3 */
};


/* String lookup table for JSON_FILE_STATUS enumeration */
static std::string JsonFileStatusStr[6] = {
  "Undefined",           /* JSON_FILE_UNDEF         = 0 */
  "Open Error",          /* JSON_FILE_OPEN_ERR      = 1 */
  "Max char/line Error", /* JSON_FILE_LINE_CHAR_ERR = 2 */
  "Max char/file Error", /* JSON_FILE_CHAR_ERR      = 3 */
  "JSMN Parser Error",   /* JSON_FILE_JSMN_ERR      = 4 */
  "Valid"                /* JSON_FILE_VALID         = 5 */
};


/******************************************************************************
** Member Functions
*/

Json::Json() {


   m_file_status     = JSON_FILE_UNDEF;
   m_file_obj_tokens = 0;
   m_jsmn_status     = JSMN_SUCCESS;
   
   memset((void*)&m_jsmn_parser,0,sizeof(jsmn_parser));
   
   memset((void*)m_file_buf,0,MAX_FILE_CHAR);
   memset((void*)m_file_tokens,0,sizeof(jsmntok_t)*MAX_FILE_TOKENS);

   m_tbl_obj_idx = 0;
   memset((void*)m_tbl_obj,0,sizeof(JsonTblObj*)*MAX_TBL_OBJECTS);
   

} // End Json()


void Json::RegisterTblObj(JsonTblObj* json_tbl_obj) {

   std::string obj_name = json_tbl_obj->name();
   
   if (m_debug_enabled) OS_printf("RegisterTblObj() - %s\n",obj_name.c_str());
   CFE_EVS_SendEvent(EID_REG_TBL_OBJ,CFE_EVS_DEBUG,"Json::RegisterTblObj()() %s at index %d", obj_name.c_str(), m_tbl_obj_idx);

   if (m_tbl_obj_idx < MAX_TBL_OBJECTS) {
      m_tbl_obj[m_tbl_obj_idx] = json_tbl_obj;
      m_tbl_obj_idx++;
   }
   else {
	  CFE_EVS_SendEvent(EID_REG_TBL_OBJ_ERR,CFE_EVS_ERROR,"JSON: Attempt to register table object %s at index %d exceeds limit %d",
     obj_name.c_str(), m_tbl_obj_idx, MAX_TBL_OBJECTS);
   }


} // End RegisterTblObj()


void Json::ResetState() {

   for (int i=0; i < m_tbl_obj_idx; i++) m_tbl_obj[i]->set_modified(FALSE);
   
} // End ResetState()


/******************************************************************************
** Function: OpenFile
**
** Notes:
**    1. Logic assumes MAX_FILE_CHAR > MAX_FILE_LINE_CHAR
**
*/
boolean Json::OpenFile(const std::string filename) {

  
   boolean ret_status   = FALSE,
           low_file_mem = FALSE,
           reading_file = TRUE;
  
   int     file_handle;
   char    low_mem_buf[MAX_FILE_LINE_CHAR];
 
   unsigned int  indx = 0,
                 len  = 0,
                 line = 1;
   
   m_file_obj_tokens = 0;
   m_file_status = JSON_FILE_UNDEF; // Internally used as a valid file read flag
   memset((void*)&m_jsmn_parser,0,sizeof(jsmn_parser));
   
   file_handle = OS_open(filename.c_str(), OS_READ_ONLY, 0);
   CFE_EVS_SendEvent(EID_OPEN_FILE,CFE_EVS_DEBUG,"JSON: OS_open(%s) returned FileHamdle = %d",filename.c_str(), file_handle);
   
   if (file_handle >= 0) {
  
      while (reading_file) {
      
         len = MAX_FILE_LINE_CHAR;
         
         if (!low_file_mem) {
            
            if (FileUtil::ReadLine (file_handle, &(m_file_buf[indx]), MAX_FILE_LINE_CHAR)) {
               len = StrLen(&m_file_buf[indx],MAX_FILE_LINE_CHAR);
            }
            else {
               reading_file = FALSE;
            }
         } // End if !low_file_mem
         else {
            
            if (FileUtil::ReadLine (file_handle, low_mem_buf, MAX_FILE_LINE_CHAR)) {
               len = StrLen(low_mem_buf,MAX_FILE_LINE_CHAR);
               if ( (indx+len) >= MAX_FILE_CHAR) {
                  m_file_status = JSON_FILE_CHAR_ERR;
                  reading_file = FALSE;
               }
               else {
                  strcpy(&(m_file_buf[indx]),low_mem_buf);
               }
            }
            else {
               reading_file = FALSE;
            }
         } /* End if low_file_mem */ 
            
         //if (m_debug_enabled) OS_printf("Line %d, Length = %d\n", line, len);
    
         if (reading_file) {
            if (len < MAX_FILE_LINE_CHAR) {
               //if (m_debug_enabled) OS_printf("%s",&Json->FileBuf[indx]);
               indx += len; /* Don't skip over '\0'. Start next line on '\0' */
               line++;
            }
            else { 
               /* No '\0' found before hit max length */
               m_file_status = JSON_FILE_LINE_CHAR_ERR;
               reading_file = FALSE;
            } /* End if valid line length */
         } /* End if reading_file */
         
         /* Set if remaining file buffer is less than the maxmimum char per file line */
         low_file_mem = ((indx+MAX_FILE_LINE_CHAR) > MAX_FILE_LINE_CHAR);
        
      } /* End while reading file */
      
      OS_close(file_handle);

      if (m_debug_enabled) OS_printf("JSON_OpenFile(): OS_\n\n%s\n\n",m_file_buf);

      if (m_file_status == JSON_FILE_UNDEF) {

          m_jsmn_status = jsmn_parse(&(m_jsmn_parser), m_file_buf, m_file_tokens, MAX_FILE_TOKENS);

          if (m_jsmn_status == JSMN_SUCCESS) {
             ret_status = TRUE;
             m_file_status = JSON_FILE_VALID;
          }
          else {
             m_file_status = JSON_FILE_JSMN_ERR;
          }
      } /* End if valid file input */
      
   } /* End if fopen() */
   else {
     
      m_file_status = JSON_FILE_OPEN_ERR;
         
   } /* End if fopen() error */
   
   return ret_status;
       
} /* End JSON_OpenFile() */


/******************************************************************************
** Function: PrintTokens
**
*/
void Json::PrintTokens(int token_cnt) {

   int i=0;
   jsmntok_t* token;

   if (m_debug_enabled) OS_printf("Json::PrintTokens(): Printing %d tokens\n", token_cnt);
   if (m_debug_enabled) OS_printf("Token[i] = (Type, Start, End, Size)\n");
  
   for (i=0; i < token_cnt; i++) {
      token = &m_file_tokens[i];
      if (m_debug_enabled) OS_printf("Token[%d] = (%d,%d,%d,%d)\n", i, 
                              token->type, token->start, token->end, token->size);
      if (m_debug_enabled) OS_printf("              %s\n", TokenToStr(m_file_buf, token));
   }
  
} // End JSON_PrintTokens()


/******************************************************************************
** Function: ProcessTokens
**
** Notes:
**   1. This processing deviates from the current standard and requires the
**      JSON file start with an object or array. Originally all JSON files had
**      to start with an object or an array (RFC-4627). RFC-7159 changed the
**      original spec and added number, string, 'null', 'true', and 'false' to
**      the valid items that can start a file.
** 
*/
void Json::ProcessTokens() {

   m_file_obj_tokens = m_file_tokens[0].size;

   if (m_debug_enabled) OS_printf("Json::ProcessTokens() - Entry\n");
   
   if (m_file_status == JSON_FILE_VALID) {
   
      CFE_EVS_SendEvent(EID_PROC_TOKENS,CFE_EVS_DEBUG,"JSON: ProcessTokens() - %d tokens.",m_file_obj_tokens);

      if (m_file_tokens[0].type == JSMN_OBJECT || m_file_tokens[0].type == JSMN_ARRAY) {
         if (m_debug_enabled) PrintTokens(m_file_obj_tokens);
         m_container_depth = 0;
         ProcessContainerToken(0);
      }
      else {
         CFE_EVS_SendEvent(EID_INVLD_TYPE,CFE_EVS_ERROR,"JSON tokens not processed. File starts with a %s: %s.  It must start with an object or array.", 
                           GetJsmnTypeStr(m_file_tokens[0].type),
                           TokenToStr(m_file_buf, &m_file_tokens[0]));
      }

   } /* End if valid file to process */  
   else {
      
      CFE_EVS_SendEvent(EID_INVLD_FILE,CFE_EVS_ERROR,"Invalid JSON file has not been read into memory. File status = %s",
                        GetFileStatusStr(m_file_status));

   } /* End if a valid file has not been processed */

} // End ProcessTokens()


/******************************************************************************
** Function: GetContainerSize
**
** Notes:
**    None   
** 
*/
int Json::GetContainerSize(int contain_token_idx) {
   
   jsmntok_t* contain_token;

   contain_token = &(m_file_tokens[contain_token_idx]);
   
   return contain_token->size;

} // End GetContainerSize()


/******************************************************************************
** Function: GetValBool
**
** Notes:
**   1. JSON boolean is a primitive, not a string type
**   2. JSON defines true/false as lower case
** 
*/
boolean Json::GetValBool(int contain_token_idx, const char* key, boolean* bool_val) {

   int    i;
   char*  token_str;
   jsmntok_t* contain_token;

   contain_token = &(m_file_tokens[contain_token_idx]);
   
   for (i=(contain_token_idx+1); i <= (contain_token_idx+contain_token->size); i++) {
      
      if (TokenStrEq(m_file_buf, &m_file_tokens[i], key)) {

         token_str = TokenToStr(m_file_buf, &m_file_tokens[i+1]);
         if (m_file_tokens[i+1].type == JSMN_PRIMITIVE) {
			
            if (strcmp(token_str,"true") == 0) {
               *bool_val = TRUE;
               return TRUE;
            }		
			
            if (strcmp(token_str,"false") == 0) {
               *bool_val = FALSE;
               return TRUE;
            }		
            
            CFE_EVS_SendEvent(EID_INVLD_BOOL_VAL,CFE_EVS_ERROR,"JSON invalid boolean string %s for key %s at container token index %d",
                              token_str, key, contain_token_idx);
			
            return FALSE;
			
         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(EID_INVLD_BOOL_TYPE,CFE_EVS_ERROR,"JSON invalid boolean type %s for key %s at container token index %d. Must be a primitive.",
                              GetJsmnTypeStr(m_file_tokens[i+1].type), key, contain_token_idx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} // End GetValBool()


/******************************************************************************
** Function: GetValShortInt
**
*/
boolean Json::GetValShortInt(int contain_token_idx, const char* key, int* int_val) {

   int    i;
   char   *token_str, *err_check;
   jsmntok_t  *contain_token;

   contain_token = &(m_file_tokens[contain_token_idx]);
   if (m_debug_enabled) OS_printf("Json::GetValShortInt() for token %d with size %d\n",contain_token_idx,contain_token->size);
   
   /* TODO - This logic doesn't allow a value to have a keyword */
   /* TODO - Protect against exceeding the array length */ 
   for (i=(contain_token_idx+1); i <= (contain_token_idx+contain_token->size); i++) {
      
      //printf("JSON_GetValShortInt() loop %d for key %s\n",i,key);
      //Token = &Json->FileTokens[i];
      //printf("Token[%d] = (%d,%d,%d,%d)\n", i, 
      //Token->type, Token->start, Token->end, Token->size);
      //printf("              %s\n", JSON_TokenToStr(Json->FileBuf, Token));

      if (TokenStrEq(m_file_buf, &m_file_tokens[i], key)) {
 
         token_str = TokenToStr(m_file_buf, &m_file_tokens[i+1]);
         //printf("JSON_GetValShortInt() Found key %s\n",key);
         if (m_file_tokens[i+1].type == JSMN_PRIMITIVE) {
            ///*int_val = atoi(token_str);
			   *int_val = (int)strtol(token_str, &err_check, 10);
			   if (err_check == token_str) {
               CFE_EVS_SendEvent(EID_INT_CONV_ERR,CFE_EVS_ERROR,"JSON short int conversion error for key %s token %s at container token index %d.",
                                 key, token_str, contain_token_idx);
               break;
            }
            return TRUE;

         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(EID_INVLD_INT_TYPE,CFE_EVS_ERROR,"JSON invalid short int type %s for key %s at container token index %d. Must be a primitive.",
                              GetJsmnTypeStr(m_file_tokens[i+1].type), key, contain_token_idx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} // End GetValShortInt()


/******************************************************************************
** Function: GetValStr
**
*/
boolean Json::GetValStr(int contain_token_idx, const char* key, char* str_val) {
   
   int i;
   jsmntok_t* contain_token;

   contain_token = &(m_file_tokens[contain_token_idx]);
   
   /* TODO - This logic doesn't allow a value to have a keyword */
   /* TODO - Protect against exceeding the array length */ 
   for (i=(contain_token_idx+1); i <= (contain_token_idx+contain_token->size); i++) {
      
      if (TokenStrEq(m_file_buf, &m_file_tokens[i], key)) {

         if (m_file_tokens[i+1].type == JSMN_STRING) {
            strcpy(str_val, TokenToStr(m_file_buf, &m_file_tokens[i+1]));
            return TRUE;
         }
         else {
			   CFE_EVS_SendEvent(EID_INVLD_STR_TYPE,CFE_EVS_ERROR,"JSON invalid string type %s for key %s at container token index %d. Must be a string.",
                              GetJsmnTypeStr(m_file_tokens[i+1].type), key, contain_token_idx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} // End GetValString()


/******************************************************************************
** Function: GetValDouble
**
*/
boolean Json::GetValDouble(int contain_token_idx, const char* key, double* double_val) {

   int    i;
   char   *token_str, *err_check;
   jsmntok_t  *contain_token;

   contain_token = &(m_file_tokens[contain_token_idx]);
 
   for (i=(contain_token_idx+1); i <= (contain_token_idx+contain_token->size); i++) {
      
      if (TokenStrEq(m_file_buf, &m_file_tokens[i], key)) {
 
         token_str = TokenToStr(m_file_buf, &m_file_tokens[i+1]);
         if (m_file_tokens[i+1].type == JSMN_PRIMITIVE) {
            *double_val = strtod(token_str, &err_check);
            if (err_check == token_str) {
               CFE_EVS_SendEvent(EID_FLT_CONV_ERR,CFE_EVS_ERROR,"JSON float conversion error for key %s token %s at container token index %d.",
			      key, token_str, contain_token_idx);
               break;
            }
            return TRUE;
			
         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(EID_INVLD_FLT_TYPE,CFE_EVS_ERROR,"JSON invalid float type %s for key %s at container token index %d. Must be a primitive.",
                              GetJsmnTypeStr(m_file_tokens[i+1].type), key, contain_token_idx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;


} // End GetValDouble()



/******************************************************************************
** Function: TokenStrEq
**
*/
boolean Json::TokenStrEq(const char *js, jsmntok_t *t, const char *s) {

   if (m_debug_enabled) OS_printf("Json::TokenStrEq(): js=%s, JsonObj=%s, len=%d\n", (js+t->start), s, (t->end - t->start));
   
   return (strncmp(js + t->start, s, t->end - t->start) == 0
           && strlen(s) == (size_t) (t->end - t->start));

} // End TokenStrEqual()



/******************************************************************************
** Function: TokenToStr
**
** Notes:
**
*/
char* Json::TokenToStr(char *js, jsmntok_t *t) {
	
    js[t->end] = '\0';
    return js + t->start;
	
} // TokenToStr()


/******************************************************************************
** Function: GetFileStatusStr
**
*/
const char* Json::GetFileStatusStr(int status) {

   if (status >= 0 && status <= JSON_FILE_VALID)
      return JsonFileStatusStr[status].c_str();
   else 
      return JsonFileStatusStr[JSON_FILE_UNDEF].c_str();

} // End GetFileStatusStr()


/******************************************************************************
** Function: GetJsmnErrStr
**
*/
const char* Json::GetJsmnErrStr(int error) {

   if (error == JSMN_SUCCESS) return JsmnErrStr[error].c_str();

   if ((error >= JSMN_ERROR_PART) && (error <= JSMN_ERROR_NOMEM)) return JsmnErrStr[abs(error)].c_str();
	   
   return UNDEF_STATUS_STR.c_str();

} // End GetJsmnErrStr()


/******************************************************************************
** Function: GetJsmnTypeStr
**
*/
const char* Json::GetJsmnTypeStr(int type) {

   if (type >= JSMN_PRIMITIVE && type <= JSMN_STRING) return JsmnTypeStr[type].c_str();
   
   return UNDEF_STATUS_STR.c_str();
   
} // End GetJsmnTypeStr()



/******************************************************************************
** Function: GetBoolStr
**
** Notes:
**   1. Input uint16 because it is a common JSON parser integer size
**   2. Use lowercase by convention
*/
const std::string Json::GetBoolStr (uint16 state) {

   static std::string BoolStr[2] = {"true", "false"};
   
   if (state == TRUE)

      return BoolStr[0];
 
   else if (state == FALSE)

      return BoolStr[1];
   
   else
      
      return UNDEF_VAL_STR;
 
} // End GetBoolStr()


/******************************************************************************
** Function: ProcessContainerToken
**
** Notes:
**   1. m_container_depth must be set to zero before the first call for a file
**   2. Max tokens checked as extra protection. Should never need to exit loop 
**      based on max tokens 
*/
int Json::ProcessContainerToken(int token_id) {

   int   local_depth;
   int   i, contain_token_cnt = 0, contain_token_max;
   int   start_idx = token_id;
   int   token_idx = token_id;
   jsmntok_t* key_token;

   local_depth = ++m_container_depth;
   
   if (m_debug_enabled) OS_printf("\n>>Enter<< ProcessContainerToken(): Depth=%d, Token=%d\n",m_container_depth, token_id);
   //while (Json->FileTokens[token_idx].size <= Json->MaxToken && token_idx < MAX_FILE_TOKENS) {
   //while (token_idx <= Json->MaxToken && token_idx < MAX_FILE_TOKENS) {
   //while (token_idx <= Json->MaxToken && token_idx < MAX_FILE_TOKENS) {
   

   /* 
   ** See file prologue for JSON file assumptions.
   */
   if (token_id > 0) {

      if (m_debug_enabled) OS_printf("ProcessContainerToken() token_id > 0\n");

      /* Check if container key name matches a registered callback function */
      i = token_id-1;
      if (m_file_tokens[i].type == JSMN_STRING) {

         key_token = &m_file_tokens[i];
         if (m_debug_enabled) OS_printf("ProcessContainerToken() key_token string = %s\n",TokenToStr(m_file_buf, key_token));

         for (i = 0; i < m_tbl_obj_idx; i++) {
        
            std::string obj_name = m_tbl_obj[i]->name();
            if (m_debug_enabled) OS_printf("ProcessContainerToken() Callback loop %d: CallbackName = %s\n",i, obj_name.c_str());
            if (TokenStrEq(m_file_buf, key_token, obj_name.c_str())) {
               m_tbl_obj[i]->ProcessToken(this, token_id);
               break;
            }
        
         } /* End JSON object loop */
      } /* End if previous token is a string */
   } /* End if token_id > 0 */

   contain_token_max = m_file_tokens[token_idx].size;
   //TODO - Check tokenMax doesn't exceed array
   token_idx++;
   while (contain_token_cnt <= contain_token_max) {

      if (m_debug_enabled) OS_printf("Container token_idx %d: Type %s, First Char %c, contain_token_cnt %d\n", 
                              token_idx, GetJsmnTypeStr(m_file_tokens[token_idx].type),
                              m_file_buf[m_file_tokens[token_idx].start], contain_token_cnt);

      if (m_file_tokens[token_idx].type == JSMN_OBJECT || m_file_tokens[token_idx].type == JSMN_ARRAY) {
         
         /*
         char  ContainerChar;
         ContainerChar = Json->FileBuf[Json->FileTokens[token_idx].start];
         if (ContainerChar == '{' || ContainerChar == '[') {
         else if (ContainerChar == '}' || ContainerChar == ']') {
         
         }
         else
         */
      
         token_idx += ProcessContainerToken(token_idx);
         
         if (m_debug_enabled) OS_printf("Continue ProcessContainerToken():  ContainerDepth=%d, local_depth=%d, token_idx=%d\n\n", 
                                 m_container_depth, local_depth, token_idx);
         
      } /* End if container */
      else
         token_idx++;
      
      contain_token_cnt++;

   } /* End while container tokens */

   if (m_debug_enabled) OS_printf(">>Exit<< ProcessContainerToken():  Depth=%d, start_idx=%d, token_idx=%d, contain_token_cnt=%d\n\n", 
                           m_container_depth, start_idx, token_idx, contain_token_cnt);
   m_container_depth--;
   
   //return contain_token_cnt;
   return (token_idx-start_idx);

} // End ProcessContainerToken()


/******************************************************************************
** Function: StrLen
**
** Notes:
**   1. Was using strnlen but wrote my own to avoid compiler warning. I wanted
**      the length protection because reading JSON data from a file 
*/
size_t Json::StrLen(const char *str, size_t max_len) {
   
   size_t i;
   for(i = 0; i < max_len && str[i]; i++);
     
   return i;

}

} // End namespace oskfw
