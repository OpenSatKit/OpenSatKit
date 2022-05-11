cmake_minimum_required(VERSION 2.6.4)
project(CFS_pl_sim_lib C)

include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/mission_inc)

aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(pl_sim_lib ${LIB_SRC_FILES})
