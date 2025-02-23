#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Structured data → SNDINFO processor.                                     │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

require_relative "corinth.rb"

class OutDef
   attr_reader :path, :file
   def initialize path, file
      @path = path
      @file = file
   end
   def to_s
      @path.join("/") + " = " + @file
   end
end

class OutDir
   attr_reader :name, :args
   def initialize name, args
      @name = name
      @args = args.dup
   end
   def self.args_to_s args
      args.map do |arg|
         if    arg.is_a? Array  then arg.join("/")
         elsif arg.is_a? String then arg
         end
      end
   end
   def to_s
      "$" + @name + " " + OutDir.args_to_s(@args).join(" ")
   end
end

class OutDbl < OutDir
   attr_reader :body
   def initialize name, args, body
      super name, args
      @body = body
   end
   def to_s
      super + " { " + OutDir.args_to_s(@body).join(" ") + " }"
   end
end

class SndParseState < ParseState
   attr_reader :snds_dir, :file_dir, :path
   def initialize filename
      super
      @snds_dir = String.new
      @file_dir = String.new
      @path     = []
   end
   def relative_path orig_file
      if @file_dir.empty?
         if File.dirname(orig_file) != "."
            raise %(file "#{orig_file}" has no directory)
         end
         return orig_file.dup
      end
      orig_file  = @file_dir + "/" + orig_file
      rela_file  = @snds_dir + "/" + orig_file
      dir, bfile = File.split rela_file
      if File.exist? rela_file
         if File.extname(orig_file) == ""
            raise %(file "#{orig_file}" has no extension)
         else
            raise %(file "#{orig_file}" was written with an extension)
         end
      end
      Dir.each_child(dir) do |dir_file|
         if File.basename(dir_file, ".*") == bfile
            return orig_file + File.extname(dir_file)
         end
      end
      raise %(file "#{orig_file}" doesn't exist in the filesystem)
   end
end

def parse_path state, orig
   tks = state.tks
   tok = tks.peek.expect_after orig, [:identi, :period]
   parse_relative_path = lambda do |path|
      tok = tks.next.expect_after tok, :identi
      path.push tok.text
      tks.peek_or :period, path do |tok|
         parse_relative_path.(path)
      end
   end
   # absolute-path -> . relative-path
   # relative-path -> identifier [ `.' relative-path ]
   # path -> absolute-path | relative-path
   if tok.type == :identi
      parse_relative_path.(state.path.dup)
   else
      tks.next
      parse_relative_path.([])
   end
end

def path_to_sndinfo path
   path.join "/"
end

def parse_ambient state, tok, directive
   tks  = state.tks
   args = []
   tok = tks.next.expect_after tok, :number
   args.push tok.text
   path = parse_path state, tok
   args.push path
   tok = tks.next.expect_after "path", :identi
   case tok.text
   when "point"
      tok = tks.next.expect_after tok, :number
      args.push "point", tok.text
      tok = tks.next.expect_after tok, :identi
   when "surround", "world"
      args.push tok.text
      tok = tks.next.expect_after tok, :identi
   end
   num = case tok.text
         when "continuous" then 0
         when "periodic"   then 1
         when "random"     then 2
         else              tok.raise_kw "mode"
         end
   args.push tok.text
   (num + 1).times do
      tok = tks.next.expect_after tok, :number
      args.push tok.text
   end
   state.write OutDir.new directive, args
end

def parse_player state, tok, directive
   tks  = state.tks
   args = []
   3.times do
      tok = tks.next.expect_after tok, :identi
      args.push tok.text
   end
   case directive
   when "playersounddup"
      tok = tks.next.expect_after tok, :identi
      args.push tok.text
   when "playeralias"
      path = parse_path state, tok
      args.push path
   else
      tok = tks.next.expect_after tok, [:identi, :string]
      args.push state.relative_path tok.text
   end
   pclasses = args[0] == "*" ? %w[player fighter]            : [args[0]]
   genders  = args[1] == "*" ? %w[neutral other female male] : [args[1]]
   pclasses.each do |pcl|
      args[0] = pcl
      genders.each do |gender|
         args[1] = gender
         state.write OutDir.new directive, args
      end
   end
end

def parse_directive state
   tks = state.tks
   tok = tks.next.expect_after tok, :identi
   case (directive = tok.text)
   when "include"
      tok = tks.next.expect_after tok, :string
      filename = state.base_dir + "/" + tok.text
      parse state.with tokenize filename
   when "sounds"
      tok = tks.next.expect_after tok, :string
      state.snds_dir.replace tok.text
   when "rolloff"
      path = path_to_sndinfo parse_path state, tok
      args = [path]
      tok = tks.next.expect_after tok, [:identi, :number]
      if tok.type == :identi
         case tok.text
         when "custom", "linear", "log"
            args.push tok.text
            tok = tks.next.expect_after tok, :number
         else
            tok.raise_kw "type"
         end
      end
      args.push tok.text
      tok = tks.next.expect_after tok, :number
      args.push tok.text
      state.write OutDir.new directive, args
   when "pitchshiftrange"
      tok = tks.next.expect_after tok, :number
      state.write OutDir.new directive, [tok.text]
   when "attenuation", "limit", "pitchshift", "volume"
      path = parse_path state, tok
      tok  = tks.next.expect_after "path", :number
      state.write OutDir.new directive, [path, tok.text]
   when "alias"
      lhs = parse_path state, tok
      tok = tks.next.expect_after "path", :assign
      rhs = parse_path state, tok
      state.write OutDir.new directive, [lhs, rhs]
   when "random"
      lhs = parse_path state, tok
      tok = tks.next.expect_after "path", :brac2o
      rhs = []
      tks.until_drop :brac2c do
         rhs.push parse_path state, tok
         tok = tks.next.expect_after "path", :comma
      end
      state.write OutDbl.new directive, [lhs], rhs
   when "playersound", "playeralias", "playersounddup"
      parse_player state, tok, directive
   when "ambient"
      parse_ambient state, tok, directive
   else
      tok.raise_kw "directive"
   end
end

def parse_definition state
   tks = state.tks
   lhs = parse_path state, nil
   rhs = tks.next.expect_after "path", [:brac2o, :identi, :string]
   if rhs.type == :brac2o
      origin = state.path.dup
      state.path.replace lhs
      tks.until_drop :brac2c do
         parse_statement state
      end
      state.path.replace origin
   else
      rhs = state.relative_path rhs.text
      state.write OutDef.new lhs, rhs
   end
end

def parse_statement state
   tks = state.tks
   tok = tks.peek.expect_in_top [:identi, :period, :dollar, :equals, :eof]
   case tok.type
   when :identi, :period
      parse_definition state
   when :dollar
      tks.next
      parse_directive state
   when :equals
      tks.next
      tok = tks.next.expect_after tok, [:identi, :string]
      state.file_dir.replace tok.text
   when :eof
      raise StopIteration
   end
end

def parse state
   loop do parse_statement state end
end

common_main do
   filename = ARGV.shift
   fp       = open ARGV.shift, "wt"
   state = SndParseState.new filename
   parse state
   fp.puts generated_header "sndc"
   state.each_out do |out| fp.puts out end
end

## EOF
