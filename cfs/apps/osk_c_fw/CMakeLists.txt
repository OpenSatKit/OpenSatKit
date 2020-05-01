cmake_minimum_required(VERSION 2.6.4)
project(CFS_OSK_C_FW C)

include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/app_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)

# The shared OSAL and cFE include directories should always be used
# Note that this intentionally does NOT include PSP-specific includes, just the generic
include_directories(${CFECORE_SOURCE_DIR}/src/inc)
include_directories(${CFEPSP_SOURCE_DIR}/fsw/inc)

aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(osk_c_fw ${LIB_SRC_FILES})

