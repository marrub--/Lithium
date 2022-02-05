#!/usr/bin/env ruby
XOFS = ARGV.shift.to_i
YOFS = ARGV.shift.to_i
IO.foreach(ARGV.shift) do |ln|
   if ln =~ /(.+)offset\(([-0-9]+), ([-0-9]+)\)(.+)/
      puts "#{$1}offset(#{$2.to_i+XOFS}, #{$3.to_i+YOFS})#{$4}"
   else
      puts ln
   end
end

## EOF
