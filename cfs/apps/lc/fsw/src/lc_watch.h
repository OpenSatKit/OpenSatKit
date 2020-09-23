/*************************************************************************
** File:
**   $Id: lc_watch.h 1.4 2017/01/22 17:24:25EST sstrege Exp  $
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
**   handle watchpoint processing
**
** Notes:
**
** 
**************************************************************************/
#ifndef _lc_watch_
#define _lc_watch_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "lc_app.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Check message for watchpoints
**  
**  \par Description
**       Processes a single software bus command pipe message that
**       doesn't match any LC predefined command or message ids,
**       which indicates it's probably a watchpoint message.
**       It will search the watchpoint definition table for matches
**       to this MessageID and handle them as needed.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessageID    A #CFE_SB_MsgId_t that holds the
**                             message ID 
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_ProcessWP
**
*************************************************************************/
void LC_CheckMsgForWPs(CFE_SB_MsgId_t  MessageID, 
                       CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Validate watchpoint definition table (WDT)
**  
**  \par Description
**       This function is called by table services when a validation of 
**       the watchpoint definition table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   *TableData     Pointer to the table data to validate
**  
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #LC_WDTVAL_ERR_DATATYPE \retdesc \copydoc LC_WDTVAL_ERR_DATATYPE \endcode
**  \retcode #LC_WDTVAL_ERR_OPER     \retdesc \copydoc LC_WDTVAL_ERR_OPER     \endcode
**  \retcode #LC_WDTVAL_ERR_MID      \retdesc \copydoc LC_WDTVAL_ERR_MID      \endcode
**  \retcode #LC_WDTVAL_ERR_FPNAN    \retdesc \copydoc LC_WDTVAL_ERR_FPNAN    \endcode
**  \retcode #LC_WDTVAL_ERR_FPINF    \retdesc \copydoc LC_WDTVAL_ERR_FPINF    \endcode
**  \endreturns
**
**  \sa #LC_ValidateADT
**
*************************************************************************/
int32 LC_ValidateWDT(void *TableData);

/************************************************************************/
/** \brief Create watchpoint hash table
**  
**  \par Description
**       Creates a hash table to optimize the process of getting direct
**       access to all the watchpoint table entries that reference a
**       particular MessageID without having to search the entire table.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #LC_GetHashTableIndex, #LC_AddWatchpoint
**
*************************************************************************/
void LC_CreateHashTable(void);

/************************************************************************/
/** \brief Process a single watchpoint
**  
**  \par Description
**       Support function for watchpoint processing that will
**       evaluate a single watchpoint
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   WatchIndex  The watchpoint number to evaluate (zero
**                            based watchpoint definition table index)
**
**  \param [in]   MessagePtr  A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message that
**                            contains the watchpoint data
**
**  \param [in]   Timestamp   A #CFE_TIME_SysTime_t timestamp to use
**                            to update the watchpoint results data
**                            if a state transition is detected
**
*************************************************************************/
void LC_ProcessWP(uint16             WatchIndex, 
                  CFE_SB_MsgPtr_t    MessagePtr,
                  CFE_TIME_SysTime_t Timestamp);

/************************************************************************/
/** \brief Operator comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       the watchpoint data comparison based upon the operator and
**       data type specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex         The watchpoint number to compare (zero
**                                 based watchpoint definition table index)
**
**  \param [in] ProcessedWPData    The watchpoint data extracted from
**                                 the message that it was contained
**                                 in. This is the data after any
**                                 sizing, bit-masking, and endianess
**                                 fixing that LC might have done
**                                 according to the watchpoint definition
**
**  \returns
**  \retcode #LC_WATCH_ERROR  \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \retstmt Return codes from #LC_SignedCompare    \endcode
**  \retstmt Return codes from #LC_UnsignedCompare  \endcode
**  \retstmt Return codes from #LC_FloatCompare     \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_OperatorCompare(uint16 WatchIndex,
                         uint32 ProcessedWPData);

/************************************************************************/
/** \brief Signed comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       a signed watchpoint data comparison based upon the operator
**       specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex    The watchpoint number to compare (zero
**                            based watchpoint definition table index)
**
**  \param [in] WPValue       The watchpoint data extracted from
**                            the message that it was contained
**                            in. This is the data after any
**                            sizing, bit-masking, and endianess
**                            fixing that LC might have done
**                            according to the watchpoint definition
**
**  \param [in] CompareValue  The comparison value specified in the 
**                            watchpoint definition table (sign
**                            extended, if needed, in an int32)
**
**  \returns
**  \retcode #LC_WATCH_TRUE  \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_SignedCompare(uint16 WatchIndex,
                       int32  WPValue, 
                       int32  CompareValue);
  
/************************************************************************/
/** \brief Unsigned comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       an unsigned watchpoint data comparison based upon the operator
**       specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex    The watchpoint number to compare (zero
**                            based watchpoint definition table index)
**
**  \param [in] WPValue       The watchpoint data extracted from
**                            the message that it was contained
**                            in. This is the data after any
**                            sizing, bit-masking, and endianess
**                            fixing that LC might have done
**                            according to the watchpoint definition
**
**  \param [in] CompareValue  The comparison value specified in the 
**                            watchpoint definition table (zero
**                            extended, if needed, in an uint32)
**
**  \returns
**  \retcode #LC_WATCH_TRUE  \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_UnsignedCompare(uint16 WatchIndex,
                         uint32 WPValue, 
                         uint32 CompareValue);
  
/************************************************************************/
/** \brief Float comparison
**  
**  \par Description
**       Support function for watchpoint processing that will perform
**       an floating point watchpoint data comparison based upon the operator
**       specified in the watchpoint definition table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in] WatchIndex        The watchpoint number to compare (zero
**                                based watchpoint definition table index)
**
**  \param [in] WPMultiType       The watchpoint data extracted from
**                                the message that it was contained
**                                in. Stored in a multi-type union.
**                                This is the data after any sizing,
**                                bit-masking, and endianess fixing
**                                that LC might have done according
**                                to the watchpoint definition
**
**  \param [in] CompareMultiType  The comparison value specified in the 
**                                watchpoint definition table. Stored
**                                in a muti-type union so it can easily
**                                be accessed as a uint32 for validity
**                                checks
**
**  \returns
**  \retcode #LC_WATCH_TRUE  \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR \retdesc \copydoc LC_WATCH_ERROR \endcode
**  \endreturns
**
*************************************************************************/
uint8 LC_FloatCompare(uint16 WatchIndex,
                      LC_MultiType_t WPMultiType, 
                      LC_MultiType_t CompareMultiType);

/************************************************************************/
/** \brief Watchpoint offset valid
**  
**  \par Description
**       Support function for watchpoint processing that will check if
**       the watchpoint offset specified in the definition table would
**       extend past the message that contains the watchpoint data
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   WatchIndex  The watchpoint number to check (zero
**                            based watchpoint definition table index)
**
**  \param [in]   MessagePtr  A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message that
**                            contains the watchpoint data
**
**  \returns
**  \retstmt Returns TRUE if the offset is within the message size \endcode
**  \retstmt Returns FALSE if the offset extends past message end  \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_WPOffsetValid(uint16          WatchIndex, 
                         CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Get sized data
**  
**  \par Description
**       Support function for watchpoint processing that will extract
**       the watchpoint data from a software bus message based upon the
**       data type specified in the watchpoint definition table and 
**       store it in a uint32. If there are any endian differences between
**       LC and the watchpoint data, this is where it will get fixed up.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  WatchIndex     The watchpoint number to extract (zero
**                              based watchpoint definition table index)
**
**  \param [in]  WPDataPtr      A pointer to the first byte of the 
**                              watchpoint data as it exists in the 
**                              software bus message it was received in
**
**  \param [in]  SizedDataPtr   A pointer to where the extracted watchpoint
*                               data should be stored
**
**  \param [out] *SizedDataPtr  Contains the extracted watchpoint data.
**                              This will be set to zero on error
**
**  \returns
**  \retstmt Returns TRUE if no error           \endcode
**  \retstmt Returns FALSE if an error occurred \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_GetSizedWPData(uint16 WatchIndex,
                          uint8  *WPDataPtr,
                          uint32 *SizedDataPtr);

/************************************************************************/
/** \brief Check uint32 for float NAN
**  
**  \par Description
**       Utility function for watchpoint processing that will test if
**       a uint32 value would result in a NAN (not-a-number) value if
**       it was interpreted as a float.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  Data     The uint32 value to check
**
**  \returns
**  \retstmt Returns TRUE if value is a float NAN      \endcode
**  \retstmt Returns FALSE if value is not a float NAN \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_Uint32IsNAN(uint32 Data);

/************************************************************************/
/** \brief Check uint32 for float infinite
**  
**  \par Description
**       Utility function for watchpoint processing that will test if
**       a uint32 value would result in an infinite value if
**       it was interpreted as a float.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  Data     The uint32 value to check
**
**  \returns
**  \retstmt Returns TRUE if value is an inifinite float      \endcode
**  \retstmt Returns FALSE if value is not an inifinite float \endcode
**  \endreturns
**
*************************************************************************/
boolean LC_Uint32IsInfinite(uint32 Data);

/************************************************************************/
/** \brief Convert messageID into hash table index
**  
**  \par Description
**       Utility function for watchpoint processing that converts a
**       messageID into an index into the watchpoint hash table.
**       
**       The following code supports use of the watchpoint hash table:
**
**       1) #LC_GetHashTableIndex - convert messageID to hash table index
**       2) #LC_CreateHashTable   - after load Watchpoint Definition Table
**       3) #LC_AddWatchpoint     - add one watchpoint to hash table
**       4) #LC_CheckMsgForWPs    - process all WP's that reference messageID
**       
**       The following data structures support the hash table:
**
**       1) Hash table (256 entries)
**       2) Array of links for messageID linked lists (LC_MAX_WATCHPOINTS)
**       3) Array of links for watchpoint linked lists (LC_MAX_WATCHPOINTS)
**       
**       Rather than search the entire Watchpoint Definition Table to find
**       the watchpoints that reference a particular messageID, LC does
**       the following:
**
**       1) Call hash table function (convert messageID to hash table index)
**       2) Get messageID linked list from indexed hash table entry
**       3) Search messageID list (max 8) for matching messageID
**       4) Get watchpoint linked list from matching messageID link
**       5) Done - only watchpoints that reference messageID are in list
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessageID   SoftwareBus packet message ID
**
**  \returns
**  \retstmt Returns index into watchpoint hash table  \endcode
**  \endreturns
**
*************************************************************************/
uint32 LC_GetHashTableIndex(CFE_SB_MsgId_t MessageID);

/************************************************************************/
/** \brief Add one watchpoint linked list entry during creation of hash table
**  
**  \par Description
**       Utility function that adds another link to the watchpoint linked list
**       for the specified messageID. The function will also add a messageID
**       linked list entry to the hash table if this is the first reference
**       to that messageID. The function will also subscribe to the messageID
**       if this is the first reference to that messageID. The function will
**       return a pointer to the watchpoint linked list entry just added.
**       
**       The following code supports use of the watchpoint hash table:
**
**       1) #LC_GetHashTableIndex - convert messageID to hash table index
**       2) #LC_CreateHashTable   - after load Watchpoint Definition Table
**       3) #LC_AddWatchpoint     - add one watchpoint to hash table
**       4) #LC_CheckMsgForWPs    - process all WP's that reference messageID
**       
**       The following data structures support the hash table:
**
**       1) Hash table (256 entries)
**       2) Array of links for messageID linked lists (LC_MAX_WATCHPOINTS)
**       3) Array of links for watchpoint linked lists (LC_MAX_WATCHPOINTS)
**       
**       Rather than search the entire Watchpoint Definition Table to find
**       the watchpoints that reference a particular messageID, LC does
**       the following:
**
**       1) Call hash table function (convert messageID to hash table index)
**       2) Get messageID linked list from indexed hash table entry
**       3) Search messageID list (max 8) for matching messageID
**       4) Get watchpoint linked list from matching messageID link
**       5) Done - only watchpoints that reference messageID are in list
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessageID   SoftwareBus packet message ID
**
**  \returns
**  \retstmt Returns pointer to the watchpoint linked list entry just added  \endcode
**  \endreturns
**
*************************************************************************/
LC_WatchPtList_t  *LC_AddWatchpoint(CFE_SB_MsgId_t MessageID);
 
#endif /* _lc_watch_ */

/************************/
/*  End of File Comment */
/************************/
