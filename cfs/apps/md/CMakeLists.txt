cmake_minimum_required(VERSION 2.6.4)
project(CFS_MD C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(md ${APP_SRC_FILES})

# Add tables to build
include_directories(fsw/tables)
include_directories(fsw/src)
aux_source_directory(fsw/tables APP_TBL_FILES)
add_cfe_tables(md ${APP_TBL_FILES})
