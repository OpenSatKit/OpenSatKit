###############################################################################
# Housekeeping Table Generation Tool
#
# Notes:
#   1. This tool reads a user supplied JSON file that defines the HK combo
#      packet content. The FSW message ID macro names are verified using
#      fsw_msg_id.rb and the packet/mnemonic names are verified using the 
#      COSMOS System.telemetry.packet definitions
# 
#   TODO - Break this into class files. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'cosmos/system'
require 'fileutils'
require 'json'

class HkCopyTbl

   
   def initialize(json_hk_file, header_file, footer_file)

      raise IOError "JSON definition file #{json_hk_file} does not exist" unless File.exist?(json_hk_file)

      @header_file  = header_file
      @footer_file  = footer_file
      
      # 12th byte is the first payload data byte
      @offset = {"HK_COMBINED_PKT1_MID" => 12, "HK_COMBINED_PKT2_MID" => 12, "HK_COMBINED_PKT3_MID" => 12} 
      
      @json_hk_file = json_hk_file
      json_hk_txt   = File.read(json_hk_file)
      @json_hk_hash = JSON.parse(json_hk_txt)

      puts "@json_hk_hash = #{@json_hk_hash}\n"
      
      @json_hk_packets = @json_hk_hash["packets"]

   end # initialize()

   # 
   # <C table file header>
   #
   # /*           inputMid        inputOffset     outputMid      outputOffset  numBytes*/
   # /*   0 */ { CFE_ES_HK_TLM_MID,    12,   HK_COMBINED_PKT1_MID,      12,      4, },
   #
   # <C table file footer>
   #
   def create_fsw_tbl(tbl_filename, description, verbose)
   
      header_str = File.read(@header_file).sub("@description@",description)
      footer_str = File.read(@footer_file)
     
      #cfe_es_hk_pkt = Cosmos::System.telemetry.packet("CFE_ES", "HK_TLM_PKT") # Return Packet
      #puts "cfe_es_hk_pkt: class=#{cfe_es_hk_pkt.class}\n"
      #cfe_es_hk_pkt_items = Cosmos::System.telemetry.items("CFE_ES", "HK_TLM_PKT") # Return PacketItem Array
      #cfe_es_hk_pkt_items.each do |item|
      #   puts "#{item.name}: bit_size=#{item.bit_size}, bit_offset=#{item.bit_offset}\n"
      #end     
      #cfe_es_hk_pkt = Cosmos::System.telemetry.item_names("CFE_ES", "HK_TLM_PKT")
      #puts "cfe_es_hk_pkt = #{cfe_es_hk_pkt}\n"
      
      File.open(tbl_filename, 'w') do |tbl_file|

         tbl_file << header_str
         
         @json_hk_packets.each do |packet|
      
            tbl_file << "#{packet}\n"
      
            hk_combo_mid_str = packet["combo-1"]["mid"]
            hk_combo_mid_val = Fsw::MsgId.get_val(packet["combo-1"]["mid"])
            puts "hk_combo_mid_str = #{hk_combo_mid_str}, hk_combo_mid_val = #{hk_combo_mid_val}\n"
            
            #~~puts "data array = #{packet["combo-1"]["data"]}\n"
            packet["combo-1"]["headers"].each do |header|
               tbl_file << "#{header}\n"
            end
            packet["combo-1"]["src-data"].each do |src_data|
               #tbl_file << "#{src_data}\n"
               src_index = get_src_tlm_index(src_data)
               if (src_index[0] != 0 and src_index[0] != 0)
                  comment_str = "  /* #{"%-90s"%[src_data]} */"
                  tbl_file << "{ #{Fsw::MsgId.get_val(src_data["tlm_mid"])}, #{"%3d" % [src_index[0]]}, #{hk_combo_mid_val}, #{"%3d" % [@offset[hk_combo_mid_str]]}, #{"%3d" % [src_index[1]]} }, #{comment_str}\n" 
                  @offset["#{hk_combo_mid_str}"] += src_index[1]
               else
                  # TODO - Raise exception
                  puts "**ERROR** Source data undefined: #{src_data}\n"
               end   
            end
            
         end # Entry loop
         
         tbl_file << footer_str

      end # tbl_file
   
   end # create_fsw_tbl()
   
   def get_src_tlm_index(json_data_entry)
      
      byte_offset = 0
      num_bytes   = 0
      
      tbl_target   = json_data_entry["tlm-item"].split
      puts "get_src_tlm_index(#{json_data_entry}): tbl_target = #{tbl_target}\n"
      
      tbl_tlm_item = tbl_target[2]
      tlm_pkt_items = Cosmos::System.telemetry.items(tbl_target[0], tbl_target[1]) # Return PacketItem Array
      tlm_pkt_items.each do |item|
         if (item.name.eql?(tbl_tlm_item))
            puts "Matched #{item.name}: bit_size=#{item.bit_size}, bit_offset=#{item.bit_offset}\n"
            byte_offset = item.bit_offset/8
            num_bytes   = item.bit_size/8
         end
      end

      return [byte_offset, num_bytes]
      
   end # get_src_tlm_index()

end # class HkCopyTbl


# irb: test = GenToTbl.new("gen_kit_to_ds_tbl.json")
# irb: test = GenToTbl.new("simsat-gen_to_ds_tbl.json")

class GenHkTbl

   # JSON Table Tool Object
   JSON_TBL_TOOL      = "hk-tbl-tool"
   JSON_VERBOSE       = "verbose"
   JSON_INPUT_PATH    = "input-path"
   JSON_OUTPUT_PATH   = "output-path"
   JSON_TEMPLATE_PATH = "template-path"
   
   # JSON Housekeeping Object
   JSON_HK_OBJ        = "housekeeping"
   JSON_HK_FILE       = "json-hk-file"
   JSON_LOG_FILE      = "log-file"
   JSON_HEADER_FILE   = "header-file"
   JSON_FOOTER_FILE   = "footer-file"
   JSON_OUTPUT_FILE   = "output-file"
   JSON_MAX_ENTRIES   = "max-entries"
   
   @verbose
   @log_file
   
   def log_msg(msg)
   
      @log_file.write("#{msg}\n")
      puts ">> #{msg}\n"
      
   end # log_msg()
   
   def initialize(config_file)
      begin

         config = read_config_file(config_file)
         
         tool_config = config[JSON_TBL_TOOL]
         hk_config   = config[JSON_TBL_TOOL][JSON_HK_OBJ]
         #~~puts "#{tool_config}\n"
         #~~puts "#{hk_config}\n"
         @verbose    = tool_config[JSON_VERBOSE]
         
         @log_filename = File.join(tool_config[JSON_OUTPUT_PATH],hk_config[JSON_LOG_FILE])
         puts "log file = #{@log_filename}\n"
         @log_file = open(@log_filename, "w")
         
         log_msg "Preparing HK copy table generation using configuration file #{config_file}"
         log_msg "> Input directory:    #{tool_config[JSON_INPUT_PATH]}"
         log_msg "> Output directory:   #{tool_config[JSON_OUTPUT_PATH]}"
         log_msg "> Template directory: #{tool_config[JSON_TEMPLATE_PATH]}\n\n"

         json_hk_file    = File.join(tool_config[JSON_INPUT_PATH],hk_config[JSON_HK_FILE])
         header_file     = File.join(config[JSON_TBL_TOOL][JSON_TEMPLATE_PATH],hk_config[JSON_HEADER_FILE])
         footer_file     = File.join(config[JSON_TBL_TOOL][JSON_TEMPLATE_PATH],hk_config[JSON_FOOTER_FILE])
         hk_cpy_tbl_file = File.join(config[JSON_TBL_TOOL][JSON_OUTPUT_PATH],hk_config[JSON_OUTPUT_FILE])
         
         log_msg "Generating HK copy table using"
         log_msg "> Tbl def file: #{json_hk_file}"
         log_msg "> Header file:  #{header_file}"
         log_msg "> Footer file:  #{footer_file}"
          
         hk_cpy_tbl   = HkCopyTbl.new(json_hk_file,header_file,footer_file)
         hk_cpy_tbl.create_fsw_tbl(hk_cpy_tbl_file, "Table generated from #{json_hk_file}", @verbose)
         #~~ hk_cpy_tbl.create_gnd_tbl(hk_cpy_tbl_file, "Table generated from #{json_hk_file}", @verbose)

         log_msg "Sucessfully created #{hk_cpy_tbl_file} at #{Time.now.to_s}\n"                 
         
         @log_file.close
         
      rescue Exception => e

         log_msg "Exception: TODO - Make more meaningful message" unless @log_file.nil?
         puts e.message
         puts e.backtrace
	   
      end
   end # initialize()
   
   def read_config_file(config_file)

      raise IOError "Configuration file #{config_file} does not exist" unless File.exist?(config_file)
      
      config_json = File.read(config_file)
      config = JSON.parse(config_json)
      
      tool_config = config[JSON_TBL_TOOL]
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_VERBOSE}\"} definition")       unless tool_config.key?(JSON_VERBOSE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_INPUT_PATH}\"} definition")    unless tool_config.key?(JSON_INPUT_PATH)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_OUTPUT_PATH}\"} definition")   unless tool_config.key?(JSON_OUTPUT_PATH)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_TEMPLATE_PATH}\"} definition") unless tool_config.key?(JSON_TEMPLATE_PATH)
  
      hk_config = tool_config[JSON_HK_OBJ]
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_HK_OBJ}\": {\"#{JSON_LOG_FILE}\"} definition")    unless hk_config.key?(JSON_HK_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_HK_OBJ}\": {\"#{JSON_LOG_FILE}\"} definition")    unless hk_config.key?(JSON_LOG_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_HK_OBJ}\": {\"#{JSON_HEADER_FILE}\"} definition") unless hk_config.key?(JSON_HEADER_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_HK_OBJ}\": {\"#{JSON_FOOTER_FILE}\"} definition") unless hk_config.key?(JSON_FOOTER_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_HK_OBJ}\": {\"#{JSON_OUTPUT_FILE}\"} definition") unless hk_config.key?(JSON_OUTPUT_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_HK_OBJ}\": {\"#{JSON_MAX_ENTRIES}\"} definition") unless hk_config.key?(JSON_MAX_ENTRIES)
     
      return config
      
   end # read_config_file()

end # class GenToTbl
