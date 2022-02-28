# CMake snippet for building elf2cfetbl
#
include_directories(${MISSION_BINARY_DIR}/inc)
include_directories(${osal_MISSION_DIR}/src/os/inc)
include_directories(${cfe-core_MISSION_DIR}/src/inc)

add_executable(elf2cfetbl elf2cfetbl.c)

install(TARGETS elf2cfetbl DESTINATION host)


