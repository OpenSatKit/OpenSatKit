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
#include "to_filter.h"
#include "cfe_tbl_filedef.h"
#include "cdh_lib.h"

/*
** Exported Data
*/

/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "TO_FilterTable", TO_APP_FILTER_TABLE_NAME, TO_FILTER_TABLE_TITLE,
    "TO_@file-substr@_tbl.tbl", sizeof(TO_FilterTable_t)
};

/*
** Default table data
*/
TO_FilterTable_t TO_FilterTable =
{
    32000 /* bps */,
    {
        /** Tlm Pipe,   Filter Method,   N,   X,   O  **/
