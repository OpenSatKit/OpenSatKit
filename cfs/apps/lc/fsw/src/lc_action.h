/*************************************************************************
** File:
**   $Id: lc_action.h 1.4 2017/01/22 17:24:52EST sstrege Exp  $
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
** Purpose: 
**   Specification for the CFS Limit Checker (LC) routines that
**   handle actionpoint processing
**
** Notes:
**
** 
**************************************************************************/
#ifndef _lc_action_
#define _lc_action_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name LC Actionpoint Event Trailer */ 
/** \{ */
#define LC_AP_EVENT_TAIL_STR       ": AP = %d, FailCount = %d, RTS = %d"

#define LC_AP_EVENT_TAIL_LEN       36   /**< \brief Length of string  
                                                    including NUL. 
                                                    Needed by LC_verify.h */
/** \} */

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Sample actionpoints
**  
**  \par Description
**       Support function for #LC_SampleAPReq that will sample the
**       selected actionpoints.  The start and end arguments define
**       which actionpoint(s) the command will sample.  If both the
**       start and end arguments are set to #LC_ALL_ACTIONPOINTS,
**       the command will be interpreted as a request to sample all
**       actionpoints (heritage).  Otherwise, the start index must
**       be less than or equal to the end index, and both must be
**       within the bounds of the actionpoint table.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   StartIndex   The first actionpoint to sample
**                             (zero based actionpoint table index)
**
**  \param [in]   EndIndex     The last actionpoint to sample
**                             (zero based actionpoint table index)
**
**  \sa #LC_SampleAPReq
**
*************************************************************************/
void LC_SampleAPs(uint16 StartIndex, uint16 EndIndex);

/************************************************************************/
/** \brief Validate actionpoint definition table (ADT)
**  
**  \par Description
**       This function is called by table services when a validation of 
**       the actionpoint definition table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   *TableData     Pointer to the table data to validate
**  
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #LC_ADTVAL_ERR_DEFSTATE \retdesc \copydoc LC_ADTVAL_ERR_DEFSTATE \endcode
**  \retcode #LC_ADTVAL_ERR_RTSID    \retdesc \copydoc LC_ADTVAL_ERR_RTSID    \endcode
**  \retcode #LC_ADTVAL_ERR_FAILCNT  \retdesc \copydoc LC_ADTVAL_ERR_FAILCNT  \endcode
**  \retcode #LC_ADTVAL_ERR_EVTTYPE  \retdesc \copydoc LC_ADTVAL_ERR_EVTTYPE  \endcode
**  \retcode #LC_ADTVAL_ERR_RPN      \retdesc \copydoc LC_ADTVAL_ERR_RPN      \endcode
**  \endreturns
**
**  \sa #LC_ValidateWDT
**
*************************************************************************/
int32 LC_ValidateADT(void *TableData);

/************************************************************************/
/** \brief Sample single actionpoint
**  
**  \par Description
**       Support function for actionpoint processing that will sample
**       a single actionpoint and handle the result as needed
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   APNumber     The actionpoint number to sample (zero
**                             based actionpoint definition table index)
**
*************************************************************************/
void LC_SampleSingleAP(uint16 APNumber);

/************************************************************************/
/** \brief Evaluate RPN
**  
**  \par Description
**       Support function for actionpoint processing that evaluates
**       the reverse polish notation (RPN) equation for the specified
**       actionpoint and returns the result
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   APNumber     The actionpoint number to evaluate (zero
**                             based actionpoint definition table index)
**
**  \returns
**  \retcode #LC_ACTION_PASS         \retdesc \copydoc LC_ACTION_PASS   \endcode
**  \retcode #LC_ACTION_FAIL         \retdesc \copydoc LC_ACTION_FAIL   \endcode
**  \retcode #LC_ACTION_STALE        \retdesc \copydoc LC_ACTION_STALE  \endcode
**  \retcode #LC_ACTION_ERROR        \retdesc \copydoc LC_ACTION_ERROR  \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_EvaluateRPN(uint16 APNumber);
 
/************************************************************************/
/** \brief Validate RPN expression
**  
**  \par Description
**       Support function for actionpoint definition table validation
**       that checks a reverse polish notation (RPN) equation for
**       possible errors.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   RPNPtr            Pointer to the RPN equation
**
**  \param [in]   IndexValue        A pointer where to store the equation
**                                  index value if an error is detected
**
**  \param [in]   StackDepthValue   A pointer where to store the equation
**                                  stack depth value if an error is detected
**
**  \param [out]  *IndexValue       Equation index value where error was
**                                  found. Not modified if return code is
**                                  #LC_ADTVAL_NO_ERR
**
**  \param [out]  *StackDepthValue  Equation stack depth value where error
**                                  found. Not modified if return code is
**                                  #LC_ADTVAL_NO_ERR
**
**  \returns
**  \retcode #LC_ADTVAL_NO_ERR   \retdesc \copydoc LC_ADTVAL_NO_ERR  \endcode
**  \retcode #LC_ADTVAL_ERR_RPN  \retdesc \copydoc LC_ADTVAL_ERR_RPN \endcode
**  \endreturns
**
**  \sa #LC_ValidateADT
**
*************************************************************************/
int32 LC_ValidateRPN(uint16 *RPNPtr, 
                      int32  *IndexValue, 
                      int32  *StackDepthValue);
 
#endif /* _lc_action_ */

/************************/
/*  End of File Comment */
/************************/
