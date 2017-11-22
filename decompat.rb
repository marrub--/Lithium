#!/usr/bin/env ruby
## Copyright © 2017 Graham Sanderson
## DeCompat: ZScript ↔ DECORATE shared syntax preprocessor.

def tozsc fp, out
   indefault = false
   instates  = false

   for ln in fp
      ln = ln.chomp.sub(/<Actor>/, "class")
      if ln.include? "<Default>"
         indefault = true
         out.write "   default\n   {\n"
      elsif ln.include? "<EndDefault>"
         indefault = false
         out.write "   }\n"
      elsif ln.include? "<States>"
         instates = true
         out.write "   states\n   {\n"
      elsif ln.include? "<EndStates>"
         instates = false
         out.write "   }\n"
      else
         if indefault && !ln.empty?
            out.write "   " + ln + ";\n"
         elsif instates && !ln.empty? && !ln.end_with?(":")
            out.write ln + ";\n"
         else
            out.write ln + "\n"
         end
      end
   end
end

def todec fp, out
   for ln in fp
      ln = ln.chomp.sub(/<Actor>/, "actor")
      if ln.include? "<Default>" or ln.include? "<EndDefault>" then next
      elsif ln.include? "<States>"
         out.write "   states\n   {\n"
      elsif ln.include? "<EndStates>"
         out.write "   }\n"
      else
         out.write ln + "\n"
      end
   end
end

for arg in ARGV
   fp = open(arg, "rt")
   dec = fp.gets[12..-1].chomp
   zsc = fp.gets[12..-1].chomp
   fp.gets
   tozsc fp, open(zsc, "wt")
   fp.rewind
   3.times {fp.gets}
   todec fp, open(dec, "wt")
end

## EOF
