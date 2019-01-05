#!/usr/bin/env ruby
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
## CmpSnd: Compresses sounds.

log = open "rensnds_log", "w"

Dir[ARGV[0]].each do |f|
   if File.file?(f) && open(f) {|f| f.readpartial(4) == "fLaC"}
      fo = `flac --best --no-padding --preserve-modtime -f "#{f}" -o "#{f}" 2>&1`
      mo = `metaflac --dont-use-padding --remove-all "#{f}" 2>&1`
      log << "*** #{f}\n"
      log << fo
      log << mo
   end
end

## EOF
