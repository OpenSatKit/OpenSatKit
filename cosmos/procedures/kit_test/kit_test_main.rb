###############################################################################
# Top-level function that manages the starter kit integration tests.
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'message_ids'

require 'kit_test_cfe_es'
require 'kit_test_cfe_evs'
require 'kit_test_cfe_sb'
require 'kit_test_cfe_tbl'
require 'kit_test_cfe_time'

require 'kit_test_cs'
require 'kit_test_ds'
require 'kit_test_fm'
require 'kit_test_hs'
require 'kit_test_lc'
require 'kit_test_md'
require 'kit_test_mm'
require 'kit_test_sc'

require 'kit_test_bm'
require 'kit_test_f42'
require 'kit_test_hc'
require 'kit_test_hsim'
require 'kit_test_i42'
require 'kit_test_kit_ci'
require 'kit_test_kit_sch'
require 'kit_test_kit_to'
require 'kit_test_tftp'

puts 'cFS Starter Kit Integration script initiated'

###############################################################################
## Instantiate test objects
###############################################################################

# cFE Apps

cfe_es  = App.new("CFE_ES","CFE_ES","HK_TLM_PKT","0x1806")
es_test = KitTestCfeEs.new(cfe_es)

cfe_evs  = App.new("CFE_EVS","CFE_EVS","HK_TLM_PKT","0x1801")
evs_test = KitTestCfeEvs.new(cfe_evs)

cfe_sb  = App.new("CFE_SB","CFE_SB","HK_TLM_PKT","0x1803")
sb_test = KitTestCfeSb.new(cfe_sb)

cfe_tbl  = App.new("CFE_TBL","CFE_TBL","HK_TLM_PKT","0x1804")
tbl_test = KitTestCfeTbl.new(cfe_tbl)

cfe_time  = App.new("CFE_TIME","CFE_TIME","HK_TLM_PKT","0x1805")
time_test = KitTestCfeTime.new(cfe_time)

# cFS Apps

cs      = App.new("CS_APP","CS","HK_TLM_PKT","0x189F")
cs_test = KitTestCs.new(cs)

ds      = App.new("DS_APP","DS","HK_TLM_PKT","0x18BB")
ds_test = KitTestDs.new(ds)

fm       = App.new("FM_APP","FM","HK_TLM_PKT","0x188C")
fm_test = KitTestFm.new(fm)

hs      = App.new("HS_APP","HS","HK_TLM_PKT","0x18AE")
hs_test = KitTestHs.new(hs)

lc      = App.new("LC_APP","LC","HK_TLM_PKT","0x18A4")
lc_test = KitTestLc.new(lc)

md      = App.new("MD_APP","MD","HK_TLM_PKT","0x1890")
md_test = KitTestMd.new(md)

mm      = App.new("MM_APP","MM","HK_TLM_PKT","0x1888")
mm_test = KitTestMm.new(mm)

sc      = App.new("SC_APP","SC","HK_TLM_PKT","0x18A9")
sc_test = KitTestSc.new(sc)

# Custom Starter Kit Apps

bm      = App.new("BM_APP","BM","HK_TLM_PKT",MessageIds::BM_CMD_MID)
bm_test = KitTestBm.new(bm)

f42      = App.new("F42_APP","F42","HK_TLM_PKT",MessageIds::F42_CMD_MID)
f42_test = KitTestF42.new(f42)

hc      = App.new("HC_APP","HC","HK_TLM_PKT","0x19A5")
hc_test = KitTestHc.new(hc)

hsim      = App.new("HSIM_APP","HSIM","HK_TLM_PKT","0x19B1")
hsim_test = KitTestHsim.new(hsim)

i42      = App.new("I42_APP","I42","HK_TLM_PKT",MessageIds::I42_CMD_MID)
i42_test = KitTestI42.new(i42)

kit_ci      = App.new("KIT_CI_APP","KIT_CI","HK_TLM_PKT","0x1884")
kit_ci_test = KitTestKitCi.new(kit_ci)

kit_sch      = App.new("KIT_SCH_APP","KIT_SCH","HK_TLM_PKT","0x1895")
kit_sch_test = KitTestKitSch.new(kit_sch)

kit_to      = App.new("KIT_TO_APP","KIT_TO","HK_TLM_PKT","0x1880")
kit_to_test = KitTestKitTo.new(kit_to)

tftp      = App.new("TFTP_APP","TFTP","HK_TLM_PKT","0x18B5")
tftp_test = KitTestTftp.new(tftp)

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

bm_test.run
puts bm.name + " " + bm_test.result_str

f42_test.run
puts f42.name + " " + f42_test.result_str

hc_test.run
puts hc.name + " " + hc_test.result_str

hsim_test.run
puts hsim.name + " " + hsim_test.result_str

i42_test.run
puts i42.name + " " + i42_test.result_str

kit_ci_test.run
puts kit_ci.name + " " + kit_ci_test.result_str

kit_sch_test.run
puts kit_sch.name + " " + kit_sch_test.result_str

kit_to_test.run
puts kit_to.name + " " + kit_to_test.result_str

tftp_test.run
puts tftp.name + " " + tftp_test.result_str

puts 'cFS Starter Kit Integration script completed'
