# CMake recipe for building tblCRCTool
#

# This tool references the definitions of the CFE_TBL_FileHdr_t and CFE_FS_Header_t
# structures, which are defined in the CFE header files.  This, in turn, requires
# the common_types.h file from OSAL and the global cfe_mission_cfg.h file. 
include_directories(${MISSION_BINARY_DIR}/inc)
include_directories(${osal_MISSION_DIR}/src/os/inc)
include_directories(${cfe-core_MISSION_DIR}/src/inc)

add_executable(cfe_ts_crc cfe_ts_crc.c)

install(TARGETS cfe_ts_crc DESTINATION host)


