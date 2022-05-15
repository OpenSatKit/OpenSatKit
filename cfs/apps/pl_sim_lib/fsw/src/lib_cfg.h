/*
** Purpose: Define configurations for the Payload Simulator Library
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**   2. This must be named lib_cfg.h and not app_cfg.h because apps
**      need to add the library soucre directory to their include
**      search path and they'd get a conflict with app_cfg.h.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _lib_cfg_
#define _lib_cfg_

/*
** Includes
*/

#include "cfe.h"
#include "pl_sim_lib_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** PL_SIM Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release based on OSK Instrument Simulator(ISIM)
*/

#define  PL_SIM_LIB_MAJOR_VER   1
#define  PL_SIM_LIB_MINOR_VER   0


/******************************************************************************
** JSON init file definitions/declarations.
**    
*/

#define CFG_PWR_INIT_CYCLES    PWR_INIT_CYCLES
#define CFG_PWR_RESET_CYCLES   PWR_RESET_CYCLES

#define LIB_CONFIG(XX) \
   XX(PWR_INIT_CYCLES,uint32) \
   XX(PWR_RESET_CYCLES,uint32) \

DECLARE_ENUM(Config,LIB_CONFIG)


#endif /* _lib_cfg_ */
