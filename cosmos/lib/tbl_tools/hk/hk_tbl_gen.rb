###############################################################################
# Housekeeping Table Generation Tool
#
# Notes:
#   1. This tool reads a user supplied JSON file that defines the HK combo
#      packet content. The FSW message ID macro names are verified using
#      fsw_msg_id.rb and the packet/mnemonic names are verified using the 
#      COSMOS System.telemetry.packet definitions
#   2. Currently the JSON definition file is manually created. This isn't
#      very helpful because the developer has to look up each telemetry item
#      they want in a combo packet so they might as well just figure out
#      the offsets as they go. Ideally a GUI that allows a user to graphically
#      browse telemetry packets and drag/drop them into a combo packet
#      container would be most helpful. Alos a developer usually optimizes a
#      table by copying continguous blocks when possible rather than creating
#      a separate table entry for each telemetry item.
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


#
# JSON definition file
#
# Knowledge of JSON file structure should be retained in
# this class and not required by users
#
class HkJsonDefFile

   # These constants are common to the JSON and FSW definitions
   HK_COMBO_PKT1_MID_STR = "HK_COMBINED_PKT1_MID"
   HK_COMBO_PKT2_MID_STR = "HK_COMBINED_PKT2_MID"
   HK_COMBO_PKT3_MID_STR = "HK_COMBINED_PKT3_MID"

   HK_COMBO_PKT_MID_STR  = [HK_COMBO_PKT1_MID_STR, HK_COMBO_PKT2_MID_STR, HK_COMBO_PKT3_MID_STR]
   
   attr_reader :filename
   
   def initialize(hk_json_def_filename, verbose)
   
      @verbose = verbose
      raise IOError.new("JSON definition file #{hk_json_def_filename} does not exist") unless File.exist?(hk_json_def_filename)
      
      @filename         = hk_json_def_filename
      @hk_json_def_txt  = File.read(@filename)
      @hk_json_def_hash = JSON.parse(@hk_json_def_txt)
      @hk_json_packets  = @hk_json_def_hash["packets"]

      puts "hk_json_def_hash = #{@hk_json_def_hash}\n" if @verbose
       
   end # initialize()


   def process_combo_pkt(combo_pkt_mid)
   
      @hk_json_packets.each do |packet|
      
         if combo_pkt_mid.eql?(packet["mid"])
            puts ">>>>>>>>>>  YIELDING PACKET #{packet["mid"]} <<<<<<<<<<\n" if @verbose
            yield(packet["mid"],packet["src-data"])
         end            
         
      end # Packet loop
   
   end # process_combo_pkt()
   
  
   # Translate the JSON scr-data object into an array of data (see return statement)
   # JSON tlm-item object example: "tlm-item": "CFE_ES HK_TLM_PKT CFE_CORE_CHECKSUM"
   #
   def get_src_tlm_item(src_data_obj)
      
      item_name   = nil
      byte_offset = 0
      num_bytes   = 0
      data_type   = nil
      
      tbl_target   = src_data_obj["tlm-item"].split
      puts "get_src_tlm_item(#{src_data_obj}): tbl_target = #{tbl_target}\n" if @verbose
      
      tbl_tlm_item = tbl_target[2]
      tlm_pkt_items = Cosmos::System.telemetry.items(tbl_target[0], tbl_target[1]) # Return PacketItem Array
      tlm_pkt_items.each do |item|
         if (item.name.eql?(tbl_tlm_item))
            puts "Matched #{item.name}: bit_size=#{item.bit_size}, bit_offset=#{item.bit_offset}, data_type=#{item.data_type}\n" if @verbose
            item_name   = tbl_tlm_item
            byte_offset = item.bit_offset/8
            num_bytes   = item.bit_size/8
            data_type   = item.data_type
         end
      end

      return [src_data_obj["tlm-mid"], item_name, byte_offset, num_bytes, data_type]
      
   end # get_src_tlm_item()

end # class HkJsonDefFile


#
# Manage the creation of the FSW HK copy table
#
class HkCfsCpyTbl

   def initialize(hk_json_def_file, description, verbose)

      @hk_json_def_file = hk_json_def_file
      @description      = description
      @verbose          = verbose

      # Contains each combo packet's byte offset as the packet items are added
      @combo_offset   = {}
      HkJsonDefFile::HK_COMBO_PKT_MID_STR.each do |combo_pkt|
         @combo_offset[combo_pkt]   = 12     # 12th byte is the first packet payload data byte after the CCSDS header
      end
            
   end # initialize()

   # 
   # <C table file header>
   #
   # FSW table entry string formats:
   #   /*  inputMid        inputOffset     outputMid      outputOffset  numBytes*/
   #   { CFE_ES_HK_TLM_MID,    12,   HK_COMBINED_PKT1_MID,      12,      4, },
   #   { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,         0,      0, },
   #
   # <C table file footer>
   #
   def create_tbl(fsw_cpy_tbl_filename, header_file, footer_file)

      cpy_tbl_entry_cnt = 0
      header_str = File.read(header_file).sub("@src-file@",@hk_json_def_file.filename)
      footer_str = File.read(footer_file)
          
      File.open(fsw_cpy_tbl_filename, 'w') do |fsw_cpy_tbl_file|

         fsw_cpy_tbl_file << header_str.sub("@date-time@",Time.now.to_s)
         
         HkJsonDefFile::HK_COMBO_PKT_MID_STR.each do |combo_pkt|
            
            @hk_json_def_file.process_combo_pkt(combo_pkt) do |hk_combo_mid_str, src_data_array|
               
               hk_combo_mid_val = Fsw::MsgId.get_val(hk_combo_mid_str)
               puts "hk_combo_mid_str = #{hk_combo_mid_str}, mid_val = #{hk_combo_mid_val}\n" if @verbose
               
               raise IndexError.new("JSON definition file contains #{src_data_array.length} that is greater than HK's max entry configuration #{FswConfigParam::HK_COPY_TABLE_ENTRIES}\n")   unless src_data_array.length <= FswConfigParam::HK_COPY_TABLE_ENTRIES
               src_data_array.each do |src_data|
               
                  src_item = @hk_json_def_file.get_src_tlm_item(src_data) # Index: 0=MID, 1=Name, 2=Offset, 3=Size, 4=Data Type
                  puts "src_item = #{src_item}\n" if @verbose
                  
                  if (src_item[1] != nil and src_item[2] != 0 and src_item[3] != 0)
                     comment_str = "  /* #{"%03d"%[cpy_tbl_entry_cnt]}: #{"%-90s"%[src_data]} */"
                     fsw_cpy_tbl_file << "{ #{Fsw::MsgId.get_val(src_item[0])}, #{"%3d" % [src_item[2]]}, #{hk_combo_mid_val}, #{"%3d" % [@combo_offset[hk_combo_mid_str]]}, #{"%3d" % [src_item[3]]} }, #{comment_str}\n" 
                     @combo_offset[hk_combo_mid_str] += src_item[3]
                     cpy_tbl_entry_cnt += 1
                  else
                     raise IndexError.new("JSON definition file contains an ivalid entry #{src_item}\n")
                  end
               
               end # src_data loop
            end # Combo pkt tlm item loop
         end # JSON combo pkt loop

         # Populate unused entries          
         while cpy_tbl_entry_cnt < FswConfigParam::HK_COPY_TABLE_ENTRIES
            comment_str = "  /* #{"%03d"%[cpy_tbl_entry_cnt]}: Unused entry */"
            fsw_cpy_tbl_file << "{ HK_UNDEFINED_ENTRY, 0, HK_UNDEFINED_ENTRY, 0, 0 }, #{comment_str}\n"
            cpy_tbl_entry_cnt += 1
         end
         
         fsw_cpy_tbl_file << footer_str

      end # fsw_cpy_tbl_file
   
   end # create_fsw_tbl()

end # class HkCfsCpyTbl
   

#
# Manage the creation of the COSMOS HK combo  telemetry definition files
#
class HkCosmosTlmDef

   def initialize(hk_json_def_file, description, verbose)

      @hk_json_def_file = hk_json_def_file
      @description      = description
      @verbose          = verbose
      
      # Contains each combo packet's byte offset as the packet items are added
      @combo_pkt_str = {}
      HkJsonDefFile::HK_COMBO_PKT_MID_STR.each do |combo_pkt|
         @combo_pkt_str[combo_pkt] = ""
      end
            
   end # initialize()

   # 
   # There's only one template file for creating the telemetry definition file as 
   # opposed to a header and a footer file. The template has template parameter
   # @combo_pkt_1_data@ for each combo packet that needs to be loaded
   #
   # The code needs to build a string for each combo packet that that can be
   # substituted
   #   
   # COSMOS telemetry item format
   #    APPEND_ITEM CMD_VALID_COUNT     8 UINT "The ES Application Command Counter."
   #
   def create_def_file(tlm_def_filename, tlm_def_template_file)

      #
      # Create combo packet substitution strings 
      #
      HkJsonDefFile::HK_COMBO_PKT_MID_STR.each do |combo_pkt|
         
         @hk_json_def_file.process_combo_pkt(combo_pkt) do |hk_combo_mid_str, src_data_array|
                              
            src_data_array.each do |src_data|
               
               src_item = @hk_json_def_file.get_src_tlm_item(src_data) # Index: 0=MID, 1=Name, 2=Offset, 3=Size, 4=Data Type
                  
               if (src_item[1] != nil and src_item[2] != 0 and src_item[3] != 0)
                  @combo_pkt_str[combo_pkt] <<  "  APPEND_ITEM #{src_item[1]} #{src_item[3]*8} #{src_item[4]}\n"  
               else
                  raise IndexError.new("JSON definition file contains an ivalid entry #{src_item}\n")
               end
               
            end # src_data loop
         end # Combo pkt tlm item loop
         
      end # JSON combo pkt loop
   
      tlm_def_str = File.read(tlm_def_template_file)
      
      HkJsonDefFile::HK_COMBO_PKT_MID_STR.each do |combo_pkt|
         tlm_def_str = tlm_def_str.sub("@#{combo_pkt}@",@combo_pkt_str[combo_pkt])
      end
      tlm_def_str = tlm_def_str.sub("@src-file@",@hk_json_def_file.filename)
      
      File.open(tlm_def_filename, 'w') do |tlm_def_file|
         tlm_def_file << tlm_def_str.sub("@date-time@",Time.now.to_s)
      end # tlm_def_file
      
   end # create_def_file()

end # class HkCosmosTlmDef



# irb: test = GenToTbl.new("gen_kit_to_ds_tbl.json")
# irb: test = GenToTbl.new("simsat-gen_to_ds_tbl.json")

class HkTblGen

   # JSON Table Tool Object
   JSON_TBL_TOOL      = "hk-tbl-tool"
   JSON_VERBOSE       = "verbose"
   JSON_INPUT_PATH    = "input-path"
   JSON_OUTPUT_PATH   = "output-path"
   JSON_TEMPLATE_PATH = "template-path"
   JSON_LOG_FILE      = "log-file"
   
   # JSON cFS Object
   JSON_CFS_OBJ         = "cfs"
   JSON_CFS_HK_DEF_FILE = "json-hk-file"
   JSON_CFS_HEADER_FILE = "header-file"
   JSON_CFS_FOOTER_FILE = "footer-file"
   JSON_CFS_OUTPUT_FILE = "output-file"
   
   # JSON COSMOS Object
   JSON_COSMOS_OBJ           = "cosmos"
   JSON_COSMOS_TEMPLATE_FILE = "template-file"
   JSON_COSMOS_OUTPUT_FILE   = "output-file"

   @config_file
   @verbose
   @log_file
   
   def log_msg(msg)
   
      @log_file.write("#{msg}\n")
      puts "[log_msg] #{msg}\n" if @verbose
      
   end # log_msg()
   
   def initialize(config_file)
      begin
         
         @config_file = config_file
         config = read_config_file(config_file)
         
         tool_config   = config[JSON_TBL_TOOL]
         cfs_config    = config[JSON_TBL_TOOL][JSON_CFS_OBJ]
         cosmos_config = config[JSON_TBL_TOOL][JSON_COSMOS_OBJ]

         @verbose    = tool_config[JSON_VERBOSE]
         
         @log_filename = File.join(tool_config[JSON_OUTPUT_PATH],tool_config[JSON_LOG_FILE])
         @log_file = open(@log_filename, "w")
         
         log_msg "Preparing HK copy table generation using configuration file #{config_file}"
         log_msg "> Input directory:    #{tool_config[JSON_INPUT_PATH]}"
         log_msg "> Output directory:   #{tool_config[JSON_OUTPUT_PATH]}"
         log_msg "> Template directory: #{tool_config[JSON_TEMPLATE_PATH]}\n\n"

         #
         # Process JSON definition file
         #

         json_hk_file = File.join(tool_config[JSON_INPUT_PATH],    cfs_config[JSON_CFS_HK_DEF_FILE])
         hk_json_def  = HkJsonDefFile.new(json_hk_file, @verbose)

         #
         # Create cFS files
         #
         
         header_file     = File.join(tool_config[JSON_TEMPLATE_PATH], cfs_config[JSON_CFS_HEADER_FILE])
         footer_file     = File.join(tool_config[JSON_TEMPLATE_PATH], cfs_config[JSON_CFS_FOOTER_FILE])
         hk_cpy_tbl_file = File.join(tool_config[JSON_OUTPUT_PATH],   cfs_config[JSON_CFS_OUTPUT_FILE])

         log_msg "Generating cFS HK copy table using"
         log_msg "> Tbl def file: #{json_hk_file}"
         log_msg "> Header file:  #{header_file}"
         log_msg "> Footer file:  #{footer_file}"
         
         hk_cpy_tbl = HkCfsCpyTbl.new(hk_json_def, "Table generated from #{json_hk_file}", @verbose)
         hk_cpy_tbl.create_tbl(hk_cpy_tbl_file, header_file, footer_file)
         
         log_msg "Sucessfully created #{hk_cpy_tbl_file} at #{Time.now.to_s}\n"                 

         #
         # Create COSMOS files
         #

         cosmos_template_file = File.join(tool_config[JSON_TEMPLATE_PATH], cosmos_config[JSON_COSMOS_TEMPLATE_FILE])
         hk_tlm_def_file      = File.join(tool_config[JSON_OUTPUT_PATH],   cosmos_config[JSON_COSMOS_OUTPUT_FILE])

         log_msg "\nGenerating COSMOS HK telemetry definition file using"
         log_msg "> Tbl def file:  #{json_hk_file}"
         log_msg "> Template file: #{header_file}"
         
         hk_cosmos_def = HkCosmosTlmDef.new(hk_json_def, "Table generated from #{json_hk_file}", @verbose)
         hk_cosmos_def.create_def_file(hk_tlm_def_file, cosmos_template_file)
         
         log_msg "Sucessfully created #{hk_tlm_def_file} at #{Time.now.to_s}\n"                 

         @log_file.close
         
      rescue Exception => e

         if not @log_file.nil?
            log_msg "Exception while processing #{@config_file}" 
            log_msg "#{e.message}\n"
            log_msg "#{e.backtrace}\n"         
         end
         puts "Exception while processing #{@config_file}" 
         puts e.message
         puts e.backtrace
         
      end  # Rescue
   end # initialize()
   
   def read_config_file(config_file)

      raise IOError.new("Configuration file #{config_file} does not exist") unless File.exist?(config_file)
      
      config_json = File.read(config_file)
      config = JSON.parse(config_json)
      
      tool_config = config[JSON_TBL_TOOL]
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_VERBOSE}\"} definition")       unless tool_config.key?(JSON_VERBOSE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_INPUT_PATH}\"} definition")    unless tool_config.key?(JSON_INPUT_PATH)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_OUTPUT_PATH}\"} definition")   unless tool_config.key?(JSON_OUTPUT_PATH)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TBL_TOOL}\": {\"#{JSON_TEMPLATE_PATH}\"} definition") unless tool_config.key?(JSON_TEMPLATE_PATH)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_LOG_OBJ}\":  {\"#{JSON_LOG_FILE}\"} definition")      unless tool_config.key?(JSON_LOG_FILE)
  
      cfs_config = tool_config[JSON_CFS_OBJ]
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_FSW_OBJ}\": {\"#{JSON_CFS_HK_DEF_FILE}\"} definition") unless cfs_config.key?(JSON_CFS_HK_DEF_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_FSW_OBJ}\": {\"#{JSON_CFS_HEADER_FILE}\"} definition") unless cfs_config.key?(JSON_CFS_HEADER_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_FSW_OBJ}\": {\"#{JSON_CFS_FOOTER_FILE}\"} definition") unless cfs_config.key?(JSON_CFS_FOOTER_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_FSW_OBJ}\": {\"#{JSON_CFS_OUTPUT_FILE}\"} definition") unless cfs_config.key?(JSON_CFS_OUTPUT_FILE)
      
      cosmos_config = tool_config[JSON_COSMOS_OBJ]
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_COSMOS_OBJ}\": {\"#{JSON_COSMOS_TEMPLATE_FILE}\"} definition") unless cosmos_config.key?(JSON_COSMOS_TEMPLATE_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_COSMOS_OBJ}\": {\"#{JSON_COSMOS_OUTPUT_FILE}\"} definition")   unless cosmos_config.key?(JSON_COSMOS_OUTPUT_FILE)

      return config
      
   end # read_config_file()

end # class HkTblGen
