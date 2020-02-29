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
## Tokenizer.
##
## ---------------------------------------------------------------------------|

class Symbol
   def tk_to_s
      case self
      when :assign; "`='"
      when :brac2c; "`}'"
      when :brac2o; "`{'"
      when :colon ; "`:'"
      when :comma ; "`,'"
      when :dollar; "`$'"
      when :equals; "`=='"
      when :identi; "identifier"
      when :modulo; "`%'"
      when :number; "number"
      when :period; "`.'"
      when :plus  ; "`+'"
      when :semico; "`;'"
      when :string; "string"
      else;         "unknown token"
      end
   end
end

class Token
   SPACE = /\s/
   IDENS = /[a-zA-Z_*\\]/
   IDENC = /[a-zA-Z0-9_\/-]/
   NUMRS = /[0-9]/
   NUMRC = /[0-9.]/

   attr_reader :pos, :type, :text

   def initialize pos, type, text = nil
      @pos  = pos
      @type = type
      @text = text
   end

   def expect_after pr, nx
      expect_in nx do |nx|
         "#{nx} must follow #{pr} (but found #{self} instead)"
      end
   end

   def expect_in_top nx
      expect_in nx do |nx|
         "#{nx} expected in toplevel but got #{self}"
      end
   end

   def expect_in nx
      nx = [nx] if nx.is_a? Symbol
      unless nx.include? @type
         nx = nx.map do |sy| sy.tk_to_s end.join ", "
         raise "#{@pos}: #{yield nx}"
      end
      self
   end

   def raise_kw kind
      raise "#{@pos}: invalid #{kind} #{self}"
   end

   def to_s
      case @type
      when :identi; "`#{@text}'"
      when :string; "\"#{@text}\""
      when :number; "#{@text}"
      else;         @type.tk_to_s
      end
   end
end

def tokenize filename
   read = PosReader.read filename
   data = read.each
   colu = 1
   line = 1

   tokens = []

   tok_s = lambda do |p, type, text| tokens.push Token.new p, type, -text end
   tok_1 = lambda do |p, type|       tokens.push Token.new p, type        end

   tok_2 = lambda do |p, nx, type_a, type_b|
      c = data.peek
      if c == nx then data.next
         tok_1.(p, type_a)
      else
         tok_1.(p, type_b)
      end
   end

   loop do
      case c = data.next
      when Token::SPACE; next
      when "#"; read_until_from data do |c| c == "\n" end
      when "$"; tok_1.(read.pos, :dollar)
      when "%"; tok_1.(read.pos, :modulo)
      when "+"; tok_1.(read.pos, :plus  )
      when ","; tok_1.(read.pos, :comma )
      when "."; tok_1.(read.pos, :period)
      when ":"; tok_1.(read.pos, :colon )
      when ";"; tok_1.(read.pos, :semico)
      when "="; tok_2.(read.pos, "=", :equals, :assign)
      when "{"; tok_1.(read.pos, :brac2o)
      when "}"; tok_1.(read.pos, :brac2c)
      when '"'
         pos = read.pos
         s   = read_until_from data do |c| c == '"' end
         tok_s.(pos, :string, s)
      when Token::IDENS
         c   = "" if c == "\\"
         pos = read.pos
         s   = read_while_from data, +c do |c| c =~ Token::IDENC end
         tok_s.(pos, :identi, s)
      when Token::NUMRS
         pos = read.pos
         s   = read_while_from data, +c do |c| c =~ Token::NUMRC end
         tok_s.(pos, :number, s)
      else
         raise "#{read.pos}: invalid character `#{c}'"
      end
   end

   tokens
end

## EOF
