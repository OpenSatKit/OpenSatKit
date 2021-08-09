/*
** Purpose: Define Spacecraft Simulator Object
**
** Notes:
**   1. For the initial versions of the subsystem simulations I broke from
**      convention of defining one object per file. The simulations are
**      trivial and tightly coupled for hard coded scenarios.
**   2. The simulation models will evolve as will the combination of 
**      using models vs actual apps for function like C&DH recorder and also
**      the incorporation of 42. My goal is NOT to rewrite 42 but leverage
**      its capabilities.
**   3. Consider managing the sim with a state machine that has events
**      that define sim model behavior. Could be very generic with an
**      event-sim-model superclass. I would like the sim models to be driven
**      by 42 reality.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _scsim_
#define _scsim_

/*
** Includes
*/

#include "app_cfg.h"
#include "scsimtbl.h"

/***********************/
/** Macro Definitions **/
/***********************/


/*
** Event Message IDs
*/

#define SCSIM_START_SIM_EID      (SCSIM_BASE_EID +  0)
#define SCSIM_START_SIM_ERR_EID  (SCSIM_BASE_EID +  1)
#define SCSIM_STOP_SIM_EID       (SCSIM_BASE_EID +  2)
#define SCSIM_LOAD_TBL_EID       (SCSIM_BASE_EID +  3)
#define SCSIM_LOAD_TBL_OBJ_EID   (SCSIM_BASE_EID +  4)
#define SCSIM_START_REC_PLBK_EID (SCSIM_BASE_EID +  5)
#define SCSIM_STOP_REC_PLBK_EID  (SCSIM_BASE_EID +  6)
#define SCSIM_ADD_EVENT_EID      (SCSIM_BASE_EID +  7)
#define SCSIM_EXECUTE_EVENT_EID  (SCSIM_BASE_EID +  8)
#define SCSIM_EVENT_ERR_EID      (SCSIM_BASE_EID +  9)
#define SCSIM_EXECUTE_EID        (SCSIM_BASE_EID + 10)

#define ADCS_ENTER_ECLIPSE_EID   (SCSIM_BASE_EID + 20)
#define ADCS_EXIT_ECLIPSE_EID    (SCSIM_BASE_EID + 21)
#define ADCS_CHANGE_MODE_EID     (SCSIM_BASE_EID + 22)

#define CDH_WATCHDOG_RESET_EID   (SCSIM_BASE_EID + 30)

#define COMM_START_CONTACT_EID   (SCSIM_BASE_EID + 40)
#define COMM_PROCESS_EVENT_EID   (SCSIM_BASE_EID + 41)

// FSW + 50

#define INSTR_DIS_POWER_EID      (SCSIM_BASE_EID + 60)

// PWR + 70

// THRM + 80

/*
** Simulation management 
**
** Simulations have the following phases:
**   1. Initialization
**      - Each subsystem should have one or more commands to initialize the
**        subsystem to a known state
**   2. Time-lapse
**      - The simulation runs much faster than realtime to propagate each 
**        subsystem state to a point for the simulation to start
**      - Time-lapse starts with the first command scheduled prior to the
**        realtime epoch. 
**   3. Realtime
**      - The simulation progresses at realtime
**
** Sim event time tag conventions drive each simulation phase. 
** - 0: A single init time of zero is used to initialize models so all cmds
**   used to set initial model states should have the same time stamp.
** - 1..(Epoch-1) time is used for the time-lapse phase. This time is 
**   relative to the epoch.
** - Epoch defines when realtime begins. The epoch was defined so one
**   orbit can be performed during the time-lapsed phase.
*/

#define SCSIM_IDLE_TIME             (0)  /* No sim active */

#define SCSIM_INIT_TIME             (1)  /* Model initialization */  

#define SCSIM_TIME_LAPSE_EXE_CNT (1000)  /* Number of simulation seconds to perform in one SCSIM execution cycle during time lapse phase */

#define SCSIM_REALTIME_EPOCH    (10000)  /* Time when realtime simulation starts */ 
#define SCSIM_REALTIME_END      (20000)  /* Sim doesn't execute until this time. Thsi time indicates sim is over */ 

#define SCSIM_EVT_CMD_MAX          (25)  /* Maximum number of event commands */
#define SCSIM_EVT_CMD_NULL_IDX     (99)  /* Maximum number of event commands */

/**********************/
/** Type Definitions **/
/**********************/

/* 
** An OO design would use constructs like abstract base classes
** for subsystems and events. This uses enums and structs that 
** need to be modified as subsystems and events evolve. 
*/

typedef enum {
   
   SCSIM_PHASE_UNDEF       = 0,
   SCSIM_PHASE_IDLE        = 1,
   SCSIM_PHASE_INIT        = 2,
   SCSIM_PHASE_TIME_LAPSE  = 3,
   SCSIM_PHASE_REALTIME    = 4
   
} SCSIM_Phase;

typedef enum {

   SCSIM_SUBSYS_UNDEF  = 0,
   SCSIM_SUBSYS_SIM    = 1,
   SCSIM_SUBSYS_ADCS   = 2,
   SCSIM_SUBSYS_CDH    = 3,
   SCSIM_SUBSYS_COMM   = 4,
   SCSIM_SUBSYS_FSW    = 5,
   SCSIM_SUBSYS_INSTR  = 6,
   SCSIM_SUBSYS_PWR    = 7,
   SCSIM_SUBSYS_THRM   = 8,
   SCSIM_SUBSYS_COUNT  = 9
   
} SCSIM_SubSys;

typedef enum {

   SIM_EVT_UNDEF     = 0,
   SIM_EVT_IDLE      = 1,
   SIM_EVT_START_SIM = 2,
   SIM_EVT_STOP_SIM  = 3
   
} SIM_EvtCmd;

typedef enum {

   SCSIM_SCANF_UNDEF    = 0,
   SCSIM_SCANF_1_INT    = 1,
   SCSIM_SCANF_2_INT    = 2,
   SCSIM_SCANF_3_INT    = 3,
   SCSIM_SCANF_1_FLT    = 4,
   SCSIM_SCANF_3_FLT    = 5,
   SCSIM_SCANF_4_FLT    = 6,
   SCSIM_SCANF_NONE     = 7,
   SCSIM_SCANF_TYPE_CNT = 8
   
} SCSIM_SCANF_TYPE;

typedef struct {
   
   int32  OneInt;
   int32  TwoInt[2];
   int32  ThreeInt[3];
   float  OneFlt;
   float  ThreeFlt[3];
   float  FourFlt[4];
   
} SCSIM_EVT_CMD_PARAM;
/*
** Events have the general format of 
** - Time, Subsystem ID, Subsystem Event, Scanf Type, Parameters 
**
** See file prologue and macro section above for a simulation architecture
** overview.
*/

typedef struct {

   uint16  Prev;
   uint16  Next;
      
} SCSIM_EvtCmdLink;

typedef struct {

   SCSIM_EvtCmdLink Link;
   int32            Time;
   SCSIM_SubSys     SubSys;
   uint8            Id;
   SCSIM_SCANF_TYPE ScanfType;
   char*            Param;
      
} SCSIM_EvtCmd;


/*
** Subsystem telemetry on last event command sent.
*/ 
typedef struct {

   int32   Time;
   uint32  Id;    /* Keep 32-bit aligned */ 

} SCSIM_EvtCmdTlm;


/**********/
/** ADCS **/
/**********/

/* Preliminary events to get started. Need scenarios to determine what's needed */
typedef enum {

   ADCS_EVT_UNDEF          = 0,
   ADCS_EVT_SET_MODE       = 1,
   ADCS_EVT_ENTER_ECLIPSE  = 2,
   ADCS_EVT_EXIT_ECLIPSE   = 3,
   ADCS_EVT_SET_ATTITUDE   = 4,
   ADCS_EVT_SET_ORBIT      = 5

} ADCS_EvtCmd;

typedef enum {

   ADCS_MODE_UNDEF      = 0,
   ADCS_MODE_SAFEHOLD   = 1,
   ADCS_MODE_SUN_POINT  = 2,
   ADCS_MODE_INERTIAL   = 3,
   ADCS_MODE_SLEW       = 4

} ADCS_Mode;

typedef struct {


   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */
   
   boolean     Eclipse;
   ADCS_Mode   Mode;
   double      AttErr;
   
} ADCS_Model;


/*********/
/** CDH **/
/*********/


typedef enum {

   CDH_EVT_UNDEF        = 0,
   CDH_EVT_WATCHDOG_RST = 1,
   CDH_EVT_SEND_HW_CMD  = 2
   
} CDH_EvtCmd;


typedef enum {

   CDH_HW_CMD_UNDEF       = 0,
   CDH_HW_CMD_RST_SBC     = 1,
   CDH_HW_CMD_PWR_CYC_SBC = 2,
   CDH_HW_CMD_SEL_BOOT_A  = 3,
   CDH_HW_CMD_SEL_BOOT_B  = 4
   
} CDH_HardwareCmd;


typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   uint16  SbcRstCnt;   /* From either watchdog or hardware command */
   
   uint16  HwCmdCnt;
   CDH_HardwareCmd  LastHwCmd;
   
} CDH_Model;

/**********/
/** COMM **/
/**********/

typedef enum {

   COMM_EVT_UNDEF         = 0,
   COMM_EVT_SCH_AOS       = 1,  /* Schedule relative start time, length, Link Type */
   COMM_EVT_LOS           = 2,  /* Schedule loss of signal */
   COMM_EVT_SET_DATA_RATE = 3,  /* Set rate for next contact */
   COMM_EVT_SET_TDRS_ID   = 4,
   COMM_EVT_ABORT_CONTACT = 5
   
} COMM_EvtCmd;


typedef enum {

   COMM_TDRS_UNDEF    = 0,
   COMM_TDRS_ATLANTIC = 1,
   COMM_TDRS_INDIAN   = 2,
   COMM_TDRS_PACIFIC  = 3
   
} COMM_TdrsId;


typedef enum {

   COMM_LINK_UNDEF    = 0,
   COMM_LINK_DUPLEX   = 1,
   COMM_LINK_SIMPLEX  = 2

} COMM_Link;

typedef struct {

   COMM_Link Link;
   uint16    TdrsId;
   uint16    DataRate;
   int16     TimePending;  /* Countdown timer until next contact. Negative value indicates no contect pending */ 
   uint16    Length;
   uint16    TimeConsumed;
   uint16    TimeRemaining;
        
} COMM_Contact;


typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   boolean       InContact;
   COMM_Contact  Contact;
   
} COMM_Model;


/*********/
/** FSW **/
/*********/

/* 
** FSW uses a combination of simulated behavior and actual FSW status. Things like 
** playback are much easier to manipulate in a simulation. 
*/
typedef enum {

   FSW_EVT_UNDEF             = 0,
   FSW_EVT_SET_REC_FILE_CNT  = 1, /* Percentage of recorder memory used */
   FSW_EVT_SET_REC_PCT_USED  = 2, /* Number of files in recorder */
   FSW_EVT_START_REC_PLBK    = 3,
   FSW_EVT_STOP_REC_PLBK     = 4,
   FSW_EVT_CLR_EVT_LOG       = 5

} FSW_EvtCmd;


typedef struct {

   float    PctUsed;
   uint16   FileCnt;
   boolean  PlaybackEna;

} FSW_Recorder;


typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   FSW_Recorder Recorder;
        
} FSW_Model;


/****************/
/** Instrument **/
/****************/

#define INSTR_CYCLES_PER_FILE  30 /* Number of simulation cycles to 'generate' a new file */
 
typedef enum {

   INSTR_EVT_UNDEF       = 0,
   INSTR_EVT_ENA_POWER   = 1,
   INSTR_EVT_DIS_POWER   = 2,
   INSTR_EVT_ENA_SCIENCE = 3,
   INSTR_EVT_DIS_SCIENCE = 4

} INSTR_EvtCmd;

typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   boolean   PwrEna;
   boolean   SciEna;
   
   int16     FileCnt;
   int16     FileCycCnt;
        
} INSTR_Model;


/***********/
/** Power **/
/***********/

typedef enum {

   PWR_EVT_UNDEF          = 0,
   PWR_EVT_SET_BATT_SOC   = 1,
   PWR_EVT_SET_SA_CURRENT = 2

} PWR_EvtCmd;

typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   float   BattSoc;
   float   SaCurrent;
   
} PWR_Model;


/*************/
/** Thermal **/
/*************/

typedef enum {

   THRM_EVT_UNDEF        = 0,
   THRM_EVT_ENA_HEATER_1 = 1,
   THRM_EVT_ENA_HEATER_2 = 2

} THRM_EvtCmd;

typedef struct {

   /* Model Management */

   SCSIM_EvtCmd  LastEvtCmd;

   /* Model State */

   boolean   Heater1Ena;
   boolean   Heater2Ena;
        
} THRM_Model;


/******************************************************************************
** Command Packets
**
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} SCSIM_NoArgsCmdMsg;
#define SCSIM_NO_ARGS_CMD_DATA_LEN  (sizeof(SCSIM_NoArgsCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   
   uint16  Id;

} SCSIM_StartSimCmdMsg;
#define SCSIM_START_SIM_CMD_DATA_LEN  (sizeof(SCSIM_StartSimCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef SCSIM_NoArgsCmdMsg SCSIM_StopSimCmdMsg;
typedef SCSIM_NoArgsCmdMsg SCSIM_StartPlaybackCmdMsg;
typedef SCSIM_NoArgsCmdMsg SCSIM_StopPlaybackCmdMsg;


/******************************************************************************
** Telemetry Packets
**
** - The simulation object telemetry must be separate from the HK packet that
**   provides simulation state information. The simulation status is sent and
**   displayed even when a simulated ground contact is not active. The 
**   subsystem sim model data is only sent when a ground contact is active.
**   
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   uint32   SimTime;
   boolean  SimActive;
   uint8    SimPhase;
   
   /* 
   ** Contact info is in both the management tlm and model telemetry
   */
   uint16    ContactTimePending;
   uint16    ContactLength;
   uint16    ContactTimeConsumed;
   uint16    ContactTimeRemaining;

   uint8    LastEvtSubSysId;   /* Last event command executed */
   uint8    LastEvtCmdId;
   uint8    NextEvtSubSysId;   /* Next command to be executed */
   uint8    NextEvtCmdId;

   SCSIM_EvtCmdTlm  AdcsLastEvtCmd;
   SCSIM_EvtCmdTlm  CdhLastEvtCmd;
   SCSIM_EvtCmdTlm  CommLastEvtCmd;
   SCSIM_EvtCmdTlm  PwrLastEvtCmd;
   SCSIM_EvtCmdTlm  ThrmLastEvtCmd;

} OS_PACK SCSIM_MgmtPkt;
#define SCSIM_MGMT_PKT_LEN sizeof (SCSIM_MgmtPkt)


typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];


   /*
   ** ADCS
   */

   boolean   Eclipse;
   uint8     AdcsMode;


   /*
   ** C&DH
   */

   uint16  SbcRstCnt;
   uint16  HwCmdCnt;
   uint16  LastHwCmd;

   /*
   ** Comm
   */
   
   uint16  InContact;
   uint16  ContactTimePending;
   uint16  ContactTimeConsumed;
   uint16  ContactTimeRemaining;
   uint8   ContactLink;
   uint8   ContactTdrsId;
   uint16  ContactDataRate;
  
   /*
   ** FSW
   */

   float   RecPctUsed;
   uint16  RecFileCnt;
   uint8   RecPlaybackEna;
   uint8   FswSpare;
   
   /*
   ** Instrument
   */

   uint8   InstrPwrEna; 
   uint8   InstrSciEna;

   uint16  InstrFileCnt; 
   uint16  InstrFileCycCnt;

   /*
   ** Power
   */
   
   float   BattSoc;
   float   SaCurrent;
   
   /*
   ** Thermal
   */
      
   boolean   Heater1Ena;
   boolean   Heater2Ena;
   
} OS_PACK SCSIM_ModelPkt;
#define SCSIM_MODEL_PKT_LEN sizeof (SCSIM_ModelPkt)


/******************************************************************************
** SCSIM_Class
*/

typedef struct {

   /*
   ** Commands
   */
  
   /*
   ** Tables
   */
  
   SCSIMTBL_Struct Tbl;
   
   /*
   ** Telemetry Packets
   */
  
   SCSIM_MgmtPkt  MgmtPkt;
   SCSIM_ModelPkt ModelPkt;


   /*
   ** Objects
   */   


   /* Sim Management */
   
   boolean             Active;
   SCSIM_Phase         Phase;
   CFE_TIME_SysTime_t  Time;   /* Subseconds unused */
   
   SCSIM_EvtCmd*  LastEvtCmd;
   SCSIM_EvtCmd*  NextEvtCmd;   
   SCSIM_EVT_CMD_PARAM EvtCmdParam;

   SCSIM_EvtCmd*  Scenario;
   uint16         ScenarioId;
   uint16         RunTimeCmdIdx;
 
   /* Sim Models */
   
   ADCS_Model   Adcs;
   CDH_Model    Cdh;
   COMM_Model   Comm;
   FSW_Model    Fsw;
   INSTR_Model  Instr;
   PWR_Model    Pwr;
   THRM_Model   Thrm;
   

} SCSIM_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: SCSIM_Constructor
**
** Initialize the intrument simulator to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void SCSIM_Constructor(SCSIM_Class *ScSimPtr, CFE_SB_MsgId_t MgmtPktMsgId,
                       CFE_SB_MsgId_t ModelPktMsgId);


/******************************************************************************
** Function: SCSIM_Execute
**
** Execute a single simulation step.
**
*/
boolean SCSIM_Execute(void);


/******************************************************************************
** Function: SCSIM_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void SCSIM_ResetStatus(void);


/******************************************************************************
** Function: SCSIM_GetTblPtr
**
** Get pointer to SCSIM's table data
**
** Notes:
**  1. This function must match the SCSIMTBL_GetTblPtr definition.
**  2. Supplied as a callback to ScSsimTbl.
**
*/
const SCSIMTBL_Struct* SCSIM_GetTblPtr(void);


/******************************************************************************
** Function: SCSIM_LoadTbl
**
** Load data into SCSIM's table.
**
** Notes:
**  1. This function must match the SCSIMTBL_LoadTblFunc definition.
**  2. Supplied as a callback to ScSimTbl.
**
*/
boolean SCSIM_LoadTbl (SCSIMTBL_Struct* NewTbl);


/******************************************************************************
** Function: SCSIM_LoadEntry
**
** Load data into SCSIM's table.
**
** Notes:
**  1. This function must match the SCSIMTBL_LoadEntryFunc definition.
**  2. Supplied as a callback to ScSimTbl.
**
*/
boolean SCSIM_LoadTblEntry (uint16 ObjId, void* ObjData);


/******************************************************************************
** Functions: SCSIM_StartSimCmd
**
** Start a precanned simulation. Currently this is a placeholder.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StartSimCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);

/******************************************************************************
** Functions: SCSIM_StopSimCmd
**
** Stop the current simulation. Currently this is a placeholder.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StopSimCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: SCSIM_StartPlbkCmd
**
** Stop a recorder playback.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StartPlbkCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: SCSIM_StopPlbkCmd
**
** Stop a recorder playback.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**
*/
boolean SCSIM_StopPlbkCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _scsim_ */
