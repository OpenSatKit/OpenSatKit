require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

	class PDUPrompt

		attr_reader :length
		attr_accessor :responseRequired
		@@fdCode = 9
		@@exactlyLength = 1
		@@totalVariblesInstanceds = 2

		def initialize(*content)

			if (content[0].is_a?(Array))

				content = content[0]
				Utils_visiona.verifyLength("less", content.length, @@exactlyLength)
				@responseRequired = Utils_visiona.getBits(content[0], 8, 8)
				setLength

			elsif (content[0].is_a?(Hash))

				content = content[0]
				@responseRequired = content[:responseRequired] unless content[:responseRequired].nil?
			end
		end

		def valid?

			begin; setLength; rescue; return false; end
			Utils_visiona.compareValues(self.instance_variables.length, @@totalVariblesInstanceds, "variables instanceds")
			return true
		end

		def setLength

			# 1 for fdCode and 1 for response
			@length = 1 + 1
		end

		def pack

			valid?
			binArray = Array.new
			binArray << @@fdCode
			binArray << (@responseRequired<<7)
		end

		def to_s

			output = ""
			output << "This is the beggining of the PDUPrompt\n"
			output << "Response Required is #{@responseRequired} (0-NAK; 1-KeepAlive)\n"
			output << "This is the ending of the PDUPrompt\n"
			return output
		end
	end # end class PDUPrompt
end # end module CFDP