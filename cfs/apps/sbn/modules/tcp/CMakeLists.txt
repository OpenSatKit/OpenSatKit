cmake_minimum_required(VERSION 2.6.4)
project(SBN_TCP C)

if(NOT(IS_DIRECTORY ${SBN_APP_SOURCE_DIR}))
    message(FATAL_ERROR "SBN_APP_SOURCE_DIR not defined, is sbn in the target list before this module?")
endif()

include_directories(${SBN_APP_SOURCE_DIR}/fsw/platform_inc)

aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(sbn_tcp ${LIB_SRC_FILES})
