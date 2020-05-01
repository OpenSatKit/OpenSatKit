cmake_minimum_required(VERSION 2.6.4)
project(CFS_KIT_CI C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(./../inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${cfs_lib_PLATFORM_DIR}/fsw/public_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${to_MISSION_DIR}/fsw/platform_inc)
include_directories(${to_MISSION_DIR}/fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(kit_ci ${APP_SRC_FILES})

