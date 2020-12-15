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
## Compile a directory structure into an enumerated list.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

require 'fileutils'

common_main do
   fname, pkdir, dir = ARGV
   pdir  = "#{pkdir}#{dir}"
   re    = /#{Regexp.escape pdir}\/(.+)\.png/
   names = Dir.glob("#{pdir}/**/*.png").sort

   of = open fname, "w"
   of.puts "identifier,default"
   for item in names
      unless item.include? "/Font/"
         m = re.match item
         of.puts ":#{m[1].gsub "/", ":"},#{item.gsub pkdir, ""}"
      end
   end
end

## EOF
