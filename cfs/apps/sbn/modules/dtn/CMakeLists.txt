cmake_minimum_required(VERSION 2.6.4)
project(SBN_DTN C)

if(NOT(IS_DIRECTORY ${SBN_APP_SOURCE_DIR}))
    message(FATAL_ERROR "SBN_APP_SOURCE_DIR not defined, is sbn in the target list before this module?")
endif()

include_directories(${SBN_APP_SOURCE_DIR}/fsw/platform_inc)
#include_directories(${ION}/include)
include_directories(/home/vmuser/ion/include)

aux_source_directory(fsw/src LIB_SRC_FILES)

link_directories(/home/vmuser/ion/lib)

find_library(ION_BP libbp.a PATHS /home/vmuser/ion/lib )
find_library(ION_ICI libici.a PATHS /home/vmuser/ion/lib )

# Create the app module
add_cfe_app(sbn_dtn ${LIB_SRC_FILES})

#set_target_properties(sbn_dtn PROPERTIES LINK_FLAGS "-Wl,--whole-archive")
target_link_libraries(sbn_dtn bp ici)
