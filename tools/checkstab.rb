#!/usr/bin/env ruby
# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Checks the string table for duplicates and unused strings.
##
## ---------------------------------------------------------------------------|

require 'set'

names = Set[]

open("source/include/m_stab.h").each_line do |ln|
   if ln =~ /^X\((\w+),.+\)/
      name = $~[1]
      raise "double definition of #{name}" if names.include? name
      names.add name
   end
end

used = Set[]

Dir.glob("source/**/*.{c,h}").each do |f|
   next if f == "source/include/m_stab.h"
   s = open(f).read
   for name in names
      used.add name if s =~ /\b#{name}\b/
   end
end

diff = names - used
raise diff.to_a.join "\n" unless diff.empty?

## EOF
