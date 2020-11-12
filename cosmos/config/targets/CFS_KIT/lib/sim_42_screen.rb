###############################################################################
# cFS Kit 42 Simulator Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'osk_global'
require 'osk_system'

################################################################################
## Send Commands
################################################################################

def sim_42_send_cmd(screen, cmd)

   if (cmd == "START_42_SIM")
      Osk::System.start_42
   elsif (cmd == "STOP_42_SIM")
      Osk::System.stop_42
   elsif (cmd == "RECONNECT_42")
      continue = message_box("This should only be used in an erroneous situaton when the 42 simulator is running but not connected. It may terminate the 42 simulator if it is currently conneccted. Do you want to Continue?",Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO,false)
      if (continue == Osk::MSG_BUTTON_YES)
         Osk::flight.send_cmd("I42","CONNECT_42")
      end
   elsif (cmd == "SET_42_EXEC_RATE")
      Osk::Ops::send_flt_cmd("I42", "CONFIG_EXECUTE")
   elsif (cmd == "SET_CTRL_GAINS")
      display("F42 CTRL_GAINS_SCREEN",50,50)
   elsif (cmd == "SET_WHL_TGT_MOM")
      display("F42 WHL_TGT_MOM_CMD_SCREEN",50,50)
   elsif (cmd == "CONFIG_SUN_VALID")
      # When 'cancel button' flag is set to true the COSMOS puts the cancel button in the middle of the list so the cancel 
      # button is set to false and the last button is defined as cancel. 
      selection = vertical_message_box("Select SunValid Option  ","Use 42 Sim Value","Override with Valid","Override with Invalid","Cancel",false)
	   case selection
	      when 'Use 42 Sim Value'
            Osk::flight.send_cmd("F42","SET_OVR_STATE with ID 0, STATE 1")
	      when 'Override with Valid'
            Osk::flight.send_cmd("F42","SET_OVR_STATE with ID 0, STATE 2")
	      when 'Override with Invalid'
            Osk::flight.send_cmd("F42","SET_OVR_STATE with ID 0, STATE 3")
	   end
   elsif (cmd == "MANAGE_CTRL_TBL")
      display("F42 TBL_SCREEN",50,50)
   elsif (cmd == "SET_CTRL_MODE")
      selection = vertical_message_box("Select the new control mode",'Reset Current Mode','To Be Defined')
	   case selection
	      when 'Reset Current Mode'
	         new_mode = 1
	      when 'To Be Defined'
	         new_mode = 2
	   end
	   Osk::flight.send_cmd("F42","SET_CTRL_MODE with NEW_MODE #{new_mode}")  
   elsif (cmd == "SET_SUN_AXIS")
      selection = combo_box("Select the target axis",'+X','-X','+Y','-Y','+Z','-Z')
	   axis_id = 2  # Default to +Y
	   case selection
	      when '+X'
	         axis_id = 0
	      when '-X'
	         axis_id = 1
	      when '+Y'
	         axis_id = 2
	      when '-Y'
	         axis_id = 3
	      when '+Z'
	         axis_id = 4
	      when '-Z'
	         axis_id = 5
	   end
	   Osk::flight.send_cmd("F42","SET_SUN_TARGET with AXIS #{axis_id}")  
   elsif (cmd == "PLOT_ATT_ERR")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'F42 CONTROL_PKT THERR_X' -i 'F42 CONTROL_PKT THERR_Y' -i 'F42 CONTROL_PKT THERR_Z'")
   elsif (cmd == "PLOT_RATE_ERR")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'F42 CONTROL_PKT WERR_X' -i 'F42 CONTROL_PKT WERR_Y' -i 'F42 CONTROL_PKT WERR_Z'")
   elsif (cmd == "PLOT_TORQ_CMD")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'F42 CONTROL_PKT TRQ_CMD_X' -i 'F42 CONTROL_PKT TRQ_CMD_Y' -i 'F42 CONTROL_PKT TRQ_CMD_Z'")
   elsif (cmd == "PLOT_MOM_ERR")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'F42 CONTROL_PKT HVB_X' -i 'F42 CONTROL_PKT HVB_Y' -i 'F42 CONTROL_PKT HVB_Z'")
   elsif (cmd == "PLOT_MOM_CMD")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'F42 CONTROL_PKT MOM_CMD_X' -i 'F42 CONTROL_PKT MOM_CMD_Y' -i 'F42 CONTROL_PKT MOM_CMD_Z'")
   elsif (cmd == "PLOT_SA_GIM_CMD")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'F42 CONTROL_PKT SA_GIM_CMD'")
   elsif (cmd == "TODO")
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined command sent to sim_42_send_cmd()"
   end
  
end # sim_42_send_cmd()

