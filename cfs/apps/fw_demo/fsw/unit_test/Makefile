###############################################################################
## $Id:$
##
## Purpose:
##   Build template's unit test
##
## Notes:
##   1. See ut-lib's config.mak and rule.mak for what variables must be defined.
##   2. No need to use LOCAL_INC since the tool headers are already
##      included by config.mak
##   3. "-include" used for dependencies because if file doesn't exist it
##      will not report an error.
##
## $Log: $
##
###############################################################################

###############################################################################
# Directory base
#
UT_APPS_BASE = $(shell cd ../..; pwd)
UT_TOOL_BASE = $(shell cd ../../../tools; pwd)
REF_MISSION_BASE = /cygdrive/c/dmccomas/projects/gnc-fsw-proto/user/proto

###############################################################################
# Source, Object, and Executable Definitions
# - OBJS: Object files required to build unit
# - SRC:  Source files required to build unit; used to generate dependencies.
# - 

SUBSYS = demo
BUILD_TYPE = EXE
UNIT = ut_demo_app

FSW_OBJS := demo.o
FSW_OBJS += tblmgr.o
FSW_OBJS += cmdmgr.o
FSW_OBJS += exobj.o

UT_OBJS := exobj_testcase.o
UT_OBJS += cmdmgr_testcase.o
UT_OBJS += tblmgr_testcase.o
UT_OBJS += demo_testrunner.o

SRC := $(OBJS:.o=.c)

UT_TEST_EXE = demo_testrunner.exe
OBJS :=  $(UT_OBJS) $(FSW_OBJS)
 
###############################################################################
# Local Directories and Options
# - Source and include file
# - Library
#

LOCAL_SRC := ../fsw
LOCAL_SRC += ../ut

LOCAL_INC := ../fsw
LOCAL_INC += ../deploy
LOCAL_INC += $(REF_MISSION_BASE)/build/pc-linux/inc


LOCAL_LIB_DIR  := 
 
LOCAL_LIB_FILE :=

LOCAL_COPTS := -D_LINUX_OS_

###############################################################################
# Standard configuration and rules
#

#all:$(UT_TEST_EXE)
#$(UT_TEST_EXE): $(UT_OBJS) $(FSW_OBJS)

CONFIG_MAK = $(UT_TOOL_BASE)/ut-lib/build/config.mak

include $(CONFIG_MAK)

include $(RULE_MAK)

###############################################################################
# Include the dependancy list
#
-include $(SUBSYS).d

# eof

