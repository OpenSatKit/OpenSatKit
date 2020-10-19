/*************************************************************************
** File:
**  $Id: lc_def_adt.c $
**
** Purpose:
**   Limit Checker (LC) Actionpoint Definition Table (ADT)
**
** Notes:
**   This table is computer generated and is NOT to be hand modified.
**   Generated April 15, 2014, 1:08 pm
**
**
**   $Log: lc_def_adt.c  $
**   2014/04/15 13:19:00EDT acalder
**   GPM FSSE CCR029 Update to AP 82 persistence
**   2014/11/18 M. Lambertson, Columbus Technologies and Services Inc.
**                            CCR-035: Added AP 157 for GMI SMA Undertemperature.
**   2015/05/18 M. Lambertson, Columbus Technologies and Services Inc.
**                            CCR-052: Set AP-082 to "LC_ACTION_NOT_USED".
**
*/
/*************************************************************************
** Includes
*************************************************************************/
#include "lc_msg.h"
#include "lc_tbl.h"
#include "cfe_tbl_filedef.h"

/*************************************************************************
** Exported Data
*************************************************************************/
/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "LC_ADT", "LC.LC_ADT", "LC actionpoint definition table",
    "lc_def_adt.tbl", (sizeof(LC_ADTEntry_t) * LC_MAX_ACTIONPOINTS)
};
/*
** Actionpoint Definition Table (ADT) data
*/
LC_ADTEntry_t LC_ADT[LC_MAX_ACTIONPOINTS] =
{
    /** DefaultState,   MaxPassiveEvents,   MaxPassFailEvents,   MaxFailPassEvents,   RTSId,   MaxFailsBeforeRTS,   RPNEquation,   EventType,   EventID,   EventText **/

    /* # 0 ( Reserve ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 1 ( L&EO Ops ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 2, 50, { 1,LC_RPN_NOT,2,LC_RPN_NOT,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 1, {"STARTUP: LEO Operations"}},

    /* # 2 ( Normal Ops ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 3, 30, { 1,2,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 2, {"STARTUP: Normal Operations"}},

    /* # 3 ( Fairing Separation (AP a) (MAC T-0 BW) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 4, 50, { 3,4,LC_RPN_AND,27,LC_RPN_AND,28,LC_RPN_AND,174,LC_RPN_AND,5,6,LC_RPN_AND,29,LC_RPN_AND,30,LC_RPN_AND,175,LC_RPN_AND,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 3, {"LIFTOFF: MAC T-0 BW"}},

    /* # 4 ( Fairing Separation (AP b) (SCOMM BW) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 5, 150, { 7,8,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 4, {"Fairing SEP: SCOMM BW"}},

    /* # 5 ( Fairing Separation (AP c) (MAC BW) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 6, 50, { 9,10,LC_RPN_AND,27,LC_RPN_AND,28,LC_RPN_AND,174,LC_RPN_AND,11,12,LC_RPN_AND,29,LC_RPN_AND,30,LC_RPN_AND,175,LC_RPN_AND,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 5, {"Fairing SEP: MAC BW"}},

    /* # 6 ( Spacecraft Separation (AP x) (LV BW 2A or 2B) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 7, 50, { 13,14,LC_RPN_AND,27,LC_RPN_AND,28,LC_RPN_AND,174,LC_RPN_AND,15,16,LC_RPN_AND,29,LC_RPN_AND,30,LC_RPN_AND,175,LC_RPN_AND,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 6, {"SC SEP: MAC LV BW 1"}},

    /* # 7 ( Spacecraft Separation (AP y) (SCOMM Sep) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 8, 150, { 17,18,LC_RPN_OR,19,LC_RPN_OR,20,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 7, {"SC SEP: SCOMM Sep Switch"}},

    /* # 8 ( Spacecraft Separation (AP z) (LV BW 1A or 1B) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 9, 50, { 21,27,LC_RPN_AND,28,LC_RPN_AND,174,LC_RPN_AND,22,29,LC_RPN_AND,30,LC_RPN_AND,175,LC_RPN_AND,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 8, {"SC SEP: MAC LV BW 2"}},

    /* # 9 ( +Y S/A Boom Deployed (AP l) (in RNM, SPM, GSP) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 36, 300, { 91,92,LC_RPN_OR,93,LC_RPN_OR,23,24,LC_RPN_OR,1,LC_RPN_OR,2,LC_RPN_OR,LC_RPN_AND,31,32,LC_RPN_AND,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 9, {"SA: +Y Rotating to SPP"}},

    /* # 10 ( -Y S/A Boom Deployed (AP m) (in RNM, SPM, GSP) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 37, 300, { 91,92,LC_RPN_OR,93,LC_RPN_OR,25,26,LC_RPN_OR,1,LC_RPN_OR,2,LC_RPN_OR,LC_RPN_AND,33,34,LC_RPN_AND,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 10, {"SA: -Y Rotating to SPP"}},

    /* # 11 ( Yaw -53° for +Y S/A Stuck (in SPM, GSP) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 38, 30, { 92,93,LC_RPN_OR,31,32,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 11, {"SH: Yaw 53deg for +Y SA Fault"}},

    /* # 12 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 13 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 14 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 15 ( No Data Received via 1553 ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 56, 600, { 127,137,LC_RPN_AND,142,LC_RPN_AND,143,LC_RPN_AND,144,LC_RPN_AND,145,LC_RPN_AND,146,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 15, {"SBC: XB BC Failure"}},

    /* # 16 ( Recorder 70% Full ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 58, 150, { 39,LC_RPN_EQUAL }, CFE_EVS_ERROR, 16, {"DSB: Level 2 Capacity"}},

    /* # 17 ( File Write Errors ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 58, 150, { 40,41,LC_RPN_OR,44,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 17, {"DSB: File Write Errors"}},

    /* # 18 ( MAC Data Unavailable ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 115, 300, { 42,LC_RPN_EQUAL }, CFE_EVS_ERROR, 18, {"MAC: MAC Data Unavailable"}},

    /* # 19 ( No SpaceWire Link between SBC A_SCOMM A & B ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 56, 600, { 37,38,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 19, {"SpW: SBCA/SCOMMA-SCOMMB Link Err"}},

    /* # 20 ( CFDP Open Transactions Limit ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 61, 150, { 43,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 20, {"CFDP: Near Max Open Transactions"}},

    /* # 21 ( Spacewire Blabbermouth ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 99, 150, { 184,185,LC_RPN_OR,186,LC_RPN_OR,187,LC_RPN_OR,188,LC_RPN_OR,189,LC_RPN_OR,190,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 21, {"SpW: Blabbermouth"}},

    /* # 22 ( Recorder 35% Full ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 55, 150, { 45,LC_RPN_EQUAL }, CFE_EVS_ERROR, 22, {"DSB: Level 1 Capacity"}},

    /* # 23 ( DI Message Limit Error Count level 1 ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 55, 150, { 46,LC_RPN_EQUAL }, CFE_EVS_ERROR, 22, {"DI Msg Lim Err Cnt: Level 1"}},

    /* # 24 ( DI Message Limit Error Count level 2 ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 58, 150, { 47,LC_RPN_EQUAL }, CFE_EVS_ERROR, 22, {"DI Msg Lim Err Cnt: Level 2"}},

    /* # 25 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 26 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 27 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 28 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 29 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 30 ( No Ground Commands Received (Tier 1) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 46, 10000, { 48,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 30, {"COMM: No Gnd Cmds Rcvd 1"}},

    /* # 31 ( No Ground Commands Received (Tier 2) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 47, 10000, { 49,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 31, {"COMM: No Gnd Cmds Rcvd 2"}},

    /* # 32 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 33 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 34 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 35 ( HGA Gimbal Position Unavailable ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 48, 150, { 50,LC_RPN_EQUAL }, CFE_EVS_ERROR, 35, {"HGA: Gimbal Position Unavailable"}},

    /* # 36 ( Stuck HGA motor ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 48, 100, { 51,52,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 36, {"HGA: Stuck Motor"}},

    /* # 37 ( HGA Soft Stop Position Error ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 48, 30, { 53,54,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 37, {"HGA: Soft Stop Position Error"}},

    /* # 38 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 39 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 40 ( PSE A & B Power Off ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 70, 150, { 55,56,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 40, {"PSE: PSE A & B Powered Off"}},

    /* # 41 ( PSE A 1553 RT Failure ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 72, 300, { 57,LC_RPN_EQUAL }, CFE_EVS_ERROR, 41, {"PSE: PSE A 1553 RT Failure"}},

    /* # 42 ( PSE A Controller Failure ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 72, 300, { 58,LC_RPN_EQUAL }, CFE_EVS_ERROR, 42, {"PSE: PSE A Controller Failure"}},

    /* # 43 ( PSE PWM Check ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 71, 150, { 59,60,LC_RPN_AND,59,LC_RPN_NOT,60,LC_RPN_NOT,LC_RPN_AND,LC_RPN_OR,177,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 43, {"PSE: PSE A PWM Failure"}},

    /* # 44 ( PSE A failed & B not in control ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 113, 150, { 35,36,LC_RPN_OR,192,193,LC_RPN_AND,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 44, {"PSE: PSE B not in control"}},

    /* # 45 ( Battery Undervoltage 1 ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 73, 300, { 61,62,LC_RPN_AND,62,63,LC_RPN_AND,LC_RPN_OR,61,63,LC_RPN_AND,LC_RPN_OR,176,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 45, {"BATT: Undervoltage 1"}},

    /* # 46 ( Battery Undervoltage 2 ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 74, 6000, { 64,65,LC_RPN_AND,65,66,LC_RPN_AND,LC_RPN_OR,64,66,LC_RPN_AND,LC_RPN_OR,176,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 46, {"BATT: Undervoltage 2"}},

    /* # 47 ( Battery Overvoltage ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 72, 300, { 67,68,LC_RPN_AND,68,69,LC_RPN_AND,LC_RPN_OR,67,69,LC_RPN_AND,LC_RPN_OR,176,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 47, {"BATT: Overvoltage"}},

    /* # 48 ( Battery Discharging and more than one solar array circuit is being shunted ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 72, 1800, { 70,71,LC_RPN_AND,176,LC_RPN_AND,79,LC_RPN_AND,98,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 48, {"BATT: Batt Disch w/ SA shunt"}},

    /* # 49 ( Battery Overtemperature (battery discharging) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 76, 600, { 76,77,LC_RPN_OR,78,LC_RPN_OR,73,74,LC_RPN_AND,74,75,LC_RPN_AND,LC_RPN_OR,73,75,LC_RPN_AND,LC_RPN_OR,176,LC_RPN_AND,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 49, {"BATT: OT (discharging)"}},

    /* # 50 ( Invalid Battery Reading (all batteries) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 81, 200, { 80,178,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 50, {"BATT: Invalid Battery Voltage"}},

    /* # 51 ( Max Battery Charge Current Exceeded ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 72, 200, { 81,176,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 51, {"BATT: Max Batt Charge Exceeded"}},

    /* # 52 ( Battery Protection for short Eclipse ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 85, 300, { 72,82,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 52, {"BATT: Batt Protect (short Eclips"}},

    /* # 53 ( Battery Discharge for short Eclipse ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 83, 300, { 191,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 53, {"BATT: Batt Discharge (short Ecli"}},

    /* # 54 ( PSE A failed & B not in control 2 ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 112, 150, { 35,36,LC_RPN_OR,192,LC_RPN_NOT,193,LC_RPN_NOT,LC_RPN_AND,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 54, {"PSE: PSE B not in control 2"}},

    /* # 55 ( S/A Data Unavailable  ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 76, 200, { 83,94,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 55, {"SA: Data Unavailable"}},

    /* # 56 ( S/A Data Unavailable (in SH) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 75, 50, { 83,91,92,LC_RPN_OR,93,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 56, {"SA: Data Unavailable (in SH)"}},

    /* # 57 ( +Y S/A Stuck  ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 77, 100, { 84,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 57, {"SA: +Y Stuck"}},

    /* # 58 ( -Y S/A Stuck ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 78, 100, { 85,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 58, {"SA: -Y Stuck"}},

    /* # 59 ( +Y and -Y StuckArray Words Set ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 82, 50, { 31,32,LC_RPN_OR,33,34,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 59, {"SA: +Y & -Y StuckArray Words Set"}},

    /* # 60 ( +Y S/A not in SPP in SPM ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 79, 14400, { 86,179,LC_RPN_AND,31,32,LC_RPN_OR,LC_RPN_NOT,LC_RPN_AND,92,93,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 60, {"SA: +Y not in SPP in SPM"}},

    /* # 61 ( -Y S/A not in SPP in SPM ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 80, 14400, { 87,179,LC_RPN_AND,33,34,LC_RPN_OR,LC_RPN_NOT,LC_RPN_AND,92,93,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 61, {"SA: -Y not in SPP in SPM"}},

    /* # 62 ( S/A Soft Stop Position Error (+Y) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 77, 30, { 88,161,LC_RPN_OR,25,26,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 62, {"SA: +Y Soft Stop Exceeded"}},

    /* # 63 ( S/A Soft Stop Position Error (-Y) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 78, 30, { 89,162,LC_RPN_OR,23,24,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 63, {"SA: -Y Soft Stop Exceeded"}},

    /* # 64 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 65 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 66 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 67 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 68 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 69 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 70 ( Sun Keep-out Angle ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 1200, { 99,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 70, {"L1 SYS: Sun Keep Out Angle"}},

    /* # 71 ( System Momentum (in non-thruster modes) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 22, 5, { 100,92,93,LC_RPN_OR,94,LC_RPN_OR,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 71, {"L1 MOM: Sys Mom"}},

    /* # 72 ( System Momentum (in Delta-V Mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 5, { 101,96,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 72, {"L1 MOM: Sys Mom (DVM)"}},

    /* # 73 ( System Momentum (in Delta-H Mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 300, { 102,97,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 73, {"L1 MOM: Sys Mom (DHM)"}},

    /* # 74 ( Delta System Momentum (in non-thruster modes) ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 22, 3, { 103,91,92,LC_RPN_OR,93,LC_RPN_OR,94,LC_RPN_OR,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 74, {"L1 MOM: Delta Sys"}},

    /* # 75 ( Delta System Momentum (in thruster modes) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 3, { 104,96,97,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 75, {"L1 MOM: Delta Sys (DVM)"}},

    /* # 76 ( Attitude Error (in Mission Mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 100, { 105,LC_RPN_EQUAL }, CFE_EVS_ERROR, 76, {"L2 ATT CTRL: Att Err (MSM)"}},

    /* # 77 ( Attitude Error (in Delta-V Mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 5, { 106,LC_RPN_EQUAL }, CFE_EVS_ERROR, 77, {"L2 ATT CTRL: Att Err (DVM)"}},

    /* # 78 ( Attitude Error (in Slew Mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 300, { 107,LC_RPN_EQUAL }, CFE_EVS_ERROR, 78, {"L2 ATT CTRL: Att Err (SLM)"}},

    /* # 79 ( Slew Mode Time-out ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 100, { 108,95,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 79, {"L2 MODE: SLM Time-out"}},

    /* # 80 ( Delta-H Mode Time-out ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 10, { 109,LC_RPN_EQUAL }, CFE_EVS_ERROR, 80, {"L2 MODE: DHM Time-out"}},

    /* # 81 ( Safe Hold Performance ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 32, 3000, { 110,111,LC_RPN_AND,98,LC_RPN_NOT,LC_RPN_AND,92,112,LC_RPN_AND,93,113,LC_RPN_AND,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 81, {"L2 MODE: Sun Acq Incomplete"}},

    /* # 82 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 83 ( Coarse ST-SD Residual Error ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 100, { 115,93,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 83, {"L2 ATT DET: ST-SD Residual Err"}},

    /* # 84 ( Kalman Filter Drifting (Tier 1) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 86, 300, { 116,118,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 84, {"L2 ATT DET: KF Drifting 1"}},

    /* # 85 ( Kalman Filter Drifting (Tier 2) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 300, { 116,118,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 85, {"L2 ATT DET: KF Drifting 2"}},

    /* # 86 ( ST-KF Adjusted Residual Error (Tier 1) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 87, 600, { 117,118,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 86, {"L2 ATT DET: ST-KF Res Err 1"}},

    /* # 87 ( ST-KF Adjusted Residual Error (Tier 2) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 600, { 117,118,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 87, {"L2 ATT DET: ST-KF Res Err 2"}},

    /* # 88 ( KF Covariance Not Converged (Tier 1) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 88, 3000, { 119,120,LC_RPN_OR,121,LC_RPN_OR,122,LC_RPN_OR,123,LC_RPN_OR,124,LC_RPN_OR,118,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 88, {"L2 ATT DET: KF Covariance 1"}},

    /* # 89 ( KF Covariance Not Converged (Tier 2) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 3000, { 119,120,LC_RPN_OR,121,LC_RPN_OR,122,LC_RPN_OR,123,LC_RPN_OR,124,LC_RPN_OR,118,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 89, {"L2 ATT DET: KF Covariance 2"}},

    /* # 90 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 91 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 92 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 93 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 94 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 95 ( ATB A Data Unavailable ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 30, 30, { 127,128,LC_RPN_OR,129,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 95, {"ATB: ATB A Data Unavailable"}},

    /* # 96 ( Propulsion I/O 1/2 A 1553 RT Failure (in Thruster Modes) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 21, 150, { 130,131,LC_RPN_OR,96,97,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 96, {"PROP: Prop 1A/2A 1553 RT Failure"}},

    /* # 97 ( GPS Point Solution Unavailable (Tier 1) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 101, 1200, { 134,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 97, {"GPS: Point Solution Unavailable "}},

    /* # 98 ( GPS Point Solution Unavailable (Tier 2) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 65530, { 134,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 98, {"GPS: Point Solution Unavailable "}},

    /* # 99 ( GPS Point Solution Exceeds Limits ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 18000, { 135,94,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 99, {"GPS: Point Solution Err"}},

    /* # 100 ( GPS/GEONS Solution Exceeds Limits ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 18000, { 136,94,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 100, {"GPS: GEONS Solution Err"}},

    /* # 101 ( IRU Data Invalid (in flight/non-thruster modes) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 89, 20, { 137,138,LC_RPN_OR,139,LC_RPN_OR,91,92,LC_RPN_OR,94,LC_RPN_OR,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 101, {"IRU: Data Invalid"}},

    /* # 102 ( IRU Data Invalid (on B-side) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 33, 10, { 137,138,LC_RPN_OR,139,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 102, {"IRU: Data Invalid (IRU B-side)"}},

    /* # 103 ( IRU Gyro Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 33, 100, { 140,LC_RPN_EQUAL }, CFE_EVS_ERROR, 103, {"IRU: IRU Cross-Miscompare"}},

    /* # 104 ( MTB Current Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 30, 30, { 141,LC_RPN_EQUAL }, CFE_EVS_ERROR, 104, {"MTB: Current Miscompare"}},

    /* # 105 ( RWA 1 Data Stale ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 91, 300, { 142,143,LC_RPN_NOT,LC_RPN_AND,144,LC_RPN_NOT,LC_RPN_AND,145,LC_RPN_NOT,LC_RPN_AND,146,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 105, {"RWA: Wheel 1 Data Invalid"}},

    /* # 106 ( RWA 2 Data Stale ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 92, 300, { 142,LC_RPN_NOT,143,LC_RPN_AND,144,LC_RPN_NOT,LC_RPN_AND,145,LC_RPN_NOT,LC_RPN_AND,146,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 106, {"RWA: Wheel 2 Data Invalid"}},

    /* # 107 ( RWA 3 Data Stale ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 93, 300, { 142,LC_RPN_NOT,143,LC_RPN_NOT,LC_RPN_AND,144,LC_RPN_AND,145,LC_RPN_NOT,LC_RPN_AND,146,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 107, {"RWA: Wheel 3 Data Invalid"}},

    /* # 108 ( RWA 4 Data Stale ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 94, 300, { 142,LC_RPN_NOT,143,LC_RPN_NOT,LC_RPN_AND,144,LC_RPN_NOT,LC_RPN_AND,145,LC_RPN_AND,146,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 108, {"RWA: Wheel 4 Data Invalid"}},

    /* # 109 ( RWA 5 Data Stale ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 95, 300, { 142,LC_RPN_NOT,143,LC_RPN_NOT,LC_RPN_AND,144,LC_RPN_NOT,LC_RPN_AND,145,LC_RPN_NOT,LC_RPN_AND,146,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 109, {"RWA: Wheel 5 Data Invalid"}},

    /* # 110 ( RWA 1 Torque-Tach Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 91, 100, { 147,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 110, {"RWA: Wheel 1 Torque-Tach Miscomp"}},

    /* # 111 ( RWA 2 Torque-Tach Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 92, 100, { 148,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 111, {"RWA: Wheel 2 Torque-Tach Miscomp"}},

    /* # 112 ( RWA 3 Torque-Tach Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 93, 100, { 149,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 112, {"RWA: Wheel 3 Torque-Tach Miscomp"}},

    /* # 113 ( RWA 4 Torque-Tach Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 94, 100, { 150,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 113, {"RWA: Wheel 4 Torque-Tach Miscomp"}},

    /* # 114 ( RWA 5 Torque-Tach Miscompare ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 95, 100, { 151,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 114, {"RWA: Wheel 5 Torque-Tach Miscomp"}},

    /* # 115 ( Multiple RWAs Off ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 97, 150, { 152,90,LC_RPN_NOT,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_CRITICAL, 115, {"RWA: Too Many Wheels OFF"}},

    /* # 116 ( ST Data Invalid ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 18000, { 153,156,LC_RPN_AND,157,LC_RPN_AND,158,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 116, {"ST: Data Invalid"}},

    /* # 117 ( ST Attitude Angular Change Check ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 18000, { 154,156,LC_RPN_AND,157,LC_RPN_AND,158,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 117, {"ST: Attitude Angular Change"}},

    /* # 118 ( Improper STA Mode ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 98, 3, { 155,156,LC_RPN_AND,157,LC_RPN_AND,158,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 118, {"STA: Improper Mode"}},

    /* # 119 ( TAM Mag Field Vector Magnitude Error ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 30, 30, { 159,160,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 119, {"TAM: Mag Field Vector"}},

    /* # 120 ( IRU Data Invalid (in thruster mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 23, 5, { 137,138,LC_RPN_OR,139,LC_RPN_OR,96,97,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 120, {"IRU: Data Invalid"}},

    /* # 121 ( IRU Data Invalid (in launch phase / non-thruster mode) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 89, 900, { 137,138,LC_RPN_OR,139,LC_RPN_OR,91,92,LC_RPN_OR,94,LC_RPN_OR,95,LC_RPN_OR,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 121, {"IRU: Data Invalid"}},

    /* # 122 ( Improper STB Mode ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 102, 3, { 133,156,LC_RPN_AND,157,LC_RPN_AND,158,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 122, {"STB: Improper Mode"}},

    /* # 123 ( GPS GEONS Unavailable (Tier 1) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 103, 1200, { 132,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 123, {"GPS: GEONS Unavailable 1"}},

    /* # 124 ( GPS GEONS Unavailable (Tier 2) ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 31, 65530, { 132,94,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 124, {"GPS: GEONS Unavailable 2"}},

    /* # 125 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 126 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 127 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 128 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 129 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 130 ( PS Prop Tank S/W Heater Control Disabled ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 115, 200, { 163,LC_RPN_EQUAL }, CFE_EVS_ERROR, 130, {"THR: Prop SW Htr Cntl Disabled"}},

    /* # 131 ( PSE B Side Configuration for S/W Heater Control ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 117, 50, { 35,36,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 131, {"THR: PS configured for PSE B "}},

    /* # 132 ( HGAS Damper Temp Low ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 118, 50, { 180,181,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 132, {"THR: HGAS Dmpr Htrs On"}},

    /* # 133 ( HGAS Damper Temp High ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 119, 50, { 182,183,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 133, {"THR: HGAS Dmpr Htrs Off"}},

    /* # 134 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 135 ( Instrument in SH ) */
    { LC_APSTATE_ACTIVE, 2, 2, 2, 120, 50, { 91,92,LC_RPN_OR,93,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 135, {"INST: GMI: OFF, DPR: Safe Hold"}},

    /* # 136 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 137 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 138 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 139 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 140 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 141 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 142 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 143 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 144 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 145 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 146 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 147 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 148 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 149 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 150 ( GMI Category 1 Error ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 130, 30, { 164,LC_RPN_EQUAL }, CFE_EVS_ERROR, 150, {"GMI: GMI Category 1 Error"}},

    /* # 151 ( GMI KeepAlive Stale 1 ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 132, 1800, { 165,LC_RPN_EQUAL }, CFE_EVS_ERROR, 151, {"GMI: KeepAlive Stale 1"}},

    /* # 152 ( GMI KeepAlive Stale 2 ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 130, 1800, { 165,LC_RPN_EQUAL }, CFE_EVS_ERROR, 152, {"GMI: KeepAlive Stale 2"}},

    /* # 153 ( GMI 1553 Monitor 1 ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 133, 150, { 166,LC_RPN_EQUAL }, CFE_EVS_ERROR, 153, {"GMI: 1553 Monitor 1"}},

    /* # 154 ( GMI 1553 Monitor 2 ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 131, 150, { 167,LC_RPN_EQUAL }, CFE_EVS_ERROR, 154, {"GMI: 1553 Monitor 2"}},

    /* # 155 ( GMI Overtemperature ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 134, 600, { 168,169,LC_RPN_OR,170,LC_RPN_OR,174,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 155, {"GMI: GMI Overtemperature"}},

    /* # 156 ( GMI Undertemperature ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 135, 600, { 171,172,LC_RPN_OR,173,LC_RPN_OR,174,LC_RPN_AND,LC_RPN_EQUAL }, CFE_EVS_ERROR, 156, {"GMI: GMI Undertemperature"}},

    /* # 157 ( GMI SMA Undertemperature ) */
    { LC_APSTATE_DISABLED, 2, 2, 2, 136, 60, { 194,195,LC_RPN_OR,196,LC_RPN_OR,LC_RPN_EQUAL }, CFE_EVS_ERROR, 157, {"GMI: GMI SMA Undertemperature"}},

    /* # 158 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},

    /* # 159 (  ) */
    { LC_ACTION_NOT_USED, 0, 0, 0, 0, 0, { 0,LC_RPN_EQUAL }, CFE_EVS_INFORMATION, 0, {""}},


} ; /* end LC_ADT */

/************************/
/*  End of File */
/************************/
