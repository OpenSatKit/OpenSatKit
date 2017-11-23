/*************************************************************************
** File:
**   $Id: lc_tbl.h 1.3 2015/03/04 16:09:53EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   Specification for the CFS Limit Checker (LC) table related data 
**   structures
**
** Notes:
**   Constants and enumerated types related to these table structures
**   are defined in lc_tbldefs.h. They are kept separate to allow easy 
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in lc_tbldefs.h for more 
**   info).
**
**   $Log: lc_tbl.h  $
**   Revision 1.3 2015/03/04 16:09:53EST sstrege 
**   Added copyright information
**   Revision 1.2 2012/08/01 15:41:30EDT lwalling 
**   Add STALE counters to watchpoint definition and result tables
**   Revision 1.1 2012/07/31 13:53:39PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.5 2010/02/19 17:44:15EST lwalling 
**   Change state events limits to individual limits for Passive AP, FailToPass and PassToFail
**   Revision 1.4 2009/12/28 14:50:53EST lwalling 
**   Add event limiter to a/p def table, add event counter to a/p results table
**   Revision 1.3 2009/04/18 14:50:45EDT dkobe 
**   Cleaned up comments for users guide generation
**   Revision 1.2 2009/01/15 15:36:17EST dahardis 
**   Unit test fixes
**   Revision 1.1 2008/12/10 10:57:04EST dahardis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_tbl_
#define _lc_tbl_

#define _ix86_  /* TODO - Fix endianness definitions */ 

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "lc_platform_cfg.h"
#include "lc_tbldefs.h"

#define _STRUCT_LOW_BIT_FIRST_ /* TODO - Fix endianness definitions */

/*************************************************************************
** Type Definitions
*************************************************************************/
/*
** We need to know if we are being compiled for a big or little endian
** target to layout these structures correctly. These compiler switches
** are defined in the OSAL header common_types.h and assume the processor
** family has been defined properly in the make file.
*/
#ifdef _STRUCT_HIGH_BIT_FIRST_

    /** 
    **  \brief Signed 8-bit byte in 32 bit double word data type, Big Endian
    */
    typedef struct {
        int8    Bits31to24;
        int8    Bits23to16;
        int8    Bits15to8;
        int8    Signed8;

    } LC_Signed8in32_t;

    /** 
    **  \brief Unsigned 8-bit byte in 32 bit double word data type, Big Endian
    */
    typedef struct {
        uint8   Bits31to24;
        uint8   Bits23to16;
        uint8   Bits15to8;
        uint8   Unsigned8;

    } LC_Unsigned8in32_t;

    /** 
    **  \brief Signed 16-bit word in 32 bit double word data type, Big Endian
    */
    typedef struct {
        int16   Bits31to16;
        int16   Signed16;

    } LC_Signed16in32_t;
 
    /** 
    **  \brief Unsigned 16-bit word in 32 bit double word data type, Big Endian
    */
    typedef struct {
        uint16  Bits31to16;
        uint16  Unsigned16;

    } LC_Unsigned16in32_t;
    
#endif /* _STRUCT_HIGH_BIT_FIRST_ */

#ifdef _STRUCT_LOW_BIT_FIRST_

    /** 
    **  \brief Signed 8-bit byte in 32 bit double word data type, Little Endian
    */
    typedef struct {
        int8    Signed8;
        int8    Bits15to8;
        int8    Bits23to16;
        int8    Bits31to24;

    } LC_Signed8in32_t;

    /** 
    **  \brief Unsigned 8-bit byte in 32 bit double word data type, Little Endian
    */
    typedef struct {
        uint8   Unsigned8;
        uint8   Bits15to8;
        uint8   Bits23to16;
        uint8   Bits31to24;

    } LC_Unsigned8in32_t;

    /** 
    **  \brief Signed 16-bit word in 32 bit double word data type, Little Endian
    */
    typedef struct {
        int16   Signed16;
        int16   Bits31to16;

    } LC_Signed16in32_t;

    /** 
    **  \brief Unsigned 16-bit word in 32 bit double word data type, Little Endian
    */
    typedef struct {
        uint16  Unsigned16;
        uint16  Bits31to16;

    } LC_Unsigned16in32_t;

#endif /* _STRUCT_LOW_BIT_FIRST_ */

/** 
    \brief Watchpoint Definition Table (WDT) MultiType Union
    This union is used to set and extract the comparison value
    in each WDT entry since it needs to fill a 32 bit fixed
    width field. A conditional compile switch gets the endianess 
    right.   
*/
typedef union {
    LC_Signed8in32_t      Signed8in32;
    LC_Unsigned8in32_t    Unsigned8in32;
    
    LC_Signed16in32_t     Signed16in32;
    LC_Unsigned16in32_t   Unsigned16in32;
    
    int32                 Signed32;
    uint32                Unsigned32;
    float                 Float32;
    
} LC_MultiType_t;

/** 
**  \brief Watchpoint Definition Table (WDT) Entry
*/
typedef struct {
    uint8           DataType;                 /**< \brief Watchpoint Data Type (enumerated)     */
    uint8           OperatorID;               /**< \brief Comparison type (enumerated)          */
    uint16          MessageID;                /**< \brief Message ID for the message containing 
                                                          the watchpoint                        */
    uint32          WatchpointOffset;         /**< \brief Byte offset from the beginning of
                                                          the message (including any headers) 
                                                          to the watchpoint                     */
    uint32          BitMask;                  /**< \brief Value to be masked with watchpoint
                                                          data prior to comparison              */
    LC_MultiType_t  ComparisonValue;          /**< \brief Value watchpoint data is compared
                                                          against                               */
    uint32          ResultAgeWhenStale;       /**< \brief Number of LC Sample Actionpoint
                                                          commands that must be processed after
                                                          comparison before result goes stale   */
    uint32          CustomFuncArgument;       /**< \brief Data passed to the custom function 
                                                          when Operator_ID is set to 
                                                          #LC_OPER_CUSTOM                       */
} LC_WDTEntry_t;

/** 
**  \brief Actionpoint Definition Table (ADT) Entry
*/
typedef struct {
    uint8           DefaultState;             /**< \brief Default state for this AP (enumerated)    
                                                          States are defined in lc_msgdefs.h        */
    uint8           MaxPassiveEvents;         /**< \brief Max number of events before filter
                                                          - RTS not started because AP is passive   */
    uint8           MaxPassFailEvents;        /**< \brief Max number of events before filter
                                                          - AP result transition from pass to fail  */
    uint8           MaxFailPassEvents;        /**< \brief Max number of events before filter
                                                          - AP result transition from fail to pass  */
    uint16          RTSId;                    /**< \brief RTS to request if this AP fails           */
    uint16          MaxFailsBeforeRTS;        /**< \brief How may consecutive failures before 
                                                          an RTS request is issued                  */

    uint16          RPNEquation [LC_MAX_RPN_EQU_SIZE];  /**< \brief Reverse Polish Equation that 
                                                                    specifies when this actionpoint
                                                                    should fail                     */

    uint16          EventType;                /**< \brief Event type used for event msg if AP fails:
                                                          #CFE_EVS_INFORMATION, #CFE_EVS_DEBUG, 
                                                          #CFE_EVS_ERROR, or #CFE_EVS_CRITICAL      */
                                                          
    uint16          EventID;                  /**< \brief Event ID used for event msg if AP fails   
                                                          See lc_events.h for those already in use  */
    
    char            EventText[LC_MAX_ACTION_TEXT];      /**< \brief Text used for the event msg when 
                                                                    this AP fails                   */
} LC_ADTEntry_t;

/** 
**  \brief Watchpoint Transition Structure
*/
typedef struct
{
    uint32               Value;            /**< \brief Watchpoint value at comparison that caused 
                                                       the transition                               */
    CFE_TIME_SysTime_t   Timestamp;        /**< \brief Timstamp when the transition was detected    */

} LC_WRTTransition_t;

/** 
**  \brief Watchpoint Results Table (WRT) Entry
*/
typedef struct {
    uint8                WatchResult;             /**< \brief Result for the last evaluation of this
                                                              watchpoint (enumerated)                */
    uint8                Padding[3];
    
    uint32               CountdownToStale;        /**< \brief Number of LC Sample Actionpoint 
                                                              commands still to be processed
                                                              before WatchResult becomes stale        */
    uint32               EvaluationCount;         /**< \brief How many times this watchpoint has
                                                              been evaluated                         */
    uint32               FalseToTrueCount;        /**< \brief How many times this watchpoint has
                                                              transitioned from FALSE to TRUE        */
    uint32               ConsecutiveTrueCount;    /**< \brief Number of consecutive times this  
                                                              watchpoint has evaluated to TRUE       */
    uint32               CumulativeTrueCount;     /**< \brief Total number of times this watchpoint
                                                              has evaluated to TRUE                  */ 
    LC_WRTTransition_t   LastFalseToTrue;         /**< \brief Last transition from FALSE to TRUE     */
    LC_WRTTransition_t   LastTrueToFalse;         /**< \brief Last transition from TRUE to FALSE     */
    
} LC_WRTEntry_t;

/** 
**  \brief Actionpoint Results Table (ART) Entry
*/
typedef struct {
    uint8                ActionResult;            /**< \brief Result for the last sample of this
                                                              actionpoint                            */
    uint8                CurrentState;            /**< \brief Current state of this actionpoint      */

    uint16               Padding;
    
    uint32               PassiveAPCount;          /**< \brief Total number of times RTS not invoked
                                                              because this AP was passive            */
    uint32               FailToPassCount;         /**< \brief How many times this actionpoint has
                                                              transitioned from Fail to Pass         */
    uint32               PassToFailCount;         /**< \brief How many times this actionpoint has
                                                              transitioned from Pass to Fail         */
    uint32               ConsecutiveFailCount;    /**< \brief Number of consecutive times this  
                                                              actionpoint has evaluated to Fail      */
    uint32               CumulativeFailCount;     /**< \brief Total number of times this actionpoint   
                                                              has evaluated to Fail                  */
    uint32               CumulativeRTSExecCount;  /**< \brief Total number of times an RTS request
                                                              has been sent for this actionpoint     */
} LC_ARTEntry_t;

#endif /*_lc_tbl_*/

/************************/
/*  End of File Comment */
/************************/
