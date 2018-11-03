#!/usr/bin/env ruby
## Copyright © 2018 Alison Sanderson, all rights reserved.
## FindDep: Lazily finds dependencies of a C file.

require 'set'

$inc = Set[]
$re = Regexp.new /#include "(.+)"/

def walk fname
   txt = begin open(fname, "rb").read rescue return end
   if sc = txt.scan($re)
      sc.each do |m|
         m = m[0]
         name = m[-2..-1] == ".h" ? "source/Headers/#{m}" : "source/Main/#{m}"
         unless $inc === name
            $inc << name
            walk name
         end
      end
   end
end

walk ARGV[0]
$inc.each do |inc| puts "h:#{inc}" end

## EOF
