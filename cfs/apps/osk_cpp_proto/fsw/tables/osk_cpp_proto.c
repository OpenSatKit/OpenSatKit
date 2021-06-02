/************************************************************************
** Example cFE Table
**
*/

#include "osk_cpp_proto_platform_cfg.h"
#include "ExTblData.h"
#include "cfe_tbl_filedef.h"

static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "g_ex_cfe_tbl", OSK_CPP_PROTO_APP_NAME "." OSK_CPP_PROTO_CFE_TBL_NAME, "Example C++ cFE Table",
    OSK_CPP_PROTO_CFE_TBL_FILENAME, sizeof (ExTblParam)
};

ExTblParam g_ex_cfe_tbl = {{
   {  1,  2,  3},  //  0
   {  4,  5,  6},  //  1
   {  7,  8,  9},  //  2
   { 10, 11, 12},  //  3
   { 13, 14, 15},  //  4
   { 16, 17, 18},  //  5
   { 19, 20, 21},  //  6
   { 22, 23, 24},  //  7
   { 25, 26, 27},  //  8
   { 28, 29, 30},  //  9
   { 31, 32, 33},  // 10
   { 34, 35, 36},  // 11
   { 37, 38, 39},  // 12
   { 40, 41, 42},  // 13
   { 43, 44, 45},  // 14
   { 46, 47, 48},  // 15
   { 49, 50, 51},  // 16
   { 52, 53, 54},  // 17
   { 55, 56, 57},  // 18
   { 58, 59, 60},  // 19
   { 61, 62, 63},  // 20
   { 64, 65, 66},  // 21
   { 67, 68, 69},  // 22
   { 70, 71, 72},  // 23
   { 73, 74, 75},  // 24
   { 76, 77, 78},  // 25
   { 79, 80, 81},  // 26
   { 82, 83, 84},  // 27
   { 85, 86, 87},  // 28
   { 88, 89, 90},  // 29
   { 91, 92, 93},  // 30
   { 94, 95, 96}   // 31
}};

/*
** Table file header
**
** Macro parameters:
**   1. Table structure type
**   2. Name of table to be placed in the cFE Table File Header (App name must match name used in ES startup script)
**   3. A brief description of the table
**   4. Name of table created by the 'make' build process. Must match name used in call to CFE_TBL_Load() and the
**      base filename must match the C source base filename
*/

// CFE_TBL_FILEDEF(g_ex_cfe_tbl, CPP_PROTO_APP.ExCfeTbl, ExCfeTbl, CppApp_ExCfe.tbl)

