/*
** Purpose: Define the 42 Library interface
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application. The source files in this 
**      library have been copied from 42. For convenience they are contained
**      in a single directory although they come from the following 42 
**      source directories:
**         42/Include
**         42/Kit
**         42/Source
**
**   2. This library is used by the I42 and F42 apps. Effort has been made to
**      minimize changes to the 42 source files. All changes are marked with a
**      "~osk~" tag in a comment. 
**   3. For more 42 information please see
**         42/Docs/
**         42/Database/Readme.txt
**         42/Standalone/Readme.txt
**
** References:
**   1. OpenSat Object-based Application Developer's Guide
**   2. cFS Application Developer's Guide
**   3. 42/Docs/Standalone AcApp text file
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _osk_42_lib_h_
#define _osk_42_lib_h_

/*
** Includes
*/

#include "cfe.h"
#include "ac42.h"

/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: OSK_42_LIB_LibInit
**
** Library Initialization Function
**
** Notes:
**   1. This function is required by CFE to initialize the library. It should
**      be specified in the cfe_es_startup.scr file as part of loading this 
**      library.  It is not directly invoked by applications.
**
*/
int32 OSK_42_LIB_LibInit(void);


/******************************************************************************
** 42's AcApp.c is used as OSK's example FSW app. They don't follow cFS/OSK
** naming conventions but the chance of a name clash are slim and a porting
** goal was to minimize any changes to the 42 code.
**
*/

void GyroProcessing(struct AcType *AC);
void MagnetometerProcessing(struct AcType *AC);
void CssProcessing(struct AcType *AC);
void FssProcessing(struct AcType *AC);
void StarTrackerProcessing(struct AcType *AC);
void GpsProcessing(struct AcType *AC);
     
void WheelProcessing(struct AcType *AC);
void MtbProcessing(struct AcType *AC);

void InitAC(struct AcType *AC);
void AcFsw(struct AcType *AC);

#endif /* _42_lib_h_ */

