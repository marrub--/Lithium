#!/usr/bin/env ruby
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Renames stuff in sndinfo files.
##
## An input file can be generated with:
##
## git diff --staged --name-status -C |
## sed '/R100/!d' |
## cut -f 2,3 |
## sed 's/pk7\///g'
##
## ---------------------------------------------------------------------------|

renames = Hash[open(ARGV[0], "rt").each_line.map{|l| l.chomp.split(?\t, 2)}]

Dir["pk7/sndinfo.*"].each do |f|
   s = open(f, "rb").read
   renames.each{|k, v| s.sub! k, v}
   open(f, "wb").write s
end

## EOF
