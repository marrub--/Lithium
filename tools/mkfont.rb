#!/usr/bin/env -S ruby --enable=frozen-string-literal -w

require_relative "corinth.rb"
require "set"
require "rmagick"

chrmap = Set[]
open("pk7/language.txt").each_char do |c| chrmap.add c end
chrmap -= ("\uE000".."\uEFFF").entries
chrmap.delete " "
chrmap.delete "\n"
chrmap.delete "\r"

def new_font name, &p
   fnt = {name: name, dir: "pk7/fonts/#{name}"}
   fnt.instance_exec(&p)
   fnt
end

def escape_text_character ch
   case ch
   when "\\" then "\\\\"
   when " " then "\\ "
   else ch
   end
end

def make_character_image ptsize, font, ch, &blk
   dr = Magick::Draw.new
   dr.pointsize = ptsize
   dr.font = font
   dr.fill = "white"
   dr.gravity = Magick::SouthWestGravity
   txt_chr = escape_text_character ch
   metr = dr.get_type_metrics txt_chr
   return nil if metr.bounds.x1.truncate == metr.bounds.x2.truncate
   im = Magick::Image.new metr.max_advance, metr.ascent - metr.descent do |ii|
      ii.background_color = "none"
      ii.image_type = Magick::GrayscaleAlphaType
      ii.depth = 8
   end
   if blk
      blk.call dr, im, txt_chr
   else
      dr.text 0, 0, txt_chr
      dr.draw im
   end
   im
end

def push_image il, im, fl, ch
   il.push im
   fl.push sprintf "%08X", ch.ord
end

def write_files il, fl
   il.write "#{self[:dir]}/%d.png"
   for f, i in fl.each_with_index
      File.rename "#{self[:dir]}/#{i}.png", "#{self[:dir]}/#{f}.png"
   end
end

def do_stroke dr, im, txt_chr
   dr.text 0, 0, txt_chr
   dr.draw im
   dr.stroke = "none"
   dr.stroke_width = 0
   dr.text 0, 0, txt_chr
   dr.draw im
end

def all_files_glob
   "#{self[:dir]}/*.png"
end

def delete_all_files
   File.delete(*Dir.glob(all_files_glob))
end

def call_optipng
   system "optipng",
   "-strip", "all",
   "-preserve", "-clobber",
   "-o", "2", *Dir.glob(all_files_glob)
end

jsmlfont = new_font "ljsmlfnt" do
   il = Magick::ImageList.new
   fl = []
   grad = Magick::GradientFill.new 0, 0, 16, 0, "white", "#696969"
   grad = Magick::Image.new 32, 16, grad
   self[:start_fn] = proc do delete_all_files end
   self[:char_fn] = proc do |ch|
      im = make_character_image 16, "working/x12y16pxMaruMonica.ttf", ch do |dr, im, txt_chr|
         dr.text 0, 0, txt_chr
         dr.draw im
         im.define "trim:edges", "east"
         im.trim!
      end
      if im
         out = im.composite grad, 0, 0, Magick::InCompositeOp
         out = im
            .negate_channel(true, Magick::DefaultChannels)
            .extent(im.columns + 1, im.rows + 1)
            .roll(1, 1)
            .composite(out, 0, 0, Magick::SrcOverCompositeOp)
         push_image il, out, fl, ch
      end
   end
   self[:end_fn] = proc do
      write_files il, fl
      call_optipng
   end
end
jtrmfont = new_font "ltrmfont" do
   il = Magick::ImageList.new
   fl = []
   self[:start_fn] = proc do delete_all_files end
   self[:char_fn] = proc do |ch|
      im = make_character_image 11, "working/ipaexg.ttf", ch do |dr, im, txt_chr|
         dr.stroke = "black"
         dr.stroke_width = 1
         do_stroke dr, im, txt_chr
      end
      push_image il, im, fl, ch if im
   end
   self[:end_fn] = proc do
      write_files il, fl
   end
end
ltrmfont = new_font "ltrmfont" do
   il = Magick::ImageList.new
   fl = []
   self[:start_fn] = proc do end
   self[:char_fn] = proc do |ch|
      im = make_character_image 11, "working/courierprime.ttf", ch do |dr, im, txt_chr|
         dr.stroke = "black"
         dr.stroke_width = 1
         do_stroke dr, im, txt_chr
      end
      push_image il, im, fl, ch if im
   end
   self[:end_fn] = proc do
      write_files il, fl
      call_optipng
   end
end
areaname = new_font "lareanam" do
   il = Magick::ImageList.new
   fl = []
   self[:start_fn] = proc do delete_all_files end
   self[:char_fn] = proc do |ch|
      im = make_character_image 30, "working/smodgui.ttf", ch do |dr, im, txt_chr|
         dr.stroke = "black"
         dr.stroke_width = 5
         do_stroke dr, im, txt_chr
         im.define "trim:edges", "east"
         im.trim!
      end
      push_image il, im, fl, ch if im
   end
   self[:end_fn] = proc do
      write_files il, fl
      call_optipng
   end
end
[jsmlfont, jtrmfont, ltrmfont, areaname].each do |fnt|
   fnt[:start_fn].call
   chrmap.each do |ch| fnt[:char_fn].call(ch) end
   fnt[:end_fn].call
end

## EOF
