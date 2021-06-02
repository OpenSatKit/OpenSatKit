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
#ifndef _ex_tbl_data_
#define _ex_tbl_data_

#ifdef __cplusplus
   extern "C" { 
   #include "cfe.h" 
   }
#else
   #include "cfe.h"
#endif

// Table data types


typedef struct {

   uint16   data1;
   uint16   data2;
   uint16   data3;

} ExTblEntry;

typedef struct {

   ExTblEntry  entry[32];  // AppCfg::EXTBL_MAX_ENTRY_ID

} ExTblParam;


#endif // _ex_tbl_struct_

