/* 
** Purpose: Define C++ application framework library version number
**
** Notes:
**   1. Version numbering: major.minor.local_rev
**      major - Maintained by OSK, changed for functional or API changes
**      minor - Maintained by OSK, changed for bug fixes and documentation updates
**      local_rev - Maintained by OSK user, used to track local changes
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
#ifndef _osk_cpp_fw_ver_
#define _osk_cpp_fw_ver_

namespace oskfw {
   
struct Ver {

   static const int MAJOR     = 1;
   static const int MINOR     = 0;
   static const int LOCAL_REV = 0;

};
   
} // End namespace oskfw
   
#endif // _osk_cpp_fw_ver_
