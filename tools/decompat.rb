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
   lns  = open(filename, "rt").readlines
   iter = lns.each
   while (ln = iter.next) =~ /^.. (.+\.zsc)$/
      out = open $~[1], "wt"
      out.write generated_header "decompat", true

      for ln in lns
         out.write ln.chomp.sub(/enum \/\* (.+) \*\//, 'enum \1') + "\n"
      end
   end
end

## EOF
