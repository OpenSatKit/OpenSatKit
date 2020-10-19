/*************************************************************************
** File:
**  $Id: DS_Safe_tbl.c 1.13 2013/08/26 14:22:56EDT sstrege Exp  $
**
** Purpose:
**   Data Storage (DS) Packet Filter table
**
** Notes:
**   This table is computer generated and is NOT to be hand modified.
**   Generated August 26, 2013, 1:11 pm
**
**
**   $Log: DS_Safe_tbl.c  $
**   Revision 1.13 2013/08/26 14:22:56EDT sstrege
**   GPM-SCoRe-2270 Updates
**
*/
/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "ds_platform_cfg.h"
#include "ds_appdefs.h"
#include "ds_app.h"
#include "ds_msg.h"
#include "cfe_msgids.h"
#include "ds_msgids.h"

#define FILE_ALL_EVENTS       0
#define FILE_ALL_APP_HK_PKTS  1
#define FILE_ALL_APP_TLM_PKTS 2
#define FILE_ALL_HW_TLM_PKTS  3
#define FILE_CFE_APP_HK_PKTS  4
#define FILE_CFE_APP_TLM_PKTS 5

/*************************************************************************
** Exported Data
*************************************************************************/
/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "DS_FilterTable", "DS.FILTER_TBL", "DS Packet Filter Table",
    "DS_Safe_tbl.tbl", sizeof(DS_FilterTable_t)
};
/*
** Packet filter table data
*/
DS_FilterTable_t DS_FilterTable =
{
    "DS_Safe filter",
    {
        /** Message ID,   {File table index,   Filter type,   N,   X,   O},   {------},   **/
        /* Packet 0x800 ( cFE Executive Services (ES) HK ) */
        { 0x800, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x801 ( cFE Event Services (EVS) HK ) */
        { 0x801, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x803 ( cFE Software Bus (SB) HK ) */
        { 0x803, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x804 ( cFE Table Services (TBL) HK ) */
        { 0x804, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x805 ( cFE Time Services (TIME) HK ) */
        { 0x805, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x806 ( cFE Time Services (TIME) DiagTlm ) */
        { 0x806, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x808 ( cFE Event Service (EVS) Message Tlm ) */
        { 0x808, { { 1, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x80A ( cFE Software Bus (SB) Statistics Tlm ) */
        { 0x80A, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x80B ( cFE Executive Service (ES) Application Tlm ) */
        { 0x80B, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x80C ( cFE Table Services (TBL) Register Tlm ) */
        { 0x80C, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x80F ( cFE Executive Service (ES) Shell Tlm ) */
        { 0x80F, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x810 ( cFE Executive Service (ES) Memory Stats Tlm ) */
        { 0x810, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x86E ( S-Comm A Xponder A HK ) */
        { 0x86E, { { 0, 1, 1, 2, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x86F ( S-Band Comm. Card A HK ) */
        { 0x86F, { { 0, 1, 1, 2, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x871 ( S-Comm A Xponder B HK ) */
        { 0x871, { { 0, 1, 1, 2, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x878 ( S-Comm to SBC Backup Data Frame ) */
        { 0x878, { { 3, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x879 ( S-Band Comm. Card B HK ) */
        { 0x879, { { 0, 1, 1, 2, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x87A ( S-Comm B Xponder A HK ) */
        { 0x87A, { { 0, 1, 1, 2, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x87B ( S-Comm B Xponder B HK ) */
        { 0x87B, { { 0, 1, 1, 2, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x880 ( Telemetry Output (TO) HK (lab) ) */
        { 0x880, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x881 ( Telemetry Output (TO) Data Types (lab) ) */
        { 0x881, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x884 ( Command Ingest (CI) HK (lab) ) */
        { 0x884, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x887 ( Memory Manager (MM) HK ) */
        { 0x887, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x88A ( File Manager (FM) HK ) */
        { 0x88A, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x88B ( File Manager (FM) File Stats Tlm ) */
        { 0x88B, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x88C ( File Manager (FM) Directory Listing Tlm ) */
        { 0x88C, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x88D ( File Manager (FM) Open File Stats Tlm ) */
        { 0x88D, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x88E ( File Manager (FM) Free Space Tlm ) */
        { 0x88E, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x890 ( GMI Housekeeping #1 ) */
        { 0x890, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x891 ( GMI Housekeeping #2 ) */
        { 0x891, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x892 ( GMI Science ) */
        { 0x892, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x894 ( GMI Memory Dump ) */
        { 0x894, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x895 ( GMI Calibration Data ) */
        { 0x895, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x899 ( Scheduler (SCH) HK ) */
        { 0x899, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x89A ( Scheduler (SCH) Diag ) */
        { 0x89A, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x89B ( Housekeeping (HK) HK ) */
        { 0x89B, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x89C ( Housekeeping (HK) Combined1 - Xpndr ) */
        { 0x89C, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x89D ( Housekeeping (HK) Combined2 - CDH + MAC A ) */
        { 0x89D, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x89E ( Housekeeping (HK) Combined3 - CDH FSW HS ) */
        { 0x89E, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x89F ( Housekeeping (HK) Combined4 - GNC FSW HS ) */
        { 0x89F, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8A0 ( Housekeeping (HK) Combined5 - GNC CL DV ) */
        { 0x8A0, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8A1 ( Housekeeping (HK) Combined6 - CDH + MAC B ) */
        { 0x8A1, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8A2 ( Spare ) */
        { 0x8A2, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8A3 ( Spare ) */
        { 0x8A3, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8A4 ( Checksum (CS) HK ) */
        { 0x8A4, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8A7 ( Limit Checker (LC) HK ) */
        { 0x8A7, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8AA ( Stored Command (SC) HK ) */
        { 0x8AA, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8AD ( Health and Safety (HS) HK ) */
        { 0x8AD, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8AE ( Health and Safety (HS) TEP HK ) */
        { 0x8AE, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8B0 ( CCSDS File Delivery Protocl (CF) HK ) */
        { 0x8B0, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8B1 ( CF Transaction HK ) */
        { 0x8B1, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8B3 ( CF Config HK ) */
        { 0x8B3, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8B8 ( Data Storage (DS) HK ) */
        { 0x8B8, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8BB ( Memory Dwell (MD) HK ) */
        { 0x8BB, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8BC ( MD Pkt1 ) */
        { 0x8BC, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8BD ( MD Pkt2 ) */
        { 0x8BD, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8BE ( MD Pkt3 ) */
        { 0x8BE, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8BF ( MD Pkt4 ) */
        { 0x8BF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8FA ( AGS IRU A ) */
        { 0x8FA, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8FB ( AGS ST A ) */
        { 0x8FB, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8FC ( AGS ATB A ) */
        { 0x8FC, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8FD ( AGS IRU B ) */
        { 0x8FD, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8FE ( AGS ST B ) */
        { 0x8FE, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x8FF ( AGS ATB B ) */
        { 0x8FF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x990 ( Spare ) */
        { 0x990, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x991 ( Spare ) */
        { 0x991, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x992 ( Spare ) */
        { 0x992, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x993 ( Spare ) */
        { 0x993, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x994 ( Spare ) */
        { 0x994, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x995 ( Spare ) */
        { 0x995, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x996 ( Spare ) */
        { 0x996, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x997 ( Spare ) */
        { 0x997, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x998 ( Spare ) */
        { 0x998, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x999 ( Spare ) */
        { 0x999, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x99A ( Spacewire (SW) EOP HK ) */
        { 0x99A, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x99B ( Spacewire (SW) S-Comm HK ) */
        { 0x99B, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x99C ( File Commanding (FC) HK ) */
        { 0x99C, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x99D ( Housekeeping Recorder (HR) HK ) */
        { 0x99D, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x99E ( Time Manager DIO Latch HK ) */
        { 0x99E, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x99F ( Time Manager DownSync HK ) */
        { 0x99F, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A0 ( Command Ingest (CI) HK ) */
        { 0x9A0, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A1 ( Telemetry Output (TO) HK ) */
        { 0x9A1, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A2 ( DPR Instrument Manager (DM) Tlm ) */
        { 0x9A2, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A3 ( GMI Instrument Manager (GM) Tlm ) */
        { 0x9A3, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A4 ( Memory Scrub (MS) HK ) */
        { 0x9A4, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A5 ( Power and Support (PS) HK ) */
        { 0x9A5, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A6 ( Recorder Manager (RM) HK ) */
        { 0x9A6, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A7 ( SpaceWire (SW) HK ) */
        { 0x9A7, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A8 ( External Bus (XB) HK ) */
        { 0x9A8, { { 0, 1, 1, 3, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9A9 ( Telemetry Output (TO) Data Types ) */
        { 0x9A9, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9AA ( XB Diagnostic Telemetry ) */
        { 0x9AA, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9AB ( DM Diagnostic Telemetry ) */
        { 0x9AB, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9AC ( GM Diagnostic Telemetry ) */
        { 0x9AC, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9AD ( SDR Housekeeping Telemetry Packet ) */
        { 0x9AD, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9AE ( SDR Diagnostic Telemetry Packet ) */
        { 0x9AE, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9AF ( SDR Bad Memory Map Diagnostic Packet ) */
        { 0x9AF, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B0 ( RM SDB Diagnostic Packet ) */
        { 0x9B0, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B1 ( Time Manager HK ) */
        { 0x9B1, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B2 ( Time Manager Diag HK ) */
        { 0x9B2, { { 0, 1, 1, 12, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B3 ( CL SA Diag Tlm ) */
        { 0x9B3, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B4 ( CL HGA Diag Tlm ) */
        { 0x9B4, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B5 ( CL Target Diagnostic Generation ) */
        { 0x9B5, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B6 ( Spare ) */
        { 0x9B6, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B7 ( Spare ) */
        { 0x9B7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B8 ( Spare ) */
        { 0x9B8, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9B9 ( Spare ) */
        { 0x9B9, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9BA ( Spare ) */
        { 0x9BA, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9BB ( Spare ) */
        { 0x9BB, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9BC ( Spare ) */
        { 0x9BC, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9BD ( Spare ) */
        { 0x9BD, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9BE ( Spare ) */
        { 0x9BE, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9BF ( Spare ) */
        { 0x9BF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C0 ( Spare ) */
        { 0x9C0, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C1 ( SD SS/TAM Diagnostic Telemetry ) */
        { 0x9C1, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C2 ( SD KF Diagnostic Telemetry ) */
        { 0x9C2, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C3 ( SD Momentum Telemetry ) */
        { 0x9C3, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C4 ( CG MTB Diag Tlm ) */
        { 0x9C4, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C5 ( CG RW Diag Tlm ) */
        { 0x9C5, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C6 ( CG SA Diag Tlm ) */
        { 0x9C6, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C7 ( CG HGA Diag Tlm ) */
        { 0x9C7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C8 ( CG Prop Diag Tlm ) */
        { 0x9C8, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9C9 ( DI HK ) */
        { 0x9C9, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9CA ( OM HK ) */
        { 0x9CA, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9CB ( SD HK ) */
        { 0x9CB, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9CC ( CL HK ) */
        { 0x9CC, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9CD ( CG HK ) */
        { 0x9CD, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9CE ( DI FDC Tlm ) */
        { 0x9CE, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9CF ( OM FDC Tlm ) */
        { 0x9CF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D0 ( SD FDC Tlm ) */
        { 0x9D0, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D1 ( CL FDC Tlm ) */
        { 0x9D1, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D2 ( CG FDC Tlm ) */
        { 0x9D2, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D3 ( DI Downlink Telemetry ) */
        { 0x9D3, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D4 ( OM Downllink Telemetry ) */
        { 0x9D4, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D5 ( SD Downlink Telemetry ) */
        { 0x9D5, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D6 ( CL Downlink Telemetry ) */
        { 0x9D6, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D7 ( CG Downlink Telemetry ) */
        { 0x9D7, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D8 ( DI to OM Internal Telemetry ) */
        { 0x9D8, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9D9 ( OM to SD Internal Telemetry ) */
        { 0x9D9, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9DA ( SD to CL Internal Telemetry ) */
        { 0x9DA, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9DB ( CL to CG Internal Telemetry ) */
        { 0x9DB, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9DD ( ATB A Raw Thermal Telemetry ) */
        { 0x9DD, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9DE ( ATB A Raw Position Telemetry ) */
        { 0x9DE, { { 0, 1, 1, 60, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9DF ( ATB B Raw Thermal Telemetry ) */
        { 0x9DF, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E0 ( ATB B Raw Position Telemetry ) */
        { 0x9E0, { { 0, 1, 1, 60, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E3 ( IRU A BIT Raw Telemetry ) */
        { 0x9E3, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E4 ( IRU B BIT Raw Telemetry ) */
        { 0x9E4, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E5 ( Prop Card A1 Raw Telemetry ) */
        { 0x9E5, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E6 ( Prop Card A2 Raw Telemetry ) */
        { 0x9E6, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E7 ( Prop Card B1 Raw Telemetry ) */
        { 0x9E7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E8 ( Prop Card B2 Raw Telemetry ) */
        { 0x9E8, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9E9 ( HGA MCE A Raw Gimbal Telemetry ) */
        { 0x9E9, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9EA ( IRU A Raw Telemetry ) */
        { 0x9EA, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9EB ( RW Raw Telemetry ) */
        { 0x9EB, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9EC ( ATB A Raw MTB/SS/TAM Telemetry ) */
        { 0x9EC, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9EE ( Star Tracker A Raw Quaternion Telemetry ) */
        { 0x9EE, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9EF ( DI to SD Internal Telemetry ) */
        { 0x9EF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9F0 ( SA MCE A Raw Gimbal Telemetry ) */
        { 0x9F0, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9F4 ( Deploy Card A1 Raw Telemetry ) */
        { 0x9F4, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9F5 ( Deploy Card A2 Raw Telemetry ) */
        { 0x9F5, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9F6 ( Deploy Card B1 Raw Telemetry ) */
        { 0x9F6, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9F7 ( Deploy Card B2 Raw Telemetry ) */
        { 0x9F7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9F9 ( HGA MCE B Raw Gimbal Telemetry ) */
        { 0x9F9, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9FA ( IRU B Raw Rate Data ) */
        { 0x9FA, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9FB ( SA MCE B Raw Gimbal Data ) */
        { 0x9FB, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9FC ( ATB B Raw MTB/SS/TAM Telemetry ) */
        { 0x9FC, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0x9FE ( Star Tracker B Raw Quaternion Data ) */
        { 0x9FE, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA00 ( MAC A HK Telemetry ) */
        { 0xA00, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA01 ( MAC A Diagnostic Telemetry ) */
        { 0xA01, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA02 ( MAC B HK Telemetry ) */
        { 0xA02, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA03 ( MAC B Diagnostic Telemetry ) */
        { 0xA03, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA04 ( ATB A HK Telemetry ) */
        { 0xA04, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA05 ( ATB A FPGA HK Telemetry ) */
        { 0xA05, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA06 ( ATB A Voltage (test) Telemetry ) */
        { 0xA06, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA07 ( ATB A DAC (test) Telemetry ) */
        { 0xA07, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA09 ( ATB B HK Telemetry ) */
        { 0xA09, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA0A ( ATB B FPGA HK Telemetry ) */
        { 0xA0A, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA0B ( ATB B Voltage (test) Telemetry ) */
        { 0xA0B, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA0C ( ATB B DAC (test) Telemetry ) */
        { 0xA0C, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA0D ( Raw 1553 Read ) */
        { 0xA0D, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA10 ( SA MCE A HK Telemetry ) */
        { 0xA10, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA11 ( SA MCE B HK Telemetry ) */
        { 0xA11, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA13 ( HGA MCE A HK Telemetry ) */
        { 0xA13, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA14 ( HGA MCE B HK Telemetry ) */
        { 0xA14, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA16 ( PSE A Analog HK Telemetry ) */
        { 0xA16, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA17 ( PSE A FDC HK Telemetry ) */
        { 0xA17, { { 0, 1, 1, 15, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA18 ( PSE A Debug HK Telemetry ) */
        { 0xA18, { { 0, 1, 1, 60, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA19 ( PSE A Overview HK Telemetry ) */
        { 0xA19, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA1A ( PSE B Analog HK Telemetry ) */
        { 0xA1A, { { 0, 1, 1, 15, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA1B ( PSE B FDC HK Telemetry ) */
        { 0xA1B, { { 0, 1, 1, 15, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA1C ( PSE B Debug HK Telemetry ) */
        { 0xA1C, { { 0, 1, 1, 60, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA1D ( PSE B Overview HK Telemetry ) */
        { 0xA1D, { { 0, 1, 1, 5, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA1E ( Star Tracker A Housekeeping Telemetry ) */
        { 0xA1E, { { 0, 1, 1, 60, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA1F ( Star Tracker A Diagnostic Telemetry ) */
        { 0xA1F, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA20 ( Star Tracker A Star Field Telemetry ) */
        { 0xA20, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA21 ( Star Tracker B Housekeeping Telemetry ) */
        { 0xA21, { { 0, 1, 1, 60, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA22 ( Star Tracker B Diagnostic Telemetry ) */
        { 0xA22, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA23 ( Star Tracker B Star Field Telemetry ) */
        { 0xA23, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA24 ( DiRWA #1 HK (SAs 3,5,6,7,8,9) ) */
        { 0xA24, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA25 ( DiRWA #2 HK (SAs 3,5,6,7,8,9) ) */
        { 0xA25, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA26 ( DiRWA #3 HK (SAs 3,5,6,7,8,9) ) */
        { 0xA26, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA27 ( DiRWA #4 HK (SAs 3,5,6,7,8,9) ) */
        { 0xA27, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA28 ( DiRWA #5 HK (SAs 3,5,6,7,8,9) ) */
        { 0xA28, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA29 ( RW SW Command Response ) */
        { 0xA29, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA2B ( IRU Interface Card A Telemetry ) */
        { 0xA2B, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA2D ( IRU Interface Card B Telemetry ) */
        { 0xA2D, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA2F ( IRU A Absolute Read ) */
        { 0xA2F, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA30 ( IRU A Gyro A Output ) */
        { 0xA30, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA31 ( IRU A Gyro B Output ) */
        { 0xA31, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA32 ( IRU A Gyro C Output ) */
        { 0xA32, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA33 ( IRU A Gyro D Output ) */
        { 0xA33, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA34 ( IRU B Absolute Read ) */
        { 0xA34, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA35 ( IRU B Gyro A Output ) */
        { 0xA35, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA36 ( IRU B Gyro B Output ) */
        { 0xA36, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA37 ( IRU B Gyro C Output ) */
        { 0xA37, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA38 ( IRU B Gyro D Output ) */
        { 0xA38, { { 0, 1, 1, 30, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA39 ( CL Rate Null Mode Diagnostic ) */
        { 0xA39, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA3A ( CL Sun Point Mode Diagnostic ) */
        { 0xA3A, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA3B ( CL Gyroless Sun Point Mode Diagnostic ) */
        { 0xA3B, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA3C ( CL Mission Science Mode Diagnostic ) */
        { 0xA3C, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA3D ( CL Slew Mode Diagnostic ) */
        { 0xA3D, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA3E ( CL Delta H Mode Diagnostic ) */
        { 0xA3E, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA3F ( CL Delta V Mode Diagnostic ) */
        { 0xA3F, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA40 ( GPS A - Reserved ) */
        { 0xA40, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA41 ( GPS A - Reserved ) */
        { 0xA41, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA42 ( GPS A - Board Health ) */
        { 0xA42, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA43 ( GPS A - Task Health ) */
        { 0xA43, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA44 ( GPS A - Single Point Solution ) */
        { 0xA44, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA45 ( GPS A - Time of Tone ) */
        { 0xA45, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA46 ( GPS A - PPS ) */
        { 0xA46, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA47 ( GPS A - Clock ) */
        { 0xA47, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA48 ( GPS A - Channel ) */
        { 0xA48, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA49 ( GPS A - SV Position ) */
        { 0xA49, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA4A ( GPS A - Measurement ) */
        { 0xA4A, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA4B ( GPS A - Pseudorange ) */
        { 0xA4B, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA4C ( GPS A - SV Prediction ) */
        { 0xA4C, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA4D ( GPS A - Reserved ) */
        { 0xA4D, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA4E ( GPS A - GEONS Real Time State ) */
        { 0xA4E, { { 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA4F ( GPS A - GEONS Covariance ) */
        { 0xA4F, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA50 ( GPS A - GEONS Residual ) */
        { 0xA50, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA51 ( GPS A - GEONS Estimated State ) */
        { 0xA51, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA52 ( GPS A - Command Ack ) */
        { 0xA52, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA53 ( GPS A - Ephemeris ) */
        { 0xA53, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA54 ( GPS A - Almanac ) */
        { 0xA54, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA55 ( GPS A - Ephemeris Status ) */
        { 0xA55, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA56 ( GPS A - SV Select Status ) */
        { 0xA56, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA57 ( GPS A - Reserved ) */
        { 0xA57, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA58 ( GPS A - FPGA Data ) */
        { 0xA58, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA59 ( GPS A - Memory ) */
        { 0xA59, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA5A ( GPS A - Memory Checksum ) */
        { 0xA5A, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA5B ( GPS A - GEONS Covariance_D Matrix ) */
        { 0xA5B, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA5C ( GPS A - GEONS Covariance_U Matrix ) */
        { 0xA5C, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA5D ( GPS A - GEONS Acceleration ) */
        { 0xA5D, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA5E ( GPS A - Acquisition ) */
        { 0xA5E, { { 0, 1, 1, 20, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA5F ( GPS A - PPBL Telemetry ) */
        { 0xA5F, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA60 ( GPS A - Detail ) */
        { 0xA60, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA61 ( GPS A - GEONS Comparison ) */
        { 0xA61, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA62 ( GPS A - GEONS Maneuver ) */
        { 0xA62, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA63 ( GPS A - GEONS Operation Parameters ) */
        { 0xA63, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA64 ( GPS A - GEONS Spacecraft Constants ) */
        { 0xA64, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA65 ( GPS A - GEONS Coordinate Transformation ) */
        { 0xA65, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA66 ( GPS A - GEONS Measurement Transmitter ) */
        { 0xA66, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA67 ( GPS A - GEONS Process Noise ) */
        { 0xA67, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA68 ( GPS A - GEONS Measurement Processing ) */
        { 0xA68, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA69 ( GPS A - GEONS Raw ) */
        { 0xA69, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA6A ( GPS A - Configuration Data ) */
        { 0xA6A, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA6B ( GPS A - Event ) */
        { 0xA6B, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA80 ( KaPR Housekeeping Packet ) */
        { 0xA80, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA81 ( KaPR SSPA Analysis Packet ) */
        { 0xA81, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA82 ( KaPR LNA Analysis Packet ) */
        { 0xA82, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA83 ( KaPR External Calibration Packet ) */
        { 0xA83, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA84 ( KaPR Internal Calibration Packet ) */
        { 0xA84, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA85 ( KaPR Phase Code Dump Packet ) */
        { 0xA85, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA86 ( KaPR Variable PRF Packet ) */
        { 0xA86, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA87 ( KaPR Memory Dump Packet ) */
        { 0xA87, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA88 ( KaPR Observation Packet ) */
        { 0xA88, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xA90 ( DPR Ancillary 4 Second Packet ) */
        { 0xA90, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA0 ( KuPR Housekeeping Packet ) */
        { 0xAA0, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA1 ( KuPR SSPA Analysis Packet ) */
        { 0xAA1, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA2 ( KuPR LNA Analysis Packet ) */
        { 0xAA2, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA3 ( KuPR External Calibration Packet ) */
        { 0xAA3, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA4 ( KuPR Internal Calibration Packet ) */
        { 0xAA4, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA5 ( KuPR Phase Code Dump Packet ) */
        { 0xAA5, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA6 ( KuPR Variable PRF Packet ) */
        { 0xAA6, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA7 ( KuPR Memory Dump Packet ) */
        { 0xAA7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAA8 ( KuPR Observation Packet ) */
        { 0xAA8, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB0 ( DI CSS Diag Tlm ) */
        { 0xAB0, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB1 ( DI HGA Diag Tlm ) */
        { 0xAB1, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB2 ( DI IRU Diag Tlm ) */
        { 0xAB2, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB3 ( DI RW Diag Tlm ) */
        { 0xAB3, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB4 ( DI SA Diag Tlm ) */
        { 0xAB4, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB5 ( DI ST Diag Tlm ) */
        { 0xAB5, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB6 ( DI MSS Diag Tlm ) */
        { 0xAB6, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB7 ( DI TAM Diag Tlm ) */
        { 0xAB7, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB8 ( DI MTB Diag Tlm ) */
        { 0xAB8, { { 0, 1, 1, 50, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAB9 ( DI GPS Diag Tlm ) */
        { 0xAB9, { { 0, 1, 1, 150, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xABA ( OM Diag Tlm ) */
        { 0xABA, { { 0, 1, 1, 100, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xABC ( GN&C / Ancillary 2Hz Combined ) */
        { 0xABC, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xABF ( GN&C / Ancillary 10Hz Combined ) */
        { 0xABF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC0 ( GPS B - Reserved ) */
        { 0xAC0, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC1 ( GPS B - Reserved ) */
        { 0xAC1, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC2 ( GPS B - Board Health ) */
        { 0xAC2, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC3 ( GPS B - Task Health ) */
        { 0xAC3, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC4 ( GPS B - Single Point Solution ) */
        { 0xAC4, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC5 ( GPS B - Time of Tone ) */
        { 0xAC5, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC6 ( GPS B - PPS ) */
        { 0xAC6, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC7 ( GPS B - Clock ) */
        { 0xAC7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC8 ( GPS B - Channel ) */
        { 0xAC8, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAC9 ( GPS B - SV Position ) */
        { 0xAC9, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xACA ( GPS B - Measurement ) */
        { 0xACA, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xACB ( GPS B - Pseudorange ) */
        { 0xACB, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xACC ( GPS B - SV Prediction ) */
        { 0xACC, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xACD ( GPS B - Reserved ) */
        { 0xACD, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xACE ( GPS B - GEONS Real Time State ) */
        { 0xACE, { { 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xACF ( GPS B - GEONS Covariance ) */
        { 0xACF, { { 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD0 ( GPS B - GEONS Residual ) */
        { 0xAD0, { { 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD1 ( GPS B - GEONS Estimated State ) */
        { 0xAD1, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD2 ( GPS B - Command Ack ) */
        { 0xAD2, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD3 ( GPS B - Ephemeris ) */
        { 0xAD3, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD4 ( GPS B - Almanac ) */
        { 0xAD4, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD5 ( GPS B - Ephemeris Status ) */
        { 0xAD5, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD6 ( GPS B - SV Select Status ) */
        { 0xAD6, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD7 ( GPS B - Reserved ) */
        { 0xAD7, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD8 ( GPS B - FPGA Data ) */
        { 0xAD8, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAD9 ( GPS B - Memory ) */
        { 0xAD9, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xADA ( GPS B - Memory Checksum ) */
        { 0xADA, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xADB ( GPS B - GEONS Covariance_D Matrix ) */
        { 0xADB, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xADC ( GPS B - GEONS Covariance_U Matrix ) */
        { 0xADC, { { 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xADD ( GPS B - GEONS Acceleration ) */
        { 0xADD, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xADE ( GPS B- Acquisition ) */
        { 0xADE, { { 0, 1, 1, 20, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xADF ( GPS B - PPBL Telemetry ) */
        { 0xADF, { { 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE0 ( GPS B - Detail ) */
        { 0xAE0, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE1 ( GPS B - GEONS Comparison ) */
        { 0xAE1, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE2 ( GPS B - GEONS Maneuver ) */
        { 0xAE2, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE3 ( GPS B - GEONS Operation Parameters ) */
        { 0xAE3, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE4 ( GPS B - GEONS Spacecraft Constants ) */
        { 0xAE4, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE5 ( GPS B - GEONS Coordinate Transformation ) */
        { 0xAE5, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE6 ( GPS B - GEONS Measurement Transmitter ) */
        { 0xAE6, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE7 ( GPS B - GEONS Process Noise ) */
        { 0xAE7, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE8 ( GPS B - GEONS Measurement Processing ) */
        { 0xAE8, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAE9 ( GPS B - GEONS Raw ) */
        { 0xAE9, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAEA ( GPS B - Configuration Data ) */
        { 0xAEA, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

        /* Packet 0xAEB ( GPS B - Event ) */
        { 0xAEB, { { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } },

    } 

} ; /* end DS_FilterTable */ 

/************************/ 
/*  End of File */ 
/************************/ 
