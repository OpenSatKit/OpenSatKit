/*
** Purpose: Implement the MQTT-to-SB Table
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "mq2sb.h"
#include "mqtt_msg.h"

/*
** Global File Data
*/

static MQ2SB_Class*  Mq2Sb = NULL;

/*
** Local Function Prototypes
*/

static int32 ValidateTbl(void* TblPtr);


/******************************************************************************
** Function: MQ2SB_Constructor
**
*/
void MQ2SB_Constructor(MQ2SB_Class*  Mq2SbPtr,
                       const char* DefTblFilename,
                       const char* CfeTblName,
                       int32 TblErrCode)
{
 
   Mq2Sb = Mq2SbPtr;

   CFE_PSP_MemSet((void*)Mq2Sb, 0, sizeof(MQ2SB_Class));
   Mq2Sb->CfeTbl.DataPtr = (MQ2SB_TblData *) NULL;
   Mq2Sb->CfeTbl.Name    = CfeTblName;
   Mq2Sb->CfeTbl.ErrCode = TblErrCode;
   
   Mq2Sb->CfeTbl.Status = CFE_TBL_Register(&Mq2Sb->CfeTbl.Handle, Mq2Sb->CfeTbl.Name,
                                           sizeof(MQ2SB_TblData), CFE_TBL_OPT_DEFAULT, 
                                           (CFE_TBL_CallbackFuncPtr_t)ValidateTbl);
    
   Mq2Sb->CfeTbl.Registered = (Mq2Sb->CfeTbl.Status == CFE_SUCCESS);
   
   /* DataPtr will remain NULL if data not loaded. */
   if (Mq2Sb->CfeTbl.Registered) {
   
      Mq2Sb->CfeTbl.Status = CFE_TBL_Load(Mq2Sb->CfeTbl.Handle, CFE_TBL_SRC_FILE, DefTblFilename);
      if (Mq2Sb->CfeTbl.Status == CFE_SUCCESS) {
         CFE_TBL_GetAddress((void **)&(Mq2Sb->CfeTbl.DataPtr), Mq2Sb->CfeTbl.Handle);
      }
   }
   else {
      
      CFE_EVS_SendEvent(MQ2SB_TBL_REGISTER_ERR_EID, CFE_EVS_ERROR,
                        "Error registering table %s, CFE_TBL_Register() status = 0x%08X",
                        DefTblFilename, Mq2Sb->CfeTbl.Status);                        
   }


} /* End MQ2SB_Constructor() */


/******************************************************************************
** Function:  MQ2SB_ResetStatus
**
*/
void MQ2SB_ResetStatus()
{

   /* Nothing to do */

} /* End MQ2SB_ResetStatus() */



/******************************************************************************
** Function: MQ2SB_ManageTbl
**
*/
boolean MQ2SB_ManageTbl(void)
{

   boolean TblUpdated = FALSE;
   
   if (Mq2Sb->CfeTbl.Registered) {
   
      Mq2Sb->CfeTbl.Status = CFE_TBL_ReleaseAddress(Mq2Sb->CfeTbl.Handle);
      
      if (Mq2Sb->CfeTbl.Status == CFE_TBL_ERR_NEVER_LOADED) {
         
         Mq2Sb->CfeTbl.DataPtr = (MQ2SB_TblData *) NULL;
      
      } 
      else { 
      
         Mq2Sb->CfeTbl.Status = CFE_TBL_Manage(Mq2Sb->CfeTbl.Handle);
      
         if (Mq2Sb->CfeTbl.Status == CFE_TBL_INFO_UPDATED) {

            Mq2Sb->CfeTbl.Status = CFE_TBL_GetAddress((void **)&(Mq2Sb->CfeTbl.DataPtr), Mq2Sb->CfeTbl.Handle);
            TblUpdated = TRUE;
            
         } /* End if table updated */
      } /* End if table ever loaded */
   } /* End if table registered */
   
   return TblUpdated;
   
} /* End MQ2SB_ManageTbl() */


/******************************************************************************
** Function: ValidateTbl
**
** Callback function used by CFE Table service to allow a table to be validated
** prior to being committed.
**
** Function signature must match CFE_TBL_CallbackFuncPtr_t
**
*/
static int32 ValidateTbl(void* VoidTblPtr) 
{
   
   MQ2SB_TblData* Tbl = (MQ2SB_TblData *) VoidTblPtr;

   int32   RetStatus = Mq2Sb->CfeTbl.ErrCode;
   uint16  i;
   uint16  undef_cnt        = 0;
   uint16  primitive_cnt    = 0;
   uint16  json_packet_cnt  = 0;
   uint16  json_payload_cnt = 0;
   uint16  error_cnt        = 0;
             
   /* 
   ** TODO - Add checks for gaps and duplicate MIDs
   */

   for (i = 0; i < MQTT_MQ2SB_TBL_ENTRIES; i++) {
   
      switch(Tbl->Entry[i].MsgType) {
         case MQTT_MSG_TYPE_UNDEF:
            ++undef_cnt;
            break;
         case MQTT_MSG_TYPE_PRIMITIVE:
            ++primitive_cnt;
            break;
         case MQTT_MSG_TYPE_JSON_PACKET:
            ++json_packet_cnt;
            break;
         case MQTT_MSG_TYPE_JSON_PAYLOAD:
            ++json_payload_cnt;
            break;
         default:
            ++error_cnt;
      }
      
   } /* End table entry loop */

   CFE_EVS_SendEvent(MQ2SB_TBL_VERIFY_EID, CFE_EVS_INFORMATION,
                     "%s table entry verification counts: UNDEF=%d, PRIMITIVE=%d, JSON_PACKET=%d, JSON_PAYLOAD=%d, ERROR=%d",
                     Mq2Sb->CfeTbl.Name,undef_cnt, primitive_cnt, json_packet_cnt, json_payload_cnt, error_cnt);
      
   if (error_cnt == 0) RetStatus = CFE_SUCCESS;

   return RetStatus;
  
} /* End ValidateTbl() */

