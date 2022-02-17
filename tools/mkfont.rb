#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Creates fonts and downloads necessary TTFs if needed.                    │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

require_relative "corinth.rb"
require "set"

def gen_map areas
   areas.flat_map do |area|
      if area.kind_of? Range then area.entries else area end
   end
end

def label ch
   ch = "\\" + ch if %w"\\ ( ) -".include? ch
   "label:" + ch
end

Font = Struct.new :pt, :name, :cmap, :body, :outdir, :ttf

cmap_asc = gen_map [("!".."#"), "%", ("'".."?"), ("A".."}")]
cmap_pnc = gen_map [("¡".."£"), ("¥".."¬"), ("®".."´"), ("¶".."¿"),
                    "×", "÷", ("‒".."‧"), ("‰".."‽"), ("⁂".."⁏"),
                    ("⁑".."⁓"), ("⁕".."⁞")]
cmap_let = gen_map [("À".."ÿ"), "Ё", ("А".."я"), "ё"]
cmap_ext = gen_map [("!".."~"), *cmap_pnc, *cmap_let]

cmap_all = Set[]

Dir.glob("text/**/*.txt").each do |f|
   open(f).each_char do |c|
      cmap_all.add c
   end
end

cmap_all.delete " "
cmap_all.delete "\n"
cmap_all.delete "\r"
cmap_all.delete_if do |c| cmap_let.include? c end

cmap_all = cmap_all.to_a.sort

fonts = []
fonts.push Font.new 8,  "MisakiG",   cmap_all
fonts.push Font.new 8,  "MisakiM",   cmap_all
fonts.push Font.new 8,  "k6x8",      cmap_all
fonts.push Font.new 16, "jiskan16",  cmap_all
fonts.push Font.new(8,  "ljtrmfont", cmap_all, lambda do |words, ch|
                       words.push "-stroke",      "black",
                                  "-strokewidth", "1",    label(ch),
                                  "-stroke",      "none", label(ch),
                                  "-composite"
                    end)
fonts.push Font.new(30, "AreaName", cmap_asc, lambda do |words, ch|
                       words.push "-stroke",      "black",
                                  "-strokewidth", "5",    label(ch),
                                  "-stroke",      "none", label(ch),
                                  "-composite"
                    end)
fonts.push Font.new(11, "ltrmfont", cmap_ext, lambda do |words, ch|
                       words.push "-stroke",      "black",
                                  "-strokewidth", "1",    label(ch),
                                  "-stroke",      "none", label(ch),
                                  "-composite",
                                  "-extent", "7x13"
                    end)

optipng_in  = []
pngquant_in = []
advpng_in   = []

fonts.each do |fnt|
   fnt.outdir = "pk7/fonts/#{fnt.name.downcase}"
   fnt.ttf    = "bin/#{fnt.name}.ttf"
end

fonts.each do |fnt|
   system "rm", "-f", *Dir.glob("#{fnt.outdir}/*.png")
end

fonts.each do |fnt|
   unless FileTest.exist? fnt.ttf
      system "wget",
             "https://mab.greyserv.net/f/#{fnt.name}.ttf",
             "-O", fnt.ttf
   end

   words = ["convert",
            "-depth",      "1",
            "-font",       fnt.ttf,
            "-pointsize",  fnt.pt.to_s,
            "-background", "none",
            "-fill",       "white"]

   for ch in fnt.cmap
      num = sprintf "%08X", ch.ord
      f = "#{fnt.outdir}/#{num}.png"
      if fnt.body
         optipng_in.push f
      else
         pngquant_in.push f
      end
      advpng_in.push f
      words.push "("
      if fnt.body
         fnt.body.call words, ch
      else
         words.push label(ch)
      end
      words.push "-write", f, ")"
   end

   words.push "null:"

   system(*words)
end

system "optipng",
       "-strip", "all",
       "-preserve",
       "-clobber",
       "-o", "2",
       *optipng_in
system "pngquant",
       "--ext", ".png",
       "-f", "8",
       "-s", "1",
       *pngquant_in
system "advpng",
       "-z4",
       *advpng_in

## EOF
