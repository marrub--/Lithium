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
## Compresses sounds.
##
## ---------------------------------------------------------------------------|

Dir.glob(ARGV[0]).each do |f|
   if File.file?(f) && open(f) do |f| f.readpartial(4) == "fLaC" end
      puts "*** #{f}"
      `flac --best --no-padding --preserve-modtime -f "#{f}" -o "#{f}" 2>&1`
      `metaflac --dont-use-padding --remove-all "#{f}" 2>&1`
   end
end

## EOF
