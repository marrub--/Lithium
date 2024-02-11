#!/usr/bin/env -S ruby --enable=frozen-string-literal -w

s = ARGV.pop.chars.map do |c|
   case c
   when "0" then "\\u{E109}"
   when "1" then "\\u{E10A}"
   when "2" then "\\u{E10B}"
   when "3" then "\\u{E10C}"
   when "4" then "\\u{E10D}"
   when "5" then "\\u{E10E}"
   when "X" then "\\u{E10F}"
   when "Y" then "\\u{E110}"
   when "Z" then "\\u{E111}"
   when "%" then "\\u{E112}"
   when "." then "\\u{E113}"
   when "r" then "\\u{E100}"
   when "z" then "\\u{E101}"
   when "n" then "\\u{E102}"
   when "k" then "\\u{E103}"
   when "t" then "\\u{E104}"
   when "h" then "\\u{E105}"
   when "s" then "\\u{E106}"
   when "w" then "\\u{E107}"
   when "i" then "\\u{E108}"
   else c
   end
end
puts s.reverse.join

## EOF
