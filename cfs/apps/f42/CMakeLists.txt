cmake_minimum_required(VERSION 2.6.4)
project(CFS_F42 C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

include_directories(${i42_MISSION_DIR}/fsw/src)
include_directories(${i42_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_42_lib_MISSION_DIR}/fsw/app_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_c_fw_MISSION_DIR}/fsw/mission_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(f42 ${APP_SRC_FILES})
target_link_libraries (f42 m)


