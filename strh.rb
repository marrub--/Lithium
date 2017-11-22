#!/usr/bin/env ruby
## Copyright © 2017 Graham Sanderson, all rights reserved.
## strh: Simple string hash utility

res = 0

for ch in ARGV[0].each_codepoint
   res = res * 101 + ch
   res &= 0x7FFFFFFF
end

print res

## EOF
