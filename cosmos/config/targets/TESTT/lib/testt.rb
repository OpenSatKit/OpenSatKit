require 'cosmos'

class Testt
  attr_reader :tgt_name

  # It is good practice to pass in the name of the target when instantiating
  # the library. This way if the target name changes by COSMOS target name
  # substitution or by simply renaming in the filesystem, the library continues
  # to work simply by passing in the new name.
  def initialize(tgt_name = 'TESTT')
    @tgt_name = tgt_name
  end

  def noop
    count = tlm("#{@tgt_name} STATUS COUNT")
    cmd("#{@tgt_name} NOOP")
    # Wait 5s for the counter to increment. Note the double equals!
    wait_check("#{@tgt_name} STATUS COUNT == #{count + 1}", 5)
  end
end
