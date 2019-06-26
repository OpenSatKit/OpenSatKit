require 'cosmos'
require 'cosmos/script'
excluded_tlm_items = ["RECEIVED_TIMESECONDS",
       "RECEIVED_TIMEFORMATTED",
       "RECEIVED_COUNT",
       "CCSDS_STREAMID", 
       "CCSDS_SEQUENCE", 
       "CCSDS_LENGTH", 
       "CCSDS_SECONDS", 
       "CCSDS_SUBSECS", 
       "COMMAND_COUNT", 
       "ERROR_COUNT", 
       "SPARE2"]

parameters = Hash.new(0)

main_dialog_buttons = ["Change parameters", 
       "Run benchmarks",
       "Exit script"]

cmd("CFS ENABLE_TELEMETRY") # Enable receipt of data

loop do
  #load params from telem
  cmd("CFS BM_SENDHK")
  wait_packet("CFS", "CFE_BM_HKTLMPKT", 1, 5)
  get_tlm_packet("CFS", "CFE_BM_HKTLMPKT").each {|item|
    if (!excluded_tlm_items.include?(item[0]))
      parameters[item[0]] = item[1]
    end
  }
  #run modify exit
  choice = message_box("What would you like to do?", 
    main_dialog_buttons[0], main_dialog_buttons[1], main_dialog_buttons[2])
    
    case choice
    when main_dialog_buttons[0]
      #modify
      parameters.each_pair {|key, value|
        input = ask("Enter a value for #{key} (currently #{value}):", true)
        if (input != "")
          parameters[key] = input
        end
      }
      #send changes
      parameter_cmd_str = "CFS BM_SB_SETPARAMS with"
      parameters.each_pair {|key, value|
       parameter_cmd_str = parameter_cmd_str + " #{key} #{value},"
      }
      parameter_cmd_str.chomp!
      cmd(parameter_cmd_str);
      #beginning
    when main_dialog_buttons[1]
      cmd("CFS BM_RUNALL") #run benchmarks
    when main_dialog_buttons[2]
      break #exit script
    end
   
end