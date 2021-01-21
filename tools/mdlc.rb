#!/usr/bin/env ruby
# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Zoe Elsie Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Model data → MODELDEF processor.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

class OutProp
   attr_reader :name, :args

   def initialize name, args
      @name = name
      @args = args
   end

   def to_s
      "   #{name} #{args.join " "}\n"
   end
end

class OutModel
   attr_reader :name, :props, :bodies

   def initialize name, props, bodies
      @name   = name
      @props  = props
      @bodies = bodies
   end

   def to_s
      s = String.new
      for body in @bodies
         s << "model \"#{@name}\" {\n#{@props.join}#{body.join}}\n"
      end
      s
   end
end

class MdlParseState < ParseState
   attr_reader :presets

   def initialize filename
      super
      @presets = {}
   end
end

def parse_prop state, tok
   tks  = state.tks
   name = tok.text
   args = tks.while_is([:string, :number]).map do |tk|
      text = (tok = tk).text
      text = '"' + text + '"' if tk.type == :string
      text
   end
   tok  = tks.next.expect_after tok, :semico
   prop = OutProp.new name, args
   [prop, tok]
end

def parse_props state, tok
   tks = state.tks

   props = []

   loop do
      tok = tks.next.expect_after tok, [:identi, :brac2c]
      case tok.type
      when :identi
         prop, tok = parse_prop state, tok
         props.push prop
      when :brac2c
         break
      end
   end

   props
end

def parse_model state, tok, name
   name = name.gsub "#", "Lith_"

   tks = state.tks

   presets = tks.while_is(:identi).map do |tk| (tok = tk).text end
   props   = presets.map do |pre| state.presets[pre] end.flatten

   bodies = []
   while (tok = tks.drop :brac2o)
      bodies.push parse_props state, tok
   end

   state.write OutModel.new name, props, bodies
end

def parse_preset state, tok, name
   state.presets[name] = parse_props state, tok
end

def parse_definition state
   tks = state.tks
   tok = tks.next.expect_in_top [:identi, :dollar, :eof]

   case tok.type
   when :dollar
      tok = tks.next.expect_after tok, :identi
      case tok.text
      when "output"
         state.out_file.replace tks.next.expect_after(tok, :string).text
      else
         tok.raise_kw "directive"
      end
   when :identi
      name = tok.text
      tok  = tks.next.expect_after tok, [:colon, :brac2o]
      case tok.type
      when :colon  then parse_model  state, tok, name
      when :brac2o then parse_preset state, tok, name
      end
   when :eof
      raise StopIteration
   end
end

def parse state
   loop do parse_definition state end
end

common_main do
   for filename in ARGV
      state = MdlParseState.new filename
      parse state

      fp = open state.out_file, "wt"
      fp.puts generated_header "mdlc"

      state.each_out do |out| fp.puts out end
   end
end

## EOF
