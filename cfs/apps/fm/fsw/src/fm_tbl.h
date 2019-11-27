/*
** $Id: fm_tbl.h 1.2.1.2 2017/01/23 21:53:16EST sstrege Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: File Manager (FM) Table Definitions
**
** Purpose: Unit specification for the CFS File Manager table structures.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
**
*/

#ifndef _fm_tbl_h_
#define _fm_tbl_h_

#include "cfe.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM table global function prototypes                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Table Initialization Function
**
**  \par Description
**       This function is invoked during FM application startup initialization to
**       create and initialize the FM file system free space table.  The purpose
**       for the table is to define the list of file systems for which free space
**       must be reported.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS  \endcode
**  \retstmt Error return codes from #CFE_TBL_Register    \endcode
**  \endreturns
**
**  \sa /FM_AppInit
**/
int32 FM_TableInit(void);


/**
**  \brief Table Verification Function
**
**  \par Description
**       This function is called from the CFE Table Services as part of the
**       initial table load, and later inresponse to a Table Validate command.
**       The function verifies that the table data is acceptable to populate the
**       FM file system free space table.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  TableData - Pointer to table data for verification.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt #FM_TABLE_VALIDATION_ERR if table validation fails \endcode
**  \endreturns
**
**  \sa /FM_AppInit
**/
int32 FM_ValidateTable(void *TableData);


/**
**  \brief Acquire Table Data Pointer Function
**
**  \par Description
**       This function is invoked to acquire a pointer to the FM file system free
**       space table data.  The pointer is maintained in the FM global data
**       structure.  Note that the table data pointer will be set to NULL if the
**       table has not yet been successfully loaded.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_GlobalData_t
**/
void  FM_AcquireTablePointers(void);


/**
**  \brief Release Table Data Pointer Function
**
**  \par Description
**       This function is invoked to release the pointer to the FM file system free
**       space table data.  The pointer is maintained in the FM global data
**       structure.  The table data pointer must be periodically released to allow
**       CFE Table Services an opportunity to load or dump the table without risk
**       of interfering with users of the table data.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_GlobalData_t
**/
void  FM_ReleaseTablePointers(void);


#endif /* _fm_tbl_h_ */

/************************/
/*  End of File Comment */
/************************/
