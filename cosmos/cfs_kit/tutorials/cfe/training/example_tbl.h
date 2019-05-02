/************************************************************************
** Example Table Definitions
**
*/

#ifndef _example_tbl_
#define _example_tbl_


#include "cfe.h"


#define EXAMPLE_TBL_DEFAULT_FILE  "/cf/example_tbl.tbl"
#define EXAMPLE_TBL_VAL_LIM   10
#define EXAMPLE_TBL_LIM_ERR   -1

typedef struct {

  int32    Int32; 
  int16    Int16;  
  int8     Int8;
  
} EXAMPLE_Tbl_t;


int32 EXAMPLE_TblValidationFunc(void *TblData);

#endif  /* _example_tbl_ */

/************************/
/*  End of File Comment */
/************************/
