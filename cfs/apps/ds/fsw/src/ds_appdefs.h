/************************************************************************
** File: ds_appdefs.h 
**
**  NASA Docket No. GSC-18448-1, and identified as "cFS Data Storage (DS) 
**  application version 2.5.2” 
**  
**  Copyright © 2019 United States Government as represented by the Administrator 
**  of the National Aeronautics and Space Administration.  All Rights Reserved. 
**
**  Licensed under the Apache License, Version 2.0 (the "License"); 
**  you may not use this file except in compliance with the License. 
**  You may obtain a copy of the License at 
**  http://www.apache.org/licenses/LICENSE-2.0 
**  Unless required by applicable law or agreed to in writing, software 
**  distributed under the License is distributed on an "AS IS" BASIS, 
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
**  See the License for the specific language governing permissions and 
**  limitations under the License. 
**  
**
** Purpose: 
**  The CFS Data Storage (DS) Application header file
**
** Notes:
**
*************************************************************************/
#ifndef _ds_appdefs_h_
#define _ds_appdefs_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS common application macro definitions                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#define DS_UNUSED                       0                  /**< \brief Unused entries in DS tables */

#define DS_DISABLED                     0                  /**< \brief Enable/disable state selection */
#define DS_ENABLED                      1                  /**< \brief Enable/disable state selection */

#define DS_CLOSED                       0                  /**< \brief File is closed */
#define DS_OPEN                         1                  /**< \brief File is open */

#define DS_BY_COUNT                     1                  /**< \brief Action is based on packet sequence count */
#define DS_BY_TIME                      2                  /**< \brief Action is based on packet timestamp */

#define DS_STRING_REQUIRED              true               /**< \brief String text is required */
#define DS_STRING_OPTIONAL              false              /**< \brief String text is optional */

#define DS_FILENAME_TEXT                true               /**< \brief String text is part of a filename */
#define DS_DESCRIPTIVE_TEXT             false              /**< \brief String text is not part of a filename */

#define DS_INDEX_NONE                   -1                 /**< \brief Packet filter table look-up = not found */

#define DS_PATH_SEPARATOR               '/'                /**< \brief File system path separator */
#define DS_EMPTY_STRING                 ""                 /**< \brief Empty string buffer entries in DS tables */
#define DS_STRING_TERMINATOR            '\0'               /**< \brief ASCIIZ string terminator character */

#define DS_TABLE_VERIFY_ERR             0xFFFFFFFF         /**< \brief Table verification error return value */

#define DS_CLOSED_FILE_HANDLE           0xFFFFFFFF         /**< \brief File handle is closed */

#define DS_FILE_HEADER_NONE             0                  /**< \brief File header type is NONE */
#define DS_FILE_HEADER_CFE              1                  /**< \brief File header type is CFE */

#endif /* _ds_appdefs_h_ */


/************************/
/*  End of File Comment */
/************************/

