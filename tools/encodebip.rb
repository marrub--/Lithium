#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
puts(
   STDIN.each_codepoint.map do |v|
      if /[[:print:]]/.match?(v.chr) then v ^ 7 else v end
   end.pack("U*"))
