# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Parser state.
##
## ---------------------------------------------------------------------------|

class ParseState
   attr_reader   :out_file, :base_dir
   attr_accessor :tks

   def initialize filename
      @tks      = tokenize filename
      @out_file = String.new
      @base_dir = File.dirname filename
      @out      = []
   end

   def with tks
      new = self.clone
      new.tks = tks
      new
   end

   def write data
      @out.append data
   end

   def each_out &block
      if block_given?
         @out.each &block
      else
         to_enum :each_out
      end
   end
end

## EOF
