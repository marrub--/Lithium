#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Modifies offsets in a simple ZScript file for reasons.                   │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

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
