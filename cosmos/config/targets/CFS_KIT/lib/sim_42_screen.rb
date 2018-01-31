###############################################################################
# cFS Kit 42 Simulator Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'cfs_kit_global'

################################################################################
## Global Variables
################################################################################


################################################################################
## Send Commands
################################################################################

def sim_42_send_cmd(screen, cmd)

  if (cmd == "RUN")
    continue = message_box("This runs an example simulation with 42. It takes 20 seconds to start. Continue?",'Yes','No')
    if (continue == 'Yes')
       spawn("xfce4-terminal --default-working-directory=""#{Cosmos::USERPATH}/#{REL_DIR_42}"" --execute ./42""")
       wait(20)
       cmd("I42 CONNECT_TO_42")
    end
  elsif (cmd == "CONNECT")
    continue = message_box("The 42 simulator must be running for connect to succeed. Continue?",'Yes','No')
    if (continue == 'Yes')
       cmd("I42 CONNECT_TO_42")
    end
  elsif (cmd == "CONFIG_CSS_FAULT")
    selection = vertical_message_box("Set CSS Fault",'Set CSS Fault','Clear CSS Fault')
	case selection
	  when 'Set CSS Fault'
        cmd("F42 SET_FAULT_STATE with ID 0, STATE 1")
	  when 'Clear CSS Fault'
        cmd("F42 SET_FAULT_STATE with ID 0, STATE 0")
	end
  elsif (cmd == "CHANGE_GAINS")
    display("CFS_KIT F42_TBL_SCREEN",50,50)
    #Cosmos.run_process("ruby lib/OskTblEditor")
    #Cosmos.run_process("ruby tools/ConfigEditor")
	#require 'osk_tbl_editor'
    #Cosmos::OskTblEditor.run
    #Cosmos.run_cosmos_tool('ConfigEditor')
  elsif (cmd == "SET_CTRL_MODE")
    selection = vertical_message_box("Select the new control mode",'Reset Current Mode','To Be Defined')
	case selection
	  when 'Reset Current Mode'
	    new_mode = 1
	  when 'To Be Defined'
	    new_mode = 2
	end
	cmd("F42 SET_CTRL_MODE with NEW_MODE #{new_mode}")  
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
	cmd("F42 SET_SUN_TARGET with AXIS #{axis_id}")  
  elsif (cmd == "PLOT_THETA_ERR")
    spawn("ruby #{Cosmos::USERPATH}/tools/TlmGrapher -s -i 'F42 CONTROL_PKT THERR_1' -i 'F42 CONTROL_PKT THERR_2' -i 'F42 CONTROL_PKT THERR_3'")
  elsif (cmd == "PLOT_WHEEL_CMD")
    spawn("ruby #{Cosmos::USERPATH}/tools/TlmGrapher -s -i 'F42 CONTROL_PKT TWHLCMD_1' -i 'F42 CONTROL_PKT TWHLCMD_2' -i 'F42 CONTROL_PKT TWHLCMD_3'")
  elsif (cmd == "TODO")
    prompt("Feature coming soon...")
  else
    prompt("Error in screen definition file. Undefined command sent to sim_42_send_cmd()")
  end
  
end # sim_42_send_cmd()

