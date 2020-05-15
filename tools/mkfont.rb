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
## Creates fonts and downloads necessary TTFs if needed.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

require 'set'

def gen_map areas
   areas.flat_map do |area|
      if area.kind_of? Range then area.entries else area end
   end
end

Font = Struct.new :pt, :name, :cmap, :body

cmap_asc = gen_map [("!".."#"), ["%"], ("'".."?"), ("A".."}")]
cmap_ext = gen_map [("!".."~"), ("¡".."£"), ("¥".."¬"), ("®".."´"),
                    ("¶".."ÿ"), ("‒".."‧"), ("‰".."‽"), ("⁂".."⁏"),
                    ("⁑".."⁓"), ("⁕".."⁞"), "Ё", ("А".."я"), "ё"]

cmap_all = Set[]

Dir.glob("text/**/*.txt").each do |f|
   open(f).each_char do |c|
      cmap_all.add c
   end
end

cmap_all.delete " "
cmap_all.delete "\n"
cmap_all.delete "\u{5c}"

cmap_all = cmap_all.to_a.sort

fonts = []
fonts.push Font.new 8,  "MisakiG",   cmap_all
fonts.push Font.new 8,  "MisakiM",   cmap_all
fonts.push Font.new 8,  "k6x8",      cmap_all
fonts.push Font.new 16, "jiskan16",  cmap_all
fonts.push Font.new(8,  "ljtrmfont", cmap_all, lambda do |words, ch|
                       words.push *%W"-stroke black -strokewidth 1 label:#{ch}
                                      -stroke none                 label:#{ch}
                                      -composite"
                    end)
fonts.push Font.new(30, "AreaName", cmap_asc, lambda do |words, ch|
                       words.push *%W"-stroke black -strokewidth 5 label:#{ch}
                                      -stroke none                 label:#{ch}
                                      -composite"
                    end)
fonts.push Font.new(11, "ltrmfont", cmap_ext, lambda do |words, ch|
                       words.push *%W"-stroke black -strokewidth 1 label:#{ch}
                                      -stroke none                 label:#{ch}
                                      -composite -extent 7x13"
                    end)

optipng_in  = []
pngquant_in = []
advpng_in   = []

for fnt in fonts
   ttf = "bin/#{fnt.name}.ttf"

   unless FileTest.exist? ttf
      system *%W"wget http://mab.greyserv.net/f/#{fnt.name}.ttf -O #{ttf}"
   end

   outdir = "pk7/fonts/#{fnt.name.downcase}"

   system *%W"rm -f".push(*Dir.glob("#{outdir}/*.png"))

   words = %W"convert -depth 1 -font #{ttf} -pointsize #{fnt.pt}
              -background none -fill white"

   for ch in fnt.cmap
      num = sprintf "%08X", ch.ord
      f = "#{outdir}/#{num}.png"
      if fnt.body
         optipng_in.push f
      else
         pngquant_in.push f
      end
      advpng_in.push f
      words.push "("
      ch = "\\" + ch if %w"\\ ( ) -".include? ch
      if fnt.body
         fnt.body.call words, ch
      else
         words.push "label:#{ch}"
      end
      words.push *%W"-write #{f} )"
   end

   system *words
end

system *%w"optipng -strip all -preserve -clobber -o 2".push(*optipng_in)
system *%w"pngquant --ext .png -f 8 -s 1".push(*pngquant_in)
system *%w"advpng -z4".push(*advpng_in)

## EOF
