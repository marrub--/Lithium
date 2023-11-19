#!/usr/bin/env -S ruby --enable=frozen-string-literal -w

require_relative "corinth.rb"
require "set"
require "rmagick"

def flatten_chrmap areas
   areas = areas.flat_map do |area|
      if area.kind_of? Range
         area.entries.map do |n|
            if n.kind_of? Numeric
               n.chr(Encoding::UTF_8)
            else
               n
            end
         end
      else
         area
      end
   end
   Set.new(areas)
end

def add_file_chars chrmap, fname
   open(fname).each_char do |c| chrmap.add c end
end

CHRMAP_ASCII = flatten_chrmap [0x20..0x7E]
CHRMAP_EXTND = flatten_chrmap [*CHRMAP_ASCII, 0x80..0x2E7F]
CHRMAP = Set[]
add_file_chars CHRMAP, "pk7/language.txt"
add_file_chars CHRMAP, "source/debug.c"
CHRMAP.subtract "\uE000".."\uEFFF"
CHRMAP.delete " "
CHRMAP.delete "\n"
CHRMAP.delete "\r"

def new_font name, cm = CHRMAP, &p
   fnt = {name: name, chrmap: cm, dir: "pk7/fonts/#{name}"}
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

def make_character_image ptsize, font, ch, x_size = 0, y_size = 0, &blk
   dr = Magick::Draw.new
   dr.pointsize = ptsize
   dr.font = font
   dr.fill = "white"
   dr.gravity = Magick::SouthWestGravity
   tch = escape_text_character ch
   metr = dr.get_type_metrics tch
   return nil if metr.bounds.x1.truncate == metr.bounds.x2.truncate
   x_size = if x_size.respond_to? :call then x_size.call metr
            else metr.bounds.x2.truncate + x_size end
   y_size = if y_size.respond_to? :call then y_size.call metr
            else metr.ascent - metr.descent + y_size end
   im = Magick::Image.new x_size, y_size do |ii|
      ii.background_color = "none"
      ii.image_type = Magick::GrayscaleAlphaType
      ii.depth = 8
   end
   if blk
      blk.call dr, im, tch
   else
      dr_text dr, im, tch
   end
   im
end

def push_image il, im, fl, ch
   il.push im
   fl.push sprintf "%08X", ch.ord
end

def write_files il, fl
   il.write "#{self[:dir]}/temp_%d.png"
   for f, i in fl.each_with_index
      File.rename "#{self[:dir]}/temp_#{i}.png", "#{self[:dir]}/#{f}.png"
   end
end

def dr_text dr, im, tch, x = 0, y = 0
   dr.text x, y, tch
   dr.draw im
end

def dr_stroke dr, im, tch
   dr_text dr, im, tch
   dr.stroke = "none"
   dr.stroke_width = 0
   dr_text dr, im, tch
end

def all_files_glob
   "#{self[:dir]}/*.png"
end

def delete_files glob
   File.delete(*Dir.glob(glob))
end

def call_optipng glob
   system "optipng",
   "-strip", "all",
   "-preserve", "-clobber",
   "-o", "2", *Dir.glob(glob)
end

@jbigfont = new_font("lbigfont", CHRMAP - CHRMAP_EXTND) do
   il = Magick::ImageList.new
   fl = []
   glob = "#{self[:dir]}/0000*.png"
   self[:start_fn] = proc do delete_files glob end
   self[:char_fn] = proc do |ch|
      im = make_character_image(12, "working/x12y12pxMaruMinya.ttf", ch, 0, proc do |metr|
         metr.ascent - metr.descent - 1
      end)
      if im
         out = Magick::Image.new im.columns + 2, im.rows + 2 do |ii|
            ii.background_color = "none"
         end
         im_cr = im.colorize 1, 1, 1, "#005500"
         out = out.composite im_cr, -1+1, -1+1, Magick::SrcOverCompositeOp
         out = out.composite im_cr, -1+1,  1+1, Magick::SrcOverCompositeOp
         out = out.composite im_cr,  1+1, -1+1, Magick::SrcOverCompositeOp
         out = out.composite im_cr,  1+1,  1+1, Magick::SrcOverCompositeOp
         im_cr = im.colorize 1, 1, 1, "#007300"
         out = out.composite im_cr, -1+1,  0+1, Magick::SrcOverCompositeOp
         out = out.composite im_cr,  1+1,  0+1, Magick::SrcOverCompositeOp
         out = out.composite im_cr,  0+1, -1+1, Magick::SrcOverCompositeOp
         out = out.composite im_cr,  0+1,  1+1, Magick::SrcOverCompositeOp
         im_cr = im.colorize 1, 1, 1, "#00ff00"
         out = out.composite im_cr, 0+1, 0+1, Magick::SrcOverCompositeOp
         push_image il, out, fl, ch
      end
   end
   self[:end_fn] = proc do
      write_files il, fl
      call_optipng glob
   end
end
@jsmlfont = new_font "ljsmlfnt" do
   il = Magick::ImageList.new
   fl = []
   grad = Magick::GradientFill.new 0, 0, 16, 0, "white", "#696969"
   grad = Magick::Image.new 32, 16, grad do |ii|
      ii.image_type = Magick::GrayscaleAlphaType
      ii.depth = 8
   end
   self[:start_fn] = proc do delete_files all_files_glob end
   self[:char_fn] = proc do |ch|
      im = make_character_image 16, "working/x12y16pxMaruMonica.ttf", ch
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
      call_optipng all_files_glob
   end
end
@jtrmfont = new_font "ltrmfont" do
   il = Magick::ImageList.new
   fl = []
   self[:start_fn] = proc do delete_files all_files_glob end
   self[:char_fn] = proc do |ch|
      im = make_character_image(11, "working/ipaexg.ttf", ch, proc do
         14
      end) do |dr, im, tch|
         dr.stroke = "black"
         dr.stroke_width = 1
         dr_stroke dr, im, tch
      end
      push_image il, im, fl, ch if im
   end
   self[:end_fn] = proc do
      write_files il, fl
   end
end
@ltrmfont = new_font "ltrmfont" do
   il = Magick::ImageList.new
   fl = []
   self[:start_fn] = proc do end
   self[:char_fn] = proc do |ch|
      im = make_character_image(11, "working/courierprime.ttf", ch, proc do
         7
      end) do |dr, im, tch|
         dr.stroke = "black"
         dr.stroke_width = 1
         dr_stroke dr, im, tch
      end
      push_image il, im, fl, ch if im
   end
   self[:end_fn] = proc do
      write_files il, fl
      call_optipng all_files_glob
   end
end
@areaname = new_font "lareanam", CHRMAP_ASCII do
   il = Magick::ImageList.new
   fl = []
   self[:start_fn] = proc do delete_files all_files_glob end
   self[:char_fn] = proc do |ch|
      im = make_character_image 30, "working/smodgui.ttf", ch, 3 do |dr, im, tch|
         dr.stroke = "black"
         dr.stroke_width = 5
         dr_stroke dr, im, tch
      end
      push_image il, im, fl, ch if im
   end
   self[:end_fn] = proc do
      write_files il, fl
      call_optipng all_files_glob
   end
end
fontlist = ARGV.map do |v| instance_variable_get :"@#{v}" end
fontlist =
   [@jbigfont, @jsmlfont, @jtrmfont, @ltrmfont, @areaname] if fontlist.empty?
fontlist.each do |fnt|
   fnt[:start_fn].call
   fnt[:chrmap].each do |ch| fnt[:char_fn].call(ch) end
   fnt[:end_fn].call
end

## EOF
