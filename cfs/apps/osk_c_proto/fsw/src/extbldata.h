/*
** Purpose: Define Example Table Data structure
**
** Notes:
**   1. Only define data types so it can be included in both C and C++ files
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _extbldata_
#define _extbldata_

#include "app_cfg.h"

// Table data types


typedef struct {

   uint16   Data1;
   uint16   Data2;
   uint16   Data3;

} ExTblData_Entry;

typedef struct {

   ExTblData_Entry  Entry[OSK_C_PROTO_TBL_MAX_ENTRY_ID];  

} ExTblData_Param;


#endif // _extbldata_

