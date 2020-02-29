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
## Structured data → SNDINFO processor.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

class OutDef
   attr_reader :path, :file

   def initialize path, file
      @path = path
      @file = file
   end

   def to_s
      @path.join("/") + " " + @file
   end
end

class OutDir
   attr_reader :name, :args

   def initialize name, args
      @name = name
      @args = args
   end

   def self.args_to_s args
      args.map do |arg|
            if arg.is_a? Array  then arg.join("/")
         elsif arg.is_a? String then arg           end
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

class ParseState
   attr_reader   :out_file, :snds_dir, :file_dir, :path
   attr_accessor :tks

   def initialize tks
      @tks      = tks
      @out_file = String.new
      @snds_dir = String.new
      @file_dir = String.new
      @out      = []
      @ignore   = []
      @path     = []
   end

   def ignore s
      @ignore.push s
   end

   def ignore? s
      @ignore.include? s
   end

   def with tks
      new = self.clone
      new.tks = tks
      new
   end

   def write data
      @out.append data
   end

   def prepend_file_dir s
      if @file_dir.empty?
         s.dup
      else
         @file_dir + "/" + s
      end
   end

   def each_out &block
      if block_given?
         @out.each &block
      else
         to_enum :each_out
      end
   end
end

def parse_path state, orig
   tks = state.tks
   tok = Token.expect tks.peek, orig, [:identi, :period]

   parse_relative_path = lambda do |path|
      tok = Token.expect tks.next, tok, :identi
      path.push tok.text
      if tks.peek.type == :period
         tks.next
         parse_relative_path.(path)
      else
         path
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

   tok = Token.expect tks.next, tok, :number
   args.push tok.text

   path = parse_path state, tok
   args.push path

   tok = Token.expect tks.next, "path", :identi
   case tok.text
   when "point"
      tok = Token.expect tks.next, tok, :number
      args.push "point", tok.text
      tok = Token.expect tks.next, tok, :identi
   when "surround", "world"
      args.push tok.text
      tok = Token.expect tks.next, tok, :identi
   end

   num = case tok.text
         when "continuous"; 0
         when "periodic";   1
         when "random";     2
         else; Token.raise_kw tok, "mode"
         end

   args.push tok.text

   (num + 1).times do
      tok = Token.expect tks.next, tok, :number
      args.push tok.text
   end

   state.write OutDir.new directive, args
end

def parse_player state, tok, directive
   tks  = state.tks
   args = []

   3.times do
      tok = Token.expect tks.next, tok, :identi
      args.push tok.text
   end

   case directive
   when "playersounddup"
      tok = Token.expect tks.next, tok, :identi
      args.push tok.text
   when "playeralias"
      path = parse_path state, tok
      args.push path
   else
      tok = Token.expect tks.next, tok, [:identi, :string]
      args.push state.prepend_file_dir tok.text
   end

   state.write OutDir.new directive, args
end

def parse_directive state
   tks = state.tks

   tok = Token.expect tks.next, tok, :identi
   case (directive = tok.text)
   when "include"
      tok       = Token.expect tks.next, tok, :string
      new_tks   = tokenize tok.text
      new_state = state.with new_tks.each
      parse new_state
   when "output"
      tok = Token.expect tks.next, tok, :string
      state.out_file.replace tok.text
   when "sounds"
      tok = Token.expect tks.next, tok, :string
      state.snds_dir.replace tok.text
   when "ignore"
      tok = Token.expect tks.next, tok, [:identi, :string]
      state.ignore state.prepend_file_dir tok.text
   when "rolloff"
      path = path_to_sndinfo parse_path state, tok
      args = [path]

      tok = Token.expect tks.next, tok, [:identi, :number]
      if tok.type == :identi
         case tok.text
         when "custom", "linear", "log"
            args.push tok.text
            tok = Token.expect tks.next, tok, :number
         else
            Token.raise_kw "type"
         end
      end
      args.push tok.text

      tok = Token.expect tks.next, tok, :number
      args.push tok.text
      state.write OutDir.new directive, args
   when "pitchshiftrange"
      tok = Token.expect tks.next, tok, :number
      state.write OutDir.new directive, [tok.text]
   when "attenuation", "limit", "pitchshift", "volume"
      path = parse_path state, tok
      tok  = Token.expect tks.next, "path", :number
      state.write OutDir.new directive, [path, tok.text]
   when "alias"
      lhs = parse_path state, tok
      tok = Token.expect tks.next, "path", :assign
      rhs = parse_path state, tok
      state.write OutDir.new directive, [lhs, rhs]
   when "random"
      lhs = parse_path state, tok
      tok = Token.expect tks.next, "path", :brac2o
      rhs = []

      loop do
         if tks.peek.type == :brac2c
            tks.next
            break
         end
         rhs.push parse_path state, tok
         tok = Token.expect tks.next, "path", :comma
      end

      state.write OutDbl.new directive, [lhs], rhs
   when "playersound", "playeralias", "playersounddup"
      parse_player state, tok, directive
   when "ambient"
      parse_ambient state, tok, directive
   else
      Token.raise_kw tok, "directive"
   end
end

def parse_definition state
   tks = state.tks
   lhs = parse_path state, nil
   rhs = Token.expect tks.next, "path", [:brac2o, :identi, :string]
   if rhs.type == :brac2o
      origin = state.path.dup
      state.path.replace lhs
      loop do
         if tks.peek.type == :brac2c
            tks.next
            break
         end
         parse_statement state
      end
      state.path.replace origin
   else
      rhs = state.prepend_file_dir rhs.text
      state.write OutDef.new lhs, rhs
   end
end

def parse_statement state
   tks = state.tks
   tok = Token.expect_in tks.peek,
                         [:identi, :period, :dollar, :equals] do |cr, nx|
      "#{nx} expected in toplevel but got #{cr}"
   end

   case tok.type
   when :identi, :period
      parse_definition state
   when :dollar
      tks.next
      parse_directive state
   when :equals
      tks.next
      tok = Token.expect tks.next, tok, [:identi, :string]
      state.file_dir.replace tok.text
   end
end

def parse state
   loop do
      parse_statement state
   end
end

begin
   base = Dir.pwd
   for filename in ARGV
      Dir.chdir File.dirname filename
      filename = File.basename filename

      tks   = tokenize filename
      state = ParseState.new tks.each
      parse state

      fp = open state.out_file, "wt"
      fp.puts generated_header "compilesnd"

      for out in state.each_out
         if out.is_a? OutDef
            file = state.snds_dir + "/" + out.file
            unless state.ignore?(out.file) || File.exist?(file)
               raise %(file "#{file}" doesn't exist in the filesystem)
            end
         end
         fp.puts out
      end
      Dir.chdir base
   end
rescue => exc
   puts "Error:\n#{exc.backtrace.join "\n"}\n#{exc.message}"
end

## EOF
