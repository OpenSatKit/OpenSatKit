################################################################################
# cFS Benchmarking Tool Telemetry Screen Helper Script
#
# Notes:
#   1. Written by Ian Wilmoth, licensed under the copyleft GNU
#      General Public License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

# Global variables because nothing else will stay in memory.

$enable_param_widget_names = [
#WRITEENABLEPARAMS
	"bm_sb_benchmarkenable",
	"bm_fs_benchmarkenable",
	"bm_cpu_benchmarkenable"
#END
]

$param_widget_names = {
#WRITEPARAMS
	"bm_sb_zerocopyenable" => [0, 1],
	"bm_sb_numthreads" => [1, 8],
	"bm_sb_repetitions" => [1, 1000],
	"bm_sb_numtestmsgs" => [1, 1000000],
	"bm_sb_messagedatasize" => [1, 100000],
	"bm_sb_msgtimeout" => [100000, 1000000],
	"bm_fs_repetitions" => [1, 1000],
	"bm_fs_sequential_size" => [1, 9999999999],
	"bm_cpu_dhrystonerepetitions" => [1, 10000000],
	"bm_cpu_whetstonerepetitions" => [1, 10000000]
#END
}

$result_widget_names = [
#WRITERESULTS
	"bm_sb_besttime",
	"bm_sb_worsttime",
	"bm_sb_averagetime",
	"bm_sb_messagespersecond",
	"bm_fs_bestsequentialreadtime",
	"bm_fs_worstsequentialreadtime",
	"bm_fs_averagesequentialreadtime",
	"bm_fs_bestsequentialwritetime",
	"bm_fs_worstsequentialwritetime",
	"bm_fs_averagesequentialwritetime",
	"bm_fs_sequentialreadaveragemegabytespersecond",
	"bm_fs_sequentialwriteaveragemegabytespersecond",
	"bm_cpu_dhrystonespersecond",
	"bm_cpu_whetstonespersecond"
#END
]

def go(screen, method)

	if (method == "send_parameters")
		send_parameters(screen)
	elsif (method == "run_benchmarks")
		run_benchmarks(screen)
	elsif (method == "run_benchmarks_multi")
		run_benchmarks_multi(screen)
	end
end

def send_parameters(screen)

	cmd("BM SEND_HK")
  wait_packet("BM", "HK_TLM_PKT", 1, 5)
  parameter_cmd_str = "BM SET_PARAMS with"

  $enable_param_widget_names.each {|widget_name|
  		
	  widget = screen.get_named_widget(widget_name)
	  text_widget = screen.get_named_widget(widget_name + "_text")

    if (text_widget.text == "")
      parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{widget.value.split.first},"
    else
      parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{text_widget.text},"
      text_widget.text = ""
    end
  }

	$param_widget_names.each_key {|widget_name|

    widget = screen.get_named_widget(widget_name)
    text_widget = screen.get_named_widget(widget_name + "_text")

    if (text_widget.text == "")
      parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{widget.value.split.first},"
    else
      parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{text_widget.text},"
      text_widget.text = ""
    end
  }

  parameter_cmd_str.chop!
  cmd(parameter_cmd_str)
end

def run_benchmarks(screen)

	cmd("BM SEND_HK")
  wait_packet("BM", "HK_TLM_PKT", 1, 5)
	wait_check_expression("tlm('BM HK_TLM_PKT BM_RUNNINGBENCHMARKS') == 0", 1000)
	cmd("BM RUN_ALL")
	text_widget = screen.get_named_widget("benchmark_multi_status")
	text_widget.text = "Running benchmarks..."
	wait_check_expression("tlm('BM HK_TLM_PKT BM_RUNNINGBENCHMARKS') == 0", 1000)
	text_widget.text = "Benchmarks complete."

end

def run_benchmarks_multi(screen)

	cmd("BM SEND_HK")
  wait_packet("BM", "HK_TLM_PKT", 1, 5)
	wait_check_expression("tlm('BM HK_TLM_PKT BM_RUNNINGBENCHMARKS') == 0", 1000)

	text_widget = screen.get_named_widget("benchmark_datapoints")

	if (text_widget.text == "")
		num_datapoints = 1
		text_widget.text = "1"
	else
		num_datapoints = text_widget.text.to_i

		if (num_datapoints > 99999)
			num_datapoints = 99999
		elsif (num_datapoints < 1)
			num_datapoints = 1
		end
	end

	text_widget = screen.get_named_widget("benchmark_dir")

	if (text_widget.text == "")
		benchmark_dir = "#{Cosmos::USERPATH}/outputs/benchmarks/"
		text_widget.text = benchmark_dir
	else
		benchmark_dir = File.expand_path(text_widget.text)
		if (!benchmark_dir.end_with?("/"))
			benchmark_dir = benchmark_dir + "/"
			text_widget.text = benchmark_dir
		end
	end

	benchmark_data = Hash.new
	limits_data = Hash.new
	values_data = Hash.new
	param_enable_data = Hash.new
	previous_enable_values = Hash.new
	previous_values = Hash.new
	time = Time.new

	text_widget = screen.get_named_widget("benchmark_multi_status")

	$param_widget_names.each_key {|name|
		benchmark_data[name] = Array.new
		limits_data[name] = Array.new
	}

	$result_widget_names.each {|name|
		benchmark_data[name] = Array.new
	}

	limits_data.each_pair {|key, value|

		text = screen.get_named_widget(key + "_rangebottom").text
		if (text == "")
			value.push($param_widget_names[key][0])
		else
			value.push(text.to_i)
		end
		text = screen.get_named_widget(key + "_rangetop").text
		if (text == "")
			value.push($param_widget_names[key][1])
		else
			value.push(text.to_i)
		end
	}

	$param_widget_names.each_key {|key|
		min = limits_data[key][0]
		max = limits_data[key][1]
		delta = max - min + 1
		step = delta / num_datapoints

		if (step < 1)
			step = 1
		end

		arr = (min..max).step(step).to_a

		if (arr.length > num_datapoints)
			arr.slice!(num_datapoints..(arr.length - 1))
		elsif (arr.length < num_datapoints)
			arr.push(*Array.new((num_datapoints - arr.length), arr.last))
		end
		values_data[key] = arr
	}

	$param_widget_names.each_key {|key|
		param_enable_data[key] = screen.get_named_widget(key + "_vary").checked?
	}

	$enable_param_widget_names.each {|key|
		widget = screen.get_named_widget(key)
		previous_enable_values[key] = widget.value.split.first
	}

	$param_widget_names.each_key {|key|
		widget = screen.get_named_widget(key)
		previous_values[key] = widget.value.split.first
	}

	for i in 0..(num_datapoints - 1)

		wait_check_expression("tlm('BM HK_TLM_PKT BM_RUNNINGBENCHMARKS') == 0", 1000)
		text_widget.text = "Running benchmarks... (#{i}/#{num_datapoints})"
		parameter_cmd_str = "BM SET_PARAMS with"
	  	$enable_param_widget_names.each {|widget_name|
			parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{previous_enable_values[widget_name]},"
	  	}

		$param_widget_names.each_key {|widget_name|
			if (param_enable_data[widget_name])
				parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{values_data[widget_name][i]},"
			else
				parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{previous_values[widget_name]},"
			end
		}

		parameter_cmd_str.chop!
	  cmd(parameter_cmd_str)
    cmd("BM SEND_HK")
    wait_packet("BM", "HK_TLM_PKT", 1, 5)
    cmd("BM RUN_ALL")
    wait_check_expression("tlm('BM HK_TLM_PKT BM_RUNNINGBENCHMARKS') == 1", 1000)
    wait_check_expression("tlm('BM HK_TLM_PKT BM_RUNNINGBENCHMARKS') == 0", 1000)
		
    benchmark_data.each_pair {|key, value|
      value[i] = tlm("BM HK_TLM_PKT #{key.upcase}")
    }

  end

	parameter_cmd_str = "BM SET_PARAMS with"

  	$enable_param_widget_names.each {|widget_name|
		parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{previous_enable_values[widget_name]},"
  	}

	$param_widget_names.each_key {|widget_name|
		parameter_cmd_str = parameter_cmd_str + " #{widget_name.upcase} #{previous_values[widget_name]},"
	}

	parameter_cmd_str.chop!
    cmd(parameter_cmd_str)

	if (!Dir.exists?(benchmark_dir))
		FileUtils.mkdir(benchmark_dir)
	end

	file_name = "benchmark_#{time.year}-#{time.month}-#{time.day}-#{time.hour}-#{time.min}-#{time.sec}.txt"
	data_file = File.open(benchmark_dir + file_name, "w")

	$param_widget_names.each_key {|name|
		data_file << name + "\t"
	}

	$result_widget_names.each {|name|
		data_file << name + "\t"
	}

	data_file << "\n"

	for i in 0..(num_datapoints - 1)

		$param_widget_names.each_key {|name|
			data_file << "#{benchmark_data[name][i]}" + "\t"
		}

		$result_widget_names.each {|name|
			data_file << "#{benchmark_data[name][i]}" + "\t"
		}

		data_file << "\n"

	end

	data_file.close

	text_widget.text = "Wrote results to " + benchmark_dir + file_name

end
