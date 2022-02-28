/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*! @file
 * @brief Purpose:
 *  @details Provide version identifiers for the ELF to cFE Table Converter. @n
 *  See @ref cfsversions for version and build number and description
 *
 */
#ifndef ELF2CFETBL_VERSION_H
#define ELF2CFETBL_VERSION_H

/*
 * Development Build Macro Definitions
 */
#define ELF2CFETBL_BUILD_NUMBER 11 /*!< @brief Number of commits since baseline */
#define ELF2CFETBL_BUILD_BASELINE \
    "v3.3.0-rc4" /*!< @brief Development Build: git tag that is the base for the current */

/*
 * Version Macro Definitions
 */
#define ELF2CFETBL_MAJOR_VERSION 3 /*!< @brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define ELF2CFETBL_MINOR_VERSION 1 /*!< @brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define ELF2CFETBL_REVISION      0 /*!< @brief ONLY APPLY for OFFICIAL releases. Revision version number. */

/*!
 * @brief Mission revision.
 *
 * Set to 0 on OFFICIAL releases, and set to 255 (0xFF) on development versions.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed.
 */
#define ELF2CFETBL_MISSION_REV 0xFF

/*
 * Tools to construct version string
 */
#define ELF2CFETBL_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define ELF2CFETBL_STR(x) \
    ELF2CFETBL_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define ELF2CFETBL_VERSION ELF2CFETBL_BUILD_BASELINE ELF2CFETBL_STR(ELF2CFETBL_BUILD_NUMBER)

/*! @brief Development Build Version String.
 * @details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 * official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define ELF2CFETBL_VERSION_STRING                                                        \
    " elf2cfetbl Development Build\n"                                                    \
    " " ELF2CFETBL_VERSION " (Codename: Draco)\n" /* Codename for current development */ \
    " Last Official Release: elf2cfetbl v3.1.0"   /* For full support please use official release version */

#endif /* ELF2CFETBL_VERSION_H */
