###############################################################################
# Top-level function that manages SimSat's integration test.
#
# Notes:
#   1. This was written before I learned about COSMOS test runner which is the
#      appropriate tool to use for an integration test. 
#   2. TODO - Migrate this test to test runner framework 
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'fsw_msg_id'

require './config/targets/CFE_ES/procedures/cfe_es_intg_test'
require './config/targets/CFE_EVS/procedures/cfe_evs_intg_test'
require './config/targets/CFE_SB/procedures/cfe_sb_intg_test'
require './config/targets/CFE_TBL/procedures/cfe_tbl_intg_test'
require './config/targets/CFE_TIME/procedures/cfe_time_intg_test'

require './config/targets/CS/procedures/cs_intg_test'
require './config/targets/DS/procedures/ds_intg_test'
require './config/targets/FM/procedures/fm_intg_test'
require './config/targets/HK/procedures/hk_intg_test'
require './config/targets/HS/procedures/hs_intg_test'
require './config/targets/LC/procedures/lc_intg_test'
require './config/targets/MD/procedures/md_intg_test'
require './config/targets/MM/procedures/mm_intg_test'
require './config/targets/SC/procedures/sc_intg_test'

require './config/targets/F42/procedures/f42_intg_test'
require './config/targets/I42/procedures/i42_intg_test'
require './config/targets/ISIM/procedures/isim_intg_test'
require './config/targets/KIT_CI/procedures/kit_ci_intg_test'
require './config/targets/KIT_SCH/procedures/kit_sch_intg_test'
require './config/targets/KIT_TO/procedures/kit_to_intg_test'
require './config/targets/TFTP/procedures/tftp_intg_test'

puts 'cFS Starter Kit Integration script initiated'

###############################################################################
## Instantiate test objects
###############################################################################

# cFE Apps

cfe_es  = App.new("CFE_ES","CFE_ES","HK_TLM_PKT",Fsw::MsgId::CFE_ES_CMD_MID)
es_test = CfeEsIntgTest.new(cfe_es)

cfe_evs  = App.new("CFE_EVS","CFE_EVS","HK_TLM_PKT",Fsw::MsgId::CFE_EVS_CMD_MID)
evs_test = CfeEvsIntgTest.new(cfe_evs)

cfe_sb  = App.new("CFE_SB","CFE_SB","HK_TLM_PKT",Fsw::MsgId::CFE_SB_CMD_MID)
sb_test = CfeSbIntgTest.new(cfe_sb)

cfe_tbl  = App.new("CFE_TBL","CFE_TBL","HK_TLM_PKT",Fsw::MsgId::CFE_TBL_CMD_MID)
tbl_test = CfeTblIntgTest.new(cfe_tbl)

cfe_time  = App.new("CFE_TIME","CFE_TIME","HK_TLM_PKT",Fsw::MsgId::CFE_TIME_CMD_MID)
time_test = CfeTimeIntgTest.new(cfe_time)

# cFS Apps

cs      = App.new("CS_APP","CS","HK_TLM_PKT",Fsw::MsgId::CS_CMD_MID)
cs_test = CsIntgTest.new(cs)

ds      = App.new("DS_APP","DS","HK_TLM_PKT",Fsw::MsgId::DS_CMD_MID)
ds_test = DsIntgTest.new(ds)

fm       = App.new("FM_APP","FM","HK_TLM_PKT",Fsw::MsgId::FM_CMD_MID)
fm_test = FmIntgTest.new(fm)

hk      = App.new("HK_APP","HK","HK_TLM_PKT",Fsw::MsgId::HK_CMD_MID)
hk_test = HkIntgTest.new(hk)

hs      = App.new("HS_APP","HS","HK_TLM_PKT",Fsw::MsgId::HS_CMD_MID)
hs_test = HsIntgTest.new(hs)

lc      = App.new("LC_APP","LC","HK_TLM_PKT",Fsw::MsgId::LC_CMD_MID)
lc_test = LcIntgTest.new(lc)

md      = App.new("MD_APP","MD","HK_TLM_PKT",Fsw::MsgId::MD_CMD_MID)
md_test = MdIntgTest.new(md)

mm      = App.new("MM_APP","MM","HK_TLM_PKT",Fsw::MsgId::MM_CMD_MID)
mm_test = MmIntgTest.new(mm)

sc      = App.new("SC_APP","SC","HK_TLM_PKT",Fsw::MsgId::SC_CMD_MID)
sc_test = ScIntgTest.new(sc)

# OSK Kit Apps

f42      = App.new("F42_APP","F42","HK_TLM_PKT",Fsw::MsgId::F42_CMD_MID)
f42_test = F42IntgTest.new(f42)

i42      = App.new("I42_APP","I42","HK_TLM_PKT",Fsw::MsgId::I42_CMD_MID)
i42_test = I42IntgTest.new(i42)

isim      = App.new("ISIM_APP","ISIM","HK_TLM_PKT",Fsw::MsgId::ISIM_CMD_MID)
isim_test = IsimIntgTest.new(isim)

kit_ci      = App.new("KIT_CI_APP","KIT_CI","HK_TLM_PKT",Fsw::MsgId::KIT_CI_CMD_MID)
kit_ci_test = KitCiIntgTest.new(kit_ci)

kit_sch      = App.new("KIT_SCH_APP","KIT_SCH","HK_TLM_PKT",Fsw::MsgId::KIT_SCH_CMD_MID)
kit_sch_test = KitSchIntgTest.new(kit_sch)

kit_to      = App.new("KIT_TO_APP","KIT_TO","HK_TLM_PKT",Fsw::MsgId::KIT_TO_CMD_MID)
kit_to_test = KitToIntgTest.new(kit_to)

tftp      = App.new("TFTP_APP","TFTP","HK_TLM_PKT",Fsw::MsgId::TFTP_CMD_MID)
tftp_test = TftpIntgTest.new(tftp)

###############################################################################
## Run Tests and Report Results
###############################################################################

puts "\nTest Results"

# cFE Apps

es_test.run
puts cfe_es.name + " " + es_test.result_str

evs_test.run
puts cfe_evs.name + " " + evs_test.result_str

sb_test.run
puts cfe_sb.name + " " + sb_test.result_str

tbl_test.run
puts cfe_tbl.name + " " + tbl_test.result_str

time_test.run
puts cfe_time.name + " " + time_test.result_str

# cFS Apps

cs_test.run
puts cs.name + " " + cs_test.result_str

ds_test.run
puts ds.name + " " + ds_test.result_str

fm_test.run
puts fm.name + " " + fm_test.result_str

hk_test.run
puts hk.name + " " + hk_test.result_str

hs_test.run
puts hs.name + " " + hs_test.result_str

lc_test.run
puts lc.name + " " + lc_test.result_str

md_test.run
puts md.name + " " + md_test.result_str

mm_test.run
puts mm.name + " " + mm_test.result_str

sc_test.run
puts sc.name + " " + sc_test.result_str

# Custom Starter Kit Apps

f42_test.run
puts f42.name + " " + f42_test.result_str

i42_test.run
puts i42.name + " " + i42_test.result_str

isim_test.run
puts isim.name + " " + isim_test.result_str

kit_ci_test.run
puts kit_ci.name + " " + kit_ci_test.result_str

kit_sch_test.run
puts kit_sch.name + " " + kit_sch_test.result_str

kit_to_test.run
puts kit_to.name + " " + kit_to_test.result_str

tftp_test.run
puts tftp.name + " " + tftp_test.result_str

puts 'cFS Starter Kit Integration script completed'
