cmake_minimum_required(VERSION 2.6.4)
project(CFS_SC C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(./../inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${lc_MISSION_DIR}/fsw/platform_inc)
include_directories(${lc_MISSION_DIR}/fsw/src)
include_directories(${pl_sim_MISSION_DIR}/fsw/platform_inc)
include_directories(${pl_sim_MISSION_DIR}/fsw/mission_inc)
include_directories(${pl_mgr_MISSION_DIR}/fsw/platform_inc)
include_directories(${pl_mgr_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/mission_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)

# Create the app module
add_cfe_app(sc ${APP_SRC_FILES})
add_cfe_tables(sc ${APP_TABLE_FILES})
