###############################################################################
# Top-level function that manages SimSat's integration test.
#
# Notes:
#   1. This was written before I learned about COSMOS test runner but even 
#      after I was aware of test runner I decided not to refactor the tests
#      because the integration tests are instantiated and run use OSK's app
#      list which helps verify OSK's system integration.  
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_targets'
require 'osk_system'

# cFE Apps

require './config/targets/CFE_ES/procedures/cfe_es_intg_test'
require './config/targets/CFE_EVS/procedures/cfe_evs_intg_test'
require './config/targets/CFE_SB/procedures/cfe_sb_intg_test'
require './config/targets/CFE_TBL/procedures/cfe_tbl_intg_test'
require './config/targets/CFE_TIME/procedures/cfe_time_intg_test'

# cFS Apps

require './config/targets/CS/procedures/cs_intg_test'
require './config/targets/DS/procedures/ds_intg_test'
require './config/targets/FM/procedures/fm_intg_test'
require './config/targets/HK/procedures/hk_intg_test'
require './config/targets/HS/procedures/hs_intg_test'
require './config/targets/LC/procedures/lc_intg_test'
require './config/targets/MD/procedures/md_intg_test'
require './config/targets/MM/procedures/mm_intg_test'
require './config/targets/SC/procedures/sc_intg_test'

# OSK Kit Apps

require './config/targets/F42/procedures/f42_intg_test'
require './config/targets/HC/procedures/hc_intg_test'
require './config/targets/HSIM/procedures/hsim_intg_test'
require './config/targets/I42/procedures/i42_intg_test'
require './config/targets/ISIM/procedures/isim_intg_test'
require './config/targets/KIT_CI/procedures/kit_ci_intg_test'
require './config/targets/KIT_SCH/procedures/kit_sch_intg_test'
require './config/targets/KIT_TO/procedures/kit_to_intg_test'
require './config/targets/TFTP/procedures/tftp_intg_test'

###############################################################################
## Instantiate test objects
###############################################################################

# cFE Apps

cfe_es_test   = CfeEsIntgTest.new(Osk::flight.app["CFE_ES"])
cfe_evs_test  = CfeEvsIntgTest.new(Osk::flight.app["CFE_EVS"])
cfe_sb_test   = CfeSbIntgTest.new(Osk::flight.app["CFE_SB"])
cfe_tbl_test  = CfeTblIntgTest.new(Osk::flight.app["CFE_TBL"])
cfe_time_test = CfeTimeIntgTest.new(Osk::flight.app["CFE_TIME"])

# cFS Apps

cs_test   = CsIntgTest.new(Osk::flight.app["CS"])
ds_test   = DsIntgTest.new(Osk::flight.app["DS"])
fm_test   = FmIntgTest.new(Osk::flight.app["FM"])
hk_test   = HkIntgTest.new(Osk::flight.app["HK"])
hs_test   = HsIntgTest.new(Osk::flight.app["HS"])
lc_test   = LcIntgTest.new(Osk::flight.app["LC"])
md_test   = MdIntgTest.new(Osk::flight.app["MD"])
mm_test   = MmIntgTest.new(Osk::flight.app["MM"])
sc_test   = ScIntgTest.new(Osk::flight.app["SC"])

# OSK Kit Apps

f42_test     = F42IntgTest.new(Osk::flight.app["F42"])
hc_test      = HcIntgTest.new(Osk::flight.app["HC"])
hsim_test    = HsimIntgTest.new(Osk::flight.app["HSIM"])
i42_test     = I42IntgTest.new(Osk::flight.app["I42"])
isim_test    = IsimIntgTest.new(Osk::flight.app["ISIM"])
kit_ci_test  = KitCiIntgTest.new(Osk::flight.app["KIT_CI"])
kit_sch_test = KitSchIntgTest.new(Osk::flight.app["KIT_SCH"])
kit_to_test  = KitToIntgTest.new(Osk::flight.app["KIT_TO"])
tftp_test    = TftpIntgTest.new(Osk::flight.app["TFTP"])

###############################################################################
## Run Tests and Report Results
###############################################################################

puts '*******************************************'
puts '*** SimSat Integration script started'
puts '*******************************************'

# cFE Apps

cfe_es_test.run
puts cfe_es_test.app.target + " " + cfe_es_test.result_str

cfe_evs_test.run
puts cfe_evs_test.app.target + " " + cfe_evs_test.result_str

cfe_sb_test.run
puts cfe_sb_test.app.target + " " + cfe_sb_test.result_str

cfe_tbl_test.run
puts cfe_tbl_test.app.target + " " + cfe_tbl_test.result_str

cfe_time_test.run
puts cfe_time_test.app.target + " " + cfe_time_test.result_str

# cFS Apps

cs_test.run
puts cs_test.app.target + " " + cs_test.result_str

ds_test.run
puts ds_test.app.target + " " + ds_test.result_str

fm_test.run
puts fm_test.app.target + " " + fm_test.result_str

hk_test.run
puts hk_test.app.target + " " + hk_test.result_str

hs_test.run
puts hs_test.app.target + " " + hs_test.result_str

lc_test.run
puts lc_test.app.target + " " + lc_test.result_str

md_test.run
puts md_test.app.target + " " + md_test.result_str

mm_test.run
puts mm_test.app.target + " " + mm_test.result_str

sc_test.run
puts sc_test.app.target + " " + sc_test.result_str


# OSK Kit Apps

f42_test.run
puts f42_test.app.target + " " + f42_test.result_str

hc_test.run
puts hc_test.app.target + " " + hc_test.result_str

hsim_test.run
puts hsim_test.app.target + " " + hsim_test.result_str

i42_test.run
puts i42_test.app.target + " " + i42_test.result_str

isim_test.run
puts isim_test.app.target + " " + isim_test.result_str

kit_ci_test.run
puts kit_ci_test.app.target + " " + kit_ci_test.result_str

kit_sch_test.run
puts kit_sch_test.app.target + " " + kit_sch_test.result_str

kit_to_test.run
puts kit_to_test.app.target + " " + kit_to_test.result_str

tftp_test.run
puts tftp_test.app.target + " " + tftp_test.result_str


puts '*******************************************'
puts '*** Simsat Integration script completed'
puts '*******************************************'
