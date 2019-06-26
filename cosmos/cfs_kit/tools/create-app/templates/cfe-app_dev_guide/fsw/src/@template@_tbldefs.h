/************************************************************************
** @Template@ Table Definitions
**
*/

#ifndef _@template@_tbldefs_
#define _@template@_tbldefs_


#include "cfe.h"


/*
** Definition of Table Data Structures
*/
typedef struct
{
  uint8                 TblElement1;
  uint16                TblElement2;
  uint32                TblElement3;

} OS_PACK @TEMPLATE@_Tbl_1_t;

typedef struct
{
  int32                 TblElement1;
  int16                 TblElement2;
  int8                  TblElement3;

} OS_PACK @TEMPLATE@_Tbl_2_t;


#endif  /* _@template@_tbldefs_ */

/************************/
/*  End of File Comment */
/************************/
