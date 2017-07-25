cmake_minimum_required(VERSION 2.6.4)
project(CFS_SCH_LAB C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${kit_ci_MISSION_DIR}/fsw/platform_inc)
include_directories(${kit_to_MISSION_DIR}/fsw/platform_inc)
include_directories(${sample_app_MISSION_DIR}/fsw/platform_inc)
include_directories(${ds_MISSION_DIR}/fsw/platform_inc)
include_directories(${fm_MISSION_DIR}/fsw/platform_inc)
include_directories(${hs_MISSION_DIR}/fsw/platform_inc)
include_directories(${lc_MISSION_DIR}/fsw/platform_inc)
include_directories(${sc_MISSION_DIR}/fsw/platform_inc)
# Custom Apps
include_directories(${bm_MISSION_DIR}/fsw/platform_inc)
include_directories(${tftp_MISSION_DIR}/fsw/platform_inc)
include_directories(${hc_MISSION_DIR}/fsw/platform_inc)
include_directories(${hsim_MISSION_DIR}/fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(sch_lab ${APP_SRC_FILES})

