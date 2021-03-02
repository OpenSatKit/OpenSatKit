/*************************************************************************
** File: md_platform_cfg.h
**
** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
** Memory Dwell Application Version 2.3.3” 
**
** Copyright © 2019 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
**
** Purpose: 
**   Specification for the CFS Memory Dwell constants that can
**   be configured from one platform to another
**
** Notes:
**
** 
*************************************************************************/

#ifndef _md_platform_cfg_h_
#define _md_platform_cfg_h_


/*************************************************************************
** Macro Definitions
*************************************************************************/

/**
** \name MD Command Pipe Parameters */ 
/** \{ */
#define MD_PIPE_NAME       "MD_CMD_PIPE"
#define MD_PIPE_DEPTH      50
/** \} */


/** \mdcfg Memory Dwell Base Filename
**  
**  \par Description:
**       Default base name and location for Memory Dwell filenames
**       AppInit will append 01,02,03, up to number of tables.
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define MD_TBL_FILENAME_FORMAT                "/cf/md_dw\%02d.tbl"

/**
**  \mdcfg Number of memory dwell tables.
**
**  \par Limits
**       Acceptable values for this parameter are 1 to 16.
*/
#define MD_NUM_DWELL_TABLES     4

/**
**  \mdcfg Maximum number of dwell specifications (address/delay/length) 
**    in a Dwell Table.
**
**  \par Limits
**       The maximum value for this parameter is limited by its effect
**       on the MD_DwellTableLoad_t and associated data points, limiting
**       it to 65535 points.  
*/
#define MD_DWELL_TABLE_SIZE 25    

/**
**  \mdcfg Option of whether 32 bit integers must be aligned to 32 bit
**   boundaries.  1 indicates 'yes' (32-bit boundary alignment enforced),
**   0 indicates 'no' (16-bit boundary alignment enforced).
**
**  \par Limits
**       Value must be 0 or 1.
*/
#define MD_ENFORCE_DWORD_ALIGN 1

/**
**  \mdcfg Option of whether a signature field will be reserved in 
**     dwell packets.  1 indicates 'yes', 0 indicates 'no'.
**
**  \par Limits
**       Value must be 0 or 1.
*/
#define MD_SIGNATURE_OPTION 1

/**
**  \mdcfg Number of characters used in the Signature Field
**
**  \par Limits
**       Signature field length needs to be a multiple of 4 so that
**       dwell packet is a multiple of 4 bytes and no compiler padding 
**       will occur. Note that the final character of the signature string
**       must be a null character, so the effective length of user definable
**       characters is one less than the defined length. The length 
**       specified must therefore be at least 4.
*/
#define MD_SIGNATURE_FIELD_LENGTH 32

/** \mdcfg Mission specific version number for MD application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "md_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define MD_MISSION_REV      0

#endif /* _md_platform_cfg_ */

/************************/
/*  End of File Comment */
/************************/
