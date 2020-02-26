#!/usr/bin/env ruby
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Generates a character map from the text directory.
##
## ---------------------------------------------------------------------------|

require 'set'

chs = Set[]

`find text -type f -name '*.txt'`.split(?\n).each do |f|
   f.replace(open(f).read).each_char do |c| chs << c end
end

chs.delete " "
chs.delete "\n"
chs.delete "\u{5c}"

print chs.to_a.sort!.join

## EOF
