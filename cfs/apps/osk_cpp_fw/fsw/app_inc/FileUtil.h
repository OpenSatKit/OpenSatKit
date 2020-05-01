/* 
** Purpose: Application Framework Library utilities
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_file_util_
#define _app_file_util_


/*
** Includes
*/

#include "OskCppFwCfg.h"

namespace oskfw {
   
class FileUtil {
public:

   // Read a line from a text file.
   static boolean ReadLine (int FileHandle, char* DestBuf, int MaxChar);

   // Verify file name len, termination, and characters are valid.
   static boolean VerifyFilenameStr(const char* Filename);
   
   // Verify file name is valid and that the file exists for a read operation.
   static boolean VerifyFileForRead(const char* Filename);

   // Verify file name is valid and that the directory exists.
   static boolean VerifyDirForWrite(const char* Filename);
   
};

#endif // _app_file_util_

} // end namespace oskfw
