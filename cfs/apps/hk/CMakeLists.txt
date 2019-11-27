cmake_minimum_required(VERSION 2.6.4)
project(CFS_HK C)

include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${cfs_lib_PLATFORM_DIR}/fsw/public_inc)
include_directories(${CFECORE_SOURCE_DIR}/src/inc)
include_directories(${f42_MISSION_DIR}/fsw/platform_inc)
include_directories(${isim_MISSION_DIR}/fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)

# Create the app module
add_cfe_app(hk ${APP_SRC_FILES})
add_cfe_tables(hk ${APP_TABLE_FILES})

