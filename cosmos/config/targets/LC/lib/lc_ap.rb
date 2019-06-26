#todo Resolve DFP situation
#todo Add config class
#todo Define constants group according to DB & LC macros
#todo initialize() parameters
#todo Is upcase needed? comp_value = comp_value.to_i.to_s(16).upcase.sub('X','x')
#todo Add date/time stamp to header
#todo log file
#todo protect against max APs

require 'csv'
require 'fileutils'
require 'json'

#
# This class creates a hash from telemetry CSV file. The CSV file does not
# have a headr row and each row contains
#    MNEMONIC, TYPE, APID, OFFSET, MASK, DESCRIPTION
#          
class TlmCsv

   attr_reader :points

   def initialize(csv_file)   

      @points = Hash.new
      CSV.foreach(csv_file) do |row|
         @points[:row[0].upcase] = {'TYPE':row[1], 'APID':row[2], 'OFFSET':row[3], 'MASK':row[4], 'DESCRIPTION':row[5]}
      end
	  
   end # initialize()
   
end # class TlmCsv

#
# This class processes the AP CSV file
#
class ActionPointCsv

   # Constant used in CVS file
   KEYWORD_ROW      = "KEYWORD_ROW"
   ID               = "ID"
   DESCRIPTION      = "DESCRIPTION"
   STATE            = "STATE"
   RTS_ID           = "RTS_ID"
   MAX_FAILS        = "MAX_FAILS"
   EVENT_TYPE       = "EVENT_TYPE"
   EVENT_ID         = "EVENT_ID"
   EVENT_TEXT       = "EVENT_TEXT"
   CONDITION        = "CONDITION"
   
   # These are column indices into each CSV row
   attr_reader :id
   attr_reader :description
   attr_reader :state
   attr_reader :rts_id
   attr_reader :max_fails
   attr_reader :event_type
   attr_reader :event_id
   attr_reader :event_text
   attr_reader :condition

   def initialize(csv_file)

      puts "ActionPointCsv::Initialize(#{csv_file})"
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

      raise NameError, "Action Point CSV file does not contain a row marked with #{KEYWORD_ROW} that defines the positions of the keywords" unless @keyword_found
      
   end # process_rows()
   

   #
   # Process the AP CSV file KEYWORD row. The row is passed in as an array
   # of strings. Raise an exception if required keywords is not found
   #
   def set_keyword_columns(keyword_row)

      @id = keyword_row.index(ID)
      raise ArgumentError, "Keyword row missing #{ID}" if @id.nil?

      @description = keyword_row.index(DESCRIPTION)
      raise ArgumentError, "Keyword row missing #{DESCRIPTION}" if @description.nil?  

      @state = keyword_row.index(STATE)
      raise ArgumentError, "Keyword row missing #{STATE}" if @state.nil?  
      
      @rts_id = keyword_row.index(RTS_ID)
      raise ArgumentError, "Keyword row missing #{RTS_ID}" if @rts_id.nil?  

      @max_fails = keyword_row.index(MAX_FAILS)
      raise ArgumentError, "Keyword row missing #{MAX_FAILS}" if @max_fails.nil?  

      @event_type = keyword_row.index(EVENT_TYPE)
      raise ArgumentError, "Keyword row missing #{EVENT_TYPE}" if @event_type.nil?  

      @event_id = keyword_row.index(EVENT_ID)
      raise ArgumentError, "Keyword row missing #{EVENT_ID}" if @event_id.nil?  

      @event_text = keyword_row.index(EVENT_TEXT)
      raise ArgumentError, "Keyword row missing #{EVENT_TEXT}" if @event_text.nil?  

      @condition = keyword_row.index(CONDITION)
      raise ArgumentError, "Keyword row missing #{CONDITION}" if @condition.nil?  
         
   end # set_keyword_columns()

end # class ActionPointCsv

#
# This class contains the AP "business logic" used to construct
# the AP C table structure from the CSV file. 
#
class ActionPoint


   def initialize()

      @state = { 'ACTIVE'    => 'LC_APSTATE_ACTIVE',
                 'DISABLED'  => 'LC_APSTATE_DISABLED',
                 'NOT USED'  => 'LC_ACTION_NOT_USED',
                 'PASSIVE'   => 'LC_APSTATE_PASSIVE',
                 'PERMOFF'   => 'LC_APSTATE_PERMOFF'} 
       
      @event = { 'INFO'      => 'CFE_EVS_INFORMATION',
                 'DEBUG'     => 'CFE_EVS_DEBUG',
                 'ERROR'     => 'CFE_EVS_ERROR',
                 'CRITICAL'  => 'CFE_EVS_CRITICAL'}

   end # def initialize ()

   def infix_tokens(infix_str)
    
puts "infix_tokens(#{infix_str})\n"

      token_list = []
      num_string = ""
    
      # regex returns string index so compare to 0 for found
      infix_str.split('').each { |c|
puts "c=#{c}, token_list=#{token_list}, num_string=#{num_string}\n"       
         if (c == " ")
puts "c == " "\n"
            # Nothing to do
         elsif (c =~ /[[:digit:]]/) == 0
puts "c is digit \n"
            num_string << c
         elsif (c =~ /[&|()~]/) == 0
puts "c is special char: num_string = #{num_string} \n"
            if num_string != ""
               token_list << num_string
               num_string = ""
            end
            token_list << c
         else
           raise TypeError, "Illegal token #{c} found in RPN #{infix_str}"
         end
      }
            
      if num_string != ""
         token_list << num_string
      end

      puts token_list
      return token_list
   
   end # infix_tokens()
   
   
   def postfix_to_lc (postfix_expr)
puts "postfix_to_lc (#{postfix_expr})\n"
    
      lc_op = { "~" => "LC_RPN_NOT", "&" => "LC_RPN_AND", "^" => "LC_RPN_XOR", "|" => "LC_RPN_OR" }
    
      postfix_str = "{ "
    
      if postfix_expr.length == 0
         postfix_str << "0,"
      else
         postfix_expr.each do |token|
            if (token =~ /[[:digit:]]/) == 0 
               postfix_str << token
            else
               postfix_str << lc_op[token]
            end  
            postfix_str << ","
         end
      end
    
      postfix_str << "LC_RPN_EQUAL }"
    
      return postfix_str        
        
   end # postfix_to_lc()
                

   def infix_to_postfix(infix_expr)

puts "infix_to_postfix(#{infix_expr})\n"

      prec = { '~' => 5, '&' => 4, '^' => 3, '|' => 2, '(' => 1}

      op_stack = []
      postfix_expr = []
      token = ""

      tokens = infix_tokens(infix_expr)

      tokens.each do |token|
      
         if token =~ /[[:digit:]]/
            postfix_expr << token
         elsif token == '('
            op_stack << token
         elsif token == ')'
            top_token = op_stack.pop
            while top_token != '('
               postfix_expr << top_token
               top_token = op_stack.pop
            end
         elsif prec.key? (token)
            while (op_stack.length > 0) and (prec[op_stack[-1]] >= prec[token])
               postfix_expr << op_stack.pop
            end
            op_stack << token
         else
            raise TypeError, "Illegal token #{token} found in RPN #{infix_str}"
         end
           
      end # token loop

      while op_stack.length > 0
         postfix_expr << op_stack.pop
      end
                  
      return postfix_to_lc(postfix_expr) 
       
       
   end # infix_to_postfix()

          
   #
   # Create a string formatted for C table
   #   
   def csv_row_to_tbl_str(row, ap_csv, tlm_points, json_config)

puts "csv_row_to_tbl_str()\n"

      tbl_str = ""
       
      # 
      # The naming convention is to assign the 'raw' CSV string to a
      # variable appended with "_csv" and then create a new string variable
      # appended with "_str" that is used to create the C file. If no processing
      # is required on a raw value then it is assigned to a "_str"        
      #
      # CSV returns nil for blank entries
      #

      id_csv          = row[ap_csv.id]
      description_str = row[ap_csv.description]
      state_csv       = row[ap_csv.state]
      rts_id_csv      = row[ap_csv.rts_id]
      max_fails_csv   = row[ap_csv.max_fails]
      event_type_csv  = row[ap_csv.event_type]
      event_id_csv    = row[ap_csv.event_id]
      event_text_csv  = row[ap_csv.event_text]
      condition_csv   = row[ap_csv.condition]

puts "AP CSV: id=#{id_csv}, rts_id=#rts_id_csv}, decsr=#{description_str}, state=#{state_csv}, condition_csv=#{condition_csv}\n"
      
      return tbl_str if id_csv.nil? 

      begin # Create action string in exception block

         # Remove leading zeros for compatibility with previous NASA/GSFC tool 
         id_str = id_csv.sub(/^[0:]*/,"")  
         id_str = "0" if id_str.strip == ""
puts ">>>> id_str=#{id_str}\n" 
      
         if rts_id_csv.nil?
            rts_id_str = "0"
         else
            rts_id_str = rts_id_csv.sub(/^[0:]*/,"")  
            rts_id_str = "0" if rts_id_str.strip == ""
         end
puts ">>>> rts_id_str=#{rts_id_str}\n" 

         max_fails_str = (max_fails_csv.nil? or max_fails_csv == "") ? "0" : max_fails_csv
puts ">>>> max_fails_str=#{max_fails_str}\n" 
              
         event_id_str = (event_id_csv.nil? or event_id_csv == "") ? "0" : event_id_csv
          
puts ">>>> event_id_str=#{event_id_str}\n" 

         # Logic allows an implicitly undefined state (no legal state found) and an explicitly defined unused state 
         state_str = @state.key?(state_csv) ? state_str = @state[state_csv] : "LC_ACTION_NOT_USED"
                
         if (state_str == "LC_ACTION_NOT_USED")
            # set event counts to zero to maintain compatibility with previous tool
            passive_events_str = "0"
            pass_fail_events_str = "0"
            fail_pass_events_str = "0"
         else
            passive_events_str   = json_config.ap[JsonConfig::AP_MAX_PASSIVE_EVT]
            pass_fail_events_str = json_config.ap[JsonConfig::AP_MAX_PASS_FAIL_EVT]
            fail_pass_events_str = json_config.ap[JsonConfig::AP_MAX_FAIL_PASS_EVT]
         end 

puts ">>>> state_str=#{state_str}\n" 
         
         event_text_str = event_text_csv.nil? ? "" : event_text_csv
puts ">>>> event_text_str=#{event_text_str}, json_config.ap[JsonConfig::AP_MAX_ACTION_TXT] = #{json_config.ap[JsonConfig::AP_MAX_ACTION_TXT]}\n" 
         max_txt_len = json_config.ap[JsonConfig::AP_MAX_ACTION_TXT].to_i
         if (event_text_str.length > max_txt_len)
            event_text_str = event_text_str[0..(max_txt_len-1)]
            #todo - lc_logfile.print_log_entry("Warning - Event string for AP# " + id + " truncated to " + str(lc_config.parameters['LC_MAX_ACTION_TEXT']) + " characters" )
         end
puts ">>>> event_text_str=#{event_text_str}\n" 
         
         event_type_str = (@event.key?(event_type_csv)) ? @event[event_type_csv] : "CFE_EVS_INFORMATION"

puts ">>>> event_type_str=#{event_type_str}\n" 
             
         condition_str = condition_csv.nil? ? "" : condition_csv

puts ">>>> condition_str=#{condition_str}\n" 

         tbl_str << "    /* # #{id_str} ( #{description_str} ) */\n"
         tbl_str << "    { #{state_str}"
         tbl_str << ", #{passive_events_str}"
         tbl_str << ", #{pass_fail_events_str}"
         tbl_str << ", #{fail_pass_events_str}"
         tbl_str << ", #{rts_id_str}"
         tbl_str << ", #{max_fails_str}"
         tbl_str << ", #{infix_to_postfix(condition_str)}"
         tbl_str << ", #{event_type_str}"
         tbl_str << ", #{event_id_str}"
         tbl_str << ", {\"#{event_text_str}\"}},\n\n"
      
      end # Create action string
            
puts tbl_str + "\n"      
      return tbl_str
      
   end # csv_row_to_tbl_str()

end # class ActionPoint

class JsonConfig

   LIMIT_CHECKER   = "limit-checker"
   LC_ENDIAN       = "endian"
   LC_OUPUT_PATH   = "output-path"
   LC_TLM_CSV_FILE = "tlm_csv_file"
   
   ACTION_POINT    = "action-point"
   AP_COUNT        = "count"
   AP_TBL_FILE     = "tbl-file"
   AP_LOG_FILE     = "log-file"
   AP_HEADER_FILE  = "header-file"
   AP_FOOTER_FILE  = "footer-file"
   AP_MAX_RPN_EQU_SIZE  = "max-rpn-equ-size"
   AP_MAX_ACTION_TXT    = "max-action-text"
   AP_MAX_PASSIVE_EVT   = "max-passive-events"
   AP_MAX_PASS_FAIL_EVT = "max-pass-fail-events"
   AP_MAX_FAIL_PASS_EVT = "max-fail-pass-events"
 
   attr_reader  :lc
   attr_reader  :wp
   attr_reader  :ap


   def initialize(config_file)

      raise IOError "Configuration file #{config_file} does not exist" unless File.exist?(config_file)
      
      config_json = File.read(config_file)
      config = JSON.parse(config_json)
      @lc = config[LIMIT_CHECKER]      

      raise NameError "Configuration file #{config_file} missing #{LIMIT_CHECKER} => #{LC_ENDIAN} definition"       unless lc.key?(LC_ENDIAN)
      raise NameError "Configuration file #{config_file} missing #{LIMIT_CHECKER} => #{LC_OUPUT_PATH} definition"   unless lc.key?(LC_OUPUT_PATH)
      raise NameError "Configuration file #{config_file} missing #{LIMIT_CHECKER} => #{LC_TLM_CSV_FILE} definition" unless lc.key?(LC_TLM_CSV_FILE)

      @ap = config[LIMIT_CHECKER][ACTION_POINT]      
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_COUNT} definition"       unless ap.key?(AP_COUNT)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_TBL_FILE} definition"    unless ap.key?(AP_TBL_FILE)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_LOG_FILE} definition"    unless ap.key?(AP_LOG_FILE)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_HEADER_FILE} definition" unless ap.key?(AP_HEADER_FILE)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_FOOTER_FILE} definition" unless ap.key?(AP_FOOTER_FILE)

      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_MAX_RPN_EQU_SIZE} definition"  unless ap.key?(AP_MAX_RPN_EQU_SIZE)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_MAX_ACTION_TXT} definition"    unless ap.key?(AP_MAX_ACTION_TXT)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_MAX_RPN_EQU_SIZE} definition"  unless ap.key?(AP_MAX_RPN_EQU_SIZE)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_MAX_PASSIVE_EVT} definition"   unless ap.key?(AP_MAX_PASSIVE_EVT)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_MAX_PASS_FAIL_EVT} definition" unless ap.key?(AP_MAX_PASS_FAIL_EVT)
      raise NameError "Configuration file #{config_file} missing #{ACTION_POINT} => #{AP_MAX_FAIL_PASS_EVT} definition" unless ap.key?(AP_MAX_FAIL_PASS_EVT)
      
   end # initialize()


end # Class JsonConfig

#class ActionPointGen
class Test
 

   def initialize
      begin

         json_config = JsonConfig.new("cllc_config.json")    
puts "GH#1\n"
         tlm_csv = TlmCsv.new (json_config.lc[JsonConfig::LC_TLM_CSV_FILE])
puts "GH#2\n"
     
         action_point = ActionPoint.new
puts "GH#3\n"
         action_point_csv = ActionPointCsv.new ("GPM-MSE-SPEC-0116_Fault_Management_Document_Appendix_B - AP.csv")
puts "GH#4\n"

         FileUtils.cp json_config.ap[JsonConfig::AP_HEADER_FILE], json_config.ap[JsonConfig::AP_TBL_FILE]

         File.open(json_config.ap[JsonConfig::AP_TBL_FILE], 'a') do |ap_file|

            action_point_csv.process_rows do |row|

               ap_file << action_point.csv_row_to_tbl_str(row, action_point_csv, tlm_csv.points, json_config) 

            end # action_point_csv.process_rows

            File.open(json_config.ap[JsonConfig::AP_FOOTER_FILE]).each do |footer_line|
               ap_file << footer_line
            end

         end # AP_TBL_FILE

      rescue Exception => e

         puts e.message
         puts e.backtrace.inpsect
	   
      end
   end # initialize()
   

end # class ActionPointGen

