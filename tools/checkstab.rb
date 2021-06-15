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
## Checks the string table for various issues.
##
## ---------------------------------------------------------------------------|

require "set"

STAB_H = "source/include/m_stab.h"

strings = {}

def line_no match
   match.string[0..match.begin(0)].count("\n") + 1
end

open(STAB_H).read.scan /^stab_x\((\w+),\s*s?(.+)\)/ do |_|
   strings[$~[1]] = {line: line_no($~), file: STAB_H, text: $~[2], uses: 0, stab: true}
end

Dir.glob("source/**/*.{c,h}").each do |f|
   next if f == STAB_H
   s = open(f).read

   s.scan /\bStr\((\w+),\s*s?(.+)\);/ do |_|
      strings[$~[1]] = {line: line_no($~), file: f, text: $~[2], uses: 0, stab: false}
   end

   for name, nd in strings
      if s =~ /\b#{name}\b/
         nd[:uses] += 1
      end
   end
end

strings = strings.sort_by do |name, nd|
   [nd[:file], nd[:line], name]
end

puts "checking for uses in stab.h..."
for name, nd in strings
   next unless nd[:stab]

   stat = case nd[:uses]
          when 0 then "\e[31munused\e[0m"
          when 1 then "\e[33mone use\e[0m"
          else        "\e[32mok\e[0m"
          end

   printf "%27s: %3i %s\n", name, nd[:uses], stat
end

groups = {}

puts "checking for prefixes..."
for name, nd in strings
   pfx = name[0..2]

   case pfx
   when "sc_", "sf_", "sl_", "sm_", "so_", "sp_", "ss_", "st_"
      groups[pfx] = {} unless groups[pfx]
      groups[pfx][name] = 0 unless groups[pfx][name]
      groups[pfx][name] += 1
   else
      puts "#{nd[:file]}:#{nd[:line]}: invalid prefix for `#{name}'"
   end
end

groups = groups.sort

puts "checking for name issues..."
for group, nums in groups
   print "#{group} => "

   for name, num in nums
      stat = if num != 1
                "\e[31mduplicated\e[0ms"
             elsif name =~ /[A-Z]/
                "\e[33mirregular name\e[0m"
             else
                nil
             end

      next unless stat

      printf "%27s: %3i %s", name, num, stat
      print "\n       "
   end

   puts "-" * 27
end

skip = Set[]

puts "checking for text duplicates..."
for name1, nd1 in strings
   next if skip.include? nd1

   for name2, nd2 in strings
      next if nd1 == nd2

      if nd1[:text] == nd2[:text]
         puts "duplicated text in `#{name1}':"
         puts " 1: #{nd1[:file]}:#{nd1[:line]}"
         puts " 2: #{nd2[:file]}:#{nd2[:line]}"
         skip.add nd2
      end
   end
end

=begin
puts "results:"
for name, nd in strings
   puts "#{nd[:file]}:#{nd[:line]}: `#{name}' => `#{nd[:text]}'"
end
=end

## EOF
