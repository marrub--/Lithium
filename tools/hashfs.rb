#!/usr/bin/env ruby
# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Compile a directory structure into an enumerated list.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

require 'fileutils'

def run of, pkdir, dir
   of.puts "identifier,default"

   pdir = "#{pkdir}#{dir}"
   re = /#{Regexp.escape pdir}\/(.+)\.png/
   names = Dir.glob("#{pdir}/**/*.png").sort
   for item in names
      unless item.include? "/Font/"
         m = re.match item
         of.puts ":#{m[1].gsub "/", ":"},#{item.gsub pkdir, ""}"
      end
   end
end

for arg in ARGV
   fname, pkdir, dir = arg.split ":"
   run open(fname, "w"), pkdir, dir
end
