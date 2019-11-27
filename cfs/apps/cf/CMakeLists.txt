cmake_minimum_required(VERSION 2.6.4)
project(CFS_CF C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(./../inc)

include_directories(fsw/src/PUB)
include_directories(fsw/src/PRI)

include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${cfs_lib_PLATFORM_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)
aux_source_directory(fsw/src/PRI APP_SRC_FILES)
aux_source_directory(fsw/src/PUB APP_SRC_FILES)

# Create the app module
add_cfe_app(cf ${APP_SRC_FILES})
add_cfe_tables(cf ${APP_TABLE_FILES})

#Link math library
target_link_libraries(cf m)

