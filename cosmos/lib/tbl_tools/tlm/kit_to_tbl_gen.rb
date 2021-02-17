###############################################################################
# Telemetry Output Table Generation Tool
#
# Notes:
#   1. The ground database file is not imported. The spreadsheet that generates
#      the csv input file is assumed to have the correct APIDs. This tool 
#      asssumes APID 0 is defined and no spare table records are needed before
#      the first APID. It also assumes the APIDs are defined in increasing
#      numerical values. 
#   2. Only one CSV input file is defined to generate both TO and DS tables.
#      This is typically the case since the spreadsheet is used to analyze
#      both realtime (TO) and stored (DS) telemetry.
#   TODO - Break this into class files. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'csv'
require 'fileutils'
require 'json'


#
# Virtual base class for different keyword types. The keyword
# class manages the conversion between the CSV entry and a 
# string used to produce the table source file so child classes
# shoul override teh str() method
#          
class Keyword
  
   attr_accessor :col
   
   # Save last conversion information for error reporting
   attr_reader   :csv_str
   attr_reader   :tbl_str
   
   def initialize

      @col  = nil
      
      @csv_str = "Uninitialized"
      @tbl_str = "Uninitialized"
   
   end # initialize()

   # Default behavior is to copy CSV contents (i.e. a string) 
   # into the table and return it
   def str(csv_str)
      if csv_str.nil?
         @csv_str = "Spare"
         @tbl_str = "Spare"         
      else
         @csv_str = csv_str.strip
         @tbl_str = @csv_str
      end
   end
   
end # class Keyword

#
# Class to convert a CSV string keyword by using a lookup table
# to perform a string substitution 
#
class StringSubKeyword < Keyword
     
   def initialize(str_lookup, err_str)

      super()
      @str_lookup = str_lookup
      @err_str    = err_str
            
   end # initialize()

   def str(csv_str)
      return @str_lookup["NIL"] if csv_str.nil?
      if @str_lookup.has_key? csv_str.upcase
         return @str_lookup[csv_str.upcase]
      else
         return @err_str
      end
   end
   
end # class StringSubKeyword

#
# Class to convert a CSV numeric keyword to a string
#
class NumericKeyword < Keyword
     
   def initialize(err_str)

      super()
      @err_str  = err_str

   end # initialize()

   def str(csv_str)
      return "0" if csv_str.nil? 
      if (csv_str.to_i.to_s == csv_str) # Verify its an integer
         return csv_str
      else
         return @err_str
      end
   end # str()s
   
end # class NumericKeyword


#
# This class processes the Excel CSV Filter file. It views the CSV file as
# a matrix with each row containing keywords that can be translated into 
# a table's needs. By design this class is unaware of the FSW table syntax. 
#
# @keywords is a hash that uses the CSV KEYWORD_ROW keywords
# as the hash keys and the hash value are keyword objects. 
#
class CsvFilterFile

   attr_reader :filename
   attr_reader :keywords
   attr_reader :csv_file_line
   
   #
   # keyword hash keys
   #  - CSV file can defined multiple filters so they are not constants
   #  - "NIL" is not in the CSV file but is used as a hash key when the CSV
   #    entry is 'nil' object 
   #
   KEY_KEYWORD_ROW = "KEYWORD_ROW"
   KEY_APID_NAME   = "APID_NAME"
   KEY_APID        = "APID"
   KEY_PRIORITY    = "PRIORITY"
   KEY_RELIABILITY = "RELIABILITY"
   KEY_BUF_LIM     = "BUF_LIM"
     
   attr_reader :key_filter_type
   attr_reader :key_filter_n
   attr_reader :key_filter_x
   attr_reader :key_filter_o

   # See PktUtil_FilterType in osk_c_fw::pktutil.h
   FILTER_TYPE_LOOKUP = { "COUNT"  => "2",
                          "TIME"   => "3",
                          "N/A"    => "4",
                          "NIL"    => "4"}
                     
   
   def initialize(csv_file, id)

      @filename        = csv_file
      @key_filter_type = "F#{id}_TYPE"
      @key_filter_n    = "F#{id}_N"
      @key_filter_x    = "F#{id}_X"
      @key_filter_o    = "F#{id}_O"

      @keywords = { KEY_APID_NAME    => Keyword.new,
                    KEY_APID         => NumericKeyword.new("INVALID_APID"),
                    KEY_PRIORITY     => NumericKeyword.new("INVALID_PRIORITY"),
                    KEY_RELIABILITY  => NumericKeyword.new("INVALID_RELIABILITY"),
                    KEY_BUF_LIM      => NumericKeyword.new("INVALID_BUF_LIM"),
                    @key_filter_type => StringSubKeyword.new(FILTER_TYPE_LOOKUP,"INVALID_TYPE"),
                    @key_filter_n    => NumericKeyword.new("INVALID_N"),
                    @key_filter_x    => NumericKeyword.new("INVALID_X"),
                    @key_filter_o    => NumericKeyword.new("INVALID_O")}
                          
      @handler = open(csv_file)
      @csv_string = @handler.read.encode!("UTF-8", "iso-8859-1", invalid: :replace)
      @csv_file = csv_file
      @csv_file_line = 1
      @keyword_found = false

   end # def initialize()
 
   def process_rows(verbose)
   
      CSV.parse(@csv_string) do |row|
      
         puts "csv[#{@csv_file_line.to_s}] #{row}\n" if verbose
         @csv_file_line += 1

         if @keyword_found
            puts ">>>>>>>>>>  YIELDING ROW <<<<<<<<<<\n" if verbose
            yield(row)
         end
         if !row[0].nil?
            if (row[0].upcase == KEY_KEYWORD_ROW)
               set_keyword_columns(row)
               @keyword_found = true
               puts ">>>>>>>>>>  FOUND KEYWORD ROW <<<<<<<<<<\n" if verbose
            end
         end 

      end # CSV loop

      raise NameError, "CSV file does not contain a row marked with #{KEYWORD_ROW} that defines the positions of the keywords" unless @keyword_found
      
   end # process_rows()
   
   #
   # Process the CSV file KEYWORD row. The row is passed in as an array
   # of strings. Raise an exception if required keywords is not found
   #
   def set_keyword_columns(keyword_row)
   
      @keywords.each_key { |key|
         @keywords[key].col = keyword_row.index(key.to_s) 
         raise ArgumentError, "CSV file keyword row missing #{key.to_s}" if @keywords[key].col.nil?   
      }
      
   end # set_keyword_columns()

end # class CsvFilterFile


#
# This class creates a KIT_TO JSON Table file
#
class KitToFswTblFile

   def initialize(csv_filter_file, verbose)

      @verbose = verbose
      @csv_filter_file = csv_filter_file
      
      @first_pkt = true
      @curr_row  = 0
      @last_apid = -1
      
   end # initialize()


   # 
   # <JSON file header>
   # {
   #    "name": "Telemetry Output Test Table",
   #    "description": "Send 1 of 4. Event msg are needed for setup but are not part of the test and must be disable.",
   #    "packet-array": [
   #       ... 
   #       <Packet>
   #       ...
   # <JSON file footer>
   #    ]
   # }
   #
   def create_tbl(tbl_filename, header_file, footer_file, description)
   
      description << "File generated from #{@csv_filter_file.filename} at #{Time.now.to_s}"
      header_str = File.read(header_file).sub("@description@",description)
      footer_str = File.read(footer_file)
      
      File.open(tbl_filename, 'w') do |tbl_file|

         tbl_file << header_str
         
         @csv_filter_file.process_rows(@verbose) do |row|
           
            @curr_row += 1
            tbl_str = csv_row_to_tbl_str(row) 
            puts "tbl_str[#{@curr_row}]\n#{tbl_str}" if @verbose
            tbl_file << tbl_str unless tbl_str == ""

         end # process_rows
        
         tbl_file << footer_str

      end # tbl_file
   
   end # create_tbl()
   
   #
   # Create a string formatted for a JSON table
   #
   #  "packet": {
   #      "name": "CFE_ES_APP_TLM_MID",
   #      "stream-id": "\u080B",
   #      "dec-id": 2059,
   #      "priority": 0,
   #      "reliability": 0,
   #      "buf-limit": 4,
   #      "filter": { "type": 2, "X": 1, "N": 1, "O": 0}
   #   },
   #
   # The last packet entry can't have a comma
   #
   def csv_row_to_tbl_str(row)

      tbl_str = ""
      csv_apid = row[@csv_filter_file.keywords[CsvFilterFile::KEY_APID].col]
      return tbl_str if csv_apid.nil?

      csv_apid_name = row[@csv_filter_file.keywords[CsvFilterFile::KEY_APID_NAME].col]
      return tbl_str if csv_apid_name.nil?

      apid_name = @csv_filter_file.keywords[CsvFilterFile::KEY_APID_NAME].str(csv_apid_name)
      return tbl_str if apid_name == "Spare"

      # Complete the previous entry if not the first entry
      if (not @first_pkt)
         tbl_str << ",\n\n"
      end
      @first_pkt = false
      
      apid = @csv_filter_file.keywords[CsvFilterFile::KEY_APID].str(csv_apid)

      tbl_str << "      \"packet\": {\n"
      tbl_str << "      \"name\": \"#{@csv_filter_file.keywords[CsvFilterFile::KEY_APID_NAME].str(row[@csv_filter_file.keywords[CsvFilterFile::KEY_APID_NAME].col])}\",\n"
      tbl_str << "      \"stream-id\": \"\\u#{"0x%04X" % (apid.to_i | 0x800)}\",\n"
      tbl_str << "      \"dec-id\": #{"%d" % (apid.to_i | 0x800)},\n"
      tbl_str << "      \"priority\": #{@csv_filter_file.keywords[CsvFilterFile::KEY_PRIORITY].str(row[@csv_filter_file.keywords[CsvFilterFile::KEY_PRIORITY].col])},\n"
      tbl_str << "      \"reliability\": #{@csv_filter_file.keywords[CsvFilterFile::KEY_RELIABILITY].str(row[@csv_filter_file.keywords[CsvFilterFile::KEY_RELIABILITY].col])},\n"
      tbl_str << "      \"buf-limit\": #{@csv_filter_file.keywords[CsvFilterFile::KEY_BUF_LIM].str(row[@csv_filter_file.keywords[CsvFilterFile::KEY_BUF_LIM].col])},\n"
      tbl_str << "      \"filter\": { \"type\": #{@csv_filter_file.keywords[@csv_filter_file.key_filter_type].str(row[@csv_filter_file.keywords[@csv_filter_file.key_filter_type].col])}"
      tbl_str << "      , \"X\": #{@csv_filter_file.keywords[@csv_filter_file.key_filter_x].str(row[@csv_filter_file.keywords[@csv_filter_file.key_filter_x].col])}"
      tbl_str << "      , \"N\": #{@csv_filter_file.keywords[@csv_filter_file.key_filter_n].str(row[@csv_filter_file.keywords[@csv_filter_file.key_filter_n].col])}"
      tbl_str << "      , \"O\": #{@csv_filter_file.keywords[@csv_filter_file.key_filter_o].str(row[@csv_filter_file.keywords[@csv_filter_file.key_filter_o].col])}"
      tbl_str << "}\n   }"
 
      return tbl_str
      
   end # csv_row_to_tbl_str()

end # class KitToFswTblFile

# irb: test = GenToTbl.new("gen_kit_to_ds_tbl.json")
# irb: test = GenToTbl.new("simsat-gen_to_ds_tbl.json")

class KitToTblGen

   JSON_TBL_TOOL      = "tlm-tbl-tool"
   JSON_VERBOSE       = "verbose"
   JSON_INPUT_PATH    = "input-path"
   JSON_OUTPUT_PATH   = "output-path"
   JSON_TEMPLATE_PATH = "template-path"
   
   # Entries common to TO and DS objects
   JSON_CSV_FILE      = "csv-file"
   JSON_LOG_FILE      = "log-file"
   JSON_HEADER_FILE   = "header-file"
   JSON_FOOTER_FILE   = "footer-file"
   JSON_OUTPUT_FILE   = "output-file"
   JSON_FILES         = "files" 

   JSON_TO_OBJ        = "telemetry-output"
   JSON_MAX_APID      = "max-apid"
 
   JSON_DS_OBJ        = "data-storage"
   JSON_FLTR_ENTRY    = "filters-per-entry"
   
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
         
         tool_config = config[JSON_TBL_TOOL]
         to_config   = config[JSON_TBL_TOOL][JSON_TO_OBJ]
         ds_config   = config[JSON_TBL_TOOL][JSON_DS_OBJ]

         @verbose    = tool_config[JSON_VERBOSE]
         
         @log_filename = File.join(tool_config[JSON_OUTPUT_PATH],to_config[JSON_LOG_FILE])
         @log_file = open(@log_filename, "w")
         
         input_csv_file = File.join(tool_config[JSON_INPUT_PATH],to_config[JSON_CSV_FILE])
         log_msg "Generating tables from CSV #{input_csv_file}"
         log_msg "> Input directory:    #{tool_config[JSON_INPUT_PATH]}"
         log_msg "> Output directory:   #{tool_config[JSON_OUTPUT_PATH]}"
         log_msg "> Template directory: #{tool_config[JSON_TEMPLATE_PATH]}\n\n"

         files = to_config[JSON_FILES]
         files.each do |id, file_substr|
            
            log_msg "Processing table file #{file_substr} with ID #{id}"                 
            @csv_filter_file = CsvFilterFile.new(input_csv_file, id)

            header_file  = File.join(config[JSON_TBL_TOOL][JSON_TEMPLATE_PATH],to_config[JSON_HEADER_FILE])
            footer_file  = File.join(config[JSON_TBL_TOOL][JSON_TEMPLATE_PATH],to_config[JSON_FOOTER_FILE])

            tbl_filename = File.join(config[JSON_TBL_TOOL][JSON_OUTPUT_PATH],to_config[JSON_OUTPUT_FILE].sub("@file_substr@",file_substr))
            tbl_file = KitToFswTblFile.new(@csv_filter_file, @verbose)
            tbl_file.create_tbl(tbl_filename, header_file, footer_file, "Test using #{file_substr}")

            log_msg "Sucessfully created #{tbl_filename} at #{Time.now.to_s}\n"                 

         end # files loop
         
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
        
      to_config = tool_config[JSON_TO_OBJ]
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_CSV_FILE}\"} definition")    unless to_config.key?(JSON_CSV_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_LOG_FILE}\"} definition")    unless to_config.key?(JSON_LOG_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_HEADER_FILE}\"} definition") unless to_config.key?(JSON_HEADER_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_FOOTER_FILE}\"} definition") unless to_config.key?(JSON_FOOTER_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_OUTPUT_FILE}\"} definition") unless to_config.key?(JSON_OUTPUT_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_FILES}\"} definition")       unless to_config.key?(JSON_FILES)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_TO_OBJ}\": {\"#{JSON_MAX_APID}\"} definition")    unless to_config.key?(JSON_MAX_APID)
  
      ds_config = tool_config[JSON_DS_OBJ]  
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_DS_OBJ}\": {\"#{JSON_CSV_FILE}\"} definition")    unless ds_config.key?(JSON_CSV_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_DS_OBJ}\": {\"#{JSON_LOG_FILE}\"} definition")    unless ds_config.key?(JSON_LOG_FILE)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_DS_OBJ}\": {\"#{JSON_FILES}\"} definition")       unless ds_config.key?(JSON_FILES)
      raise NameError.new("Configuration file #{config_file} missing \"#{JSON_DS_OBJ}\": {\"#{JSON_FLTR_ENTRY}\"} definition")  unless ds_config.key?(JSON_FLTR_ENTRY)
   
      return config
      
   end # read_config_file()

end # class KitToTblGen
