/************************************************************************
** @Template@ Table 1
**
*/

#include "@template@_tbldefs.h"
#include "cfe_tbl_filedef.h"

@TEMPLATE@_Tbl_1_t @TEMPLATE@_Tbl_1 = { 1, 2, 3 };

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

CFE_TBL_FILEDEF(@TEMPLATE@_Tbl_1, @TEMPLATE@.MyFirstTbl, My First Tbl, @template@_tbl_1.tbl)

/************************/
/*  End of File Comment */
/************************/
