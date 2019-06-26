##############################################################################
## File:
##   $Id: Makefile 1.7 2012/05/24 11:13:06EDT jmdagost Exp  $
##
## Purpose:
##   Makefile for building the CFS Checksum (CS) UTF unit tests
##
##   $Log: Makefile  $
##   Revision 1.7 2012/05/24 11:13:06EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.6 2012/01/04 16:17:49EST jmdagost 
##   Replaced gcov rule in CS unit test makefile.  Kept previous gcov rule as a comment for future reference.
##   Revision 1.5 2011/09/19 13:58:15EDT jmdagost 
##   CS Unit test updates
##   Revision 1.4 2010/05/21 14:27:40EDT jmdagost 
##   Updated for new OSAL paths
##   Revision 1.3 2010/05/03 17:25:26EDT jmdagost 
##   Major updates to account for changes in unit test framework, adding build options for test tables, and adjusting for relative paths.
##   Revision 1.2 2008/09/08 10:39:37EDT njyanchik 
##   Checking in of Updated unit test
##   Revision 1.1 2008/07/23 15:28:18BST njyanchik 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/project.pj
##
##############################################################################
#
# Compiler/linker definitions
#
COMPILER=gcc
LINKER=gcc
#
# Compiler/linker options
#
DEFAULT_COPT := -m32 -fPIC -Wall -Wstrict-prototypes -Wunreachable-code -ansi -g 
COPT = -D_ix86_ -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -DCLOCKS_PER_SEC=1000000 -DS_IFDIR=16384
## LOPT = -m elf_i386 -o
LOPT = -m32
#
# Options for gcov
#
GCOV_COPT= -pg -fprofile-arcs -ftest-coverage
GCOV_LOPT= -pg -fprofile-arcs -ftest-coverage
#
###########################################################################
#
# Search path definitions
#
#
###########################################################################
#
# Table tool definition
ELF2TBL=$(CFE_CORE_SRC)/../../../tools/elf2cfetbl/elf2cfetbl
#
###########################################################################
#
# Source file path definitions
#
VPATH := .
VPATH += ..
VPATH += $(CFS_APP_SRC)/cs/fsw/src
VPATH += $(CFS_APP_SRC)/cs/fsw/tables
VPATH += $(CFS_APP_SRC)/cfs_lib/fsw/src
VPATH += $(UTF_SRC)/src
VPATH += $(CFE_CORE_SRC)/sb
VPATH += $(CFE_CORE_SRC)/time
VPATH += $(CFE_CORE_SRC)/es
VPATH += $(CFE_CORE_SRC)/evs
VPATH += $(CFE_CORE_SRC)/fs
VPATH += $(CFE_CORE_SRC)/tbl
VPATH += $(OSAL_SRC)/posix
VPATH += $(OSAL_SRC)/../bsp/pc-linux/src
VPATH += $(CFE_PSP_SRC)/pc-linux/src
#
###########################################################################
#
# Header file path definitions
#
INCLUDES := -I.
INCLUDES += -I..
INCLUDES += -I$(CFS_APP_SRC)/cs/fsw/src
INCLUDES += -I$(CFS_APP_SRC)/cs/fsw/platform_inc
INCLUDES += -I$(CFS_APP_SRC)/cs/fsw/mission_inc
INCLUDES += -I$(CFS_APP_SRC)/cs/fsw/public_inc
INCLUDES += -I$(OSAL_SRC)/../../build/inc
INCLUDES += -I$(OSAL_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/pc-linux/inc
INCLUDES += -I$(UTF_SRC)/inc
INCLUDES += -I$(CFS_APP_SRC)/cfs_lib/fsw/public_inc
INCLUDES += -I$(CFE_CORE_SRC)/../../mission_inc
INCLUDES += -I$(CFE_CORE_SRC)/../../platform_inc/cpu1
INCLUDES += -I$(CFE_CORE_SRC)/inc
INCLUDES += -I$(CFE_CORE_SRC)/time
INCLUDES += -I$(CFE_CORE_SRC)/sb
INCLUDES += -I$(CFE_CORE_SRC)/es
INCLUDES += -I$(CFE_CORE_SRC)/evs
INCLUDES += -I$(CFE_CORE_SRC)/fs
INCLUDES += -I$(CFE_CORE_SRC)/tbl

#
###########################################################################
#
# UTF object files
#
UTF_OBJS := utf_osapi.o
UTF_OBJS += utf_osfilesys.o
UTF_OBJS += utf_osfileapi.o
UTF_OBJS += utf_cfe_sb_api.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_cfe_psp_memutils.o
UTF_OBJS += utf_cfe_psp_memory.o
UTF_OBJS += utf_cfe_psp_memrange.o
UTF_OBJS += utf_cfe_psp_support.o
UTF_OBJS += utf_cfe_sb.o
UTF_OBJS += utf_sim.o
UTF_OBJS += utf_cfe_es_api.o
UTF_OBJS += utf_cfe_es_cds.o
UTF_OBJS += utf_cfe_esmempool.o
UTF_OBJS += utf_cfe_evs.o
UTF_OBJS += utf_cfe_tbl_api.o
UTF_OBJS += utf_cfe_tbl_internal.o
#
###########################################################################
#
# cFE object files
#
CFE_OBJS := ccsds.o
CFE_OBJS += cfe_sb_util.o
CFE_OBJS += cfe_time_api.o
CFE_OBJS += cfe_es_cds_mempool.o
CFE_OBJS += cfe_fs_api.o
CFE_OBJS += cfe_psp_timer.o
#
###########################################################################
#
# CS object files
#
CS_OBJS := cs_app.o
CS_OBJS += cs_app_cmds.o
CS_OBJS += cs_cmds.o
CS_OBJS += cs_compute.o
CS_OBJS += cs_eeprom_cmds.o
CS_OBJS += cs_memory_cmds.o
CS_OBJS += cs_table_cmds.o
CS_OBJS += cs_table_processing.o
CS_OBJS += cs_utils.o
CS_OBJS += cs_eepromtbl.o
CS_OBJS += cs_memorytbl.o
CS_OBJS += cs_tablestbl.o
CS_OBJS += cs_apptbl.o
#

#
###########################################################################
#
# Rules to make the specified targets
#   
%.o: %.c
	$(COMPILER) -c $(COPT) $(DEFAULT_COPT) $(GCOV_COPT) $(INCLUDES) $<
#
%.bin: %.o
	$(LINKER) $(LOPT) $(GCOV_LOPT) $^ -o $*.bin 
#
all: utf_test_cs.bin defaulttbls
#
utf_test_cs.bin: utf_test_cs.o $(UTF_OBJS) $(CFE_OBJS) $(CS_OBJS)

defaulttbls ::
	$(ELF2TBL) cs_apptbl.o ./cf/apps/DefaultTable/
	$(ELF2TBL) cs_eepromtbl.o ./cf/apps/DefaultTable/
	$(ELF2TBL) cs_memorytbl.o ./cf/apps/DefaultTable/
	$(ELF2TBL) cs_tablestbl.o ./cf/apps/DefaultTable/
#
fulltest: utf_test_cs.bin defaulttbls tables
#
tables: cnftbls istbls intbls cftbls detbls qqtbls enftbls

cnftbls: ./cf/apps/CorrNonFullTable/cs_apptbl.o \
./cf/apps/CorrNonFullTable/cs_tablestbl.o \
./cf/apps/CorrNonFullTable/cs_eepromtbl.o \
./cf/apps/CorrNonFullTable/cs_memorytbl.o
	mv cs_apptbl.o ./cf/apps/CorrNonFullTable/cs_apptbl.o
	mv cs_tablestbl.o ./cf/apps/CorrNonFullTable/cs_tablestbl.o
	mv cs_eepromtbl.o ./cf/apps/CorrNonFullTable/cs_eepromtbl.o
	mv cs_memorytbl.o ./cf/apps/CorrNonFullTable/cs_memorytbl.o
	$(ELF2TBL) ./cf/apps/CorrNonFullTable/cs_apptbl.o ./cf/apps/CorrNonFullTable/
	$(ELF2TBL) ./cf/apps/CorrNonFullTable/cs_tablestbl.o ./cf/apps/CorrNonFullTable/
	$(ELF2TBL) ./cf/apps/CorrNonFullTable/cs_eepromtbl.o ./cf/apps/CorrNonFullTable/
	$(ELF2TBL) ./cf/apps/CorrNonFullTable/cs_memorytbl.o ./cf/apps/CorrNonFullTable/
	
istbls: ./cf/apps/InvldStNonFullTable/cs_apptbl.o \
./cf/apps/InvldStNonFullTable/cs_eepromtbl.o \
./cf/apps/InvldStNonFullTable/cs_tablestbl.o \
./cf/apps/InvldStNonFullTable/cs_memorytbl.o
	@mv cs_apptbl.o ./cf/apps/InvldStNonFullTable/cs_apptbl.o
	@mv cs_tablestbl.o ./cf/apps/InvldStNonFullTable/cs_tablestbl.o
	@mv cs_eepromtbl.o ./cf/apps/InvldStNonFullTable/cs_eepromtbl.o
	@mv cs_memorytbl.o ./cf/apps/InvldStNonFullTable/cs_memorytbl.o
	$(ELF2TBL) ./cf/apps/InvldStNonFullTable/cs_apptbl.o ./cf/apps/InvldStNonFullTable/
	$(ELF2TBL) ./cf/apps/InvldStNonFullTable/cs_tablestbl.o ./cf/apps/InvldStNonFullTable/
	$(ELF2TBL) ./cf/apps/InvldStNonFullTable/cs_eepromtbl.o ./cf/apps/InvldStNonFullTable/
	$(ELF2TBL) ./cf/apps/InvldStNonFullTable/cs_memorytbl.o ./cf/apps/InvldStNonFullTable/
	
intbls: ./cf/apps/InvldNameNonFullTable/cs_apptbl.o \
./cf/apps/InvldNameNonFullTable/cs_tablestbl.o
	@mv cs_apptbl.o ./cf/apps/InvldNameNonFullTable/cs_apptbl.o
	@mv cs_tablestbl.o ./cf/apps/InvldNameNonFullTable/cs_tablestbl.o
	$(ELF2TBL) ./cf/apps/InvldNameNonFullTable/cs_apptbl.o ./cf/apps/InvldNameNonFullTable/
	$(ELF2TBL) ./cf/apps/InvldNameNonFullTable/cs_tablestbl.o ./cf/apps/InvldNameNonFullTable/
	
enftbls: ./cf/apps/InvldEmptyNonFullTable/cs_apptbl.o \
./cf/apps/InvldEmptyNonFullTable/cs_tablestbl.o
	@mv cs_apptbl.o ./cf/apps/InvldEmptyNonFullTable/cs_apptbl.o
	@mv cs_tablestbl.o ./cf/apps/InvldEmptyNonFullTable/cs_tablestbl.o
	$(ELF2TBL) ./cf/apps/InvldEmptyNonFullTable/cs_apptbl.o ./cf/apps/InvldEmptyNonFullTable/
	$(ELF2TBL) ./cf/apps/InvldEmptyNonFullTable/cs_tablestbl.o ./cf/apps/InvldEmptyNonFullTable/
	
cftbls: ./cf/apps/CorrFullTable/cs_apptbl.o \
./cf/apps/CorrFullTable/cs_tablestbl.o \
./cf/apps/CorrFullTable/cs_eepromtbl.o \
./cf/apps/CorrFullTable/cs_memorytbl.o
	@mv cs_apptbl.o ./cf/apps/CorrFullTable/cs_apptbl.o
	@mv cs_tablestbl.o ./cf/apps/CorrFullTable/cs_tablestbl.o
	@mv cs_eepromtbl.o ./cf/apps/CorrFullTable/cs_eepromtbl.o
	@mv cs_memorytbl.o ./cf/apps/CorrFullTable/cs_memorytbl.o
	$(ELF2TBL) ./cf/apps/CorrFullTable/cs_apptbl.o ./cf/apps/CorrFullTable/
	$(ELF2TBL) ./cf/apps/CorrFullTable/cs_tablestbl.o ./cf/apps/CorrFullTable/
	$(ELF2TBL) ./cf/apps/CorrFullTable/cs_eepromtbl.o ./cf/apps/CorrFullTable/
	$(ELF2TBL) ./cf/apps/CorrFullTable/cs_memorytbl.o ./cf/apps/CorrFullTable/
	
detbls: ./cf/apps/DupEntryNonFullTable/cs_apptbl.o \
./cf/apps/DupEntryNonFullTable/cs_tablestbl.o
	@mv cs_apptbl.o ./cf/apps/DupEntryNonFullTable/cs_apptbl.o
	@mv cs_tablestbl.o ./cf/apps/DupEntryNonFullTable/cs_tablestbl.o
	$(ELF2TBL) ./cf/apps/DupEntryNonFullTable/cs_apptbl.o ./cf/apps/DupEntryNonFullTable/
	$(ELF2TBL) ./cf/apps/DupEntryNonFullTable/cs_tablestbl.o ./cf/apps/DupEntryNonFullTable/
	
qqtbls: ./cf/apps/CorrQQTable/cs_tablestbl.o
	@mv cs_tablestbl.o ./cf/apps/CorrQQTable/cs_tablestbl.o
	$(ELF2TBL) ./cf/apps/CorrQQTable/cs_tablestbl.o ./cf/apps/CorrQQTable/
	
clean ::
	rm -f *.o *.bin *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov *.tbl
	rm -f ./cf/apps/DefaultTable/*.o
	rm -f ./cf/apps/DefaultTable/*.tbl

cleantbl ::
	rm -f ./cf/apps/DupEntryNonFullTable/*.o
	rm -f ./cf/apps/DupEntryNonFullTable/*.tbl
	rm -f ./cf/apps/CorrNonFullTable/*.o
	rm -f ./cf/apps/CorrNonFullTable/*.tbl
	rm -f ./cf/apps/CorrFullTable/*.o
	rm -f ./cf/apps/CorrFullTable/*.tbl
	rm -f ./cf/apps/CorrQQTable/*.o
	rm -f ./cf/apps/CorrQQTable/*.tbl
	rm -f ./cf/apps/InvldNameNonFullTable/*.o
	rm -f ./cf/apps/InvldNameNonFullTable/*.tbl
	rm -f ./cf/apps/InvldStNonFullTable/*.o
	rm -f ./cf/apps/InvldStNonFullTable/*.tbl
	rm -f ./cf/apps/InvldEmptyNonFullTable/*.o
	rm -f ./cf/apps/InvldEmptyNonFullTable/*.tbl

cleantest : clean cleantbl

## Note that the following gcov sequence uses two awk scripts that are not universally
## applied in the CFS unit test makefiles.  If you have these awk scripts, and have
## added the CFS_TOOLS environment variable to your shell script you can uncomment
## this gcov definition (and comment out the one below it).
## Note that you should also include the compiler option '-Wunreachable-code' to the
## DEFAULT_COPTS variable near the beginning of this makefile.	
#gcov ::
#	@echo
#	@gcov -b $(CS_OBJS:.o=.gcda) | awk -f $(CFS_TOOLS)/gcov_parser.awk
#	@awk -f $(CFS_TOOLS)/gcov_warning.awk $(CS_OBJS:.o=.c.gcov)
#	@rm -f *.gcda *.gcno
#	@echo	

gcov ::
	@echo
	@gcov $(CS_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo

#
###########################################################################
#
# end of file
