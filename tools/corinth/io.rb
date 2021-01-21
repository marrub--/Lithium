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
## I/O utilities.
##
## ---------------------------------------------------------------------------|

class Position
   attr_accessor :name, :line, :colu

   def initialize name, line, colu
      @name = name
      @line = line
      @colu = colu
   end

   def to_s
      "#{@name}@#{@line}:#{@colu}"
   end
end

class PosReader
   include Enumerable

   def initialize name, io
      @io  = io
      @pos = Position.new name, 1, 1
   end

   def pos
      @pos.dup
   end

   def each
      if block_given?
         @io.each_char do |c|
            res = yield c
            case c
            when "\n"
               @pos.line += 1
               @pos.colu  = 1
            else
               @pos.colu += 1
            end
            res
         end
      else
         to_enum :each
      end
   end

   def self.read filename
      PosReader.new File.basename(filename), open(filename, "rt")
   end
end

def read_until_from text, s = ""
   s = +s
   until yield c = text.next do s << c end
   -s
end

def read_while_from text, s = ""
   s = +s
   while yield text.peek do s << text.next end
   -s
end

def read_lines filename
   open(filename, "rt").read.chomp.lines
end

## EOF
