/******************************************************************************
** TO_@file-substr@_tbl.c
**
** Purpose:
**   This table defines which APIDs should be subscribed to, which TO pipe to
**   use and the packet filtering parameters 
**
** Notes:
**   1. This table is automatically generated and is NOT to be hand modified.
**   2. The header files and variable names must match the mission's TO app.
**   3. This file was generated at @date-time@
**
** Template parameters:
**   <file-substr>  The exact substring to be included in the table filename
**                  TO_<file-substr>_tbl.tbl. This substring should indicate
**                  the mission mode and data rate.
**   <date-time>    The date and time when the file was generated.
**
******************************************************************************/

/*
** Includes
*/

#include "cfe.h"
#include "cfe_msgids.h"
#include "hk_utils.h"
#include "hk_app.h"
#include "hk_msgids.h"
#include "hk_tbldefs.h"
#include "cfe_tbl_filedef.h"

/*
** Exported Data
*/

/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "HK_CopyTable", TO_APP_FILTER_TABLE_NAME, TO_FILTER_TABLE_TITLE,
    "HK_@file-substr@_tbl.tbl", (sizeof(hk_copy_table_entry_t)*(HK_COPY_TABLE_ENTRIES))
};

/*
** Default table data
*/
hk_copy_table_entry_t      HK_CopyTable[HK_COPY_TABLE_ENTRIES] =
{
/*         inputMid        inputOffset     outputMid    outputOffset  numBytes*/



{"combo-1"=>{"mid"=>"HK_COMBINED_PKT1_MID", "headers"=>["isim_msgids.h", "f42_msgids.h"], "src-data"=>#<Array:47126530881620>}}
isim_msgids.h
f42_msgids.h
{ 0x0800,  12, 0x089C,  12,   1 },   /* {"tlm_mid"=>"CFE_ES_HK_TLM_MID", "tlm-item"=>"CFE_ES HK_TLM_PKT CMD_VALID_COUNT"}                    */
{ 0x0800,  13, 0x089C,  13,   1 },   /* {"tlm_mid"=>"CFE_ES_HK_TLM_MID", "tlm-item"=>"CFE_ES HK_TLM_PKT CMD_ERROR_COUNT"}                    */
{ 0x0800,  14, 0x089C,  14,   2 },   /* {"tlm_mid"=>"CFE_ES_HK_TLM_MID", "tlm-item"=>"CFE_ES HK_TLM_PKT CFE_CORE_CHECKSUM"}                  */
{ 0x0801,  12, 0x089C,  16,   1 },   /* {"tlm_mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT CMD_VALID_COUNT"}                  */
{ 0x0801,  13, 0x089C,  17,   1 },   /* {"tlm_mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT CMD_ERROR_COUNT"}                  */
{ 0x0801,  14, 0x089C,  18,   1 },   /* {"tlm_mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT MSG_FMT_MODE"}                     */
{ 0x0801,  15, 0x089C,  19,   1 },   /* {"tlm_mid"=>"CFE_EVS_HK_TLM_MID", "tlm-item"=>"CFE_EVS HK_TLM_PKT MSG_TRUNC_CTR"}                    */
{ 0x0803,  12, 0x089C,  20,   1 },   /* {"tlm_mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT CMD_VALID_COUNT"}                    */
{ 0x0803,  13, 0x089C,  21,   1 },   /* {"tlm_mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT CMD_ERROR_COUNT"}                    */
{ 0x0803,  14, 0x089C,  22,   1 },   /* {"tlm_mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT NO_SUBSCRIBE_CNT"}                   */
{ 0x0803,  15, 0x089C,  23,   1 },   /* {"tlm_mid"=>"CFE_SB_HK_TLM_MID", "tlm-item"=>"CFE_SB HK_TLM_PKT MSG_SEND_ERR_CNT"}                   */
{ 0x0804,  12, 0x089C,  24,   1 },   /* {"tlm_mid"=>"CFE_TBL_HK_TLM_MID", "tlm-item"=>"CFE_TBL HK_TLM_PKT CMD_VALID_COUNT"}                  */
{ 0x0804,  13, 0x089C,  25,   1 },   /* {"tlm_mid"=>"CFE_TBL_HK_TLM_MID", "tlm-item"=>"CFE_TBL HK_TLM_PKT CMD_ERROR_COUNT"}                  */
{ 0x0804,  14, 0x089C,  26,   2 },   /* {"tlm_mid"=>"CFE_TBL_HK_TLM_MID", "tlm-item"=>"CFE_TBL HK_TLM_PKT NUM_TABLES"}                       */
{ 0x0805,  12, 0x089C,  28,   1 },   /* {"tlm_mid"=>"CFE_TIME_HK_TLM_MID", "tlm-item"=>"CFE_TIME HK_TLM_PKT CMD_VALID_COUNT"}                */
{ 0x0805,  13, 0x089C,  29,   1 },   /* {"tlm_mid"=>"CFE_TIME_HK_TLM_MID", "tlm-item"=>"CFE_TIME HK_TLM_PKT CMD_ERROR_COUNT"}                */
{ 0x0805,  14, 0x089C,  30,   2 },   /* {"tlm_mid"=>"CFE_TIME_HK_TLM_MID", "tlm-item"=>"CFE_TIME HK_TLM_PKT CLOCK_STATE_FLAGS"}              */

} ; /* end LC_ADT */

/************************/
/*  End of File */
/************************/
