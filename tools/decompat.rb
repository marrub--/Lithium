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
## ZScript ↔ C shared syntax preprocessor.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

for filename in ARGV
   lns = open(filename, "rt").readlines
   outfiles = []
   out      = []
   cut      = false

   for ln in lns
      ln = ln.chomp
      case ln
      when "/* decompat-cut */"
         cut = true
      when "/* decompat-end */"
         cut = false
      when /\/\* decompat-out (.+) \*\//
         outfiles.push $~[1]
      else
         if ln =~ /enum\s+\w*\s*\/\*\s*([^\s]+)\s*\*\/( {)?/
            ln = "enum #{$~[1]}#{$~[2]}"
         end
         out.push ln unless cut
      end
   end

   out = out.join "\n"

   for outfile in outfiles
      fp = open outfile, "wt"
      fp.write generated_header "decompat"
      fp.puts  out
   end
end

## EOF
