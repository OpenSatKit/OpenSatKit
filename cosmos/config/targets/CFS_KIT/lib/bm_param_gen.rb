###############################################################################
# cFS Benchmarking Tool Parameter Generation Script
#
# Notes:
#   1. Written by Ian Wilmoth, licensed under the copyleft GNU
#      General Public License (GPL).
#   2. Parameter definitions and notes can be found in params.txt
#
###############################################################################

require 'fileutils'

# Classes to read the benchmark definitions into

class Benchmark

	attr_accessor :name
	attr_accessor :description
	attr_accessor :enable_param
	attr_accessor :params
	attr_accessor :results

	def initialize(name)
		@name = name
		@description = ""
		@enable_param = nil
		@params = Array.new
		@results = Array.new
	end

	def to_s
		puts @name
		puts @description
		puts @enable_param
		puts @params
		puts @results
	end
end

class Param

	attr_accessor :name
	attr_accessor :type
	attr_accessor :default
	attr_accessor :range
	attr_accessor :description
	attr_accessor :units

	def initialize(name)
		@name = name
		@type = "uint32"
		@default = 0
		@range = [0, 1]
		@description = ""
		@units = ["", ""]
	end

	def to_s
		puts @name
		puts @type
		puts @default
		puts @range
		puts @description
		puts @units
	end
end

class Result

	attr_accessor :name
	attr_accessor :type
	attr_accessor :description
	attr_accessor :units

	def initialize(name)
		@name = name
		@type = "uint32"
		@description = ""
		@units = ["", ""]
	end

	def to_s
		puts @name
		puts @type
		puts @description
		puts @units
	end
end

# Data structures

cfe_to_cosmos_type = {"uint32" => "UINT", "float" => "FLOAT"}
cfe_to_cosmos_bits = {"uint32" => 32, "float" => 32}
cfe_to_cosmos_format_string = {"float" => "%f"}
file_paths = ["cfs-project/apps/bm/fsw/src/bm_app.h",
	"cfs-project/apps/bm/fsw/src/bm_private_types.h",
	"cfs-project/apps/bm/fsw/src/bm_init_params.c",
	"cosmos-project/config/targets/BM/cmd_tlm/bm_cmd.txt",
	"cosmos-project/config/targets/BM/cmd_tlm/bm_tlm.txt",
	"cosmos-project/config/targets/SYSTEM/screens/benchmark.txt",
	"cosmos-project/procedures/kit_util/bm_utility.rb"]

$benchmarks = Array.new

read_benchmarks = false
read_params = false
read_results = false
current_benchmark = nil
current_item = nil
proj_dir = File.expand_path("~") + "/projects/"
backup_dir_name = "File backup"
time = Time.new

# File writer

def write_file(path, &writer)
	file = File.open(path)
	file_name = path.split("/").last.split(".").first
	new_file = File.open(file_name, "w")
	writer.call(file, new_file)
	file.close
	new_file.close
	FileUtils.mv(new_file, path)
end

# Read in benchmarks

param_file = File.open("params.txt")

param_file.each {|line|
	if (line =~ /^\s*#BENCHMARK_NAMES\s*$/ && !read_results && !read_params)
		read_benchmarks = true
	elsif (line =~ /^\s*#PARAM_NAMES\s*$/ && !read_results && read_benchmarks)
		read_params = true
	elsif (line =~ /^\s*#RESULT_NAMES\s*$/ && !read_params && read_benchmarks)
		read_results = true
	elsif (line =~ /^\s*#END\s*$/)
		if (read_benchmarks)
			if (read_params)
				read_params = false
			elsif (read_results)
				read_results = false
			else
				read_benchmarks = false
			end
		end
	elsif (line =~ /^\s*#END_BENCHMARK\s*$/ && read_benchmarks && current_benchmark != nil)
		$benchmarks.push(current_benchmark)
		current_benchmark = nil
	end

	if (read_benchmarks && !read_params && !read_results)
		if (current_benchmark == nil && line =~ /^\s*(\w+)\s*$/)
			if (!$benchmarks.any? {|benchmark| benchmark.name == ("BM_" + $1)})
				current_benchmark = Benchmark.new("BM_" + $1)
				enable_param = Param.new(current_benchmark.name + "Enable")
				enable_param.default = 1
				current_benchmark.enable_param = enable_param
			end
		elsif (line =~ /^\s*#DESCRIPTION\s+"(.*)"\s*$/)
			current_benchmark.description = $1
			current_benchmark.enable_param.description = "Enable " + $1
		end
	elsif (read_params || read_results)
		if (current_item == nil && line =~ /^\s*(\w+)\s*$/)
			if (read_params && !current_benchmark.params.any? {|param| param.name == ("BM_" + $1)})
				current_item = Param.new("BM_" + $1)
			elsif (read_results && !current_benchmark.results.any? {|result| result.name == ("BM_" + $1)})
				current_item = Result.new("BM_" + $1)
			end
		elsif (line =~ /^\s*#TYPE\s+(\w+)\s*$/)
			current_item.type = $1
		elsif (read_params && line =~ /^\s*#DEFAULT\s+(\w+)\s*$/)
			current_item.default = $1
		elsif (read_params && line =~ /^\s*#RANGE\s+(\w+)\s+(\w+)\s*$/)
			current_item.range = [$1, $2]
		elsif (line =~ /^\s*#DESCRIPTION\s+"(.*)"\s*$/)
			current_item.description = $1
		elsif (line =~ /^\s*#UNITS\s+([\w|\/]+)\s+([\w|\/]+)\s*$/)
			current_item.units = [$1, $2]
		else
			if (read_params && current_item != nil)
				current_benchmark.params.push(current_item)
			elsif (read_results && current_item != nil)
				current_benchmark.results.push(current_item)
			end
			current_item = nil
		end
	end
}

param_file.close

$params = Array.new

$benchmarks.each {|benchmark|
	$params.push(*(benchmark.params))
}

$results = Array.new

$benchmarks.each {|benchmark|
	$results.push(*(benchmark.results))
}

# Back up files to be overwritten
# puts "Backing up files that will be modified to \"#{backup_dir_name}\""

if (!Dir.exists?(backup_dir_name))
	FileUtils.mkdir(backup_dir_name)
end

file_paths.each {|path|
	cur_file_path = proj_dir + path
	cur_name = File.basename(cur_file_path)
	cur_extension = cur_name.split(".").last
	cur_name_noext = cur_name.slice(0..((cur_name.index(".") - 1)))
	FileUtils.cp(cur_file_path, backup_dir_name)
	File.rename(backup_dir_name + "/" + cur_name, backup_dir_name + "/" + cur_name_noext + "_#{time.year}-#{time.month}-#{time.day}-#{time.hour}-#{time.min}-#{time.sec}." + cur_extension)
}

# Define how to write to each file 

writer_procs = [

# bm_app.h (BM app parameter defaults)

	Proc.new {|file, new_file|
		write_params = false
		wait_for_end = false

		file.each{|line|
			if (line =~ /^\s*\/\*\s*#WRITEPARAMS\s*\*\/\s*$/)
				write_params = true
			elsif (line =~ /^\s*\/\*\s*#END\s*\*\/\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_params)
				$benchmarks.each {|benchmark|
					new_file << "#define #{benchmark.enable_param.name.upcase}\t\t\t#{benchmark.enable_param.default}\n"
					benchmark.params.each {|param|
						new_file << "#define #{param.name.upcase}\t\t\t#{param.default}\n"
					}
				}
				write_params = false
				wait_for_end = true
			end
		}
	},

# bm_private_types.h (BM app structure definition)

	Proc.new {|file, new_file|
		write_params = false
		write_results = false
		wait_for_end = false

		file.each {|line|
			if (line =~ /^\s*\/\*\s*#WRITEPARAMS\s*\*\/\s*$/)
				write_params = true
			elsif (line =~ /^\s*\/\*\s*#WRITERESULTS\s*\*\/\s*$/)
				write_results = true
			elsif (line =~ /^\s*\/\*\s*#END\s*\*\/\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_params)
				$benchmarks.each {|benchmark|
					new_file << "    #{benchmark.enable_param.type}  #{benchmark.enable_param.name};\n"
					benchmark.params.each {|param|
						new_file << "    #{param.type}  #{param.name};\n"
					}
				}
				write_params = false
				wait_for_end = true
			elsif (write_results)
				$benchmarks.each {|benchmark|
					benchmark.results.each {|result|
						new_file << "    #{result.type}  #{result.name};\n"
					}
				}
				write_results = false
				wait_for_end = true
			end
		}
	},

# bm_init_params.c (BM app parameter initialization from defaults)

	Proc.new {|file, new_file|
		write_params = false
		wait_for_end = false

		file.each{|line|
			if (line =~ /^\s*\/\*\s*#WRITEPARAMS\s*\*\/\s*$/)
				write_params = true
			elsif (line =~ /^\s*\/\*\s*#END\s*\*\/\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_params)
				$benchmarks.each {|benchmark|
					new_file << "\tg_BM_AppData.NewParams.#{benchmark.enable_param.name} = #{benchmark.enable_param.name.upcase};\n"
					benchmark.params.each {|param|
						new_file << "\tg_BM_AppData.NewParams.#{param.name} = #{param.name.upcase};\n"
					}
				}
				write_params = false
				wait_for_end = true
			end
		}
	},

# CFS_BM_CMD.txt (COSMOS set parameters command definition)

	Proc.new {|file, new_file|
		write_params = false
		wait_for_end = false

		file.each {|line|
			if (line =~ /^\s*#WRITEPARAMS\s*$/)
				write_params = true
			elsif (line =~ /^\s*#END\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_params)
				$benchmarks.each {|benchmark|
					new_file << "  APPEND_PARAMETER #{benchmark.enable_param.name.upcase} #{cfe_to_cosmos_bits[benchmark.enable_param.type]} #{cfe_to_cosmos_type[benchmark.enable_param.type]} #{benchmark.enable_param.range[0]} #{benchmark.enable_param.range[1]} #{benchmark.enable_param.default} \"#{benchmark.enable_param.description}\"\n"
					benchmark.params.each {|param|
						new_file << "  APPEND_PARAMETER #{param.name.upcase} #{cfe_to_cosmos_bits[param.type]} #{cfe_to_cosmos_type[param.type]} #{param.range[0]} #{param.range[1]} #{param.default} \"#{param.description}\"\n"
						units = param.units
						if (units[0].length > 0)
							new_file << "    UNITS #{units[0]} #{units[1]}\n"
						end
					}
				}
				write_params = false
				wait_for_end = true
			end
		}
	},

# CFE_BM_HKPACKET.txt (COSMOS telemetry packet definition)

	Proc.new {|file, new_file|
		write_params = false
		wait_for_end = false

		file.each {|line|
			if (line =~ /^\s*#WRITEPARAMS\s*$/)
				write_params = true
			elsif (line =~ /^\s*#END\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_params)
				$benchmarks.each {|benchmark|
					new_file << "  APPEND_ITEM #{benchmark.enable_param.name.upcase} #{cfe_to_cosmos_bits[benchmark.enable_param.type]} #{cfe_to_cosmos_type[benchmark.enable_param.type]} \"#{benchmark.enable_param.description}\"\n"
					benchmark.params.each {|param|
						new_file << "  APPEND_ITEM #{param.name.upcase} #{cfe_to_cosmos_bits[param.type]} #{cfe_to_cosmos_type[param.type]} \"#{param.description}\"\n"
						units = param.units
						if (units[0].length > 0)
							new_file << "    UNITS #{units[0]} #{units[1]}\n"
						end
					}
				}

				$benchmarks.each {|benchmark|
					benchmark.results.each {|result|
						new_file << "  APPEND_ITEM #{result.name.upcase} #{cfe_to_cosmos_bits[result.type]} #{cfe_to_cosmos_type[result.type]} \"#{result.description}\"\n"
						format_string = cfe_to_cosmos_format_string[result.type]
						units = result.units
						if (units[0].length > 0)
							new_file << "    UNITS #{units[0]} #{units[1]}\n"
						end
						if (format_string != nil)
							new_file << "    FORMAT_STRING \"#{format_string}\"\n"
						end
					}
				}
				write_params = false
				wait_for_end = true
			end
		}
	},

# benchmark.txt (COSMOS telemetry screen)
	
	Proc.new {|file, new_file|
		write_params = false
		write_results = false
		wait_for_end = false
		spaces = ""

		max_param_unit_length = $params.max_by {|param| param.units[1].length}.units[1].length

		if (max_param_unit_length > 0)
			max_param_unit_length += 1
		end

		max_result_unit_length = $results.max_by {|result| result.units[1].length}.units[1].length

		if (max_result_unit_length > 0)
			max_result_unit_length += 1
		end

		file.each {|line|
			if (line =~ /^(\s*)#WRITEPARAMS\s*$/)
				spaces = $1
				write_params = true
			elsif (line =~ /^(\s*)#WRITERESULTS\s*$/)
				spaces = $1
				write_results = true
			elsif (line =~ /^\s*#END\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_params)
				$benchmarks.each {|benchmark|
					new_file << spaces + "VERTICALBOX\n\n"
					new_file << spaces + "  TITLE \"#{benchmark.description}\"\n"
					new_file << spaces + "    SETTING BACKCOLOR 200 100 100\n"
					new_file << spaces + "    SETTING TEXTCOLOR black\n\n"
					new_file << spaces + "  MATRIXBYCOLUMNS 5\n\n"
					new_file << spaces + "    LABEL \"Parameter Description and Value\"\n"
					new_file << spaces + "      SETTING TEXTCOLOR gray\n"
					new_file << spaces + "      SETTING BORDERCOLOR black\n"
					new_file << spaces + "    LABEL \"New Value\"\n"
					new_file << spaces + "      SETTING TEXTCOLOR gray\n"
					new_file << spaces + "      SETTING BORDERCOLOR black\n"
					new_file << spaces + "    LABEL \"Lower Limit\"\n"
					new_file << spaces + "      SETTING TEXTCOLOR gray\n"
					new_file << spaces + "      SETTING BORDERCOLOR black\n"
					new_file << spaces + "    LABEL \"Upper Limit\"\n"
					new_file << spaces + "      SETTING TEXTCOLOR gray\n"
					new_file << spaces + "      SETTING BORDERCOLOR black\n"
					new_file << spaces + "    LABEL \"Vary\"\n\n"
					new_file << spaces + "      SETTING TEXTCOLOR gray\n"
					new_file << spaces + "      SETTING BORDERCOLOR black\n"
					new_file << spaces + "    NAMED_WIDGET #{benchmark.enable_param.name.downcase} LABELVALUEDESC CFS CFE_BM_HKTLMPKT #{benchmark.enable_param.name.upcase} \"#{benchmark.enable_param.description}\" WITH_UNITS #{12 + max_param_unit_length}\n"
					new_file << spaces + "    NAMED_WIDGET #{benchmark.enable_param.name.downcase}_text TEXTFIELD\n"
					new_file << spaces + "    LABEL \"\"\n"
					new_file << spaces + "    LABEL \"\"\n"
					new_file << spaces + "    LABEL \"\"\n\n"
					benchmark.params.each {|param|
						new_file << spaces + "    NAMED_WIDGET #{param.name.downcase} LABELVALUEDESC CFS CFE_BM_HKTLMPKT #{param.name.upcase} \"#{param.description}\" WITH_UNITS #{12 + max_param_unit_length}\n"
						new_file << spaces + "    NAMED_WIDGET #{param.name.downcase}_text TEXTFIELD\n"
						new_file << spaces + "    NAMED_WIDGET #{param.name.downcase}_rangebottom TEXTFIELD\n"
						new_file << spaces + "    NAMED_WIDGET #{param.name.downcase}_rangetop TEXTFIELD\n"
						new_file << spaces + "    NAMED_WIDGET #{param.name.downcase}_vary CHECKBUTTON \"\"\n\n"
					}
					new_file << spaces + "  END\n"
					new_file << spaces + "END\n"
				}
				write_params = false
				wait_for_end = true
			elsif (write_results)
				$benchmarks.each {|benchmark|
					new_file << "      VERTICALBOX\n"
					new_file << "        TITLE \"#{benchmark.description}\"\n"
					new_file << "          SETTING BACKCOLOR 200 100 100\n"
					new_file << "          SETTING TEXTCOLOR black\n\n"
					new_file << "        VERTICAL\n\n"
					benchmark.results.each {|result|
						new_file << "        LABELVALUEDESC CFS CFE_BM_HKTLMPKT #{result.name.upcase} \"#{result.description}\" WITH_UNITS #{12 + max_result_unit_length}\n"
					}
					new_file << "\n        END\n"
					new_file << "      END\n"
				}
				write_results = false
				wait_for_end = true
			end
		}
	},

# utility.rb (telemetry screen helper COSMOS script)

	Proc.new {|file, new_file|
		write_enable_params = false
		write_params = false
		write_results = false
		wait_for_end = false

		file.each {|line|
			if (line =~ /^\s*#WRITEENABLEPARAMS\s*$/)
				write_enable_params = true
			elsif (line =~ /^\s*#WRITEPARAMS\s*$/)
				write_params = true
			elsif (line =~ /^\s*#WRITERESULTS\s*$/)
				write_results = true
			elsif (line =~ /^\s*#END\s*$/)
				wait_for_end = false
			end

			if (!wait_for_end)
				new_file << line
			end

			if (write_enable_params)
				$benchmarks.each_with_index {|benchmark, index|
					if (index == $benchmarks.length - 1)
						new_file << "\t\"#{benchmark.enable_param.name.downcase}\"\n"
					else
						new_file << "\t\"#{benchmark.enable_param.name.downcase}\",\n"
					end
					write_enable_params = false
					wait_for_end = true
				}
			elsif (write_params)
				$params.each_with_index {|param, index|
					if (index == $params.length - 1)
						new_file << "\t\"#{param.name.downcase}\" => [#{param.range[0]}, #{param.range[1]}]\n"
					else
						new_file << "\t\"#{param.name.downcase}\" => [#{param.range[0]}, #{param.range[1]}],\n"
					end
				}
				write_params = false
				wait_for_end = true
			elsif (write_results)
				$results.each_with_index {|result, index|
					if (index == $results.length - 1)
						new_file << "\t\"#{result.name.downcase}\"\n"
					else
						new_file << "\t\"#{result.name.downcase}\",\n"
					end
				}
				write_results = false
				wait_for_end = true
			end
		}
	}]

# Write to files

writer_procs.each_with_index {|proc, index|
	write_file((proj_dir + file_paths[index]), &proc)
}
