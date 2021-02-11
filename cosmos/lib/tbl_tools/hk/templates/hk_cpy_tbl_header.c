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



