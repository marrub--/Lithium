#!/usr/bin/env ruby
## Copyright © 2018 Alison Sanderson, all rights reserved.
## GetCMap: Generates a character map from the filedata directory.

require 'set'

chs = Set[]

`find filedata -type f -name '*.txt'`.split("\n").each do |f|
   f = open(f).read
   f.each_char {|c| chs << c}
end

print chs.to_a.sort!.join

## EOF
