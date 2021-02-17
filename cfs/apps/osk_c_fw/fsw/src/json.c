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

#include "json.h"
#include "fileutil.h"


/*
** Type Definitions
*/


/*
** Global File Data
*/

/* String lookup tables for JSMN jsmntype_t */
static char* JsmnTypeStr[] = {
  "Primitive",  /* JSMN_PRIMITIVE = 0 */
  "Object",     /* JSMN_OBJECT    = 1 */
  "Array",      /* JSMN_ARRAY     = 2 */
  "String",     /* JSMN_STRING    = 3 */
};

/* String lookup tables for JSMN jsmnerr_t */
static char* JsmnErrStr[] = {
  "Success",          /* JSMN_SUCCESS     =  0 */
  "Memory Error",     /* JSMN_ERROR_NOMEM = -1 */
  "Invalid Char",     /* JSMN_ERROR_INVAL = -2 */
  "Partial String",   /* JSMN_ERROR_PART  = -3 */
};


/* String lookup table for JSON_FILE_STATUS enumeration */
static char* JsonFileStatusStr[] = {
  "Undefined",           /* JSON_FILE_UNDEF         = 0 */
  "Open Error",          /* JSON_FILE_OPEN_ERR      = 1 */
  "Max char/line Error", /* JSON_FILE_LINE_CHAR_ERR = 2 */
  "Max char/file Error", /* JSON_FILE_CHAR_ERR      = 3 */
  "JSMN Parser Error",   /* JSON_FILE_JSMN_ERR      = 4 */
  "Valid"                /* JSON_FILE_VALID         = 5 */
};



/*
** Local File Function Prototypes
*/

static size_t StrLen(const char *Str, size_t MaxLen);

/******************************************************************************
** Function: ProcessContainerToken
**
** Processes JSON container tokens and calls all callback routines that 
** have been resgitered to process the token.
**
*/
static int ProcessContainerToken(JSON_Class* Json, int TokenId);


/******************************************************************************
** Function: JSON_ObjConstructor
**
** Notes:
**    1. This must be called prior to any other functions using the JSON_OBJ
**    2. The object name must be identical (case sensitive) to the name in the
**       JSON file. 
**
*/
void JSON_ObjConstructor(JSON_Obj*              Obj,
                         char*                  Name,
                         JSON_ContainerFuncPtr  Callback,
                         void*                  UserData)
{
	
   strncpy(&(Obj->Name[0]), Name, JSON_MAX_OBJ_NAME_CHAR);
   Obj->Modified = FALSE;
   Obj->Callback = Callback;
   Obj->UserData = UserData;
   

} /* End JSON_ObjConstructor() */


/******************************************************************************
** Function: JSON_ObjArrayReset
**
** Notes:
**    None
**
*/
void JSON_ObjArrayReset(JSON_Obj* ObjArray,
                        uint16    ObjCnt)
{
	int i;
   
   for (i=0; i < ObjCnt; i++) ObjArray[i].Modified = FALSE;
   
} /* End JSON_ObjArrayReset() */


/******************************************************************************
** Function: JSON_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void JSON_Constructor(JSON_Class* Json, char* FileBuf, jsmntok_t* FileTokens) {

   memset((void*)Json,0,sizeof(JSON_Class));
   
   Json->FileBuf    = FileBuf;
   Json->FileTokens = FileTokens;
  

} /* End JSON_Constructor() */


/******************************************************************************
** Function: JSON_OpenFile
**
** Notes:
**    1. Logic assumes JSON_MAX_FILE_CHAR > JSON_MAX_FILE_LINE_CHAR
**
*/
boolean JSON_OpenFile(JSON_Class* Json, const char* Filename)
{
  
   boolean RetStatus = FALSE;
   boolean LowFileMem = FALSE, ReadingFile = TRUE;
  
   int     FileHandle;
   char    LowMemBuf[JSON_MAX_FILE_LINE_CHAR];
   unsigned int  Indx = 0, Len = 0, Line = 1;
   
   Json->FileObjTokens = 0;
   Json->FileStatus = JSON_FILE_UNDEF; /* Internally used as a valid file read flag */
   CFE_PSP_MemSet(&(Json->JsmnParser), 0, sizeof(jsmn_parser));
   
   FileHandle = OS_open(Filename, OS_READ_ONLY, 0);
   CFE_EVS_SendEvent(JSON_DBG_OPEN_FILE_EID,CFE_EVS_DEBUG,"JSON: OS_open(%s) returned FileHamdle = %d",Filename, FileHandle);
   
   if (FileHandle >= 0) {
  
      while (ReadingFile) {
      
         Len = JSON_MAX_FILE_LINE_CHAR;
         
         if (!LowFileMem) {
            if (FileUtil_ReadLine (FileHandle, &(Json->FileBuf[Indx]), JSON_MAX_FILE_LINE_CHAR)) {
               Len = StrLen(&(Json->FileBuf[Indx]),JSON_MAX_FILE_LINE_CHAR);
            }
            else {
               ReadingFile = FALSE;
            }
         } /* End if !LowFileMem */
         else {
            if (FileUtil_ReadLine (FileHandle, LowMemBuf, JSON_MAX_FILE_LINE_CHAR)) {
               Len = StrLen(LowMemBuf,JSON_MAX_FILE_LINE_CHAR);
               if ( (Indx+Len) >= JSON_MAX_FILE_CHAR) {
                  Json->FileStatus = JSON_FILE_CHAR_ERR;
                  ReadingFile = FALSE;
               }
               else {
                  strcpy(&(Json->FileBuf[Indx]),LowMemBuf);
               }
            }
            else {
               ReadingFile = FALSE;
            }
         } /* End if LowFileMem */ 
            
         //if (DBG_JSON) OS_printf("Line %d, Length = %d\n", Line, Len);
    
         if (ReadingFile) {
            if (Len < JSON_MAX_FILE_LINE_CHAR) {
               //if (DBG_JSON) OS_printf("%s",&Json->FileBuf[Indx]);
               Indx += Len; /* Don't skip over '\0'. Start next line on '\0' */
               Line++;
            }
            else { 
               /* No '\0' found before hit max length */
               Json->FileStatus = JSON_FILE_LINE_CHAR_ERR;
               ReadingFile = FALSE;
            } /* End if valid line length */
         } /* End if ReadingFile */
         
         /* Set if remaining file buffer is less than the maxmimum char per file line */
         LowFileMem = ((Indx+JSON_MAX_FILE_LINE_CHAR) > JSON_MAX_FILE_LINE_CHAR);
        
      } /* End while reading file */
      
      OS_close(FileHandle);

      if (DBG_JSON) OS_printf("JSON_OpenFile(): OS_\n\n%s\n\n",Json->FileBuf);

      if (Json->FileStatus == JSON_FILE_UNDEF) {

          Json->JsmnStatus = jsmn_parse(&(Json->JsmnParser), Json->FileBuf, Json->FileTokens, JSON_MAX_FILE_TOKENS);

          if (Json->JsmnStatus == JSMN_SUCCESS) {
             RetStatus = TRUE;
             Json->FileStatus = JSON_FILE_VALID;
          }
          else {
             Json->FileStatus = JSON_FILE_JSMN_ERR;
          }
      } /* End if valid file input */
      
   } /* End if fopen() */
   else {
     
      Json->FileStatus = JSON_FILE_OPEN_ERR;
         
   } /* End if fopen() error */
   
   return RetStatus;
       
} /* End JSON_OpenFile() */


/******************************************************************************
** Function: JSON_PrintTokens
**
** Notes:
**    
*/
void JSON_PrintTokens(JSON_Class* Json, int TokenCnt)
{

   int i=0;
   jsmntok_t* Token;

   if (DBG_JSON) OS_printf("JSON_PrintTokens(): Printing %d tokens\n", TokenCnt);
   if (DBG_JSON) OS_printf("Token[i] = (Type, Start, End, Size)\n");
  
   for (i=0; i < TokenCnt; i++) {
      Token = &Json->FileTokens[i];
      if (DBG_JSON) OS_printf("Token[%d] = (%d,%d,%d,%d)\n", i, 
                              Token->type, Token->start, Token->end, Token->size);
      if (DBG_JSON) OS_printf("              %s\n", JSON_TokenToStr(Json->FileBuf, Token));
   }
  
} /* End JSON_PrintTokens() */


/******************************************************************************
** Function: JSON_RegContainerCallback
**
** Notes:
**    
*/
void JSON_RegContainerCallback(JSON_Class* Json, const JSON_Obj* JsonObj)
{

   if (DBG_JSON) OS_printf("JSON_RegContainerCallback() - %s\n",JsonObj->Name);
   CFE_EVS_SendEvent(JSON_DBG_REG_CALLBACK_EID,CFE_EVS_DEBUG,"JSON: RegContainerCallback() %s at index %d", JsonObj->Name, Json->CallBackIdx);
   if (Json->CallBackIdx < JSON_MAX_CONTAINER_TOKENS) {
      strncpy(Json->ContainerCallBack[Json->CallBackIdx].Name, JsonObj->Name, JSON_MAX_STR_LEN);
      Json->ContainerCallBack[Json->CallBackIdx].FuncPtr  = JsonObj->Callback;
      Json->ContainerCallBack[Json->CallBackIdx].UserData = JsonObj->UserData;
      Json->CallBackIdx++;
   }
   else {
	  CFE_EVS_SendEvent(JSON_REG_CALLBACK_ERR_EID,CFE_EVS_ERROR,"JSON: Attempt to register callback %s at index %d exceeds limit %d", JsonObj->Name, Json->CallBackIdx, JSON_MAX_CONTAINER_TOKENS);
   }
   
} /* End JSON_RegContainerCallback() */


/******************************************************************************
** Function: JSON_ProcessTokens
**
** Notes:
**   1. See file prologue notes for JSON file assumptions
** 
*/
void JSON_ProcessTokens(JSON_Class* Json) 
{

   int TokenIdx = 0;
   
   Json->FileObjTokens = Json->FileTokens[0].size;

   if (Json->FileStatus == JSON_FILE_VALID) {
   
      CFE_EVS_SendEvent(JSON_DBG_PROC_TOKENS_EID,CFE_EVS_DEBUG,"JSON: ProcessTokens() - %d tokens.",Json->FileObjTokens);

      if (Json->FileTokens[0].type == JSMN_OBJECT || Json->FileTokens[0].type == JSMN_ARRAY) {
         if (DBG_JSON) JSON_PrintTokens(Json, Json->FileObjTokens);
         Json->ContainerDepth = 0;
         ProcessContainerToken(Json, TokenIdx);
      }
      else {
         CFE_EVS_SendEvent(JSON_INVLD_TYPE_ERR_EID,CFE_EVS_ERROR,"JSON tokens not processed. File starts with a %s: %s.  It must start with an object or array.", 
                           JSON_GetJsmnTypeStr(Json->FileTokens[0].type),
                           JSON_TokenToStr(Json->FileBuf, &Json->FileTokens[0]));
      }

   } /* End if valid file to process */  
   else {
      
      CFE_EVS_SendEvent(JSON_INVLD_FILE_EID,CFE_EVS_ERROR,"Invalid JSON file has not been read into memory. File status = %s",
                        JSON_GetFileStatusStr(Json->FileStatus));

   } /* End if a valid file has not been processed */

} /* End JSON_ProcessTokens() */


/******************************************************************************
** Function: JSON_GetContainerSize
**
** Notes:
**    None   
** 
*/
int JSON_GetContainerSize(JSON_Class* Json, int ContainTokenIdx) {
   
   jsmntok_t* ContainToken;

   ContainToken = &(Json->FileTokens[ContainTokenIdx]);
   
   return ContainToken->size;

} /* End JSON_GetContainerSize() */

   
/******************************************************************************
** Function: JSON_GetValBool
**
** Notes:
**   1. JSON boolean is a primitive, not a string type
**   2. JSON defines true/false as lower case
** 
*/
boolean JSON_GetValBool(JSON_Class* Json, int ContainTokenIdx, const char* Key, boolean* BoolVal) {

   int    i;
   char*  TokenStr;
   jsmntok_t* ContainToken;

   ContainToken = &(Json->FileTokens[ContainTokenIdx]);
   
   for (i=(ContainTokenIdx+1); i <= (ContainTokenIdx+ContainToken->size); i++) {
      
      if (JSON_TokenStrEq(Json->FileBuf, &Json->FileTokens[i], Key)) {

         TokenStr = JSON_TokenToStr(Json->FileBuf, &Json->FileTokens[i+1]);
         if (Json->FileTokens[i+1].type == JSMN_PRIMITIVE) {
			
            if (strcmp(TokenStr,"true") == 0) {
               *BoolVal = TRUE;
               return TRUE;
            }		
			
            if (strcmp(TokenStr,"false") == 0) {
               *BoolVal = FALSE;
               return TRUE;
            }		
            
            CFE_EVS_SendEvent(JSON_INVLD_BOOL_VAL_ERR_EID,CFE_EVS_ERROR,"JSON invalid boolean string %s for key %s at container token index %d",
                              TokenStr, Key, ContainTokenIdx);
			
            return FALSE;
			
         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(JSON_INVLD_BOOL_TYPE_ERR_EID,CFE_EVS_ERROR,"JSON invalid boolean type %s for key %s at container token index %d. Must be a primitive.",
                              JSON_GetJsmnTypeStr(Json->FileTokens[i+1].type), Key, ContainTokenIdx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} /* End JSON_GetValBool() */


/******************************************************************************
** Function: JSON_GetValShortInt
**
** Notes:
**   
** 
*/
boolean JSON_GetValShortInt(JSON_Class* Json, int ContainTokenIdx, const char* Key, int* IntVal) {

   int    i;
   char   *TokenStr, *ErrCheck;
   jsmntok_t  *ContainToken;

   ContainToken = &(Json->FileTokens[ContainTokenIdx]);
   if (DBG_JSON) OS_printf("JSON_GetValShortInt() for token %d with size %d\n",ContainTokenIdx,ContainToken->size);
   
   /* TODO - This logic doesn't allow a value to have a keyword */
   /* TODO - Protect against exceeding the array length */ 
   for (i=(ContainTokenIdx+1); i <= (ContainTokenIdx+ContainToken->size); i++) {
      
      //printf("JSON_GetValShortInt() loop %d for key %s\n",i,Key);
      //Token = &Json->FileTokens[i];
      //printf("Token[%d] = (%d,%d,%d,%d)\n", i, 
      //Token->type, Token->start, Token->end, Token->size);
      //printf("              %s\n", JSON_TokenToStr(Json->FileBuf, Token));

      if (JSON_TokenStrEq(Json->FileBuf, &Json->FileTokens[i], Key)) {
 
         TokenStr = JSON_TokenToStr(Json->FileBuf, &Json->FileTokens[i+1]);
         //printf("JSON_GetValShortInt() Found key %s\n",Key);
         if (Json->FileTokens[i+1].type == JSMN_PRIMITIVE) {
            ///*IntVal = atoi(TokenStr);
			   *IntVal = (int)strtol(TokenStr, &ErrCheck, 10);
			   if (ErrCheck == TokenStr) {
               CFE_EVS_SendEvent(JSON_INT_CONV_ERR_EID,CFE_EVS_ERROR,"JSON short int conversion error for key %s token %s at container token index %d.",
                                 Key, TokenStr, ContainTokenIdx);
               break;
            }
            return TRUE;

         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(JSON_INVLD_INT_TYPE_ERR_EID,CFE_EVS_ERROR,"JSON invalid short int type %s for key %s at container token index %d. Must be a primitive.",
                              JSON_GetJsmnTypeStr(Json->FileTokens[i+1].type), Key, ContainTokenIdx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} /* End JSON_GetValShortInt() */


/******************************************************************************
** Function: JSON_GetValUint32
**
** Notes:
**   1. This is a quick cut, paste and edit of JSON_GetValShortInt() that still
**      has some issues to be resolved.
** 
*/
boolean JSON_GetValUint32(JSON_Class* Json, int ContainTokenIdx, const char* Key, uint32* Uint32Val) {

   int    i;
   char   *TokenStr, *StrEndPtr;
   jsmntok_t  *ContainToken;
   
   ContainToken = &(Json->FileTokens[ContainTokenIdx]);
   if (DBG_JSON) OS_printf("JSON_GetValUint32() for token %d with size %d\n",ContainTokenIdx,ContainToken->size);
   
   for (i=(ContainTokenIdx+1); i <= (ContainTokenIdx+ContainToken->size); i++) {
      
      if (JSON_TokenStrEq(Json->FileBuf, &Json->FileTokens[i], Key)) {
 
         TokenStr = JSON_TokenToStr(Json->FileBuf, &Json->FileTokens[i+1]);
         if (Json->FileTokens[i+1].type == JSMN_PRIMITIVE) {
			   *Uint32Val = (uint32)strtoul(TokenStr, &StrEndPtr, 10);
			   /* NOt sure how to check for error. strtoul() returns 0 for an error and that's a legit value 
            if (*Uint32Val == 0) {
               CFE_EVS_SendEvent(JSON_INT_CONV_ERR_EID,CFE_EVS_ERROR,"JSON long int conversion error for key %s token %s at container token index %d.",
                                 Key, TokenStr, ContainTokenIdx);
               break;
            }
            */
            return TRUE;

         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(JSON_INVLD_INT_TYPE_ERR_EID,CFE_EVS_ERROR,"JSON invalid long int type %s for key %s at container token index %d. Must be a primitive.",
                              JSON_GetJsmnTypeStr(Json->FileTokens[i+1].type), Key, ContainTokenIdx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} /* End JSON_GetValUint32() */


/******************************************************************************
** Function: JSON_GetValStr
**
** Notes:
**   
** 
*/
boolean JSON_GetValStr(JSON_Class* Json, int ContainTokenIdx, const char* Key, char* StrVal) {
   
   int i;
   jsmntok_t* ContainToken;

   ContainToken = &(Json->FileTokens[ContainTokenIdx]);
   
   /* TODO - This logic doesn't allow a value to have a keyword */
   /* TODO - Protect against exceeding the array length */ 
   for (i=(ContainTokenIdx+1); i <= (ContainTokenIdx+ContainToken->size); i++) {
      
      if (JSON_TokenStrEq(Json->FileBuf, &Json->FileTokens[i], Key)) {

         if (Json->FileTokens[i+1].type == JSMN_STRING) {
            strcpy(StrVal, JSON_TokenToStr(Json->FileBuf, &Json->FileTokens[i+1]));
            return TRUE;
         }
         else {
			   CFE_EVS_SendEvent(JSON_INVLD_STR_TYPE_ERR_EID,CFE_EVS_ERROR,"JSON invalid string type %s for key %s at container token index %d. Must be a string.",
                              JSON_GetJsmnTypeStr(Json->FileTokens[i+1].type), Key, ContainTokenIdx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;

} /* End JSON_GetValString() */


/******************************************************************************
** Function: JSON_GetValDouble
**
** Notes:
**   
** 
*/
boolean JSON_GetValDouble(JSON_Class* Json, int ContainTokenIdx, const char* Key, double* DoubleVal)
{

   int    i;
   char   *TokenStr, *ErrCheck;
   jsmntok_t  *ContainToken;

   ContainToken = &(Json->FileTokens[ContainTokenIdx]);
 
   for (i=(ContainTokenIdx+1); i <= (ContainTokenIdx+ContainToken->size); i++) {
      
      if (JSON_TokenStrEq(Json->FileBuf, &Json->FileTokens[i], Key)) {
 
         TokenStr = JSON_TokenToStr(Json->FileBuf, &Json->FileTokens[i+1]);
         if (Json->FileTokens[i+1].type == JSMN_PRIMITIVE) {
            *DoubleVal = strtod(TokenStr, &ErrCheck);
            if (ErrCheck == TokenStr) {
               CFE_EVS_SendEvent(JSON_FLT_CONV_ERR_EID,CFE_EVS_ERROR,"JSON float conversion error for key %s token %s at container token index %d.",
			      Key, TokenStr, ContainTokenIdx);
               break;
            }
            return TRUE;
			
         } /* End if primitive */
         else {
            CFE_EVS_SendEvent(JSON_INVLD_FLT_TYPE_ERR_EID,CFE_EVS_ERROR,"JSON invalid float type %s for key %s at container token index %d. Must be a primitive.",
                              JSON_GetJsmnTypeStr(Json->FileTokens[i+1].type), Key, ContainTokenIdx);
            break;
         }
     
      } /* End if found key */

   } /* End container token loop */

   return FALSE;


} /* End JSON_GetValDouble() */



/******************************************************************************
** Function: JSON_TokenStrEq
**
** Notes:
**
*/
boolean JSON_TokenStrEq(char *js, jsmntok_t *t, const char *s) {

   return (strncmp(js + t->start, s, t->end - t->start) == 0
           && strlen(s) == (size_t) (t->end - t->start));

} /* End JSON_TokenStrEq() */



/******************************************************************************
** Function: JSON_TokenToStr
**
** Notes:
**
*/
char* JSON_TokenToStr(char *js, jsmntok_t *t) {
	
    js[t->end] = '\0';
    return js + t->start;
	
} /* End JSON_TokenToStr() */


/******************************************************************************
** Function: JSON_GetFileStatusStr
**
** Notes:
**
*/
char* JSON_GetFileStatusStr(int Enum) {

   if (Enum >= 0 && Enum <= JSON_FILE_VALID)
      return JsonFileStatusStr[Enum];
   else 
      return JsonFileStatusStr[JSON_FILE_UNDEF];

} /* End JSON_GetFileStatusStr() */


/******************************************************************************
** Function: JSON_GetJsmnErrStr
**
** Notes:
**
*/
char* JSON_GetJsmnErrStr(int Enum) {

   if (Enum == JSMN_SUCCESS) return JsmnErrStr[Enum];

   if ((Enum >= JSMN_ERROR_PART) && (Enum <= JSMN_ERROR_NOMEM)) return JsmnErrStr[abs(Enum)];
	   
   return JSON_UNDEF_STATUS_STR;

} /* End JSON_GetJsmnErrStr() */


/******************************************************************************
** Function: JSON_GetJsmnTypeStr
**
** Notes:
**
*/
char* JSON_GetJsmnTypeStr(int Enum) {

   if (Enum >= JSMN_PRIMITIVE && Enum <= JSMN_STRING) return JsmnTypeStr[Enum];
   
   return JSON_UNDEF_STATUS_STR;
   
} /* End JSON_GetJsmnTypeStr() */


/******************************************************************************
** Function: JSON_GetBoolStr
**
** Notes:
**   1. Input uint16 because it is a common JSOSN parser integer size
**   2. Use lowercase by convention
*/
char* JSON_GetBoolStr (uint16 State) {

   static char* BoolStr[] = {"true", "false", JSON_UNDEF_VAL_STR};
   
   if (State == TRUE)

      return BoolStr[0];
 
   else if (State == FALSE)

      return BoolStr[1];
   
   else
      
      return BoolStr[2];
 
} /* End JSON_GetBoolStr() */

/******************************************************************************
** Function: ProcessContainerToken
**
** Notes:
**   1. Max tokens checked as extra protection. Should never need to exit loop 
**      based on max tokens 
*/
static int ProcessContainerToken(JSON_Class* Json, int TokenId) {

   //static int ContainerDepth = 0;
   int   LocalDepth;
   int   i, ContainTokenCnt = 0, ContainTokenMax;
   int   StartIdx = TokenId;
   int   TokenIdx = TokenId;
   jsmntok_t* KeyToken;

   LocalDepth = ++Json->ContainerDepth;
   
   if (DBG_JSON) OS_printf("\n>>Enter<< ProcessContainerToken(): Depth=%d, Token=%d\n",Json->ContainerDepth, TokenId);
   //while (Json->FileTokens[TokenIdx].size <= Json->MaxToken && TokenIdx < JSON_MAX_FILE_TOKENS) {
   //while (TokenIdx <= Json->MaxToken && TokenIdx < JSON_MAX_FILE_TOKENS) {
   //while (TokenIdx <= Json->MaxToken && TokenIdx < JSON_MAX_FILE_TOKENS) {
   

   /* 
   ** See file prologue for JSON file assumptions.
   */
   if (TokenId > 0) {

      if (DBG_JSON) OS_printf("ProcessContainerToken() TokenId > 0\n");

      /* Check if container key name matches a registered callback function */
      i = TokenId-1;
      if (Json->FileTokens[i].type == JSMN_STRING) {

         KeyToken = &Json->FileTokens[i];
         if (DBG_JSON) OS_printf("ProcessContainerToken() KeyToken string = %s\n",JSON_TokenToStr(Json->FileBuf, KeyToken));

         for (i = 0; i < Json->CallBackIdx; i++) {
        
            if (DBG_JSON) OS_printf("ProcessContainerToken() Callback loop %d: CallbackName = %s\n",i, Json->ContainerCallBack[i].Name);
            if (JSON_TokenStrEq(Json->FileBuf, KeyToken, Json->ContainerCallBack[i].Name)) {
               
               (Json->ContainerCallBack[i].FuncPtr)(Json->ContainerCallBack[i].UserData, TokenId);
               break;
            }
        
         } /* End callback loop */
      } /* End if previous token is a string */
   } /* End if TokenId > 0 */

   ContainTokenMax = Json->FileTokens[TokenIdx].size;
   //TODO - Check tokenMax doesn't exceed array
   TokenIdx++;
   while (ContainTokenCnt <= ContainTokenMax) {

      if (DBG_JSON) OS_printf("Container TokenIdx %d: Type %s, First Char %c, ContainTokenCnt %d\n", 
                              TokenIdx, JSON_GetJsmnTypeStr(Json->FileTokens[TokenIdx].type),
                              Json->FileBuf[Json->FileTokens[TokenIdx].start], ContainTokenCnt);

      if (Json->FileTokens[TokenIdx].type == JSMN_OBJECT || Json->FileTokens[TokenIdx].type == JSMN_ARRAY) {
         
         /*
         char  ContainerChar;
         ContainerChar = Json->FileBuf[Json->FileTokens[TokenIdx].start];
         if (ContainerChar == '{' || ContainerChar == '[') {
         else if (ContainerChar == '}' || ContainerChar == ']') {
         
         }
         else
         */
      
         TokenIdx += ProcessContainerToken(Json, TokenIdx);
         
         if (DBG_JSON) OS_printf("Continue ProcessContainerToken():  ContainerDepth=%d, LocalDepth=%d, TokenIdx=%d\n\n", 
                                 Json->ContainerDepth, LocalDepth, TokenIdx);
         
      } /* End if container */
      else
         TokenIdx++;
      
      ContainTokenCnt++;

   } /* End while container tokens */

   if (DBG_JSON) OS_printf(">>Exit<< ProcessContainerToken():  Depth=%d, StartIdx=%d, TokenIdx=%d, ContainTokenCnt=%d\n\n", 
                           Json->ContainerDepth, StartIdx, TokenIdx, ContainTokenCnt);
   Json->ContainerDepth--;
   
   //return ContainTokenCnt;
   return (TokenIdx-StartIdx);

} /* End ProcessContainerToken() */


/******************************************************************************
** Function: StrLen
**
** Notes:
**   1. Was sing strnlen but wrote my own to avoid compiler warning. I wanted
**      the length protection because reading JSON data from a file 
*/
static size_t StrLen(const char *Str, size_t MaxLen)
{
     size_t i;
     for(i = 0; i < MaxLen && Str[i]; i++);
     return i;
}