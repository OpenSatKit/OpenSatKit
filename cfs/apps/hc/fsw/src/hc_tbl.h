/*
** $Id: $
**
** Title: Heater Control (HC) Table Definitions
**
** Purpose: Unit specification for the CFS Heater Control table structures.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $
**
*/

#ifndef _hc_tbl_h_
#define _hc_tbl_h_

#include "cfe.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC table global function prototypes                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Table Initialization Function
**
**  \par Description
**       This function is invoked during HC application startup initialization to
**       create and initialize the HC Type table and the HC Component Table. The 
**       purpose for the HC Type table is to define information for each heater
**       control type. The purpose for the HC Component Table is to define 
**       information for each heater control component. 
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS  \endcode
**  \retstmt Error return codes from #CFE_TBL_Register    \endcode
**  \endreturns
**
**  \sa /HC_AppInit
**/
int32 HC_TableInit(void);


/**
**  \brief Heater Control Type Table Verification Function
**
**  \par Description
**       This function is called from the CFE Table Services as part of the
**       initial table load, and later in response to a Table Validate command.
**       The function verifies that the table data is acceptable to populate the
**       Heater Control Type table. It validates the enable/disable flag, the 
**       heater control type, and the temperature set points. 
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  TableData - Pointer to table data for verification.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt #HC_TABLE_VALIDATION_ERR if table validation fails \endcode
**  \endreturns
**
**  \sa /HC_AppInit
**/
int32 HC_ValidateTable(void *TableData);


/**
**  \brief HC Components Table Verification Function
**
**  \par Description
**       This function is called from the CFE Table Services as part of the
**       initial table load, and later in response to a Table Validate command.
**       The function verifies that the table data is acceptable to populate the
**       HC Components table. It validates the heater control type, the number
**       of heaters, and the number of thermistors.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  TableData - Pointer to table data for verification.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt #HC_TABLE_VALIDATION_ERR if table validation fails \endcode
**  \endreturns
**
**  \sa /HC_AppInit
**/
int32 HC_COMValidateTable(void *TableData);


/**
**  \brief Acquire Table Data Pointer Function
**
**  \par Description
**       This function is invoked to acquire a pointer to the HC Type table data, 
**       and the HC Components table data. The pointers are maintained in the HC 
**       global data structure.  Note that the table data pointers will be set to 
**       NULL if the table has not yet been successfully loaded.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #HC_GlobalData_t
**/
void  HC_AcquireTablePointers(void);


/**
**  \brief Release Table Data Pointer Function
**
**  \par Description
**       This function is invoked to release the pointers to the HC Type table
**       data and the HC Components table data. The pointers are maintained in
**       the HC global data structure. The table data pointers must be periodically
**       released to allow CFE Table Services an opportunity to load or dump the
**       tables without risk of interfering with users of the table data.
**
**  \par Assumptions, External Events, and Notes:
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #HC_GlobalData_t
**/
void  HC_ReleaseTablePointers(void);


#endif /* _hc_tbl_h_ */

/************************/
/*  End of File Comment */
/************************/
