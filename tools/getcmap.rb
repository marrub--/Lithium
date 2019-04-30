#!/usr/bin/env ruby
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Generates a character map from the filedata directory.
##
## ---------------------------------------------------------------------------|

require 'set'

chs = Set[]

`find filedata -type f -name '*.txt'`.split(?\n).each do |f|
   f.replace(open(f).read).each_char{|c| chs << c}
end

print chs.to_a.sort!.join

## EOF
