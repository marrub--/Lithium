#!/usr/bin/env ruby
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Creates fonts and downloads necessary TTFs if needed.
##
## ---------------------------------------------------------------------------|

FONTS=%w(MisakiG MisakiM JFDotG)
CMAP=`tools/getcmap.rb`

out = []

for f in FONTS
   fontfile = "tools/#{f}.ttf"
   unless FileTest.exist? fontfile
      system *%W"wget http://mab.greyserv.net/f/#{f}.ttf -O #{fontfile}"
   end

   outdir = "pk7/fonts/#{f.downcase}"

   `rm -f #{outdir}/*.png`

   words = %W"convert -depth 1 -font #{fontfile} -pointsize 8 -background none -fill white"

   for ch in CMAP.each_char
      num = sprintf "%08X", ch.ord
      f = "#{outdir}/#{num}.png"
      out.push f
      words.push *%W"( label:#{ch} -write #{f} )"
   end

   system *words
end

system *%W"pngquant --ext .png -f 2".push(*out)
system *%W"advpng -z4".push(*out)

## EOF
