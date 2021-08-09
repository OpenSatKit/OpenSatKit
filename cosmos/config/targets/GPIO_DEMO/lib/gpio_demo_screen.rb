###############################################################################
# Pi-Sat GPIO Demo Screen
#
# Notes:
#   None
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'fsw_const'
require 'fsw_config_param'

################################################################################
## GUI Send Commands
################################################################################

def gpio_demo_cmd(screen, cmd)
 
   case cmd 
   when "NOOP"
      Osk::Ops::send_flt_cmd("GPIO_DEMO", Osk::CMD_STR_NOOP)
   when "RESET"
      Osk::Ops::send_flt_cmd("GPIO_DEMO", Osk::CMD_STR_RESET)
   when "SET_ON_TIME"
      on_time = ask_string("Input on time (milliseconds)", 3000)
      Osk::flight.send_cmd("GPIO_DEMO", "SET_ON_TIME with ON_TIME #{on_time}")
   when "SET_OFF_TIME"
      off_time = ask_string("Input off time (milliseconds)", 3000)
      Osk::flight.send_cmd("GPIO_DEMO", "SET_OFF_TIME with OFF_TIME #{off_time}")
   when "PLOT_LED"
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'GPIO_DEMO HK_TLM_PKT GPIO_LED_STATE'")
   else
      raise "Error in screen definition file. Undefined cmd '#{cmd}' sent gpio_demo_cmd()"
   end
  
end # gpio_demo_cmd()

