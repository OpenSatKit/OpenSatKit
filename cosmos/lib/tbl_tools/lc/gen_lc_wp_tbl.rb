###############################################################################
# Telemetry Output Table Tool
#
# Notes:
#   1. The ground database file is not imported. The spreadsheet that generates
#      the csv input file is assumed to have the correct APIDs
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

#todo Resolve DFP situation
#todo Add config class
#todo Define constants group according to DB & LC macros
#todo initialize() parameters
#todo Is upcase needed? comp_value = comp_value.to_i.to_s(16).upcase.sub('X','x')
#todo Add date/time stamp to header
#todo log file
#todo Looks like ASIST DB doesn't set 0x800 bit. Add to 'ICD' assumptions

require 'csv'
require 'fileutils'
require 'json'

#
# This class creates a hash from a ground system telemetry CSV file. The CSV
# file does not have a header row and each row contains
#    MNEMONIC, TYPE, APID, OFFSET, MASK, DESCRIPTION
#          
class TlmCsv

   attr_reader :points

   def initialize(csv_file)   

      @points = Hash.new
      CSV.foreach(csv_file) do |row|
         @points["#{row[0].upcase}"] = {"TYPE" => row[1], "APID" => row[2], "OFFSET" => row[3], "MASK" => row[4], "DESCRIPTION" => row[5]}
      end
   end # initialize()
   
end # class TlmCsv


#
# This class processes the WP CSV file
#
class WatchPointCsv

   # Constants used in CSV file
   KEYWORD_ROW      = "KEYWORD_ROW"
   ID               = "ID"
   MNEMONIC         = "MNEMONIC"
   OPERATOR         = "OPERATOR"
   COMPARISON_VALUE = "COMPARISON_VALUE"
   COMMENTS         = "COMMENTS"
   AGE_STALE        = "AGE_STALE"
   CUSTOM_ARG       = "CUSTOM_ARG"
   
   # These are column indices into each CSV row
   attr_reader :id
   attr_reader :mnemonic
   attr_reader :operator
   attr_reader :comparison_value
   attr_reader :comments
   attr_reader :age_stale
   attr_reader :custom_arg

   def initialize(csv_file)

      puts "WatchPointCsv::Initialize(#{csv_file})"
      @handler = open(csv_file)
      @csv_string = @handler.read.encode!("UTF-8", "iso-8859-1", invalid: :replace)
      @csv_file = csv_file
      @csv_file_line = 1
      @keyword_found = false

   end # def initialize ()
 
   def process_rows
   
      CSV.parse(@csv_string) do |row|
      #CSV.foreach(@csv_file) do |row|
      
         puts @csv_file_line.to_s + "\n"
         @csv_file_line += 1

         if @keyword_found
            #yield(row) unless row[mnemonic].nil?
            yield(row)
         end
         if !row[0].nil?
            if row[0].upcase.include? KEYWORD_ROW
               set_keyword_columns(row)
               @keyword_found = true
               puts "**************** FOUND KEYWORD *******************\n"
            end
         end 

      end # CSV loop

      raise NameError, "Watch Point CSV file does not contain a row marked with #{KEYWORD_ROW} that defines the positions of the keywords" unless @keyword_found
      
   end # process_rows()
   

   #
   # Process the WP CSV file KEYWORD row. The row is passed in as an array
   # of strings. Raise an exception if required keywords is not found
   #
   def set_keyword_columns(keyword_row)

      @id = keyword_row.index(ID)
      raise ArgumentError, "Watch Point CSV file keyword row missing #{ID}" if @id.nil?

      @mnemonic = keyword_row.index(MNEMONIC)
      raise ArgumentError, "Watch Point CSV file keyword row missing #{MNEMONIC}" if @mnemonic.nil?  

      @operator = keyword_row.index(OPERATOR)
      raise ArgumentError, "Watch Point CSV file keyword row missing #{OPERATOR}" if @operator.nil?  

      @comparison_value = keyword_row.index(COMPARISON_VALUE)
      raise ArgumentError, "Watch Point CSV file keyword row missing #{COMPARISON_VALUE}" if @comparison_value.nil?  

      @comments = keyword_row.index(COMMENTS)
      raise ArgumentError, "Watch Point CSV file keyword row missing #{COMMENTS}" if @comments.nil?  

      # Optional entries so don't raise exceptions if undefined 
      # - Only the extended version of LC (LCX) includes the stale option
      @age_stale = keyword_row.index(AGE_STALE).nil?   ? 0 : keyword_row.index(AGE_STALE)
      @custom_arg = keyword_row.index(CUSTOM_ARG).nil? ? 0 : keyword_row.index(CUSTOM_ARG)
            
   end # set_keyword_columns()

   def has_stale?
      return (@age_stale != 0)
   end 	

   def has_custom_arg?
puts "has_customarg? @custom_arg = #{@custom_arg}\n"
      return (@custom_arg != 0)
   end 	

end # class WatchPointCsv

#
# This class contains the WP "business logic" used to construct
# the WP C table structure from the CSV file. Minor changes like
# an operator addition/removal would be local. Larger changes
# like the LCX stale feature will have broader impacts.
#
class WatchPoint

   #
   #   get_datatype returns a tuple containing the mnemonic datatype and the 
   #   comparison value types needed to cast the value properly. Some of the types
   #   include the endianess and some don't. For the ones that don't, the lc_config
   #   parameter is used to determine it.
   #
   def get_datatype(dt, endian)
       
      if dt == "UB"
         return ["LC_DATA_UBYTE","Unsigned8in32.Unsigned8"]
      elsif dt == "B"
         return ["LC_DATA_BYTE","Signed8in32.Signed8"]
      elsif dt == "BE_SI" or ((dt == "SI") and (endian == "BIG"))
         return ["LC_DATA_WORD_BE","Signed16in32.Signed16"]
      elsif dt == "LE_SI" or ((dt == "SI") and (endian == "LITTLE"))
         return ["LC_DATA_WORD_LE","Signed16in32.Signed16"]
      elsif dt == "BE_UI" or ((dt == "UI") and (endian == "BIG"))
         return ["LC_DATA_UWORD_BE","Unsigned16in32.Unsigned16"]
      elsif dt == "LE_UI" or ((dt == "UI") and (endian == "LITTLE"))
         return ["LC_DATA_UWORD_LE","Unsigned16in32.Unsigned16"]  
      elsif dt == "BE_SLI" or ((dt == "SLI") and (endian == "BIG"))
        return ["LC_DATA_DWORD_BE","Signed32"]
      elsif dt == "LE_SLI" or ((dt == "SLI") and (endian == "LITTLE"))
         return ["LC_DATA_DWORD_LE","Signed32"]
      elsif dt == "BE_ULI" or ((dt == "ULI") and (endian == "BIG"))
         return ["LC_DATA_UDWORD_BE","Unsigned32"]
      elsif dt == "LE_ULI" or ((dt == "ULI") and (endian == "LITTLE"))
         return ["LC_DATA_UDWORD_LE","Unsigned32"]
      elsif dt == "BE_SFP" or ((dt == "SFP") and (endian == "BIG"))
         return ["LC_DATA_FLOAT_BE","Float32"]
      elsif dt == "LE_SFP" or ((dt == "SFP") and (endian == "LITTLE"))
         return ["LC_DATA_FLOAT_LE","Float32"]     
      #todo elsif dt == "DFP" and IGNORE_NICER_DFP_ISSUE    # treat as single precision float
      #todo   return ["LC_DATA_FLOAT_BE","Float32"]       
      else
         raise TypeError, "Invalid data type #{dt} - Either unknown or inconsistent with endianess #{endian}"
      end

   end # get_data_type()
   
   #
   #   get_operator converts the operator as defined in the WP CSV file into the 
   #   the operator syntax needed for the WP table.
   #
   def get_operator(op)
puts "get_operator(#{op})\n"
      if op == "LT"
         return "LC_OPER_LT"
      elsif op == "LE"
         return "LC_OPER_LE"
      elsif op == "GT"
         return "LC_OPER_GT"
      elsif op == "GE"
         return "LC_OPER_GE"
      elsif op == "EQ"
         return "LC_OPER_EQ"
      elsif op == "NE"
         return "LC_OPER_NE"
      elsif op == "NO OPER"
         return "LC_NO_OPER"
      elsif op == "CUSTOM"
         return "LC_OPER_CUSTOM"            
      else
         raise TypeError, "Invalid operator #{op}"
      end

   end # get_operator()
             
   #
   # Shift the comparison value if the bit mask contains zeroes in the 
   # least significant bit locations. Shift 1 bit for each zero
   # and stop shifting when a 1 is found in the mask
   #
        
   def shift_comparison_value(comp_val, mask)
puts "shift_comparison_value comp_val=#{comp_val}, mask=#{mask}\n"
      bit_pos = 1
    
      if (mask != 0 and mask != 4294967295)
         while (mask & bit_pos) == 0
            comp_val = comp_val << 1
            bit_pos = bit_pos << 1
         end
      end
      
      return comp_val.to_s
      
   end  # shift_comparison_value()
   
   # 
   # Numeris test methods
   #   
   def is_hex?(num_str)
puts "is_hex?(#{num_str})\n"
      (num_str.downcase =~ /0x[0-9a-f]+$/) == nil ? false : true
   end 

   def is_dec?(num_str)
puts "is_dec?(#{num_str})\n"
      (num_str =~ /^[-+]?[0-9]+$/) == nil ? false : true
   end

   def is_float?(num_str)
puts "is_float?(#{num_str})\n"
      Float(num_str) rescue false
   end
          
   #
   # Create a string formatted for C table
   #   
   def csv_row_to_tbl_str(row, wp_csv, tlm_points, endian)

      tbl_str = ""

      # 
      # The naming convention is to assign the 'raw' CSV string to a
      # variable appended with "_csv" and then create a new string variable
      # appended with "_str" that is used to create the C file. If no processing
      # is required on a raw value then it is assigned to a "_str" variable    
      #
      # CSV returns nil for blank entries
      #
      mnemonic_str         = row[wp_csv.mnemonic]
      id_csv               = row[wp_csv.id]
      operator_str         = row[wp_csv.operator]
      comparison_value_csv = row[wp_csv.comparison_value]      
      custom_arg_csv       = row[wp_csv.custom_arg] unless wp_csv.custom_arg.nil?
      age_stale_str        = row[wp_csv.age_stale]  unless wp_csv.age_stale.nil?
      
      # Remove leading zeros to maintain compatibility with previous 
      # NASA/GSFC tool and use ID of zero if first 
      if (!id_csv.nil?)
         id_str = id_csv.sub(/^[0:]*/,"")  
         id_str = "0" if id_str.strip == ""
      end
puts "id_str=#{id_str}\n"
      if (id_csv.nil?)   # Skip text only and blank lines
puts ">>> Skipping text only and blank lines\n"      
         tbl_str = ""  
      
      elsif (mnemonic_str.nil? or operator_str == "NO OPER")  # No watchpoint in this row, output unused WP text and comment
puts ">>> Unused WP: Nil mnemonic or NO_OPER\n"
         tbl_str <<  "    /*  #{id_str} (UNUSED - ) */\n"         
         tbl_str <<  "    { .DataType = LC_WATCH_NOT_USED, .OperatorID = LC_NO_OPER, .MessageID = 0, .WatchpointOffset = 0, .BitMask = LC_NO_BITMASK, .ComparisonValue.Unsigned32 = 0,"
         
         if wp_csv.has_stale?
            tbl_str <<  " .ResultAgeWhenStale = 0,"
         end

         tbl_str <<  " .CustomFuncArgument = 0 }, \n\n"
         
      else   # Output the watchpoint
puts ">>> Processing WP: id_str=#{id_str}, mnemonic_str=#{mnemonic_str}, operator_str=#{operator_str}, comparison_value_csv=#{comparison_value_csv}\n"
         
         begin
            
            mnemonic_str = mnemonic_str.upcase.strip
            raise NameError, "Watch Point mnemonic #{mnemonic_str} is missing from telemetry CSV file" unless tlm_points.key?(mnemonic_str) 
            
            tlm_mnemonic_int_mask = tlm_points[mnemonic_str]['MASK'].to_i
puts "GH1\n"
            type_tuple = get_datatype(tlm_points[mnemonic_str]['TYPE'], endian) # type_tuple = [tlm type, cast type] needed to generate the code
puts "GH2\n"

            # Determine bitmask first since it can either be a hex value or "LC_NO_BITMASK" if the value is 0xFFFFFFFF
            if tlm_mnemonic_int_mask == 4294967295 # 0xFFFFFFFF
               bitmask = "LC_NO_BITMASK"
            else
               bitmask = "0x%X" % tlm_mnemonic_int_mask  # Make output match previous tool's output
            end
puts "GH3: wp_csv.custom_arg = #{wp_csv.custom_arg}\n"
            
            # The WP CSV file custom argument column is optional. 
            # A custom_arg of 0 indicates not present
       
#todo            arg_value = (wp_csv.custom_arg == 0 or row[wp_csv.custom_arg] == "") ? "0" : row[wp_csv.custom_arg]
            if wp_csv.has_custom_arg?
               custom_arg_str = (row[wp_csv.custom_arg].nil? or row[wp_csv.custom_arg] == "") ? "0" : row[wp_csv.custom_arg]
            else
               custom_arg_str = "0"
            end
            # hex and dec checks are done first so a decimal is not mistakenly 
            # identified as a float
puts "tlm_mnemonic_int_mask=#{tlm_mnemonic_int_mask}, type_tuple=#{type_tuple}, bitmask=#{bitmask}, custom_arg_str=#{custom_arg_str}\n" 
            if (comparison_value_csv.nil? or comparison_value_csv == "") 
puts "Comparison empty string\n"
               # If there's a non all one mask, then convert to hex for compatibility with previous tool
               comparison_value_str = (tlm_mnemonic_int_mask == 4294967295) ? "0" : "0x0"            
            elsif is_hex? comparison_value_csv
puts "Comparison hex\n"
               comparison_value_str = shift_comparison_value(comparison_value_csv.to_i(16), tlm_mnemonic_int_mask)
               comparison_value_str = "0x%X" % comparison_value_str 
            elsif is_dec? comparison_value_csv
puts "Comparison dec\n"
               comparison_value_str = shift_comparison_value(comparison_value_csv.to_i, tlm_mnemonic_int_mask)
               if tlm_mnemonic_int_mask != 4294967295    # If there's a non all one mask, then convert to hex for compatibility with previous tool
                  comparison_value_str = "0x%X" % comparison_value_str 
               end
            
            elsif (is_float? comparison_value_csv)
puts "Comparison float\n"
               # It's possible that mnemonic type is not float. 
               # If it isn't truncate and store as hex

               if (type_tuple[1] != "Float32")
                  
                  truncated_int = comparison_value_csv.to_f.to_i
                  
                  comparison_value_str = shift_comparison_value(truncated_int, tlm_mnemonic_int_mask)
                  if (tlm_mnemonic_int_mask != 4294967295)    # If there's a non all one mask, then convert to hex for compatibility with previous tool
                     comparison_value_str = "0x%X" % comparison_value_str 
                  end
               else
                  comparison_value_str = comparison_value_csv
               end

            else # The comparison value is undefined.

               raise NameError, "Watch Point contains undefined comparison value #{comparison_value_csv}"

            end # Compare value                    
puts"About to create tbl_str\n"
            # Comment followed by the source code. Extended version
            # of LC (LCX) includes the stale feature
            
            tbl_str <<  "    /*  #{id_str} (#{mnemonic_str} - #{tlm_points[mnemonic_str]['DESCRIPTION']})  */\n"

            tbl_str <<  "    { .DataType = #{type_tuple[0]}," +
                             " .OperatorID = #{get_operator(operator_str)}," +
                             " .MessageID = #{("0x%X" % (tlm_points[mnemonic_str]['APID'].to_i | 0x800))}," +
                             " .WatchpointOffset = #{tlm_points[mnemonic_str]['OFFSET']}," +
                             " .BitMask = #{bitmask}," + 
                             " .ComparisonValue.#{type_tuple[1]} = #{comparison_value_str},"

            if wp_csv.has_stale?
               tbl_str <<  " .ResultAgeWhenStale = #{age_stale_str},"
            end
            
            tbl_str <<  " .CustomFuncArgument = #{custom_arg_str}},\n\n"
               
         rescue
            raise NameError, "Watch Point processing error for mnemonic #{mnemonic_str}, id #{id_str}"
         end
   
      end # Create watch string
      
puts tbl_str + "\n"      
      return tbl_str
      
   end # csv_row_to_tbl_str()

end # class WatchPoint

#class WatchPointGen
class Test

   JSON_LIMIT_CHECKER   = "limit-checker"
   JSON_LC_ENDIAN       = "endian"
   JSON_LC_OUPUT_PATH   = "output-path"
   JSON_LC_TLM_CSV_FILE = "tlm_csv_file"
   
   JSON_WATCH_POINT     = "watch-point"
   JSON_WP_COUNT        = "count"
   JSON_WP_TBL_FILE     = "tbl-file"
   JSON_WP_LOG_FILE     = "log-file"
   JSON_WP_HEADER_FILE  = "header-file"
   JSON_WP_FOOTER_FILE  = "footer-file"
   
   def initialize
      begin

         config    = read_config_file("cllc_config.json")    
         lc_config = config[JSON_LIMIT_CHECKER]      
         wp_config = config[JSON_LIMIT_CHECKER][JSON_WATCH_POINT]      

         tlm_csv = TlmCsv.new (lc_config[JSON_LC_TLM_CSV_FILE])
     
         watch_point = WatchPoint.new
         watch_point_csv = WatchPointCsv.new ("GPM-MSE-SPEC-0116_Fault_Management_Document_Appendix_B - WP.csv")

         FileUtils.cp wp_config[JSON_WP_HEADER_FILE], wp_config[JSON_WP_TBL_FILE]

         File.open(wp_config[JSON_WP_TBL_FILE], 'a') do |wp_file|

            watch_point_csv.process_rows do |row|

               tbl_str = watch_point.csv_row_to_tbl_str(row, watch_point_csv, tlm_csv.points, lc_config[JSON_LC_ENDIAN].upcase) 
               wp_file << tbl_str unless tbl_str == ""

            end # watch_point_csv.process_rows

            File.open(wp_config[JSON_WP_FOOTER_FILE]).each do |footer_line|
               wp_file << footer_line
            end # JSON_WP_FOOTER_FILE

         end # JSON_WP_TBL_FILE]

      rescue Exception => e

         puts e.message
         puts e.backtrace.inpsect
	   
      end
   end # initialize()
   
   def read_config_file(config_file)

      raise IOError "Configuration file #{config_file} does not exist" unless File.exist?(config_file)
      
      config_json = File.read(config_file)
      config = JSON.parse(config_json)
      lc_config = config[JSON_LIMIT_CHECKER]      

      raise NameError "Configuration file #{config_file} missing #{JSON_LIMIT_CHECKER} => #{JSON_LC_ENDIAN} definition"       unless lc_config.key?(JSON_LC_ENDIAN)
      raise NameError "Configuration file #{config_file} missing #{JSON_LIMIT_CHECKER} => #{JSON_LC_OUPUT_PATH} definition"   unless lc_config.key?(JSON_LC_OUPUT_PATH)
      raise NameError "Configuration file #{config_file} missing #{JSON_LIMIT_CHECKER} => #{JSON_LC_TLM_CSV_FILE} definition" unless lc_config.key?(JSON_LC_TLM_CSV_FILE)

      wp_config = config[JSON_LIMIT_CHECKER][JSON_WATCH_POINT]      
      raise NameError "Configuration file #{config_file} missing #{JSON_WATCH_POINT} => #{JSON_WP_COUNT} definition"    unless wp_config.key?(JSON_WP_COUNT)
      raise NameError "Configuration file #{config_file} missing #{JSON_WATCH_POINT} => #{JSON_WP_TBL_FILE} definition" unless wp_config.key?(JSON_WP_TBL_FILE)
      raise NameError "Configuration file #{config_file} missing #{JSON_WATCH_POINT} => #{JSON_WP_LOG_FILE} definition" unless wp_config.key?(JSON_WP_LOG_FILE)
      raise NameError "Configuration file #{config_file} missing #{JSON_WATCH_POINT} => #{JSON_WP_HEADER_FILE} definition" unless wp_config.key?(JSON_WP_HEADER_FILE)
      raise NameError "Configuration file #{config_file} missing #{JSON_WATCH_POINT} => #{JSON_WP_FOOTER_FILE} definition" unless wp_config.key?(JSON_WP_FOOTER_FILE)

      return config
      
   end # read_config_file()

end # class WatchPointGen
